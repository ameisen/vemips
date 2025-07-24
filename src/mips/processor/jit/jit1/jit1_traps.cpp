#include "pch.hpp"

#include "jit1.hpp"
#include "../../processor.hpp"
#include "coprocessor/coprocessor1/coprocessor1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/coprocessor1_support.hpp"
#include <cassert>
#include "codegen.hpp"
#include "mips/system.hpp"

using namespace mips;

// TODO consider implementing code properly.

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_TEQ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	// tr ? [rs] == [rt]
	if (rs == rt) [[unlikely]] {
		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		return Jit1_CodeGen::except_result::always_throw;
	}
	else {
		const Xbyak::Label no_trap;

		if (rs.is_zero())
		{
			// 0 == rt
			// rt == 0
			cmp_ex(op_rt, 0);
		}
		else if (rt.is_zero())
		{
			// rs == 0
			cmp_ex(op_rs, 0);
		}
		else
		{
			std::ignore = cmp_ex(op_rs, op_rt, ecx);
		}
		jne(no_trap, T_SHORT);

		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		L(no_trap);

		return Jit1_CodeGen::except_result::can_throw;
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_TGE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	// trap = rs >= rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	// tr ? [rs] >= [rt]
	if (rs == rt) [[unlikely]] {
		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		return Jit1_CodeGen::except_result::always_throw;
	}
	else {
		const Xbyak::Label no_trap;

		if (rs.is_zero())
		{
			// 0 >= rt
			// rt <= 0
			cmp_ex(op_rt, 0);
			jg(no_trap, T_SHORT);
		}
		else if (rt.is_zero())
		{
			// rs >= 0
			cmp_ex(op_rs, 0);
			jl(no_trap, T_SHORT);
		}
		else
		{
			std::ignore = cmp_ex(op_rs, op_rt, ecx);
			jl(no_trap, T_SHORT);
		}

		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		L(no_trap);

		return Jit1_CodeGen::except_result::can_throw;
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_TGEU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	// tr ? [rs] >= [rt]

	if (rs == rt) [[unlikely]] {
		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		return Jit1_CodeGen::except_result::always_throw;
	}
	else {
		const Xbyak::Label no_trap;

		if (rs.is_zero())
		{
			// 0 >= rt
			// rt <= 0
			cmp_ex(op_rt, 0);
			ja(no_trap, T_SHORT);
		}
		else if (rt.is_zero())
		{
			// rs >= 0
			cmp_ex(op_rs, 0);
			jb(no_trap, T_SHORT);
		}
		else
		{
			std::ignore = cmp_ex(op_rs, op_rt, ecx);
			jb(no_trap, T_SHORT);
		}

		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		L(no_trap);

		return Jit1_CodeGen::except_result::can_throw;
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_TLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	// tr ? [rs] < [rt]
	if (rs == rt) {
		// nop
		return Jit1_CodeGen::except_result::none;
	}
	else {
		const Xbyak::Label no_trap;

		if (rs.is_zero())
		{
			// 0 < rt
			// rt > 0
			cmp_ex(op_rt, 0);
			jle(no_trap, T_SHORT);
		}
		else if (rt.is_zero())
		{
			// rs < 0
			cmp_ex(op_rs, 0);
			jge(no_trap, T_SHORT);
		}
		else
		{
			std::ignore = cmp_ex(op_rs, op_rt, ecx);
			jge(no_trap, T_SHORT);
		}

		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		L(no_trap);

		return Jit1_CodeGen::except_result::can_throw;
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_TLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	// tr ? [rs] < [rt]
	if (
		rs == rt ||  // rs == rt != rs < rt
		rt.is_zero() // rs < 0 impossible
	) {
		// nop
		return Jit1_CodeGen::except_result::none;
	}
	else {
		const Xbyak::Label no_trap;

		if (rs.is_zero())
		{
			// 0 < rt
			// rt > 0
			cmp_ex(op_rt, 0);
			jbe(no_trap, T_SHORT);
		}
		else if (rt.is_zero())
		{
			// rs < 0
			cmp_ex(op_rs, 0);
			jae(no_trap, T_SHORT);
		}
		else
		{
			std::ignore = cmp_ex(op_rs, op_rt, ecx);
			jae(no_trap, T_SHORT);
		}
		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		L(no_trap);

		return Jit1_CodeGen::except_result::can_throw;
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_TNE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	// tr ? [rs] != [rt]
	if (rs == rt) {
		// nop
		return Jit1_CodeGen::except_result::none;
	}
	else {
		const Xbyak::Label no_trap;

		if (rs.is_zero())
		{
			// 0 != rt
			// rt != 0
			cmp_ex(op_rt, 0);
		}
		else if (rt.is_zero())
		{
			// rs != 0
			cmp_ex(op_rs, 0);
		}
		else
		{
			std::ignore = cmp_ex(op_rs, op_rt, ecx);
		}
		je(no_trap, T_SHORT);

		set(ecx, address);
		set(edx, code);
		jmp(intrinsics_.tr, T_NEAR);

		L(no_trap);

		return Jit1_CodeGen::except_result::can_throw;
	}
}

// syscall
namespace {
	static uint32 do_syscall(const uint32 code, processor * const __restrict processor) {
		auto* const guest = processor->get_guest_system();

		const CPU_Exception sys_ex{ CPU_Exception::Type::Sys, processor->get_program_counter(), code };

		try
		{
			return guest->handle_exception(sys_ex);
		}
		catch (const CPU_Exception& ex)
		{
			processor->set_trapped_exception(ex);
			return 0;
		}
	}

	_pragma_small_code
	_cold _nothrow static void SYS_Exception(uint32 code, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::Sys, processor.get_program_counter(), code });
	}

	_pragma_small_code
	_cold _nothrow static void BP_Exception(uint32 code, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::Bp, processor.get_program_counter(), code });
	}

	_pragma_small_code
	_cold _nothrow static void RI_Exception(uint32 code, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::RI, processor.get_program_counter(), code });
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_SYSCALL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex) {
	const uint32 code = instructions::TinyInt<20>(instruction >> 6).zextend<uint32>();

	if (
		const auto* guest = jit_.processor_.get_guest_system();
		guest && guest->get_capabilities().can_handle_syscalls_inline
	)
	{
		mov(qword[rbp + offsets.ic], rdi);
		mov(dword[rbp + offsets.pc], address);
		mov(dword[rbp + offsets.flags], ebx);
		set(ecx, code);
		lea(rdx, qword[rbp - 128]);
		std::ignore = call_ex(&do_syscall, rax);
		mov(ebx, dword[rbp + offsets.flags]);
		test(eax, eax);
		jnz(intrinsics_.save_return, T_NEAR);

		return except_result::can_throw | except_result::can_except;
	}
	else
	{
		set(ecx, code);
		mov(dword[rbp + offsets.pc], address);
		set(rax, uintptr(&SYS_Exception));
		jmp(intrinsic_ex, T_NEAR);

		return except_result::always_throw | except_result::can_except;
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_BREAK(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex) {
	const uint32 code = instructions::TinyInt<20>(instruction >> 6).zextend<uint32>();

	set(ecx, code);
	mov(dword[rbp + offsets.pc], address);
	set(rax, uintptr(&BP_Exception));
	jmp(intrinsic_ex, T_NEAR);

	return except_result::always_throw;
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_SIGRIE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex) {
	const uint32 code = instructions::TinyInt<20>(instruction >> 6).zextend<uint32>();

	set(ecx, code);
	mov(dword[rbp + offsets.pc], address);
	set(rax, uintptr(&RI_Exception));
	jmp(intrinsic_ex, T_NEAR);

	return except_result::always_throw;
}
