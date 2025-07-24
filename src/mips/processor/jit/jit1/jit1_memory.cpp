#include "pch.hpp"

#include <bit>

#include "jit1.hpp"
#include "../../processor.hpp"
#include "coprocessor/coprocessor1/coprocessor1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/instructions_table.hpp"
#include "instructions/coprocessor1_support.hpp"
#include <cassert>
#include "codegen.hpp"

using namespace mips;

namespace
{
	static uintptr mem_write_jit(processor * const __restrict proc, const uint32 address, const uint32 size) {
		return proc->get_mem_write_jit(address, size);
	}

	static uintptr mem_read_jit(processor * const __restrict proc, const uint32 address, const uint32 size) {
		return proc->get_mem_read_jit(address, size);
	}
}

bool Jit1_CodeGen::write_STORE(jit1::ChunkOffset & __restrict chunk_offset, const uint32 address, const instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	// rd = rs + rt
	const instructions::GPRegister<21, 5> base(instruction, jit_.processor_);

	auto&& op_base = get_register_op32(base);

	uint32 store_size;
	bool fpu = false;
	bool e = false;

	if (IS_INSTRUCTION(instruction_info, PROC_SB)) {
		store_size = 1;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SBE)) {
		e = true;
		store_size = 1;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SC)) {
		return true;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SCE)) {
		e = true;
		return true;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SCWP)) {
		return true;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SCWPE)) {
		e = true;
		return true;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SH)) {
		store_size = 2;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SHE)) {
		e = true;
		store_size = 2;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SW)) {
		store_size = 4;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SWE)) {
		e = true;
		store_size = 4;
	}
	else if(IS_INSTRUCTION(instruction_info, COP1_SDC1_v)) {
		store_size = 8;
		fpu = true;
	}
	else if(IS_INSTRUCTION(instruction_info, COP1_SWC1_v)) {
		store_size = 4;
		fpu = true;
	}
	else {
		return true;
	}

	const auto mmu_type = jit_.processor_.mmu_type_;
	
	int32 offset;
	if (!e) {
		offset = instructions::TinyInt<16>(instruction).sextend<int32>();
	}
	else {
		offset = instructions::TinyInt<9>(instruction >> 7).sextend<int32>();
	}

	if (mmu_type == mmu::emulated) {
		const Xbyak::Label valid_ptr;

		lea(rcx, dword[rbp - 128]);
		// 'rcx' is the first parameter (processor ptr)

		if (!base.is_zero()) {
			// The effective address is '[base] + offset'
			if (offset && op_base.isREG())
			{
				lea(edx, dword[op_base.as_reg() + offset]);
			}
			else
			{
				mov(edx, op_base);
				if (offset != 0) {
					add(edx, offset);
				}
			}
		}
		else {
			set(edx, offset);
		}
		// 'edx' is the second parameter (address)
		set(r8d, store_size); // TODO = store_size - 1 would be more efficient
		mov(r13d, edx); // store to non-volatile for after call if there's an exception.
		// 'r8' is the third parameter (size)
		std::ignore = call_ex(std::bit_cast<void*>(&mem_write_jit), rax);
		// rax now has our destination pointer.
		mov(r13, rax); // save the pointer off to non-volatile r13.
		test(rax, rax);
		jnz(valid_ptr, T_SHORT);
		mov(eax, ecx);
		mov(ecx, r13d);
		jmp(intrinsics_.ades, T_NEAR);
		L(valid_ptr);
	}
	else {
		// This would be far faster if we could memory map easily. Then we could just wrap the address range around.
		if (base.is_zero()) {
			// If base is 0, the address is just offset. This simplifies things, though we need to treat the offset as unsigned.
			// Check for basic range things.
			const uint32 start_address = uint32(offset);
			const uint64 end_address = uint64(offset) + store_size;

			// quick error checking
			if (mmu_type == mmu::none) {
				// this checks if it is in range, or if it overflows and thus overwrites '0'
				if (
					(end_address + jit_.processor_.stack_size_) > jit_.processor_.memory_size_ ||
					(uint64(offset) + uint64(jit_.processor_.stack_size_) + uint64(store_size)) > 0x1'0000'0000ull
				)
				{
					set(eax, address);
					set(ecx, start_address);
					jmp(intrinsics_.ades, T_NEAR);
					return false;
				}
			}
			else if (mmu_type == mmu::host) {
				// I'm going to do this the dumb way.
				for (uint64 addr = start_address; addr < end_address; ++addr) {
					if (
						addr == 0 ||
						(
							jit_.processor_.stack_size_ &&
							addr >= jit_.processor_.memory_size_ &&
							addr < uint32(0x1'0000'0000ull - jit_.processor_.stack_size_)
						)
					) {
						set(eax, address);
						set(ecx, addr);
						jmp(intrinsics_.ades, T_NEAR);
						return false;
					}
				}
			}

			mov(rdx, qword[rbp + offsets.memory_ptr]);
			set(eax, offset);
		}
		else {
			if (offset && op_base.isREG())
			{
				lea(eax, dword[op_base.as_reg() + offset]);
			}
			else
			{
				mov(eax, op_base);
				if (offset) {
					add(eax, offset);
				}
			}
			// error checking
			if (mmu_type == mmu::none) {
				const Xbyak::Label ades;

				mov(edx, eax);
				if (jit_.processor_.stack_size_)
				{
					// zero test
					lea(edx, dword[eax - 1]);
					add(edx, store_size);
					jo(ades, T_SHORT);

					// Offset for stack
					mov(ecx, eax);
					add(eax, jit_.processor_.stack_size_);

					// check for range
					cmp(eax, uint32((jit_.processor_.memory_size_ - store_size) - offset));
				}
				else
				{
					// check for range
					const int64 pow2_memsize = int64(std::bit_ceil(jit_.processor_.memory_size_));
					const int32 addend = int32(pow2_memsize + (store_size - 1));
					const int32 cmpand = int32(pow2_memsize + store_size);
					add(edx, addend);
					cmp(edx, cmpand);
					jae(ades, T_SHORT);
				}
				const Xbyak::Label no_ades;
				jbe(no_ades, T_SHORT);
				L(ades);
				set(eax, address);
				if (!jit_.processor_.stack_size_)
				{
					mov(ecx, eax);
				}
				jmp(intrinsics_.ades, T_NEAR);
				L(no_ades);
			}

			mov(rdx, qword[rbp + offsets.memory_ptr]);
		}
		// rdx == address
	}

	// perform the actual store.
	if (!fpu) {
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

		if (mmu_type != mmu::emulated) {
			auto&& target_operand = rdx + rax;

			if (rt.is_zero()) {
				switch (store_size) {
				case 1:
					mov(byte[target_operand], 0); break;
				case 2:
					mov(word[target_operand], 0); break;
				case 4:
					xor_(ecx, ecx);
					mov(dword[target_operand], ecx); break;
				case 8:
					xor_(ecx, ecx);
					mov(qword[target_operand], rcx); break;
				default:
					_assume(0);
				}
			}
			else {
				switch (store_size) {
					case 1: {
						const auto& src = get_register_op8(rt);
						const auto& dst = byte[target_operand];
						std::ignore = mov_ex(dst, src, cl);
					}
					break;
					case 2: {
						const auto& src = get_register_op16(rt);
						const auto& dst = word[target_operand];
						std::ignore = mov_ex(dst, src, cx);
					}
					break;
					case 4: {
						const auto& src = get_register_op32(rt);
						const auto& dst = dword[target_operand];
						std::ignore = mov_ex(dst, src, ecx);
					}
					break;
					case 8: {
						const auto& src = get_register_op64(rt);
						const auto& dst = qword[target_operand];
						std::ignore = mov_ex(dst, src, rcx);
					}
					break;
					default:
						_assume(0);
				}
			}
		}
		else {
			if (rt.is_zero()) {
				switch (store_size) {
				case 1:
					mov(byte[r13], 0); break;
				case 2:
					mov(word[r13], 0); break;
				case 4:
					xor_(ecx, ecx);
					mov(dword[r13], ecx); break;
				case 8:
					xor_(ecx, ecx);
					mov(qword[r13], rcx); break;
				default:
					_assume(0);
				}
			}
			else {
				switch (store_size) {
					case 1: {
						const auto& src = get_register_op8(rt);
						const auto& dst = byte[r13];
						std::ignore = mov_ex(dst, src, cl);
					}
					break;
					case 2: {
						const auto& src = get_register_op16(rt);
						const auto& dst = word[r13];
						std::ignore = mov_ex(dst, src, cx);
					}
					break;
					case 4: {
						const auto& src = get_register_op32(rt);
						const auto& dst = dword[r13];
						std::ignore = mov_ex(dst, src, ecx);
					}
					break;
					case 8: {
						const auto& src = get_register_op64(rt);
						const auto& dst = qword[r13];
						std::ignore = mov_ex(dst, src, rcx);
					}
					break;
					default:
						_assume(0);
				}
			}
		}
	}
	else
	{
		// TODO fix offsets.
		const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
		const int16 ft_offset = ft.get_offset();

		if (mmu_type != mmu::emulated) {
			auto&& target_operand = rdx + rax;

			switch (store_size) {
				case 1:
					mov(cl, byte[r12 + ft_offset]);
					mov(byte[target_operand], cl);
					break;
				case 2:
					mov(cx, word[r12 + ft_offset]);
					mov(word[target_operand], cx);
					break;
				case 4:
					mov(ecx, dword[r12 + ft_offset]);
					mov(dword[target_operand], ecx);
					break;
				case 8:
					mov(rcx, qword[r12 + ft_offset]);
					mov(qword[target_operand], rcx);
					break;
				default:
					_assume(0);
			}
		}
		else {
			switch (store_size) {
				case 1:
					mov(cl, byte[r12 + ft_offset]);
					mov(byte[r13], cl);
					break;
				case 2:
					mov(cx, word[r12 + ft_offset]);
					mov(word[r13], cx);
					break;
				case 4:
					mov(ecx, dword[r12 + ft_offset]);
					mov(dword[r13], ecx);
					break;
				case 8:
					mov(rcx, qword[r12 + ft_offset]);
					mov(qword[r13], rcx);
					break;
				default:
					_assume(0);
			}
		}
	}

	if (mmu_type == mmu::emulated && !jit_.processor_.readonly_exec_) {
		const Xbyak::Label no_flush;
		//cmp(eax, 0);
		//je(no_flush);
		test(eax, eax);
		jz(no_flush, T_SHORT);
		set(eax, address);
		jmp(intrinsics_.save_return, T_NEAR);
		L(no_flush);
	}

	return false;
}

bool Jit1_CodeGen::write_LOAD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	// rd = rs + rt
	const instructions::GPRegister<21, 5> base(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto&& op_base = get_register_op32(base);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) {
		// nop
		return false;
	}

	const auto mmu_type = jit_.processor_.mmu_type_;

	int32 nommu_offset = 0;

	const auto get_address = [&](const int32 offset, const uint32 load_size) {
		const Xbyak::Label valid_ptr;

		if (mmu_type == mmu::emulated) {
			lea(rcx, qword[rbp - 128]);
			// 'rcx' is the first parameter (processor ptr)

			if (!base.is_zero()) {
				// The effective address is '[base] + offset'
				if (offset && op_base.isREG())
				{
					lea(edx, dword[op_base.as_reg() + offset]);
				}
				else
				{
					mov(edx, op_base);

					if (offset != 0) {
						add(edx, offset);
					}
				}
			}
			else {
				set(edx, offset);
			}

			// 'edx' is the second parameter (address)
			set(r8d, load_size);
			mov(r13d, edx); // store to non-volatile for after call if there's an exception.
			// 'r8' is the third parameter (size)
			std::ignore = call_ex(std::bit_cast<void*>(&mem_read_jit), rax);
			// rax now has our destination pointer.
			mov(r13, rax); // save the pointer off to non-volatile r13.
			//cmp(rax, 0);
			//jne(valid_ptr);
			test(rax, rax);
			jnz(valid_ptr, T_SHORT);
			mov(eax, ecx);
			mov(ecx, r13d);
			jmp(intrinsics_.adel, T_NEAR);
			L(valid_ptr);
		}
		else {
			// This would be far faster if we could memory map easily. Then we could just wrap the address range around.
			if (base.is_zero()) {
				// If base is 0, the address is just offset. This simplifies things, though we need to treat the offset as unsigned.
				// Check for basic range things.
				const uint32 start_address = uint32(offset);
				const uint64 end_address = uint64(offset) + load_size;

				// quick error checking
				if (mmu_type == mmu::none) {
					// this checks if it is in range, or if it overflows and thus overwrites '0'
					if (
						(end_address + jit_.processor_.stack_size_) > jit_.processor_.memory_size_ ||
						(uint64(offset) + uint64(jit_.processor_.stack_size_) + uint64(load_size)) > 0x100000000ull
					)
					{
						set(eax, address);
						set(ecx, start_address);
						jmp(intrinsics_.adel, T_NEAR);
						return false;
					}
				}
				else if (mmu_type == mmu::host) {
					// I'm going to do this the dumb way.
					for (uint64 addr = start_address; addr < end_address; ++addr) {
						if (
							addr == 0 ||
							(
								jit_.processor_.stack_size_ &&
								addr >= jit_.processor_.memory_size_ &&
								addr < uint32(0x100000000 - jit_.processor_.stack_size_)
							)
						) {
							set(eax, address);
							set(ecx, addr);
							jmp(intrinsics_.adel, T_NEAR);
							return false;
						}
					}
				}

				mov(rdx, qword[rbp + offsets.memory_ptr]);
				set(eax, offset);
			}
			else {
				if (offset && op_base.isREG())
				{
					lea(eax, dword[op_base.as_reg() + offset]);
				}
				else
				{
					mov(eax, op_base);
					if (offset) {
						add(eax, offset);
					}
				}
				// error checking
				if (mmu_type == mmu::none) {
					const Xbyak::Label adel;

					// zero test
					lea(edx, dword[eax - 1]);
					add(edx, load_size);
					jo(adel, T_SHORT);

					// Offset for stack
					if (jit_.processor_.stack_size_)
					{
						mov(ecx, eax);
						add(eax, jit_.processor_.stack_size_);
					}

					// check for range
					cmp(eax, uint32((jit_.processor_.memory_size_ - load_size) - offset));
					const Xbyak::Label no_adel;
					jbe(no_adel, T_SHORT);
					L(adel);
					set(eax, address);
					if (!jit_.processor_.stack_size_)
					{
						mov(ecx, eax);
					}
					jmp(intrinsics_.adel, T_NEAR);
					L(no_adel);
				}

				mov(rdx, qword[rbp + offsets.memory_ptr]);
			}
			// rdx == address
		}
		return true;
	};

	// get_offset: r13 = mem pointer

	if (mmu_type == mmu::emulated) {
		try {
			if (IS_INSTRUCTION(instruction_info, PROC_LB)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LH)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LL)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLE)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWP)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWPE)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LW)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 4)) {
					return false;
				}

				std::ignore = mov_ex(op_rt, dword[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 4)) {
					return false;
				}

				std::ignore = mov_ex(op_rt, dword[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LDC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 4)) {
					return false;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(rax, qword[r13]);
				mov(qword[r12 + ft_offset], rax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LWC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 4)) {
					return false;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(eax, dword[r13]);
				mov(dword[r12 + ft_offset], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWPC)) {
				// load word PC relative - gets special offset handling.
				const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
				const int32 offset = instructions::TinyInt<21>(instruction << 2).sextend<int32>();

				const int8 rs_offset = rs.get_offset();

				if (!get_address(address + offset, 4)) {
					return false;
				}

				mov(eax, dword[r13]);
				mov(dword[rbp + rs_offset], eax);
			}
			else {
				return true;
			}
		}
		catch (...) {
			return false;
		}
	}
	else { // no mmu
		try {
			auto&& src = rdx + rax;

			if (IS_INSTRUCTION(instruction_info, PROC_LB)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 1)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LH)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movsx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 2)) {
					return false;
				}

				std::ignore = movzx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LL)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLE)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWP)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWPE)) {
				return true;
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LW)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 4)) {
					return false;
				}

				std::ignore = mov_ex(op_rt, dword[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<int32>(), 4)) {
					return false;
				}

				std::ignore = mov_ex(op_rt, dword[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LDC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 4)) {
					return false;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(rax, qword[src]);
				mov(qword[r12 + ft_offset], rax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LWC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<int32>(), 4)) {
					return false;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(eax, dword[src]);
				mov(dword[r12 + ft_offset], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWPC)) {
				// load word PC relative - gets special offset handling.
				const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
				const int32 offset = instructions::TinyInt<21>(instruction << 2).sextend<int32>();

				const int8 rs_offset = rs.get_offset();

				if (!get_address(address + offset, 4)) {
					return false;
				}

				mov(eax, dword[src]);
				mov(dword[rbp + rs_offset], eax);
			}
			else {
				return true;
			}
		}
		catch (...) {
			return false;
		}
	}

	return false;
}
