#pragma once

#include <bit>
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
			intrinsic adel_identity;
			intrinsic adel;
			intrinsic ades;
			intrinsic ov;
			intrinsic tr;
			intrinsic cpu;
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

		void exchange(
			const Xbyak::Reg& a,
			const Xbyak::Reg& b
		)
		{
			#pragma message("use three moves for proper chips instead of xchg")
			#pragma message("fix reg sizes")
			xchg(a, b);
		}

		// returns `true` if temporary was used
		[[nodiscard]]
		bool exchange(
			const Xbyak::Operand& a,
			const Xbyak::Operand& b,
			const Xbyak::Reg& tmp
		)
		{
			#pragma message("use three moves for proper chips instead of xchg")
			if (a.isREG() && b.isREG())
			{
				exchange(a.getReg(), b.getReg());
				return false;
			}
			else
			{
				mov(tmp, a);
				mov(a, b);
				mov(b, tmp);
				return true;
			}
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
				if (dst.getBit() == 64 && in_range<uint32>(imm))
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
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			if (dst.isREG() || src.isREG())
			{
				if (dst.isREG() && src.isREG() && src == dst)
				{
					return false;
				}

				mov(dst, src);
				return false;
			}

			if (spill_tmp) { spill_tmp(tmp); }
			mov(tmp, src);
			mov(dst, tmp);
			if (restore_tmp) { restore_tmp(tmp); }

			return true;
		}

		// returns `true` if the operation was actually generated
		[[nodiscard]]
		bool mov_ex(
			const Xbyak::Reg& dst,
			const Xbyak::Operand& src
		)
		{
			if (src.isREG() && src == dst)
			{
				return false;
			}

			mov(dst, src);
			return false;
		}

		// returns `true` if the operation was actually generated
		[[nodiscard]]
		bool mov_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Reg& src
		)
		{
			if (dst.isREG() && src == dst)
			{
				return false;
			}

			mov(dst, src);
			return false;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool movsx_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			if (dst.isREG())
			{
				if (dst.isREG() && src.isREG() && src == dst)
				{
					return false;
				}

				movsx(reinterpret_cast<const Xbyak::Reg&>(dst), src);
				return false;
			}

			if (spill_tmp) { spill_tmp(tmp); }
			movsx(tmp, src);
			mov(dst, tmp);
			if (restore_tmp) { restore_tmp(tmp); }

			return true;
		}

		// returns `true` if temporary register was used
		[[nodiscard]]
		bool movzx_ex(
			const Xbyak::Operand& dst,
			const Xbyak::Operand& src,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			if (dst.isREG())
			{
				if (dst.isREG() && src.isREG() && src == dst)
				{
					return false;
				}

				movzx(reinterpret_cast<const Xbyak::Reg&>(dst), src);
				return false;
			}

			if (spill_tmp) { spill_tmp(tmp); }
			movzx(tmp, src);
			mov(dst, tmp);
			if (restore_tmp) { restore_tmp(tmp); }

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
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			if (a.isREG() || b.isREG())
			{
				cmp(a, b);
				return false;
			}

			if (spill_tmp) { spill_tmp(tmp); }
			mov(tmp, a);
			cmp(tmp, b);
			if (restore_tmp) { restore_tmp(tmp); }

			return true;
		}

		// returns `true` if temporary register was used
		template <bool ExternalCall = false, uint32 Realignment = 0U>
		[[nodiscard]]
		bool call_ex(
			const void* const ptr,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			const intptr diff = uintptr(ptr) - uintptr(get_current_address() + 5); // E8'xx'xx'xx'xx

			uint32 stack_add = Realignment;

#if 0 // stack space is added in springboard, and we do not use the stack ourselves
			if constexpr (ExternalCall)
			{
				stack_add = 40;
			}
#endif

			const auto push_stack = [&tmp, stack_add, this]
			{
				const Xbyak::Reg& dummy = is_same(tmp, ecx) ?
					edx :
					ecx;

				switch (stack_add)
				{
					case 0:
						break;
					case 1:
						push(dummy.cvt8());
						break;
					case 2:
						push(dummy.cvt16());
						break;
					case 4:
						push(dummy.cvt32());
						break;
					case 8:
						push(dummy.cvt64());
						break;
					default:
						sub_ex(rsp, 40);
						break;
				}
			};

			const auto pop_stack = [&tmp, stack_add, this]
			{
				const Xbyak::Reg& dummy = is_same(tmp, ecx) ?
					edx :
					ecx;

				switch (stack_add)
				{
					case 0:
						break;
					case 1:
						pop(dummy.cvt8());
						break;
					case 2:
						pop(dummy.cvt16());
						break;
					case 4:
						pop(dummy.cvt32());
						break;
					case 8:
						pop(dummy.cvt64());
						break;
					default:
						add_ex(rsp, 40);
						break;
				}
			};

			if (in_range<int32>(diff))
			{
				push_stack();
				call(ptr);
				pop_stack();
				
				return false;
			}
			else
			{
				if (spill_tmp) { spill_tmp(tmp); }
				set(tmp, intptr(ptr));
				push_stack();
				call(tmp);
				pop_stack();
				if (restore_tmp) { restore_tmp(tmp); }
			}

			return true;
		}

		template <bool ExternalCall = false, uint32 Realignment = 0U>
		void call_ex(
			const Xbyak::Operand& target
		)
		{
			uint32 stack_add = Realignment;

#if 0 // stack space is added in springboard, and we do not use the stack ourselves
			if constexpr (ExternalCall)
			{
				stack_add = 40;
			}
#endif

			const auto push_stack = [stack_add, this]
			{
				const Xbyak::Reg& dummy = ecx;

				switch (stack_add)
				{
					case 0:
						break;
					case 1:
						push(dummy.cvt8());
						break;
					case 2:
						push(dummy.cvt16());
						break;
					case 4:
						push(dummy.cvt32());
						break;
					case 8:
						push(dummy.cvt64());
						break;
					default:
						sub_ex(rsp, 40);
						break;
				}
			};

			const auto pop_stack = [stack_add, this]
			{
				const Xbyak::Reg& dummy = ecx;

				switch (stack_add)
				{
					case 0:
						break;
					case 1:
						pop(dummy.cvt8());
						break;
					case 2:
						pop(dummy.cvt16());
						break;
					case 4:
						pop(dummy.cvt32());
						break;
					case 8:
						pop(dummy.cvt64());
						break;
					default:
						add_ex(rsp, 40);
						break;
				}
			};

			push_stack();
			call(target);
			pop_stack();
		}

		// returns `true` if temporary register was used
		template <bool ExternalCall = false, uint32 Realignment = 0U>
		[[nodiscard]]
		bool jmp_ex(
			const void* const ptr,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {}
		)
		{
			const intptr diff = uintptr(ptr) - uintptr(get_current_address() + 5); // E8'xx'xx'xx'xx

			uint32 stack_add = Realignment;

#if 0 // stack space is added in springboard, and we do not use the stack ourselves
			if constexpr (ExternalCall)
			{
				stack_add = 40;
			}
#endif

			const auto push_stack = [&tmp, stack_add, this]
			{
				const Xbyak::Reg& dummy = is_same(tmp, ecx) ?
					edx :
					ecx;

				switch (stack_add)
				{
					case 0:
						break;
					case 1:
						push(dummy.cvt8());
						break;
					case 2:
						push(dummy.cvt16());
						break;
					case 4:
						push(dummy.cvt32());
						break;
					case 8:
						push(dummy.cvt64());
						break;
					default:
						sub_ex(rsp, 40);
						break;
				}
			};

			if (in_range<int32>(diff))
			{
				push_stack();
				jmp(ptr);
				
				return false;
			}
			else
			{
				if (spill_tmp) { spill_tmp(tmp); }
				set(tmp, intptr(ptr));
				push_stack();
				jmp(tmp);
			}

			return true;
		}

		template <bool ExternalCall = false, uint32 Realignment = 0U>
		void jmp_ex(
			const Xbyak::Operand& target
		)
		{
			uint32 stack_add = Realignment;

#if 0 // stack space is added in springboard, and we do not use the stack ourselves
			if constexpr (ExternalCall)
			{
				stack_add = 40;
			}
#endif

			const Xbyak::Reg& dummy = ecx;

			switch (stack_add)
			{
				case 0:
					break;
				case 1:
					push(dummy.cvt8());
					break;
				case 2:
					push(dummy.cvt16());
					break;
				case 4:
					push(dummy.cvt32());
					break;
				case 8:
					push(dummy.cvt64());
					break;
				default:
					sub_ex(rsp, 40);
					break;
			}

			jmp(target);
		}

		void sub_ex(
			const Xbyak::Operand& dst,
			const int32 operand
		)
		{
			switch (operand)
			{
				case 0: [[unlikely]]
					break;
				case -1:
					inc(dst);
					break;
				case 1:
					dec(dst);
					break;
				default:
					if (operand < 0)
					{
						sub(dst, operand);
					}
					else
					{
						add(dst, -operand);
					}
					break;
			}
		}

		void add_ex(
			const Xbyak::Operand& dst,
			const int32 operand
		)
		{
			switch (operand)
			{
				case 0: [[unlikely]]
					break;
				case -1:
					dec(dst);
					break;
				case 1:
					inc(dst);
					break;
				default:
					if (operand < 0)
					{
						add(dst, operand);
					}
					else
					{
						sub(dst, -operand);
					}
					break;
			}
		}

		template <uint32 StackAdjust = 0>
		void escape_ret()
		{
			const uint32 stack_adjust = 40 + StackAdjust;

			const Xbyak::Reg& dummy = ecx;

			switch (stack_adjust)
			{
				case 0:
					break;
				case 1:
					pop(dummy.cvt8());
					break;
				case 2:
					pop(dummy.cvt16());
					break;
				case 4:
					pop(dummy.cvt32());
					break;
				case 8:
					pop(dummy.cvt64());
					break;
				default:
					add_ex(rsp, 40);
					break;
			}

			ret();
		}

		[[nodiscard]]
		bool flush_pc(
			const Xbyak::Reg& tmp,
			const uint32 address,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			if (tmp.isREG())
			{
				mov(dword[rbp + offsets.pc], address);
				return false;
			}

			if (spill_tmp) { spill_tmp(tmp); }
			set(tmp, address);
			mov(dword[rbp + offsets.pc], tmp);
			if (restore_tmp) { restore_tmp(tmp); }
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

			_nothrow Xbyak::Operand if_reg(const Xbyak::Operand& else_operand) const noexcept
			{
				auto&& operand = get_operand();
				return
					operand.isREG() ?
						operand :
						else_operand;
			}

			_nothrow Xbyak::Operand if_mem(const Xbyak::Operand& else_operand) const noexcept
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
				return {
					codegen,
					_register,
					static_cast<uint8>(
						std::bit_width(Size - 1UZ) -
						std::bit_width(8U - 1U)
					)
				};
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
		enum class except_result : uint32
		{
			none          = 0U,	     // neither throws nor sets exception
			can_throw     = 1U << 0, // may throw exception and return
			always_throw  = 1U << 1, // always throws exception and return
			can_except    = 1U << 2, // may set exception
			always_except = 1U << 3, // always set exception
			always_exits  = 1U << 4, // always exits generated codestream
		};

		void insert_procedure(uint32 address, const void* procedure, uint32 argument0);
		void insert_procedure(uint32 address, const void* procedure,  const Xbyak::Operand& argument0);
		except_result insert_procedure_check_hazard(uint32 address, const mips::instructions::InstructionInfo& __restrict instruction_info, uint32 argument0);
		void insert_procedure_hazard(uint32 address, const void* procedure, uint32 argument0);
		void insert_procedure_hazard(uint32 address, const void* procedure, const Xbyak::Operand& argument0);

		bool interpret_if_hazard(uint32 address, const mips::instructions::InstructionInfo& __restrict instruction_info);

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
		void write_PROC_EHB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRA(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SYNC(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_SYNCI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_INS(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_LSA(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_CLZ(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_CLO(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);

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

		// returns empty if it was unhandled
		[[nodiscard]]
		std::optional<except_result> write_STORE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		// returns empty if it was unhandled
		[[nodiscard]]
		std::optional<except_result> write_LOAD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

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
			none = 0,
			near_branch,				  // Branches within this chunk			
			far_branch,						 // Branches outside this chunk
			indeterminate,					 // Branches to an unknown location
			near_branch_unhandled,		  // Branches within this chunk, use pc state	
			far_branch_unhandled,			// Branches outside this chunk, use pc state
			indeterminate_unhandled		 // Branches to an unknown location, use pc state
		};
		void handle_delay_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, bool hazard);

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
		processor::flag intrinsic_set_delay_branch(bool hazard_barrier = false);

		Xbyak::Label intrinsic_write_patch_prolog(const jit1::Chunk& __restrict chunk, void* patch_address, uint32 patch_target_address, const Xbyak::Reg& patch_target_address_reg);
		void intrinsic_write_patch_epilog(const Xbyak::Label& patch);
		void intrinsic_write_patch_jump(const jit1::Chunk& __restrict chunk, uint32 target_address, const Xbyak::Reg& patch_target_address_reg, bool set_pc);
		void intrinsic_insert_jump(const jit1::Chunk& __restrict chunk, const jit1::ChunkOffset& __restrict chunk_offset, uint32 address, const Xbyak::Operand& target_address);

		void intrinsic_clear_execution_hazards(uint32 current_address);
		void intrinsic_clear_instruction_hazards(uint32 current_address, uint32 target_address);
		void intrinsic_clear_instruction_hazards(uint32 current_address, const Xbyak::Reg& target_address_reg);
		inline void intrinsic_clear_instruction_hazards(const uint32 address)
		{
			intrinsic_clear_instruction_hazards(address, address + 4);
		}

		processor& get_processor()
		{
			return jit_.processor_;
		}

		const processor& get_processor() const
		{
			return jit_.processor_;
		}
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