#include "pch.hpp"
#include "jit1_branch_common.hpp"

#include "codegen.hpp"
#include "jit1.hpp"
#include "processor/processor.hpp"

#include <cstddef>
#include <utility>

#include "global_state.hpp"

#if _MSC_VER
#	pragma intrinsic(_ReturnAddress)
#	pragma intrinsic(_AddressOfReturnAddress)
#endif
// __builtin_frame_address does not work consistently


namespace mips
{
	namespace
	{
		static constexpr usize max_full_jump_patch_size = 0x100;
		static constexpr usize expected_jump_patch_size = 12;

		#if !_MSC_VER
		[[nodiscard]]
		#endif
		VEMIPS_JIT_ABI static _nothrow
		#if _MSC_VER
		void
		#else
		uptr
		#endif
		VEMIPS_JIT_ABI_INFIX get_and_patch_instruction(
			const uint16 patch_index
		)
		{
			// TODO : make sure it isn't significantly faster overall to pass the JIT or chunk pointer.
			auto* const jit = reinterpret_cast<jit1*>(global_state::jit::get_current());
			xassert(jit != nullptr);

			#if __GNUC__ || __clang__
			void* const return_address = __builtin_extract_return_addr(__builtin_return_address(0));
			#elif _MSC_VER
			void* const return_address = _ReturnAddress();
			#else
			#	error unimplemented platform
			#endif

			const auto* chunk = jit->get_chunk_by_pointer(uptr_cast(return_address));
			xassert(chunk != nullptr);

			const auto& patch = chunk->patches[patch_index];

			const uptr_guest target_address = patch.target;
			const uptr patch_address = patch.base_address;

			if (patch.set_pc)
			{
				jit->get_processor().set_program_counter(target_address);
			}

			const jit1::jit_instructionexec_t destination_address = jit->get_instruction(target_address);

			xassert(destination_address != nullptr);

			//#pragma message("validate that this is optimal over `patch + 2`, `patch + 10`, etc")
			// TODO : Use a new, temporary codegen to do this.
			/*
			** movabs rax, ADDRESS
			** jmp rax
			*/

			static constexpr uint16 patch_prefix = 0xB848;
			// static constexpr uint16 patch_suffix = 0xE0FF;

			uint8* const write_target = reinterpret_cast<uint8*>(patch_address);

			Jit1_CodeGen temp_cg0 { *jit, write_target, max_full_jump_patch_size};
			temp_cg0.dw(patch_prefix);                        // movabs rax, ...
			temp_cg0.dq(uptr_cast(destination_address)); // address
			temp_cg0.jmp(temp_cg0.rax);
			const usize patch_length = temp_cg0.getSize();
			xassert(patch_length == expected_jump_patch_size);

			{
				std::array<uint8, max_full_jump_patch_size> temporary_buffer;
				Jit1_CodeGen temp_cg1{ *jit, temporary_buffer.data(), temporary_buffer.size() };

				const usize required_patch_size = temp_cg1.intrinsic_write_patch_jump_patcher(
					target_address,
					reinterpret_cast<const void* const>(patch.base_address),
					patch_index,
					patch.set_pc
				);

				// We don't overwrite this part because it actually contains the `jmp` that consumes the return value of this function.
				if (
					const ssize patch_diff = static_cast<ssize>(required_patch_size - patch_length);
					patch_diff > 0
				)
				{
					temp_cg0.nop(patch_diff, true);
				}
			}

			// If we are not direct-returning, make sure that a jmp is there
			#if !_MSC_VER
			*(uint16*)return_address = 0xE0FF; // jmp rax
			#endif

			#if _MSC_VER
			*static_cast<uptr*>(_AddressOfReturnAddress()) = uptr_cast(destination_address);
			#else
			return uptr_cast(destination_address);
			#endif
		}
	}

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

		if (hazard_barrier && jit_.get_processor().handles_instruction_hazards())
		{
			flags |= processor::flag::instruction_hazard;
		}

		xassert(flags != processor::flag::none);
		or_(ebx, flags);

		return flags;
	}

	uptr Jit1_CodeGen::intrinsic_write_patch_jump_patcher(
		const uptr_guest target_address,
		const void* const start_address,
		const uint16 patch_index,
		const bool set_pc
	)
	{
		const auto* const patcher_start = getCurr();

		mov(cx, patch_index);
		// TODO : enable this for non-GCC/MSVC targets
		/*
		set(ecx, target_address);
		// TODO : move this to an intrinsic? Would simplify this and make it constant-sized at this point.
		if (set_pc)
		{
			mov(dword[rbp + offsets.pc], ecx);
		}
		
		mov(rdx, uptr_cast(start_address));
		*/
		std::ignore = call_ex<JumpFlags::ExternalCall | JumpFlags::AlwaysOutOfRange>(ptr_cast(&get_and_patch_instruction), rax);
		#if !_MSC_VER
		jmp(rax);
		#endif

		if (
			const ssize patch_size_diff = static_cast<ssize>(expected_jump_patch_size - static_cast<uptr>(getCurr() - patcher_start));
			patch_size_diff > 0
		)
		{
			nop(patch_size_diff, true);
		}

		return static_cast<usize>(getCurr() - patcher_start);
	}

	void Jit1_CodeGen::intrinsic_write_patch_jump(
		jit1::Chunk& __restrict chunk,
		const uptr_guest target_address,
		const bool set_pc
	)
	{
		auto& jit = jit_;
		const auto current_address = jit.fetch_instruction(target_address);

		const auto* const start_address = getCurr();

		const auto get_patch_length = [&]
		{
			return static_cast<usize>(getCurr() - start_address);
		};

		xassert(getSize() <= std::numeric_limits<uint32>::max());

		chunk.patches.emplace_back(
			uptr_cast(start_address),
			target_address,
			jit1::patch::types::indeterminate,
			set_pc
		);

		const uint16 patch_index = checked_cast<uint16>(chunk.patches.size() - 1);

		if (current_address != nullptr)
		{
			/*
			** movabs rax, ADDRESS
			** jmp rax
			*/
			static constexpr uint16 patch_prefix = 0xB848;
			dw(patch_prefix);                    // movabs rax, ...
			dq(uptr_cast(current_address)); // address
			jmp(rax);                        // E0FF

			const usize patch_length = get_patch_length();
			xassert(patch_length == expected_jump_patch_size);

			{
				std::array<uint8, max_full_jump_patch_size> temporary_buffer;
				Jit1_CodeGen temporary_codegen{ jit_, temporary_buffer.data(), temporary_buffer.size() };
				const usize patcher_size = temporary_codegen.intrinsic_write_patch_jump_patcher(target_address, start_address, patch_index, set_pc);

				if (
					const ssize patch_size_diff = static_cast<ssize>(patcher_size - patch_length);
					patch_size_diff > 0
				)
				{
					nop(patch_size_diff, true);
				}
			}
		}
		else
		{
			std::ignore = intrinsic_write_patch_jump_patcher(target_address, start_address, patch_index, set_pc);
		}
	}

	void Jit1_CodeGen::intrinsic_insert_jump(
		const jit1::Chunk & __restrict chunk,
		const jit1::ChunkOffset &__restrict chunk_offset,
		const uptr_guest address,
		const Xbyak::Operand& target_address
	)
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
		std::ignore = call_ex<JumpFlags::ExternalCall>(ptr_cast(&jit1::get_instruction), rax);

		jmp(rax);
	}

	namespace jit1_common::branch
	{
		void emit_local_jmp(
			Jit1_CodeGen& cg,
			const jit1::ChunkOffset& __restrict chunk_offset,
			const uint32 target_offset,
			const uint32 current_offset
		)
		{
			static constexpr uint32 max_short_jump_look_ahead = 2;

			const auto& target_label = cg.get_instruction_offset_label(target_offset);

			if (
				(
					target_offset <= current_offset &&
					(chunk_offset[current_offset] - chunk_offset[target_offset]) <= 128
				) ||
				(target_offset - current_offset) <= max_short_jump_look_ahead
			)
			{
				cg.jmp(target_label, Xbyak::CodeGenerator::T_AUTO);
			}
			else
			{
				cg.jmp(target_label, Xbyak::CodeGenerator::T_NEAR);
			}
		}
	}
}
