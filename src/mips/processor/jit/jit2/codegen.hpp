#pragma once

#include "mips_common.hpp"
#include "mips/processor/jit/xbyak/xbyak.h"
#include "mips/processor/jit/jit2/jit2.hpp"

namespace mips
{
	class jit2;
	class Jit2_CodeGen final : public Xbyak::CodeGenerator
	{
		jit2 & __restrict m_jit;
		uint32 unique_label_idx = 0;

	public:
		Jit2_CodeGen(jit2 & __restrict jit, uint8 *userptr, size_t usersz) : m_jit(jit), Xbyak::CodeGenerator(usersz, userptr) {}
		virtual ~Jit2_CodeGen() = default;

		std::string GetUniqueLabel () __restrict {
			char buffer[48];
			sprintf(buffer, "unique_%u", unique_label_idx++);
			return buffer;
		};

		std::string GetIndexLabel(uint32 index) const __restrict {
			assert(index < jit2::NumInstructionsChunk);
			char buffer[48];
			sprintf(buffer, "index_%u", index);
			return buffer;
		}

		static constexpr const uint32 mips_fp = 30;

		template <typename GPR>
		const Xbyak::Operand & get_register_op8(const GPR &reg) {
			static const auto gp_offset = value_assert<int8>(offset_of(&processor::m_registers) - 128);
		
			const auto reg_offset = value_assert<int8>(gp_offset + (4 * reg.get_register()));
		
			// otherwise we will type slice. That's bad.
			static thread_local Xbyak::Address AddrOperand = byte[rbp];
			static Xbyak::Reg8 RegOperand = r15b;
			if (reg.get_register() == mips_fp) {
				return RegOperand;
			}
			else {
				AddrOperand = byte[rbp + reg_offset];
				return AddrOperand;
			}
		}

		template <typename GPR>
		const Xbyak::Operand & get_register_op16(const GPR &reg) {
			static const auto gp_offset = value_assert<int8>(offset_of(&processor::m_registers) - 128);
		
			const auto reg_offset = value_assert<int8>(gp_offset + (4 * reg.get_register()));
		
			// otherwise we will type slice. That's bad.
			static thread_local Xbyak::Address AddrOperand = word[rbp];
			static Xbyak::Reg16 RegOperand = r15w;
			if (reg.get_register() == mips_fp) {
				return RegOperand;
			}
			else {
				AddrOperand = word[rbp + reg_offset];
				return AddrOperand;
			}
		}

		template <typename GPR>
		const Xbyak::Operand & get_register_op32(const GPR &reg) {
			static const auto gp_offset = value_assert<int8>(offset_of(&processor::m_registers) - 128);
		
			const auto reg_offset = value_assert<int8>(gp_offset + (4 * reg.get_register()));
		
			// otherwise we will type slice. That's bad.
			static thread_local Xbyak::Address AddrOperand = dword[rbp];
			static Xbyak::Reg32 RegOperand = r15d;
			if (reg.get_register() == mips_fp) {
				return RegOperand;
			}
			else {
				AddrOperand = dword[rbp + reg_offset];
				return AddrOperand;
			}
		}

		template <typename GPR>
		const Xbyak::Operand & get_register_op64(const GPR &reg) {
			static const auto gp_offset = value_assert<int8>(offset_of(&processor::m_registers) - 128);
		
			const auto reg_offset = value_assert<int8>(gp_offset + (4 * reg.get_register()));
		
			// otherwise we will type slice. That's bad.
			static thread_local Xbyak::Address AddrOperand = qword[rbp];
			static Xbyak::Reg64 RegOperand = r15;
			if (reg.get_register() == mips_fp) {
				return RegOperand;
			}
			else {
				AddrOperand = qword[rbp + reg_offset];
				return AddrOperand;
			}
		}

		void write_chunk(jit2::ChunkOffset & __restrict chunk_offset, jit2::Chunk & __restrict chunk, uint32 start_address, bool update) __restrict;

		void insert_procedure_ecx(uint32 address, uint64 procedure, uint32 _ecx, const mips::instructions::InstructionInfo & __restrict instruction_info);

		void write_PROC_SUBU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SUB(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_OR(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_NOR(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_AND(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MOVE(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_ADDIU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_ADDI(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_ADDU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_ADD(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_AUI(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_ORI(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_ANDI(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SELEQZ(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SELNEZ(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SLT(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SLTU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SLTI(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SLTIU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MUL(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MUH(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MULU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MUHU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_DIV(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MOD(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_DIVU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_MODU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_XOR(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_XORI(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SEB(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SEH(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SLL(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SRL(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SRA(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SLLV(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SRLV(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_RDHWR(jit2::ChunkOffset & __restrict chunk_offset, bool &terminate_instruction, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_SYNC(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
			void write_PROC_EXT(jit2::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info) __restrict;

		void write_PROC_TEQ(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_TGE(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_TGEU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_TLT(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_TLTU(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_PROC_TNE(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;

		void write_PROC_SYSCALL(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;


		// returns 'true' if it was unhandled
		bool write_STORE(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		bool write_LOAD(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;

		// FPU ops
		void write_COP1_MFC1(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_COP1_MTC1(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_COP1_MFHC1(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_COP1_MTHC1(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		void write_COP1_SEL(jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;

		// returns 'true' if compact branch patch is needed.
		bool write_compact_branch(jit2::Chunk & __restrict chunk, bool &terminate_instruction, jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;

		// returns 'true' if it was unhandled.
		bool write_delay_branch(bool &terminate_instruction, jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
		enum class branch_type : uint32 {
			near_branch = 0,				  // Branches within this chunk			
			far_branch,						 // Branches outside this chunk
			indeterminate,					 // Branches to an unknown location
			near_branch_unhandled,		  // Branches within this chunk, use pc state	
			far_branch_unhandled,			// Branches outside this chunk, use pc state
			indeterminate_unhandled		 // Branches to an unknown location, use pc state
		};
		void handle_delay_branch(jit2::Chunk & __restrict chunk, jit2::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict;
	};
}