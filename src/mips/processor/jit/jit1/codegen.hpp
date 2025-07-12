#pragma once

#include <variant>

#include "mips_common.hpp"
#include "instructions/instructions_common.hpp"
#include "mips/processor/jit/xbyak/xbyak.h"
#include "mips/processor/jit/jit1/jit1.hpp"

namespace mips
{
	class jit1;
	class Jit1_CodeGen final : public Xbyak::CodeGenerator
	{
		std::array<Xbyak::Label, jit1::NumInstructionsChunk> instruction_offset_labels_;
		struct {
			struct intrinsic final {
				Xbyak::Label label;
				mutable bool used = false;

				_nothrow operator Xbyak::Label&() noexcept {
					used = true;
					return label;
				}

				_nothrow operator const Xbyak::Label&() const noexcept {
					used = true;
					return label;
				}
			};

			intrinsic ri;
			intrinsic adel;
			intrinsic ades;
			intrinsic ov;
			intrinsic tr;
			intrinsic store_flush;
			intrinsic check_ex;
			intrinsic save_return_eax_pc;
			intrinsic save;
			intrinsic save_return;
			intrinsic stats;
			intrinsic emulated_stats;

			intrinsic code_start;
			intrinsic chunk_start;
			intrinsic intrinsic_start;
		} intrinsics_;
		jit1 & __restrict jit_;
		uint8* const address_;

	public:

		Jit1_CodeGen(jit1 & __restrict jit, uint8 *userptr, size_t usersz) : Xbyak::CodeGenerator(usersz, userptr), jit_(jit), address_(userptr) {}
		virtual ~Jit1_CodeGen() override = default;

		uint8* get_address() const {
			return address_;
		}

		uint8* get_current_address() const {
			return address_ + getSize();
		}

		void set(const Xbyak::Operand& dst, const uint64 imm)
		{
			if (!dst.isREG())
			{
				mov(dst, imm);
				return;
			}

			const auto& dest = [&]
			{
				if (dst.getBit() == 64 && imm <= std::numeric_limits<uint32>::max())
				{
					Xbyak::Operand result = dst;
					result.setBit(32);
					return result;
				}
				else
				{
					return dst;
				}
			}();

			if (imm == 0ULL)
			{
				xor_(dest, dest);
			}
			else
			{
				mov(dest, imm);
			}
		}

		[[nodiscard]]
		Xbyak::Label & get_instruction_offset_label(const uint32 offset) {
			xassert(offset < jit1::NumInstructionsChunk);
			return instruction_offset_labels_[offset];
		}

		static constexpr const uint32 mips_fp = 30;

		class LazyOperand
		{
		protected:
			mutable const Xbyak::Operand* operand_ = nullptr;

		protected:
			virtual _nothrow const Xbyak::Operand& get_value() const noexcept = 0;

		public:
			virtual _nothrow ~LazyOperand() noexcept = default;

		public:
			_nothrow operator const Xbyak::Operand&() const noexcept
			{
				if (!operand_)
				{
					operand_ = &get_value();
					xassert(operand_);
				}

				return *operand_;
			}
		};

		class LazyRegisterOperand final : public LazyOperand
		{
			Jit1_CodeGen& codegen_;
			instructions::GPRegisterInfo register_;
			const uint8_t size_;

		protected:
			_nothrow LazyRegisterOperand(
				Jit1_CodeGen& codegen,
				const instructions::GPRegisterInfo& _register,
				const uint8 size
			) noexcept
				: codegen_(codegen)
				, register_(_register)
				, size_(size)
			{}

			virtual _nothrow const Xbyak::Operand& get_value() const noexcept override
			{
				switch (size_)
				{
					case 0:
						return codegen_.get_register_op8(register_);
					case 1:
						return codegen_.get_register_op16(register_);
					case 2:
						return codegen_.get_register_op32(register_);
					case 3: [[unlikely]]
						return codegen_.get_register_op64(register_);
					default: [[unlikely]]
						xassert(false);
				}
			}

		public:
			template <usize Size>
			requires(Size == 8 || Size == 16 || Size == 32 || Size == 64)
			static _nothrow LazyRegisterOperand get(
				Jit1_CodeGen& codegen,
				const instructions::GPRegisterInfo& _register
			) noexcept
			{
				return { codegen, _register, log2_ceil(Size) - log2_ceil(8) };
			}
		};

		template <typename GPR>
		const Xbyak::Operand & get_register_op8(const GPR &reg) {
			const auto reg_offset = reg.get_offset();
		
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
		LazyRegisterOperand get_register_op8_lazy(const GPR &reg) {
			return LazyRegisterOperand::get<8>(*this, reg);
		}

		template <typename GPR>
		const Xbyak::Operand & get_register_op16(const GPR &reg) {
			const auto reg_offset = reg.get_offset();
		
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
		LazyRegisterOperand get_register_op16_lazy(const GPR &reg) {
			return LazyRegisterOperand::get<16>(*this, reg);
		}

		template <typename GPR>
		const Xbyak::Operand & get_register_op32(const GPR &reg) {
			const auto reg_offset = reg.get_offset();
		
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
		LazyRegisterOperand get_register_op32_lazy(const GPR &reg) {
			return LazyRegisterOperand::get<32>(*this, reg);
		}

		template <typename GPR>
		const Xbyak::Operand &get_register_op64(const GPR &reg) {
			const auto reg_offset = reg.get_offset();
		
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

		template <typename GPR>
		LazyRegisterOperand get_register_op64_lazy(const GPR &reg) {
			return LazyRegisterOperand::get<64>(*this, reg);
		}

		void write_chunk(jit1::ChunkOffset & __restrict chunk_offset, jit1::Chunk & __restrict chunk, uint32 start_address, bool update);

		void insert_procedure_ecx(uint32 address, uint64 procedure, uint32 _ecx, const mips::instructions::InstructionInfo & __restrict instruction_info);

		enum class except_result : uint32
		{
			none          = 0U,	     // neither throws nor sets exception
			can_throw     = 1U << 0, // may throw exception and return
			always_throw  = 1U << 1, // always throws exception and return
			can_except    = 1U << 2, // may set exception
			always_except = 1U << 3, // always set exception
		};

		void write_PROC_SUBU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_SUB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_OR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_NOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_AND(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MOVE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_ADDIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_ADDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_ADDU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_ADD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_AUI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_ORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_ANDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SELEQZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SELNEZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLTI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLTIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MUL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MUH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MULU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MUHU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_DIV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MOD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_DIVU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MODU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_XOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_XORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SEB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SEH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRA(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SYNC(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_INS(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_LSA(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);

		[[nodiscard]]
		except_result write_PROC_TEQ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TGE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TGEU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TNE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		[[nodiscard]]
		except_result write_PROC_SYSCALL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex);

		[[nodiscard]]
		except_result write_PROC_BREAK(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex);

		[[nodiscard]]
		except_result write_PROC_SIGRIE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex);

		// returns 'true' if it was unhandled
		bool write_STORE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		bool write_LOAD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		// FPU ops
		void write_COP1_MFC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_MTC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_MFHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_MTHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_SEL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		// returns 'true' if compact branch patch is needed.
		std::pair<bool, except_result> write_compact_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		// returns 'true' if it was unhandled.
		[[nodiscard]]
		except_result write_delay_branch(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		enum class branch_type : uint32 {
			near_branch = 0,				  // Branches within this chunk			
			far_branch,						 // Branches outside this chunk
			indeterminate,					 // Branches to an unknown location
			near_branch_unhandled,		  // Branches within this chunk, use pc state	
			far_branch_unhandled,			// Branches outside this chunk, use pc state
			indeterminate_unhandled		 // Branches to an unknown location, use pc state
		};
		void handle_delay_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		template <typename... Args>
		_forceinline _nothrow void db(uint8 arg, Args... args) {
			Xbyak::CodeGenerator::db(arg);
			(Xbyak::CodeGenerator::db(std::forward<Args>(args)), ...);
		}

		template <EnumC T>
		void or_(const Xbyak::Operand& op, T imm) { Xbyak::CodeGenerator::or_(op, std::underlying_type_t<T>(imm)); }

		using Xbyak::CodeGenerator::or_;

		template <EnumC T>
		void and_(const Xbyak::Operand& op, T imm) { Xbyak::CodeGenerator::and_(op, std::underlying_type_t<T>(imm)); }

		using Xbyak::CodeGenerator::and_;

		template <EnumC T>
		void test(const Xbyak::Operand& op, T imm) { Xbyak::CodeGenerator::test(op, std::underlying_type_t<T>(imm)); }

		using Xbyak::CodeGenerator::test;
	};

	static constexpr Jit1_CodeGen::except_result operator | (const Jit1_CodeGen::except_result a, const Jit1_CodeGen::except_result b)
	{
		return Jit1_CodeGen::except_result(std::to_underlying(a) | std::to_underlying(b));
	}

	static constexpr Jit1_CodeGen::except_result operator & (const Jit1_CodeGen::except_result a, const Jit1_CodeGen::except_result b)
	{
		return Jit1_CodeGen::except_result(std::to_underlying(a) & std::to_underlying(b));
	}
}