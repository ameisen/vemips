#include "pch.hpp"

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

static constexpr uint32 MaxShortJumpLookAhead = 2;

Jit1_CodeGen::except_result Jit1_CodeGen::write_delay_branch(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const uint32 this_address = address;

	const uint32 chunk_begin = this_address & ~(jit1::ChunkSize - 1);
	const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
	const uint32 next_chunk = chunk_last + 1;
	const uint32 this_offset = (this_address - chunk_begin) / 4u;

	const instructions::GPRegister<> r31 = {31U};

	auto&& op_r31 = get_register_op32(r31);

	const Xbyak::Label no_jump;

	if (IS_INSTRUCTION(instruction_info, COP1_BC1EQZ_v))
	{
		const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
		const int16 ft_offset = ft.get_offset();

		const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		test(dword[r12 + ft_offset], 1); // ZF set to 1 if [ft] & 1 == 0
		jnz(no_jump);
		set(esi, target_address);
		or_(ebx, processor::flag::branch_delay);
		L(no_jump);
		intrinsic_set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, COP1_BC1NEZ_v))
	{
		const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
		const int16 ft_offset = ft.get_offset();

		const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();

		const uint32 target_address = address + 4 + offset;
		test(dword[r12 + ft_offset], 1); // ZF set to 1 if [ft] & 1 == 0
		jz(no_jump);
		set(esi, target_address);
		or_(ebx, processor::flag::branch_delay);
		L(no_jump);
		intrinsic_set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BAL))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;
		const uint32 link_address = address + 8;

		set(op_r31, link_address);
		set(esi, target_address);
		intrinsic_set_delay_branch();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BEQ))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		const uint32 target_address = address + 4 + immediate;

		if (rs == rt)
		{
			set(esi, target_address); // 0 == 0
			intrinsic_set_delay_branch();
		}
		else
		{
			if (rs.is_zero())
			{
				cmp_ex(op_rt, 0);
			}
			else if (rt.is_zero())
			{
				cmp_ex(op_rs, 0);
			}
			else
			{
				std::ignore = cmp_ex(op_rs, op_rt, eax);
			}
			jne(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);
			intrinsic_set_cti_flag();
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGEZ))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;

		auto&& op_rs = get_register_op32(rs);

		if (rs.is_zero()) // 0 >= 0
		{
			set(esi, target_address);
			intrinsic_set_delay_branch();
		}
		else
		{
			cmp_ex(op_rs, 0);
			jl(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);
			intrinsic_set_cti_flag();
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZ))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;

		auto&& op_rs = get_register_op32(rs);

		if (rs.is_zero()) // 0 !> 0
		{
			//nop
		}
		else
		{
			cmp_ex(op_rs, 0);
			jle(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);
		}

		intrinsic_set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLEZ))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;

		auto&& op_rs = get_register_op32(rs);

		if (rs.is_zero()) // 0 <= 0
		{
			set(esi, target_address);
			intrinsic_set_delay_branch();
		}
		else
		{
			cmp_ex(op_rs, 0);
			jg(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);

			intrinsic_set_cti_flag();
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLTZ))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;
		
		auto&& op_rs = get_register_op32(rs);

		if (rs.is_zero()) // 0 !< 0
		{
			//nop
		}
		else
		{
			cmp_ex(op_rs, 0);
			jge(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);
		}

		intrinsic_set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BNE))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rt = get_register_op32(rt);

		if (rs == rt)
		{
			//nop
		}
		else
		{
			if (rs.is_zero())
			{
				cmp_ex(op_rt, 0);
			}
			else if (rt.is_zero())
			{
				cmp_ex(op_rs, 0);
			}
			else
			{
				std::ignore = cmp_ex(op_rs, op_rt, eax);
			}
			je(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);
		}
		
		intrinsic_set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_J))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();

		const uint32 target_address = (address & instructions::HighBits(4)) | instr_index;

		set(esi, target_address);
		intrinsic_set_delay_branch();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JAL))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();

		const uint32 target_address = (address & instructions::HighBits(4)) | instr_index;

		const uint32 link_address = address + 8;

		set(op_r31, link_address);
		set(esi, target_address);
		intrinsic_set_delay_branch();
	}
	else if (auto&& jalr = IS_INSTRUCTION_HB(instruction_info, PROC_JALR))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

		const uint8 hint = instructions::TinyInt<5>(instruction >> 6).zextend<uint8>();

		const uint32 link_address = address + 8;

		auto&& op_rs = get_register_op32(rs);
		auto&& op_rd = get_register_op32(rd);

		if (rs.is_zero()) [[unlikely]]
		{
			set(esi, 0);
		}
		else
		{
			mov(esi, op_rs);
		}
		if (!rd.is_zero()) [[likely]]
		{
			set(op_rd, link_address);
		}

		xassert(jalr.is_hazard_barrier == (hint == 0b10000U));

		intrinsic_set_delay_branch(jalr.is_hazard_barrier);
	}
	else if (auto&& jr = IS_INSTRUCTION_HB(instruction_info, PROC_JR))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);

		const uint8 hint = instructions::TinyInt<5>(instruction >> 6).zextend<uint8>();

		auto&& op_rs = get_register_op32(rs);

		if (rs.is_zero()) [[unlikely]]
		{
			set(esi, 0);
		}
		else
		{
			mov(esi, op_rs);
		}

		xassert(jr.is_hazard_barrier == (hint == 0b10000U));

		intrinsic_set_delay_branch(jr.is_hazard_barrier);
	}
	else
	{
		xwarn(false, "delay branch implementation missing");
		return insert_procedure_check_hazard(address, instruction_info, instruction);
	}

	return except_result::none;
}

// enum class branch_type : uint32
// {
//	 near_branch = 0,		  // Branches within this chunk			
//	 far_branch,				 // Branches outside this chunk
//	 indeterminate			  // Branches to an unknown location
// };
void Jit1_CodeGen::handle_delay_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const bool hazard)
{
	branch_type branch_type = branch_type::none;
	std::optional<uint32> target_address;

	const uint32 this_address = address + 4;

	const uint32 chunk_begin = this_address & ~(jit1::ChunkSize - 1);
	const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
	const uint32 next_chunk = chunk_last + 1;
	const uint32 this_offset = (this_address - chunk_begin) / 4u;

	const auto safe_jmp = [&](const Xbyak::Label &target_label, const uint32 instruction_offset)
	{
		const bool can_be_short =
			(
				instruction_offset <= this_offset &&
				(chunk_offset[this_offset] - chunk_offset[instruction_offset]) <= 128
			) ||
			(instruction_offset - this_offset) <= MaxShortJumpLookAhead;

		const LabelType label_type =
			can_be_short ?
				T_AUTO :
				T_NEAR;

		jmp(target_label, label_type);
	};

	if (IS_INSTRUCTION(instruction_info, COP1_BC1EQZ_v))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();
		target_address = address + 4 + offset;
	}
	else if (IS_INSTRUCTION(instruction_info, COP1_BC1NEZ_v))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();
		target_address = address + 4 + offset;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BAL))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + immediate;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BEQ))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + immediate;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGEZ))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + immediate;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZ))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLEZ))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLTZ))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BNE))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_J))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();
		target_address =  (address & instructions::HighBits(4)) | instr_index;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JAL))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();
		target_address =  (address & instructions::HighBits(4)) | instr_index;
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JALR))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);

		if (!rs.is_zero())
		{
			branch_type = branch_type::indeterminate;
		}
		else
		{
			target_address = 0;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JR))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);

		if (!rs.is_zero())
		{
			branch_type = branch_type::indeterminate;
		}
		else
		{
			target_address = 0;
		}
	}
	else
	{
		xwarn(false, "delay branch implementation missing");
		branch_type = branch_type::indeterminate_unhandled;
	}

	if (branch_type == branch_type::none)
	{
		if (target_address.has_value())
		{
			if (chunk_begin <= target_address && chunk_last >= target_address)
			{
				branch_type = branch_type::near_branch;
			}
			else
			{
				branch_type = branch_type::far_branch;
			}
		}
		else
		{
			branch_type = branch_type::indeterminate;
		}
	}

	// In the case where the hazard flush doesn't clear the current chunk, it will continue execution as normal.
	// In the case where it does, it will change the program counter to our target.
	// TODO: This is actually slightly suboptimal for the `near_branch`-case, as it adds an additional `jmp`.
	const auto insert_hazard = [&] (const int32 target)
	{
		if (!hazard)
		{
			return;
		}
		const Xbyak::Label no_hazard;
		test(ebx, processor::flag::instruction_hazard);
		jz(no_hazard);
		//std::ignore = flush_pc(eax, instruction_offset);
		intrinsic_clear_instruction_hazards(this_offset, target);
		L(no_hazard);
	};

	const auto insert_hazard_reg = [&] (const Xbyak::Reg& target)
	{
		if (!hazard)
		{
			return;
		}
		const Xbyak::Label no_hazard;
		test(ebx, processor::flag::instruction_hazard);
		jz(no_hazard);
		//std::ignore = flush_pc(eax, instruction_offset);
		intrinsic_clear_instruction_hazards(this_offset, target);
		L(no_hazard);
	};

	switch (branch_type)
	{
		case branch_type::near_branch:						// Branches within this chunk
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			// what is the offset of the target address?
			const uint32 target_offset = (*target_address - chunk_begin) / 4u;
			const auto& target_label = get_instruction_offset_label(target_offset);
			xor_(esi, esi);
			insert_hazard(*target_address);
			safe_jmp(target_label, target_offset);
			L(no_branch);
		} break;
		case branch_type::far_branch:						 // Branches outside this chunk
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			xor_(esi, esi);
			and_(ebx, ~processor::flag::branch_delay);
			insert_hazard(*target_address);
			intrinsic_write_patch_jump(chunk, *target_address, edx, false);
			L(no_branch);
		} break;
		case branch_type::indeterminate:					 // Branches to an unknown location
		{
			const Xbyak::Label no_branch;
			const Xbyak::Label not_within;

			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			mov(eax, esi);
			xor_(esi, esi);
			insert_hazard_reg(eax);

			mov(ecx, eax);
			and_(ecx, ~(jit1::ChunkSize - 1));
			cmp(ecx, chunk_begin);
			jne(not_within);
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
			L(no_branch);
		} break;
		case branch_type::near_branch_unhandled:		  // Branches within this chunk, use pc state	
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			insert_hazard(*target_address);
			mov(dword[rbp + offsets.pc], *target_address);

			// what is the offset of the target address?
			const uint32 target_offset = (*target_address - chunk_begin) / 4u;
			const auto& target_label = get_instruction_offset_label(target_offset);

			safe_jmp(target_label, target_offset);
			L(no_branch);
		} break;
		case branch_type::far_branch_unhandled:			// Branches outside this chunk, use pc state
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			insert_hazard(*target_address);
			mov(edx, *target_address);
			mov(dword[rbp + offsets.pc], edx);

			mov(rcx, uintptr(&jit_));
			std::ignore = call_ex<true>(ptr_cast(&jit1::get_instruction), rax);
			jmp(rax);

			L(no_branch);
		} break;
		case branch_type::indeterminate_unhandled:		// Branches to an unknown location, use pc state
		{
			const Xbyak::Label no_branch;
			const Xbyak::Label not_within;

			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			mov(eax, dword[rbp + offsets.dbt]);
			insert_hazard_reg(eax);
			mov(dword[rbp + offsets.pc], eax);

			mov(ecx, eax);
			and_(ecx, ~(jit1::ChunkSize - 1));
			cmp(ecx, chunk_begin);
			jne(not_within);
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
			mov(rcx, uintptr(&jit_));
			std::ignore = call_ex<true>(ptr_cast(&jit1::get_instruction), rax);
			jmp(rax);
			L(no_branch);
		} break;
		case branch_type::none:
			xunreachable("none should not be reachable");
	}

	insert_hazard(this_offset + 4);
}
