#include "pch.hpp"
#include "jit1_branch_common.hpp"

#include "codegen.hpp"
#include "processor/processor.hpp"


namespace mips
{
	processor::flag Jit1_CodeGen::intrinsic_set_cti_flag()
	{
		const auto flag =
			jit_.processor_.disable_cti_ ?
				processor::flag::none :
				processor::flag::no_cti;

		if (flag != processor::flag::none)
		{
			or_(ebx, flag);
		}

		return flag;
	}

	processor::flag Jit1_CodeGen::intrinsic_set_delay_branch(const bool hazard_barrier)
	{
		processor::flag flags = processor::flag::branch_delay |
			(
				jit_.processor_.disable_cti_ ?
					processor::flag::none :
					processor::flag::no_cti
			);

		if (hazard_barrier)
		{
			flags |= processor::flag::instruction_hazard;
		}

		xassert(flags != processor::flag::none);
		or_(ebx, flags);

		return flags;
	}

	Xbyak::Label Jit1_CodeGen::intrinsic_write_patch_prolog(const jit1::Chunk& __restrict chunk, void* const patch_address, const uint32 patch_target_address, const Xbyak::Reg& patch_target_address_reg)
	{
		// Start with a set of no-ops so that we have somewhere to write patch code.
		const auto patch = L(); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
		xassert(getSize() <= std::numeric_limits<uint32>::max());

		// TODO : make this pointer-size generic - this is 64-bit only
		// patch no-op
		if (patch_address == nullptr) {
			nop(12, true);
		}
		else {
			static constexpr uint16 patch_prefix = 0xB848;
			static constexpr uint16 patch_suffix = 0xE0FF;
			dw(patch_prefix);
			dq(uint64(patch_address));
			dw(patch_suffix);
		}

		const uint32 patch_offset = uint32(getSize());
		auto &patch_pair = chunk.m_patches->emplace_back(patch_offset, 0, jit1::Chunk::patch::types::full);
		uint32 &patch_target = patch_pair.target;

		// TODO : highly unsafe presently, though OK for now since it's in a std::list. This address cannot move.
		{
			intptr patch_target_ptr_address = intptr(&patch_target);

			const intptr diff = patch_target_ptr_address - intptr(get_current_address());

			if (in_range<int32>(diff))
			{
				mov(dword[rip + diff], edx);
			}
			else if (in_range<int32>(patch_target_ptr_address))
			{
				putSeg(ds);
				mov(dword[&patch_target], edx);  // NOLINT(performance-no-int-to-ptr)
			}
			else
			{
				set(rcx, patch_target_ptr_address);
				mov(dword[rcx], edx);
			}
		}

		set(patch_target_address_reg, patch_target_address);

		return patch;
	}

	void Jit1_CodeGen::intrinsic_write_patch_epilog(const Xbyak::Label& patch)
	{
		static constexpr uint16 patch_prefix = 0xB848;
		static constexpr uint16 patch_suffix = 0xE0FF;

		auto* address = patch.getAddress();

		#pragma message("validate that this is optimal over `patch + 2`, `patch + 10`, etc")
		mov(rcx, patch);
		mov(word[rcx], patch_prefix);
		mov(qword[rcx + 2], rax);
		mov(word[rcx + 10], patch_suffix);
	}

	void Jit1_CodeGen::intrinsic_write_patch_jump(const jit1::Chunk& __restrict chunk, const uint32 target_address, const Xbyak::Reg& patch_target_address_reg, const bool set_pc)
	{
		auto& jit = jit_;
		const auto current_address = jit.fetch_instruction(target_address);

		// In this case, we need to find the address in order to jump to it.
		auto&& patch = intrinsic_write_patch_prolog(
			chunk,
			std::bit_cast<void*>(current_address),
			target_address,
			patch_target_address_reg
		);
		if (set_pc)
		{
			mov(dword[rbp + offsets.pc], patch_target_address_reg);
		}
		if (!is_same(edx, patch_target_address_reg))
		{
			mov(edx, eax);
		}
		set(rcx, std::bit_cast<uintptr>(&jit));
		std::ignore = call_ex<true>(ptr_cast(&jit1::get_instruction), rax);
		intrinsic_write_patch_epilog(patch);
		jmp(rax);
	}

	void Jit1_CodeGen::intrinsic_insert_jump(const jit1::Chunk & __restrict chunk, const jit1::ChunkOffset &__restrict chunk_offset, const uint32 address, const Xbyak::Operand& target_address)
	{
		// 8A 42 7F 84 C0 74 6A 48 FF 42 7F C6 42 7F 00 8B 42 7F B9 FF FF FF 7F 39 C8 72 2B 3D FF FF FF 7F 77 24 29 C8 48 B9 FF FF FF FF FF FF FF 7F 8B 04 01 48 B9 FF FF FF FF FF FF FF 7F 48 01 C8 45 31 C0 48 31 C9 FF E0 52 48 89 C2 48 83 EC 20 48 B8 FF FF FF FF FF FF FF 7F 48 B9 FF FF FF FF FF FF FF 7F FF D0 48 83 C4 20 5A 45 31 C0 48 31 C9 FF E0 
		// mov al, byte [rdx + 0x7F]			  ; load [pcc_offset] (program counter changed)
		// test al, al								 ; is pcc_offset 0?
		// je no_change								; if so, jump past this routine
		// inc qword [rdx + 0x7F]				  ; increment the [ic_offset] instruction counter
		// mov byte [rdx + 0x7F], 0				; set [pcc_offset] to 0
		// mov eax, dword [rdx + 0x7F]			; load [pc_offset] (program counter), zero extended, set by the compact branch instruction
		//												 ; check first if this offset is within the current chunk.
		// mov ecx, 0x7FFFFFFF					  ; load the [chunk base]
		// cmp eax, ecx								; is the address below chunk base?
		// jb not_within							  ; if so, skip to 'not_within'
		// cmp eax, 0x7FFFFFFF					  ; is the address above [chunk last]? (chunk base + chunk size - 1) (because of 0x1'0000'0000)
		// ja not_within							  ; if so, skip to 'not within'
		//												 ; if we are jumping internally, we can hardcode the offset
		// sub eax, ecx								; subtract chunk base from our address, giving us an offset within the chunk.
		//												 ; this offset is 4-byte aligned, as are all instructions. Luckily, so is 'chunk_offset'.
		// mov rcx, qword 0x7FFFFFFFFFFFFFFF	; load chunk_offset (uint32[])
		// mov eax, dword [rcx + rax]			 ; load chunk_offset[rax] (rax vs eax doesn't matter here, the value is 32-bit zero-extended)
		// mov rcx, qword 0x7FFFFFFFFFFFFFFF	; load chunk data pointer [pointer fixed up after generation of chunk]
		// add qword rax, rcx						; add the actual offset to the chunk data pointer, which is our jump target
		// xor r8d, r8d								; clear r8d (program counter delta)
		// xor rcx, rcx								; zero rcx, as our JIT guarantees that it is 0 at the start of instructions.
		// jmp rax									  ; jump to the local jump target
		// not_within:								 ; otherwise, we are jumping to a remove jump target
		// 
		// push rdx									 ; push rdx [stores 'processor' pointer + 128, a value we must retain]
		// mov rdx, rax								; move rax to rdx, which is the second argument in Win64 ABI
		// sub rsp, 32								 ; push the home space required by the Win64 ABI
		// mov rax, 0x7FFFFFFFFFFFFFFF			; load the address of [jit1::get_instruction]
		// mov rcx, 0x7FFFFFFFFFFFFFFF			; load the [this] pointer into rcx, which is the first argument in Win64 ABI
		// call rax									 ; call [jit1::get_instruction]
		// add rsp, 32								 ; pop the home space
		// pop rdx									  ; restore rdx
		// xor r8d, r8d								; clear r8d (program counter delta)
		// xor rcx, rcx								; zero rcx, as our JIT guarantees that it is 0 at the start of instructions.
		// jmp rax									  ; jump to the remote jump target
		// no_change:								  ; otherwise, the subroutine (unencoded) compact branch did not trigger a branch.

		const uint32 chunk_begin = address & ~(jit1::ChunkSize - 1);

		const Xbyak::Label not_within;

		if (!is_same(target_address, ecx))
		{
			mov(ecx, target_address);
		}
		and_(ecx, ~(jit1::ChunkSize - 1));
		cmp(ecx, chunk_begin);
		jne(not_within, T_SHORT);
		and_(eax, (jit1::ChunkSize - 1));

		mov(eax, dword[rax + uintptr(chunk_offset.data())]);
		auto&& chunk_start = intrinsics_.chunk_start.get();
		if (chunk_start.getAddress())
		{
			lea(rax, dword[rax + uintptr(chunk_start.getAddress())]);
		}
		else 
		{
			// xbyak cannot handle `lea` with an unaddressed label
			mov(rcx, chunk_start);
			add(rax, rcx);
		}
		
		jmp(rax);
		L(not_within);

		mov(rdx, rax);
		set(rcx, uintptr(&jit_));
		std::ignore = call_ex<true>(ptr_cast(&jit1::get_instruction), rax);

		jmp(rax);
	}
}
