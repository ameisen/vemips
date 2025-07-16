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

	static const int8 flags_offset = value_assert<int8>(offsetof(processor, flags_) - 128);
	static const int8 dbt_offset =  value_assert<int8>(offsetof(processor, branch_target_) - 128);
	static const int8 pc_offset = value_assert<int8>(offsetof(processor, program_counter_) - 128);
	static const int8 ic_offset = value_assert<int8>(offsetof(processor, instruction_count_) - 128);
	const instructions::GPRegister<> r31 = {31U};

	auto&& op_r31 = get_register_op32(r31);

	const Xbyak::Label no_jump;

	const auto set_cti_flag = [&]
	{
		if (!jit_.processor_.disable_cti_) {
			or_(ebx, processor::flag::no_cti);
		}
	};

	const auto set_cti_delay_flag = [&]
	{
		if (!jit_.processor_.disable_cti_) {
			or_(ebx, processor::flag::no_cti | processor::flag::branch_delay);
		}
		else
		{
			or_(ebx, processor::flag::branch_delay);
		}
	};

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
		set_cti_flag();
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
		set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BAL))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + immediate;
		const uint32 link_address = address + 8;

		set(op_r31, link_address);
		set(esi, target_address);
		set_cti_delay_flag();
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
			set_cti_delay_flag();
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
			set_cti_flag();
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
			set_cti_delay_flag();
		}
		else
		{
			cmp_ex(op_rs, 0);
			jl(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);
			set_cti_flag();
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

		set_cti_flag();
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
			set_cti_delay_flag();
		}
		else
		{
			cmp_ex(op_rs, 0);
			jg(no_jump);
			set(esi, target_address);
			or_(ebx, processor::flag::branch_delay);
			L(no_jump);

			set_cti_flag();
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

		set_cti_flag();
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
		
		set_cti_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_J))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();

		const uint32 target_address = (address & instructions::HighBits(4)) | instr_index;

		set(esi, target_address);
		set_cti_delay_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JAL))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();

		const uint32 target_address = (address & instructions::HighBits(4)) | instr_index;

		const uint32 link_address = address + 8;

		set(op_r31, link_address);
		set(esi, target_address);
		set_cti_delay_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JALR))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
		const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

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
		set_cti_delay_flag();
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JR))
	{
		const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);

		auto&& op_rs = get_register_op32(rs);

		if (rs.is_zero()) [[unlikely]]
		{
			set(esi, 0);
		}
		else
		{
			mov(esi, op_rs);
		}
		set_cti_delay_flag();
	}
	else
	{
		xwarn(false, "delay branch implementation missing");
		insert_procedure_ecx(address, uint64(instruction_info.Proc), instruction, instruction_info);
		return except_result::can_except;
	}

	return except_result::none;
}

// enum class branch_type : uint32
// {
//	 near_branch = 0,		  // Branches within this chunk			
//	 far_branch,				 // Branches outside this chunk
//	 indeterminate			  // Branches to an unknown location
// };
void Jit1_CodeGen::handle_delay_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	branch_type branch_type;
	uint32 target_address = 0;

	const uint32 this_address = address + 4;

	const uint32 chunk_begin = this_address & ~(jit1::ChunkSize - 1);
	const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
	const uint32 next_chunk = chunk_last + 1;
	const uint32 this_offset = (this_address - chunk_begin) / 4u;

	static const int8 flags_offset = value_assert<int8>(offsetof(processor, flags_) - 128);
	static const int8 dbt_offset =  value_assert<int8>(offsetof(processor, branch_target_) - 128);
	static const int8 pc_offset = value_assert<int8>(offsetof(processor, program_counter_) - 128);
	static const int8 ic_offset = value_assert<int8>(offsetof(processor, instruction_count_) - 128);

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

	const auto patch_preprolog = [&](auto address) -> Xbyak::Label
	{
		// If execution gets past the chunk, we jump to the next chunk.
		// Start with a set of nops so that we have somewhere to write patch code.
		const auto patch = L(); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
		auto & [offset, target] = chunk.m_patches->emplace_back(uint32(getSize()), 0);
		uint32 &patch_target = target;

		// patch no-op
		if (address == nullptr) {
			nop(12, true);
		}
		else {
			static constexpr uint16 patch_prefix = 0xB848;
			static constexpr uint16 patch_suffix = 0xE0FF;
			dw(patch_prefix);
			dq(uint64(address));
			dw(patch_suffix);
		}

		write_edx_to_patch_target(patch_target);

		return patch;
	};

	const auto patch_prolog = [&]()
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

	const auto safe_jmp = [&](const Xbyak::Label &target_label, const uint32 instruction_offset)
	{
		const LabelType label_type =
			((instruction_offset <= this_offset && (chunk_offset[this_offset] - chunk_offset[instruction_offset]) <= 128) || (instruction_offset - this_offset) <= MaxShortJumpLookAhead) ?
			T_AUTO :
			T_NEAR;

		jmp(target_label, label_type);
	};

	if (IS_INSTRUCTION(instruction_info, COP1_BC1EQZ_v))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();
		target_address = address + 4 + offset;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, COP1_BC1NEZ_v))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();
		target_address = address + 4 + offset;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BAL))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + immediate;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BEQ))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + immediate;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGEZ))
	{
		const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + immediate;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZ))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLEZ))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLTZ))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BNE))
	{
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		target_address = address + 4 + offset;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_J))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();
		target_address =  (address & instructions::HighBits(4)) | instr_index;;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JAL))
	{
		const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();
		target_address =  (address & instructions::HighBits(4)) | instr_index;;

		if (chunk_begin <= target_address && chunk_last >= target_address)
		{
			branch_type = branch_type::near_branch;
		}
		else
		{
			branch_type = branch_type::far_branch;
		}
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

			if (chunk_begin <= target_address && chunk_last >= target_address)
			{
				branch_type = branch_type::near_branch;
			}
			else
			{
				branch_type = branch_type::far_branch;
			}
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

			if (chunk_begin <= target_address && chunk_last >= target_address)
			{
				branch_type = branch_type::near_branch;
			}
			else
			{
				branch_type = branch_type::far_branch;
			}
		}
	}
	else
	{
		xwarn(false, "delay branch implementation missing");
		branch_type = branch_type::indeterminate_unhandled;
	}

	switch (branch_type)
	{
		case branch_type::near_branch:						// Branches within this chunk
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			je(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			// what is the offset of the target address?
			const uint32 target_offset = (target_address - chunk_begin) / 4u;
			const auto& target_label = get_instruction_offset_label(target_offset);
			xor_(esi, esi);
			safe_jmp(target_label, target_offset);
			L(no_branch);
		} break;
		case branch_type::far_branch:						 // Branches outside this chunk
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			je(no_branch);
			xor_(esi, esi);
			and_(ebx, ~processor::flag::branch_delay);
			const auto patch = patch_preprolog(jit_.fetch_instruction(target_address));

			set(edx, target_address);

			patch_prolog();

			set(rax, std::bit_cast<uintptr>(&jit1::get_instruction));
			set(rcx, uintptr(&jit_));
			call(rax);
			patch_epilog(patch);
			jmp(rax);

			L(no_branch);
		} break;
		case branch_type::indeterminate:					 // Branches to an unknown location
		{
			const Xbyak::Label no_branch;
			const Xbyak::Label not_within;

			test(ebx, processor::flag::branch_delay);
			je(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			mov(eax, esi);
			xor_(esi, esi);

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
				mov(rcx, intrinsics_.chunk_start);
				add(rax, rcx);
			}
			jmp(rax);
			L(not_within);
			mov(rdx, rax);
			set(rax, std::bit_cast<uintptr>(&jit1::get_instruction));
			set(rcx, uintptr(&jit_));
			call(rax);
			jmp(rax);
			L(no_branch);
		} break;
		case branch_type::near_branch_unhandled:		  // Branches within this chunk, use pc state	
		{
			const Xbyak::Label no_branch;
			test(ebx, processor::flag::branch_delay);
			jz(no_branch);
			and_(ebx, ~processor::flag::branch_delay);
			mov(dword[rbp + pc_offset], target_address);

			// what is the offset of the target address?
			const uint32 target_offset = (target_address - chunk_begin) / 4u;
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
			mov(edx, target_address);
			mov(dword[rbp + pc_offset], edx);

			mov(rax, std::bit_cast<uintptr>(&jit1::get_instruction));
			mov(rcx, uintptr(&jit_));
			call(rax);
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
			mov(eax, dword[rbp + dbt_offset]);
			mov(dword[rbp + pc_offset], eax);

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
			L(no_branch);
		} break;
	}
}
