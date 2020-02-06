#include "pch.hpp"

#include "jit1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "../../processor.hpp"
#include <cassert>
#include "codegen.hpp"

using namespace mips;

extern jit1::jit_instructionexec_t jit1_get_instruction(jit1 * __restrict _this, uint32 address);
extern jit1::jit_instructionexec_t jit1_fetch_instruction(jit1* __restrict _this, uint32 address);
static constexpr uint32 MaxShortJumpLookAhead = 2;

#define IS_INSTRUCTION(instr, ref) \
	[&]() -> bool { \
		extern const mips::instructions::InstructionInfo StaticProc_ ## ref; \
		return &StaticProc_ ## ref == &instr; \
	}()

bool Jit1_CodeGen::write_compact_branch(jit1::Chunk & __restrict chunk, bool &terminate_instruction, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
	const uint32 chunk_begin = address & ~(jit1::ChunkSize - 1);
	const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
	const uint32 next_chunk = chunk_last + 1;
	const uint32 this_offset = (address - chunk_begin) / 4u;

	static const int8 flags_offset = value_assert<int8>(offset_of(&processor::m_flags) - 128);
	static const int8 dbt_offset =  value_assert<int8>(offset_of(&processor::m_branch_target) - 128);
	static const int8 pc_offset = value_assert<int8>(offset_of(&processor::m_program_counter) - 128);
	static const int8 ic_offset = value_assert<int8>(offset_of(&processor::m_instruction_count) - 128);
	static const int8 gp_offset = value_assert<int8>(offset_of(&processor::m_registers) - 128);
	static const int8 r31 = gp_offset + (31 * 4);

	const auto patch_preprolog = [&](auto address) -> std::string
	{
		// If execution gets past the chunk, we jump to the next chunk.
		// Start with a set of nops so that we have somewhere to write patch code.
		const std::string patch = GetUniqueLabel();
		L(patch); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
		chunk.m_patches.push_back({ uint32(getSize()), 0 });
		auto &patch_pair = chunk.m_patches.back();
		uint32 &patch_target = patch_pair.target;

		// patch no-op
			if (address == nullptr) {
				for (uint8 octet : { 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x00 }) db(octet);
			}
			else {
				static constexpr uint16 patch_prefix = 0xB848;
				static constexpr uint16 patch_suffix = 0xE0FF;
				dw(patch_prefix);
				dq(uint64(address));
				dw(patch_suffix);
			}

		mov(rcx, int64(&patch_target));
		mov(dword[rcx], edx);

		return patch;
	};

	const auto patch_prolog = [&]()
	{
		auto &patch_pair = chunk.m_patches.back();
		uint32 &patch_target = patch_pair.target;
		mov(rcx, int64(&patch_target));
		mov(dword[rcx], edx);
	};

	const auto patch_epilog = [&](const std::string &patch)
	{
		static constexpr uint16 patch_prefix = 0xB848;
		static constexpr uint16 patch_suffix = 0xE0FF;

		mov(rcx, patch.c_str());
		mov(word[rcx], int16_t(patch_prefix));
		mov(qword[rcx + 2], rax);
		mov(word[rcx + 10], int16_t(patch_suffix));
	};

	const auto safejmp = [&](const std::string &target_label, uint32 instruction_offset)
	{
		if ((instruction_offset <= this_offset && (chunk_offset[this_offset] - chunk_offset[instruction_offset]) <= 128) || (instruction_offset - this_offset) <= MaxShortJumpLookAhead)
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
		 const auto currentAddress = m_jit.fetch_instruction(target_address);

		// In this case, we need to find the address in order to jump to it.
		inc(rdi);
		const auto patch = patch_preprolog(currentAddress);
		mov(eax, target_address);
		patch_prolog();
		mov(dword[rbp + pc_offset], eax);
		mov(edx, eax);
		mov(rax, uint64(jit1_get_instruction));
		mov(rcx, uint64(&m_jit));
		call(rax);
		patch_epilog(patch);
		jmp(rax);
	};

	 if (IS_INSTRUCTION(instruction_info, PROC_BALC))
	{
		const int32 immediate = instructions::TinyInt<28>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + immediate;
		const uint32 link_address = address + 4;
		mov(dword[rbp + r31], link_address);	// set link

		// destination is in this chunk. This is far easier to handle.
		if (target_address >= chunk_begin && target_address <= chunk_last)
		{
			const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
			const auto target_label = GetIndexLabel(instruction_offset);
			mov(dword[rbp + pc_offset], target_address);
			inc(rdi);
			safejmp(target_label, instruction_offset);
		}
		// destination is outside of this chunk. This is more difficult to handle.
		else
		{
			// In this case, we need to find the address in order to jump to it.
			patch_jump(target_address);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BC))
	{
		const int32 immediate = instructions::TinyInt<28>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + immediate;
		const uint32 link_address = address + 4;

		// destination is in this chunk. This is far easier to handle.
		if (target_address >= chunk_begin && target_address <= chunk_last)
		{
			const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
			const auto target_label = GetIndexLabel(instruction_offset);
			mov(dword[rbp + pc_offset], target_address);
			inc(rdi);
			safejmp(target_label, instruction_offset);
		}
		// destination is outside of this chunk. This is more difficult to handle.
		else
		{
			// In this case, we need to find the address in order to jump to it.
			patch_jump(target_address);
		}
	}
	 else if (IS_INSTRUCTION(instruction_info, PROC_BLEZALC)) // branch rt <= 0 and link
	{
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;
		const uint32 link_address = address + 4;
		mov(dword[rbp + r31], link_address);	// set link

		// instruction only valid if rt != 0
		if (rt.get_register() != 0)
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jg(no_jump);															 // jump past branch

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP06))
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		if (rs == rt && rs.get_register() != 0) // BGEZALC - branch rt >= 0 and link
		{
			const uint32 link_address = address + 4;
			mov(dword[rbp + r31], link_address);	// set link

			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jl(no_jump);															 // jump past branch if < 0

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs != rt && rs.get_register() != 0 && rt.get_register() != 0) // BGEUC - branch rs >= rt
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rt)); // get [rt]
			cmp(get_register_op32(rs), eax); // compare [rs] to [rt]
			jl(no_jump);															 // jump past branch if < 0

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZALC)) // branch rt > 0 and link
	{
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;
		const uint32 link_address = address + 4;
		mov(dword[rbp + r31], link_address);	// set link

		// instruction only valid if rt != 0
		if (rt.get_register() != 0)
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jle(no_jump);															 // jump past branch

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP07))
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		if (rs == rt && rs.get_register() != 0) // BLTZALC - branch rt < 0 and link
		{
			const uint32 link_address = address + 4;
			mov(dword[rbp + r31], link_address);	// set link

			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jge(no_jump);															

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs != rt && rs.get_register() != 0 && rt.get_register() != 0) // BLTUC - branch rs < rt
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rt)); // get [rt]
			cmp(get_register_op32(rs), eax); // compare [rs] to [rt]
			jge(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP10))
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		if (rs.get_register() == 0 && rs.get_register() < rt.get_register()) // BEQZALC - branch rt == 0 and link
		{
			const uint32 link_address = address + 4;
			mov(dword[rbp + r31], link_address);	// set link

			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0);
			jne(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() < rt.get_register()) // BEQC - branch rt == rs
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rt)); // get [rt]
			cmp(get_register_op32(rs), eax); // compare [rs] to [rt]
			jne(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs.get_register() >= rt.get_register()) // BOVC - branch if rs + rt overflows (signed)
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rs)); // get [rs]
			add(eax, get_register_op32(rt)); // add [rs] and [rt]
			jno(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP30))
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		if (rs.get_register() == 0 && rs.get_register() < rt.get_register()) // BNEZALC - branch rt != 0 and link
		{
			const uint32 link_address = address + 4;
			mov(dword[rbp + r31], link_address);	// set link

			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			je(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() < rt.get_register()) // BNEC - branch rt != rs
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rt)); // get [rt]
			cmp(get_register_op32(rs), eax); // compare [rs] to [rt]
			je(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs.get_register() >= rt.get_register()) // BNVC - branch if rs + rt not overflows (signed)
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rs)); // get [rs]
			add(eax, get_register_op32(rt)); // add [rs] and [rt]
			jo(no_jump);															 

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BLEZC)) // branch rt <= 0
	{
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		// instruction only valid if rt != 0
		if (rt.get_register() != 0)
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jg(no_jump);															 // jump past branch

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP26))
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() == rt.get_register()) // BGEZC - branch [rt] >= 0
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jl(no_jump);

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() != rt.get_register()) // BGEC / BLEC - branch [rs] >= [rt]
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rs)); // get [rs]
			cmp(eax, get_register_op32(rt)); // compare [rs] and [rt]
			jl(no_jump);

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BGTZC)) // branch rt > 0
	{
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		// instruction only valid if rt != 0
		if (rt.get_register() != 0)
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jle(no_jump);															// jump past branch

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_POP27))
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

		const uint32 target_address = address + 4 + offset;

		if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() == rt.get_register()) // BLTZC - branch [rt] < 0
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rt), 0); // compare [rt] to 0
			jge(no_jump);

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() != rt.get_register()) // BLTC / BGTC - branch [rs] < [rt]
		{
			const auto no_jump = GetUniqueLabel();

			mov(eax, get_register_op32(rs)); // get [rs]
			cmp(eax, get_register_op32(rt)); // compare [rs] and [rt]
			jge(no_jump);

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BEQZC)) // branch [rs] == 0
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<23>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		// instruction only valid if rt != 0
		if (rs.get_register() != 0)
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rs), 0); // compare [rs] to 0
			jne(no_jump);															// jump past branch

			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
				mov(dword[rbp + pc_offset], target_address);
				inc(rdi);
				safejmp(target_label, instruction_offset);
			}
			// destination is outside of this chunk. This is more difficult to handle.
			else
			{
				// In this case, we need to find the address in order to jump to it.
				patch_jump(target_address);
			}

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_BNEZC)) // branch [rs] != 0
	{
		const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<23>(instruction << 2).sextend<int32>();
		const uint32 target_address = address + 4 + offset;

		// instruction only valid if rt != 0
		if (rs.get_register() != 0)
		{
			const auto no_jump = GetUniqueLabel();

			cmp(get_register_op32(rs), 0); // compare [rs] to 0
			je(no_jump);															 // jump past branch

			// near branch
			// destination is in this chunk. This is far easier to handle.
			if (target_address >= chunk_begin && target_address <= chunk_last)
			{
				const uint32 instruction_offset = (target_address - chunk_begin) / 4u;
				const auto target_label = GetIndexLabel(instruction_offset);
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

			L(no_jump);
				 if (!m_jit.m_processor.m_disable_cti)
					or_(ebx, processor::flag_bit(processor::flag::no_cti));
		}
		else
		{
			terminate_instruction = true;
			mov(eax, int32(address));
			jmp("intrinsic_ri_ex", T_NEAR);
		}
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JIC)) // branch [rt] + offset
	{
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<16>(instruction).sextend<int32>();

		const auto not_within = GetUniqueLabel();

		inc(rdi);
		mov(eax, get_register_op32(rt));
		add(eax, offset);

		mov(ecx, eax);
		and_(ecx, ~(jit1::ChunkSize - 1));
		cmp(ecx, chunk_begin);
		jne(not_within);
		and_(eax, (jit1::ChunkSize - 1));

		mov(rcx, uint64(chunk_offset.data()));
		mov(eax, dword[rcx + rax]);
		mov(rcx, "chunk_start");
		add(rax, rcx);
		
		jmp(rax);
		L(not_within);

		mov(rdx, rax);
		mov(rax, uint64(jit1_get_instruction));
		mov(rcx, uint64(&m_jit));
		call(rax);
		
		jmp(rax);
	}
	else if (IS_INSTRUCTION(instruction_info, PROC_JIALC)) // branch [rt] + offset and link
	{
		const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
		const int32 offset = instructions::TinyInt<16>(instruction).sextend<int32>();
		const uint32 link_address = address + 4;
		mov(dword[rbp + r31], link_address);	// set link

		const auto not_within = GetUniqueLabel();

		inc(rdi);
		mov(eax, get_register_op32(rt));
		add(eax, offset);

		mov(ecx, eax);
		and_(ecx, ~(jit1::ChunkSize - 1));
		cmp(ecx, chunk_begin);
		jne(not_within);
		and_(eax, (jit1::ChunkSize - 1));

		mov(rcx, uint64(chunk_offset.data()));
		mov(eax, dword[rcx + rax]);
		mov(rcx, "chunk_start");
		add(rax, rcx);
		
		jmp(rax);
		L(not_within);

		mov(rdx, rax);
		mov(rax, uint64(jit1_get_instruction));
		mov(rcx, uint64(&m_jit));
		call(rax);
		
		jmp(rax);
	}
	// default
	else
	{
		//terminate_instruction = true;
		insert_procedure_ecx(address, uint64(instruction_info.Proc), instruction, instruction_info);
		return true;
	}
	return false;
}
