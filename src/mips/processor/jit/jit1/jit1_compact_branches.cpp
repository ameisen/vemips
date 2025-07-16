#include "pch.hpp"

#include "jit1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/instructions_table.hpp"
#include "../../processor.hpp"
#include <cassert>
#include "codegen.hpp"

using namespace mips;

static constexpr uint32 MaxShortJumpLookAhead = 2;

std::pair<bool, Jit1_CodeGen::except_result> Jit1_CodeGen::write_compact_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const uint32 chunk_begin = address & ~(jit1::ChunkSize - 1);
	const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
	const uint32 next_chunk = chunk_last + 1;
	const uint32 this_offset = (address - chunk_begin) / 4u;

	static const int8 flags_offset = value_assert<int8>(offsetof(processor, flags_) - 128);
	static const int8 dbt_offset =  value_assert<int8>(offsetof(processor, branch_target_) - 128);
	static const int8 pc_offset = value_assert<int8>(offsetof(processor, program_counter_) - 128);
	static const int8 ic_offset = value_assert<int8>(offsetof(processor, instruction_count_) - 128);
	const instructions::GPRegister<> r31 = {31U};

	auto&& op_r31 = get_register_op32(r31);

	except_result exception_result = except_result::none;

	const auto write_edx_to_patch_target = [this](uint32& target)
	{
		intptr patch_target_address = intptr(&target);
		if (
			patch_target_address >= intptr(std::numeric_limits<int32>::lowest()) &&
			patch_target_address <= intptr(std::numeric_limits<int32>::max())
		)
		{
			// xbyak cannot handle this sequence properly
			// mov dword ptr [ds:patch_target_address], edx

			db(0x89, 0x14, 0x25);
			dd(uint32(patch_target_address));
		}
		else
		{
			mov(rcx, patch_target_address);
			mov(dword[rcx], edx);
		}
	};

	const auto patch_preprolog = [&](auto patch_address) -> Xbyak::Label
	{
		// If execution gets past the chunk, we jump to the next chunk.
		// Start with a set of no-ops so that we have somewhere to write patch code.
		const auto patch = L(); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
		auto &patch_pair = chunk.m_patches->emplace_back(uint32(getSize()), 0);
		uint32 &patch_target = patch_pair.target;

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

		write_edx_to_patch_target(patch_target);

		return patch;
	};

	const auto patch_prolog = [&]
	{
		auto &patch_pair = chunk.m_patches->back();
		uint32 &patch_target = patch_pair.target;
		write_edx_to_patch_target(patch_target);
	};

	const auto patch_epilog = [&](const Xbyak::Label &patch)
	{
		static constexpr uint16 patch_prefix = 0xB848;
		static constexpr uint16 patch_suffix = 0xE0FF;

		mov(rcx, patch);
		mov(word[rcx], int16_t(patch_prefix));
		mov(qword[rcx + 2], rax);
		mov(word[rcx + 10], int16_t(patch_suffix));
	};

	const auto safejmp = [&](const Xbyak::Label &target_label, const uint32 instruction_offset)
	{
		if (
			(
				instruction_offset <= this_offset &&
				(chunk_offset[this_offset] - chunk_offset[instruction_offset]) <= 128
			) ||
			(instruction_offset - this_offset) <= MaxShortJumpLookAhead
		)
		{
			jmp(target_label);
		}
		else
		{
			jmp(target_label, T_NEAR);
		}
	};

	const auto patch_jump = [&](uint32 target_address)
	{
		const auto current_address = jit_.fetch_instruction(target_address);

		// In this case, we need to find the address in order to jump to it.
		inc(rdi);
		const auto patch = patch_preprolog(current_address);
		mov(eax, target_address);
		patch_prolog();
		mov(dword[rbp + pc_offset], eax);
		mov(edx, eax);
		mov(rax, std::bit_cast<uintptr>(&jit1::get_instruction));
		mov(rcx, uintptr(&jit_));
		call(rax);
		patch_epilog(patch);
		jmp(rax);
	};

	const auto emit_near_far_jump = [&](const uint32 target_address)
	{
		// near branch
		// destination is in this chunk. This is far easier to handle.
		if (target_address >= chunk_begin && target_address <= chunk_last)
		{
			const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
			const auto& target_label = get_instruction_offset_label(instruction_offset);
			mov(dword[rbp + pc_offset], target_address);
			inc(rdi);
			safejmp(target_label, instruction_offset);
		}
		// far
		// destination is outside of this chunk. This is more difficult to handle.
		else
		{
			// In this case, we need to find the address in order to jump to it.
			patch_jump(target_address);
		}
	};

	const auto disable_cti = [this]
	{
		if (!jit_.processor_.disable_cti_) {
			or_(ebx, processor::flag::no_cti);
		}
	};

	if (IS_INSTRUCTION(instruction_info, PROC_BALC))
	{
		const int32 immediate = instructions::TinyInt<28>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + immediate;
		const uint32 link_address = address + 4;
		set(op_r31, link_address);	// set link

		emit_near_far_jump(target_address);
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BC))
	{
		const int32 immediate = instructions::TinyInt<28>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + immediate;

		emit_near_far_jump(target_address);
	}
	 else if (IS_INSTRUCTION(instruction_info, PROC_BLEZALC)) // branch rt <= 0 and link
	{
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;
		const uint32 link_address = address + 4;

		auto&& op_rt = get_register_op32(rt);

		set(op_r31, link_address);	// set link

		// instruction only valid if rt != 0
		if (!rt.is_zero())
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jg(no_jump, T_SHORT);															 // jump past branch

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP06))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + offset;

		if (rs == rt && !rs.is_zero()) // BGEZALC - branch rt >= 0 and link
		{
			const uint32 link_address = address + 4;
			set(op_r31, link_address);	// set link

			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jl(no_jump, T_SHORT);															 // jump past branch if < 0

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (rs != rt && !rs.is_zero() && !rt.is_zero()) // BGEUC - branch rs >= rt
		{
			const Xbyak::Label no_jump;

			std::ignore = cmp_ex(op_rs, op_rt, eax);
			jl(no_jump, T_SHORT);															 // jump past branch if < 0

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, address);
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZALC)) // branch rt > 0 and link
	{
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;
		const uint32 link_address = address + 4;

		auto&& op_rt = get_register_op32(rt);

		set(op_r31, link_address);	// set link

		// instruction only valid if rt != 0
		if (!rt.is_zero())
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jle(no_jump, T_SHORT);															 // jump past branch

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP07))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + offset;

		if (rs == rt && !rs.is_zero()) // BLTZALC - branch rt < 0 and link
		{
			const uint32 link_address = address + 4;
			set(op_r31, link_address);	// set link

			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jge(no_jump, T_SHORT);															

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (rs != rt && !rs.is_zero() && !rt.is_zero()) // BLTUC - branch rs < rt
		{
			const Xbyak::Label no_jump;

			std::ignore = cmp_ex(op_rs, op_rt, eax);
			jge(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP10))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + offset;

		if (rs.is_zero() && rs < rt) // BEQZALC - branch rt == 0 and link
		{
			const uint32 link_address = address + 4;
			set(op_r31, link_address);	// set link

			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0);
			jne(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (!rs.is_zero() && !rt.is_zero() && rs < rt) // BEQC - branch rt == rs
		{
			const Xbyak::Label no_jump;

			std::ignore = cmp_ex(op_rs, op_rt, eax);
			jne(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (rs >= rt) // BOVC - branch if rs + rt overflows (signed)
		{
			const Xbyak::Label no_jump;

			mov(eax, op_rs); // get [rs]
			add(eax, op_rt); // add [rs] and [rt]
			jnc(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP30))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + offset;

		if (rs.is_zero() && rs < rt) // BNEZALC - branch rt != 0 and link
		{
			const uint32 link_address = address + 4;
			set(op_r31, link_address);	// set link

			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			je(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (!rs.is_zero() && !rt.is_zero() && rs < rt) // BNEC - branch rt != rs
		{
			const Xbyak::Label no_jump;

			std::ignore = cmp_ex(op_rt, op_rs, eax);
			je(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (rs >= rt) // BNVC - branch if rs + rt not overflows (signed)
		{
			const Xbyak::Label no_jump;

			mov(eax, op_rs); // get [rs]
			add(eax, op_rt); // add [rs] and [rt]
			jc(no_jump, T_SHORT);															 

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLEZC)) // branch rt <= 0
	{
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		auto&& op_rt = get_register_op32(rt);

		// instruction only valid if rt != 0
		if (!rt.is_zero())
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jg(no_jump, T_SHORT);															 // jump past branch

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP26))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + offset;

		if (!rs.is_zero() && !rt.is_zero() && rs == rt) // BGEZC - branch [rt] >= 0
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jl(no_jump, T_SHORT);

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (!rs.is_zero() && !rt.is_zero() && rs != rt) // BGEC / BLEC - branch [rs] >= [rt]
		{
			const Xbyak::Label no_jump;

			std::ignore = cmp_ex(op_rs, op_rt, eax);
			jl(no_jump, T_SHORT);

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZC)) // branch rt > 0
	{
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		auto&& op_rt = get_register_op32(rt);

		// instruction only valid if rt != 0
		if (!rt.is_zero())
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jle(no_jump, T_SHORT);															// jump past branch

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP27))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + offset;

		if (!rs.is_zero() && !rt.is_zero() && rs == rt) // BLTZC - branch [rt] < 0
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rt, 0); // compare [rt] to 0
			jge(no_jump, T_SHORT);

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else if (!rs.is_zero() && !rt.is_zero() && rs != rt) // BLTC / BGTC - branch [rs] < [rt]
		{
			const Xbyak::Label no_jump;

			if (op_rs.isREG() || op_rt.isREG())
			{
				cmp(op_rs, op_rt);
			}
			else
			{
				mov(eax, op_rs); // get [rs]
				cmp(eax, op_rt); // compare [rs] and [rt]
			}
			jge(no_jump, T_SHORT);

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BEQZC)) // branch [rs] == 0
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<23>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		auto&& op_rs = get_register_op32(rs);

		// instruction only valid if rt != 0
		if (!rs.is_zero())
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rs, 0); // compare [rs] to 0
			jne(no_jump, T_SHORT);															// jump past branch

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BNEZC)) // branch [rs] != 0
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<23>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		auto&& op_rs = get_register_op32(rs);

		// instruction only valid if rt != 0
		if (!rs.is_zero())
		{
			const Xbyak::Label no_jump;

			cmp_ex(op_rs, 0); // compare [rs] to 0
			je(no_jump, T_SHORT);															 // jump past branch

			emit_near_far_jump(target_address);

			L(no_jump);
			disable_cti();
		}
		else
		{
			set(ecx, int32(address));
			jmp(intrinsics_.ri, T_NEAR);
			exception_result = except_result::always_throw;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JIC)) // branch [rt] + offset
	{
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<16>(instruction).sextend<int32>();

		auto&& op_rt = get_register_op32(rt);

		const Xbyak::Label not_within;

		inc(rdi);
		if (op_rt.isREG())
		{
			lea(eax, dword[op_rt.as_reg() + offset]);
		}
		else
		{
			mov(eax, op_rt);
			add(eax, offset);
		}

		mov(ecx, eax);
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
			mov(rcx, intrinsics_.chunk_start);
			add(rax, rcx);
		}
		
		jmp(rax);
		L(not_within);

		mov(rdx, rax);
		mov(rax, std::bit_cast<uintptr>(&jit1::get_instruction));
		mov(rcx, uintptr(&jit_));
		call(rax);
		
		jmp(rax);
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JIALC)) // branch [rt] + offset and link
	{
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 offset = instructions::TinyInt<16>(instruction).sextend<int32>();
		const uint32 link_address = address + 4;

		auto&& op_rt = get_register_op32(rt);

		set(op_r31, link_address);	// set link

		const Xbyak::Label not_within;

		inc(rdi);
		if (op_rt.isREG())
		{
			lea(eax, dword[op_rt.as_reg() + offset]);
		}
		else
		{
			mov(eax, op_rt);
			add(eax, offset);
		}

		mov(ecx, eax);
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
			mov(rcx, intrinsics_.chunk_start);
			add(rax, rcx);
		}
		
		jmp(rax);
		L(not_within);

		mov(rdx, rax);
		mov(rax, std::bit_cast<uintptr>(&jit1::get_instruction));
		mov(rcx, uintptr(&jit_));
		call(rax);
		
		jmp(rax);
	}
	// default
	else
	{
		//terminate_instruction = true;
		insert_procedure_ecx(address, uintptr(instruction_info.Proc), instruction, instruction_info);
		exception_result = except_result::can_except;
		return { true, exception_result };
	}
	return { false, exception_result };
}
