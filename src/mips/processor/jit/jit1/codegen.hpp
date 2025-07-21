#pragma once

#include <variant>

#define XBYAK_STRICT_CHECK_MEM_REG_SIZE 0
#include <xbyak.h>
#undef XBYAK_STRICT_CHECK_MEM_REG_SIZE

#include "mips_common.hpp"
#include "instructions/instructions_common.hpp"
#include "mips/processor/jit/jit1/jit1.hpp"
#include "mips/processor/processor.hpp"

namespace mips
{
#ifdef XBYAK_NO_EXCEPTION
	static constexpr const bool xbyak_throws = false;
	#define _xbyak_nothrow _nothrow
#else
	static constexpr const bool xbyak_throws = true;
	#define _xbyak_nothrow
#endif

	class jit1;
	class Jit1_CodeGen final : public Xbyak::CodeGenerator
	{
		static constexpr const auto offsets = processor::recompiler_offsets<>::get<int8>();
		static constexpr const auto cop1_offsets = coprocessor1::recompiler_offsets<>::get<int16>();

		std::array<Xbyak::Label, jit1::NumInstructionsChunk> instruction_offset_labels_;
		struct {
			struct intrinsic final {
				Xbyak::Label label;
				mutable bool used = false;

				_nothrow operator const Xbyak::Label& () const noexcept {
					used = true;
					return label;
				}

				_nothrow operator Xbyak::Label& () noexcept {
					used = true;
					return label;
				}

				_nothrow const Xbyak::Label& get() const noexcept
				{
					used = true;
					return label;
				}

				_nothrow Xbyak::Label& get() noexcept
				{
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

		Jit1_CodeGen(jit1 & __restrict jit, uint8 * const userptr, const size_t usersz)
			: Xbyak::CodeGenerator(usersz, userptr)
			, jit_(jit)
			, address_(userptr)
		{}
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

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool mov_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp
		)
		{
			if (dst.isREG() || src.isREG())
			{
				mov(dst, src);
				return false;
			}

			mov(tmp, src);
			mov(dst, tmp);

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool mov_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp,
			const std::function<void(const Xbyak::Reg&)>& restore_tmp
		)
		{
			if (dst.isREG() || src.isREG())
			{
				mov(dst, src);
				return false;
			}

			spill_tmp(tmp);
			mov(tmp, src);
			mov(dst, tmp);
			restore_tmp(tmp);

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool movsx_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp
		)
		{
			if (dst.isREG())
			{
				movsx(reinterpret_cast<const Xbyak::Reg&>(dst), src);
				return false;
			}

			movsx(tmp, src);
			mov(dst, tmp);

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool movsx_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp,
			const std::function<void(const Xbyak::Reg&)>& restore_tmp
		)
		{
			if (dst.isREG())
			{
				movsx(reinterpret_cast<const Xbyak::Reg&>(dst), src);
				return false;
			}

			spill_tmp(tmp);
			movsx(tmp, src);
			mov(dst, tmp);
			restore_tmp(tmp);

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool movzx_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp
		)
		{
			if (dst.isREG())
			{
				movzx(reinterpret_cast<const Xbyak::Reg&>(dst), src);
				return false;
			}

			movzx(tmp, src);
			mov(dst, tmp);

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool movzx_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp,
			const std::function<void(const Xbyak::Reg&)>& restore_tmp
		)
		{
			if (dst.isREG())
			{
				movzx(reinterpret_cast<const Xbyak::Reg&>(dst), src);
				return false;
			}

			spill_tmp(tmp);
			movzx(tmp, src);
			mov(dst, tmp);
			restore_tmp(tmp);

			return true;
		}

		void cmp_ex(const Xbyak::Operand& operand, const uint32 immediate)
		{
			if (operand.isREG() && immediate == 0)
			{
				const Xbyak::Reg& reg = static_cast<const Xbyak::Reg&>(operand);
				test(reg, reg);
			}
			else
			{
				cmp(operand, immediate);	
			}
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool cmp_ex(
			const Xbyak::Operand& a,
			const Xbyak::Operand& b,
			const Xbyak::Reg& tmp
		)
		{
			if (a.isREG() || b.isREG())
			{
				cmp(a, b);
				return false;
			}

			mov(tmp, a);
			cmp(tmp, b);

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool cmp_ex(
			const Xbyak::Operand& a,
			const Xbyak::Operand& b,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp,
			const std::function<void(const Xbyak::Reg&)>& restore_tmp
		)
		{
			if (a.isREG() || b.isREG())
			{
				cmp(a, b);
				return false;
			}

			spill_tmp(tmp);
			mov(tmp, a);
			cmp(tmp, b);
			restore_tmp(tmp);

			return true;
		}

		[[nodiscard]]
		_nothrow Xbyak::Label& get_instruction_offset_label(const uint32 offset) noexcept {
			xassert(offset < jit1::NumInstructionsChunk);
			xassert(offset < instruction_offset_labels_.size());
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
			template <typename Self>
			_nothrow copy_qualifiers_ref<Self, Xbyak::Operand> get_operand(this Self&& self) noexcept
			{
				if (!self.operand_)
				{
					self.operand_ = &self.get_value();
					xassert(self.operand_);
				}

				return *self.operand_;
			}

			_nothrow const Xbyak::Operand& operator *() const noexcept
			{
				return get_operand();
			}

			_nothrow const Xbyak::Operand* operator ->() const noexcept
			{
				return &get_operand();
			}

			_nothrow operator const Xbyak::Operand&() const noexcept
			{
				return get_operand();
			}

			_nothrow bool isMEM() const noexcept
			{
				return get_operand().isMEM();
			}

			_nothrow bool isREG() const noexcept
			{
				return get_operand().isREG();
			}

			const Xbyak::Reg& as_reg() const
			{
				const Xbyak::Operand& operand = get_operand();
				xassert(operand.isREG());
				return static_cast<const Xbyak::Reg&>(operand);
			}

			_nothrow const Xbyak::Operand& if_reg(const Xbyak::Operand& else_operand) const noexcept
			{
				auto&& operand = get_operand();
				return
					operand.isREG() ?
						operand :
						else_operand;
			}

			_nothrow const Xbyak::Operand& if_mem(const Xbyak::Operand& else_operand) const noexcept
			{
				auto&& operand = get_operand();
				return
					operand.isMEM() ?
						operand :
						else_operand;
			}
		};

		class VariantOperand final
		{
		public:
		private:
			using variant_type = std::variant<
				Xbyak::Reg8,
				Xbyak::Reg16,
				Xbyak::Reg32,
				Xbyak::Reg64,
				Xbyak::Address
			>;

			variant_type storage_;

			template <typename TVariant, typename T = copy_qualifiers_ref<TVariant, Xbyak::Operand>>
			static _nothrow T get_reference(TVariant& variant) noexcept
			{
				T result = std::visit([](auto& ref) -> T { return static_cast<T&>(ref); }, variant);
				return result;
			}

		public:
			template <typename TOperand>
			requires(std::is_constructible_v<variant_type, TOperand>)
			_xbyak_nothrow VariantOperand(TOperand&& value) noexcept(!xbyak_throws) :
				storage_(std::forward<TOperand>(value))
			{
			}

			_nothrow operator const Xbyak::Operand&() const noexcept
			{
				const Xbyak::Operand& result = get_reference(storage_);
				return result;
			}

			_nothrow operator Xbyak::Operand&() noexcept
			{
				Xbyak::Operand& result = get_reference(storage_);
				return result;
			}
		};

		class LazyRegisterOperand final : public LazyOperand
		{
			Jit1_CodeGen& codegen_;
			struct register_info final
			{
				instructions::GPRegisterInfo register_;
				uint8_t size_;
			};
			mutable std::variant<register_info, VariantOperand> storage_;

		protected:
			_nothrow LazyRegisterOperand(
				Jit1_CodeGen& codegen,
				const instructions::GPRegisterInfo& _register,
				const uint8 size
			) noexcept
				: codegen_(codegen)
				, storage_(register_info{_register, size})
			{}

			virtual _nothrow const Xbyak::Operand& get_value() const noexcept override
			{
				if (VariantOperand* variant = std::get_if<VariantOperand>(&storage_))
				{
					return *variant;
				}

				const register_info& info = *std::get_if<register_info>(&storage_);
				const Jit1_CodeGen& codegen = static_cast<const Jit1_CodeGen&>(codegen_);

				switch (info.size_)
				{
					case 0:
						return storage_.emplace<VariantOperand>(codegen.get_register_op8_internal(info.register_));
					case 1:
						return storage_.emplace<VariantOperand>(codegen.get_register_op16_internal(info.register_));
					case 2:
						return storage_.emplace<VariantOperand>(codegen.get_register_op32_internal(info.register_));
					case 3: [[unlikely]]
						return storage_.emplace<VariantOperand>(codegen.get_register_op64_internal(info.register_));
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

	private:
		template <typename GPR>
		VariantOperand get_register_op8_internal(const GPR &reg) const {
			const auto reg_offset = reg.get_offset();

			if (reg.get_register() == mips_fp) {
				return r15b;
			}
			else {
				return byte[rbp + reg_offset];
			}
		}

		template <typename GPR>
		VariantOperand get_register_op16_internal(const GPR &reg) const {
			const auto reg_offset = reg.get_offset();
		
			if (reg.get_register() == mips_fp) {
				return r15w;
			}
			else {
				return word[rbp + reg_offset];
			}
		}

		template <typename GPR>
		VariantOperand get_register_op32_internal(const GPR &reg) const {
			const auto reg_offset = reg.get_offset();
		
			if (reg.get_register() == mips_fp) {
				return r15d;
			}
			else {
				return dword[rbp + reg_offset];
			}
		}

		template <typename GPR>
		VariantOperand get_register_op64_internal(const GPR &reg) const {
			const auto reg_offset = reg.get_offset();
		
			if (reg.get_register() == mips_fp) {
				return r15;
			}
			else {
				return qword[rbp + reg_offset];
			}
		}

	public:
		template <typename GPR>
		LazyRegisterOperand get_register_op8(const GPR &reg) {
			return LazyRegisterOperand::get<8>(*this, reg);
		}

		template <typename GPR>
		LazyRegisterOperand get_register_op16(const GPR &reg) {
			return LazyRegisterOperand::get<16>(*this, reg);
		}

		template <typename GPR>
		LazyRegisterOperand get_register_op32(const GPR &reg) {
			return LazyRegisterOperand::get<32>(*this, reg);
		}

		template <typename GPR>
		LazyRegisterOperand get_register_op64(const GPR &reg) {
			return LazyRegisterOperand::get<64>(*this, reg);
		}

		void write_chunk(jit1::ChunkOffset & __restrict chunk_offset, jit1::Chunk & __restrict chunk, uint32 start_address, bool update);

		void insert_procedure_ecx(uint32 address, void* procedure, uint32 _ecx);

		enum class except_result : uint32
		{
			none          = 0U,	     // neither throws nor sets exception
			can_throw     = 1U << 0, // may throw exception and return
			always_throw  = 1U << 1, // always throws exception and return
			can_except    = 1U << 2, // may set exception
			always_except = 1U << 3, // always set exception
		};

		enum class insert_location : uint32
		{
			none = 0U,
			before_epilog,
			before_exception_check,
			before_delaybranch_check,
			after_epilog
		};

		using insert_function_type = void(const jit1::Chunk& __restrict, const jit1::ChunkOffset& __restrict, uint32);

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
		std::tuple<std::function<insert_function_type>, insert_location, except_result> write_compact_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

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

		processor::flag intrinsic_set_cti_flag();
		processor::flag intrinsic_set_delay_branch();

		Xbyak::Label intrinsic_write_patch_prolog(const jit1::Chunk& __restrict chunk, void* patch_address, uint32 patch_target_address, const Xbyak::Reg& patch_target_address_reg);
		void intrinsic_write_patch_epilog(const Xbyak::Label& patch);
		void intrinsic_write_patch_jump(const jit1::Chunk& __restrict chunk, uint32 target_address, const Xbyak::Reg& patch_target_address_reg, bool set_pc);
		void intrinsic_insert_jump(const jit1::Chunk& __restrict chunk, const jit1::ChunkOffset& __restrict chunk_offset, uint32 address, const Xbyak::Operand& target_address);

		void intrinsic_clear_hazards(uint32 address);
	};

	static constexpr Jit1_CodeGen::except_result operator | (const Jit1_CodeGen::except_result a, const Jit1_CodeGen::except_result b)
	{
		return Jit1_CodeGen::except_result(std::to_underlying(a) | std::to_underlying(b));
	}

	static constexpr Jit1_CodeGen::except_result operator & (const Jit1_CodeGen::except_result a, const Jit1_CodeGen::except_result b)
	{
		return Jit1_CodeGen::except_result(std::to_underlying(a) & std::to_underlying(b));
	}

	static bool is_same(const Xbyak::Reg& a, const Xbyak::Reg& b)
	{
		return a.getIdx() == b.getIdx();
	}

	static bool is_same(const Xbyak::Operand& a, const Xbyak::Operand& b)
	{
		if (a.isREG() && b.isREG())
		{
			return is_same(a.getReg(), b.getReg());
		}
		return a == b;
	}
}