#pragma once

#include <array>
#include <bit>
#include <functional>
#include <variant>

#include "jit1_xbyak.hpp"

#include "mips_common.hpp"
#include "abi/jit1_abi_win64.hpp"
#include "instructions/instructions_common.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"
#include "mips/processor/jit/jit1/jit1.hpp"
#include "mips/processor/processor.hpp"
#include "platform/win32/platform_win32.hpp"

#define WITH_WIN64_ABI 1


namespace mips
{
#ifdef XBYAK_NO_EXCEPTION
	static constexpr const bool xbyak_throws = false;
	#define _xbyak_nothrow _nothrow
#else
	static constexpr const bool xbyak_throws = true;
	#define _xbyak_nothrow
#endif

	[[nodiscard]]
	_pure _nothrow
	static bool is_same(const Xbyak::Reg& a, const Xbyak::Reg& b) noexcept
	{
		return a.getIdx() == b.getIdx();
	}

	[[nodiscard]]
	_pure _nothrow
	static bool is_same(const Xbyak::Operand& a, const Xbyak::Operand& b) noexcept
	{
		if (a.isREG() && b.isREG())
		{
			return is_same(a.getReg(), b.getReg());
		}
		return a == b;
	}

	enum class JumpFlags : uint8
	{
		None = 0,
		ExternalCall = 1U << 0,
		AlwaysOutOfRange = 1U << 1,
	};

	MAKE_BITFLAG_ENUM(JumpFlags)

	class jit1;
	class Jit1_CodeGen final : public Xbyak::CodeGenerator
	{
		static constexpr const auto offsets = processor::recompiler_offsets<>::get<int8>();
		static constexpr const auto cop1_offsets = coprocessor1::recompiler_offsets<>::get<int16>();

		std::array<Xbyak::Label, jit1::NumInstructionsChunk> instruction_offset_labels_;
		struct {
			struct intrinsic final {
				Xbyak::Label label;
				mutable bool used : 1 = false;

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
			intrinsic internal_flow;
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
		jit1& __restrict jit_;

	public:

		Jit1_CodeGen(jit1& __restrict jit, uint8* const user_ptr, const usize user_size)
			: Xbyak::CodeGenerator(user_size, user_ptr)
			, jit_(jit)
		{}
		virtual ~Jit1_CodeGen() override = default;

		// returns `true` if temporary was used
		[[nodiscard]]
		bool exchange(
			const Xbyak::Operand& a,
			const Xbyak::Operand& b,
			const Xbyak::Reg& tmp
		)
		{
			xassert(!is_same(a, b));
			const uint32 bit_size = a.getBit();
			xassert(bit_size == b.getBit());
			xassert(bit_size >= 8 && bit_size <= (sizeof(void*) * CHAR_BIT));

			const bool is_reg[2] = { a.isREG(), b.isREG() };

			if (
				(
					platform::get_host_features().fast_xchg_rr8_16 &&
					(bit_size >= 8 && bit_size <= 16 && is_reg[0] && is_reg[1])
				) ||
				(
					platform::get_host_features().fast_xchg_rr32_64 &&
					(bit_size >= 32 && bit_size <= 64 && is_reg[0] && is_reg[1])
				) ||
				(
					platform::get_host_features().fast_xchg_rm &&
					(is_reg[0] != is_reg[1])
				)
			)
			{
				xchg(a, b);
				return false;
			}
			else if (!is_reg[0] && !is_reg[1])
			{
				mov(tmp, a);
				xchg(b, tmp);
				mov(a, tmp);
				return true;
			}
			else
			{
				mov(tmp, a);
				mov(a, b);
				mov(b, tmp);
				return true;
			}
		}

		template <typename TImmediate>
		requires ((std::is_integral_v<TImmediate> && sizeof(TImmediate) <= sizeof(uintptr)) || std::is_pointer_v<TImmediate>)
		void set(const Xbyak::Operand& dst, const TImmediate imm)
		{
			const uintptr immediate = uintptr(imm);

			if (!dst.isREG())
			{
				mov(dst, immediate);
				return;
			}

			Xbyak::Operand dest = dst;

			if (dst.getBit() == 64 && in_range<uint32>(immediate))
			{
				dest.setBit(32);
			}

			if (immediate == 0ULL)
			{
				xor_(dest, dest);
			}
			else
			{
				mov(dest, immediate);
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

		template <bool IsCall, bool IsExternal, int32 StackAdjust = 0>
		static constexpr const int32 stack_realignment = 
			(
				// The stack is pre-adjusted to reserved bytes in the secondary springboard.
				// Thus, we do not need to adjust rsp for the ABI when making `call`s, only when `jmp`ing,
				// as `jmp` will result in a `ret` bringing us back to the springboard - we must pop from `rsp`
				// before that to make sure that the return address is available.
				(!IsCall && IsExternal) ?
					static_cast<int32>(jit::abi::win64::caller_stack_reserve) :
					0
			) +
			(
				(!IsCall && !IsExternal) ?
					-StackAdjust :
					StackAdjust
			);

		void adjust_stack(const int32 amount, const Xbyak::Reg& dummy)
		{
			if (amount > 0)
			{
				switch (amount)
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
						sub_ex(rsp, amount);
						break;
				}
			}
			else
			{
				switch (-amount)
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
						add_ex(rsp, -amount);
						break;
				}
			}
		}

		void adjust_stack(const int32 amount)
		{
			adjust_stack(amount, ecx);
		}

		struct [[clang::trivial_abi]] stack_adjuster final
		{
			Jit1_CodeGen& cg;
			int32 size = 0;
			const Xbyak::Reg& dummy;

			stack_adjuster() = delete;
			[[nodiscard]]
			_nothrow stack_adjuster(
				Jit1_CodeGen* cg,
				const int32 size,
				const std::optional<std::reference_wrapper<const Xbyak::Reg>>& dummy = {}
			) noexcept :
				cg(*cg),
				size(size),
				dummy(dummy.value_or(cg->ebx).get())
			{
				cg->adjust_stack(size, this->dummy);
			}
			stack_adjuster(const stack_adjuster&&) = delete;
			[[nodiscard]]
			_nothrow stack_adjuster(stack_adjuster&& other) noexcept :
				cg(other.cg),
				size(other.size),
				dummy(other.dummy)
			{
				other.size = 0;
			}

			_nothrow ~stack_adjuster() noexcept
			{
				cg.adjust_stack(-size, dummy);
			}

			stack_adjuster& operator=(const stack_adjuster&) = delete;
			stack_adjuster& operator=(stack_adjuster&&) = delete;
		};

		[[nodiscard]]
		_nothrow
		stack_adjuster get_stack_adjuster(const int32 size, const std::optional<std::reference_wrapper<const Xbyak::Reg>>& dummy = {}) noexcept
		{
			return {
				this,
				size,
				dummy
			};
		}

		// returns `true` if temporary register was used
		template <JumpFlags Flags = JumpFlags::None, uint32 Realignment = 0U>
		[[nodiscard]]
		bool call_ex(
			const void* const pointer,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {},
			const std::function<void(const Xbyak::Reg&)>& restore_tmp = {}
		)
		{
			// The stack is pre-adjusted to reserved bytes in the secondary springboard.
			constexpr int32 stack_adjust = stack_realignment<true, enumeration::has_all(Flags, JumpFlags::ExternalCall), Realignment>;
			
			const bool is_in_range = [&] {
				if (enumeration::has_all(Flags, JumpFlags::AlwaysOutOfRange))
				{
					return false;
				}
				else
				{
					const intptr diff = reinterpret_cast<intptr>(pointer) - intptr(getCurr() + 5); // E8'xx'xx'xx'xx
					return in_range<int32>(diff);
				}
			}();

			{
			const Xbyak::Reg& dummy = is_same(tmp, ecx) ?
				edx :
				ecx;

				stack_adjuster adjuster = get_stack_adjuster(stack_adjust, dummy);

				if (!is_in_range)
				{
					if (spill_tmp) { spill_tmp(tmp); }
					set(tmp, intptr(pointer));
					call(tmp);
				}
				else
				{
					call(pointer);
				}
			}

			if (!is_in_range && restore_tmp)
			{
				restore_tmp(tmp);
			}

			return !is_in_range;
		}

		template <JumpFlags Flags = JumpFlags::None, uint32 Realignment = 0U>
		void call_ex(
			const Xbyak::Reg& target
		)
		{
			constexpr int32 stack_adjust = stack_realignment<true, enumeration::has_all(Flags, JumpFlags::ExternalCall), Realignment>;

			{
				const Xbyak::Reg& dummy = ecx;

				stack_adjuster adjuster = get_stack_adjuster(stack_adjust, dummy);

				call(target);
			}
		}

		// returns `true` if temporary register was used
		template <JumpFlags Flags = JumpFlags::None, uint32 Realignment = 0U>
		[[nodiscard]]
		bool jmp_ex(
			const void* const pointer,
			const Xbyak::Reg& tmp,
			const std::function<void(const Xbyak::Reg&)>& spill_tmp = {}
		)
		{
			const intptr diff = reinterpret_cast<intptr>(pointer) - intptr(getCurr() + 5); // E8'xx'xx'xx'xx

			const bool is_in_range = !enumeration::has_all(Flags, JumpFlags::AlwaysOutOfRange) && in_range<int32>(diff);

#if !WITH_WIN64_ABI
			constexpr int32 stack_adjust = stack_realignment<false, enumeration::has_all(Flags, JumpFlags::ExternalCall), Realignment>;

			const Xbyak::Reg& dummy = is_same(tmp, ecx) ?
				edx :
				ecx;

			adjust_stack(-stack_adjust, dummy);
#else
			static_assert(Realignment == 0U);

			// Copy the return pointer to the top of the stack, which also will realign the stack.
			push(qword[rsp + mips::jit::abi::win64::caller_stack_reserve]);
#endif

			if (!is_in_range)
			{
				if (spill_tmp) { spill_tmp(tmp); }
				set(tmp, intptr(pointer));
				jmp(tmp);
			}
			else
			{
				jmp(pointer);
			}

			return !is_in_range;
		}

		template <JumpFlags Flags = JumpFlags::None, uint32 Realignment = 0U>
		void jmp_ex(
			const Xbyak::Reg& target
		)
		{
			// The stack is pre-adjusted to reserved bytes in the secondary springboard 
			// We need to restore it prior to an external jump, so the `ret` gets the return address correctly.
			constexpr int32 stack_adjust = stack_realignment<false, enumeration::has_all(Flags, JumpFlags::ExternalCall), Realignment>;

#if !WITH_WIN64_ABI
			const Xbyak::Reg& dummy = ecx;

			adjust_stack(-stack_adjust, dummy);
#else
			static_assert(Realignment == 0U);

			// Copy the return pointer to the top of the stack, which also will realign the stack.
			push(qword[rsp + mips::jit::abi::win64::caller_stack_reserve]);
#endif

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
					if (operand != 0x80)
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
					if (operand != 0x80)
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

		template <int32 StackAdjust = 0>
		void escape_ret()
		{
			//constexpr int32 stack_adjust = stack_realignment<false, true, StackAdjust>;

			//const Xbyak::Reg& dummy = ecx;
			//adjust_stack(stack_adjust, dummy);

			// Copy the return pointer to the top of the stack, which also will realign the stack.
			push(qword[rsp + mips::jit::abi::win64::caller_stack_reserve]);
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
			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow copy_qualifiers_ref<Self, Xbyak::Operand> get_operand(this Self&& self) noexcept
			{
				if (!self.operand_)
				{
					self.operand_ = &self.get_value();
					xassert(self.operand_);
				}

				return *self.operand_;
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow const Xbyak::Operand& operator *() const noexcept
			{
				return get_operand();
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow const Xbyak::Operand* operator ->() const noexcept
			{
				return &get_operand();
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow operator const Xbyak::Operand&() const noexcept
			{
				return get_operand();
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow bool isMEM() const noexcept
			{
				return get_operand().isMEM();
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow bool isREG() const noexcept
			{
				return get_operand().isREG();
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			const Xbyak::Reg& as_reg() const
			{
				const Xbyak::Operand& operand = get_operand();
				xassert(operand.isREG());
				return static_cast<const Xbyak::Reg&>(operand);
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow Xbyak::Operand if_reg(const Xbyak::Operand& else_operand) const noexcept
			{
				auto&& operand = get_operand();
				return
					operand.isREG() ?
						operand :
						else_operand;
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
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
			[[nodiscard]]
			_pure // not really pure, but acts like it
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

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow operator const Xbyak::Operand&() const noexcept
			{
				const Xbyak::Operand& result = get_reference(storage_);
				return result;
			}

			[[nodiscard]]
			_pure // not really pure, but acts like it
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

			[[nodiscard]]
			_pure // not really pure, but acts like it
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
						xunreachable("Unhandled Operand Size");
				}
			}

		public:
			template <usize Size>
			requires(Size == 8 || Size == 16 || Size == 32 || Size == 64)
			[[nodiscard]]
			_pure // not really pure, but acts like it
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
		[[nodiscard]]
		_pure // not really pure, but acts like it
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
		[[nodiscard]]
		_pure // not really pure, but acts like it
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
		[[nodiscard]]
		_pure // not really pure, but acts like it
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
		[[nodiscard]]
		_pure // not really pure, but acts like it
		VariantOperand get_register_op64_internal(const GPR &reg) const {
			const auto reg_offset = reg.get_offset();
		
			if (reg.get_register() == mips_fp) {
				return r15;
			}
			else {
				return qword[rbp + reg_offset];
			}
		}

		[[nodiscard]]
		_pure // not really pure, but acts like it
		_nothrow Xbyak::Address get_register_gp_mem_operand(const uint32 index) noexcept
		{
			//xassert(index != 0);
			xassert(index < processor::num_registers);

			const int8 offset = processor::recompiler_offsets<>::get<int8>().registers;

			const int8 result = value_assert<int8>(offset + (sizeof(processor::register_type) * index));

			return dword[rbp + result];
		}

	public:
		template <typename GPR>
		[[nodiscard]]
		_pure // not really pure, but acts like it
		LazyRegisterOperand get_register_op8(const GPR &reg) {
			return LazyRegisterOperand::get<8>(*this, reg);
		}

		template <typename GPR>
		[[nodiscard]]
		_pure // not really pure, but acts like it
		LazyRegisterOperand get_register_op16(const GPR &reg) {
			return LazyRegisterOperand::get<16>(*this, reg);
		}

		template <typename GPR>
		[[nodiscard]]
		_pure // not really pure, but acts like it
		LazyRegisterOperand get_register_op32(const GPR &reg) {
			return LazyRegisterOperand::get<32>(*this, reg);
		}

		template <typename GPR>
		[[nodiscard]]
		_pure // not really pure, but acts like it
		LazyRegisterOperand get_register_op64(const GPR &reg) {
			return LazyRegisterOperand::get<64>(*this, reg);
		}

		void write_chunk(jit1::ChunkOffset& __restrict chunk_offset, jit1::Chunk& __restrict chunk, uptr_guest start_address, bool update);
		enum class except_result : uint32
		{
			none          = 0U,	     // neither throws nor sets exception
			can_throw     = 1U << 0, // may throw exception and return
			always_throw  = 1U << 1, // always throws exception and return
			can_except    = 1U << 2, // may set exception
			always_except = 1U << 3, // always set exception
			always_exits  = 1U << 4, // always exits generated codestream
		};

		void insert_instruction_procedure(uptr_guest address, const void* procedure, uint32 argument0);
		void insert_instruction_procedure(uptr_guest address, const void* procedure,  const Xbyak::Operand& argument0);
		[[nodiscard]]
		except_result insert_instruction_procedure_check_hazard(uptr_guest address, const mips::instructions::InstructionInfo& __restrict instruction_info, uint32 argument0);
		void insert_procedure(uptr_guest address, const void* procedure, uint32 argument0);
		void insert_procedure(uptr_guest address, const void* procedure,  const Xbyak::Operand& argument0);
		[[nodiscard]]
		except_result insert_procedure_check_hazard(uptr_guest address, const mips::instructions::InstructionInfo& __restrict instruction_info, uint32 argument0);
		void insert_procedure_hazard(uptr_guest address, const void* procedure, uint32 argument0);
		void insert_procedure_hazard(uptr_guest address, const void* procedure, const Xbyak::Operand& argument0);

		[[nodiscard]]
		bool interpret_if_hazard(uptr_guest address, const mips::instructions::InstructionInfo& __restrict instruction_info);

		enum class insert_location : uint32
		{
			none = 0U,
			before_epilog,
			before_exception_check,
			before_delaybranch_check,
			after_epilog
		};

		using insert_function_type = void(const jit1::Chunk& __restrict, const jit1::ChunkOffset& __restrict, uint32);

		void write_PROC_SUBU(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_SUB(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_OR(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_NOR(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_AND(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_MOVE(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_ADDIU(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_ADDI(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_ADDU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_ADD(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_AUI(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_ORI(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_ANDI(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SELEQZ(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SELNEZ(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLT(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLTU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLTI(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLTIU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MUL(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MUH(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MULU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MUHU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_DIV(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MOD(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_DIVU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_MODU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_XOR(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_XORI(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SEB(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SEH(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_EHB(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRL(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRA(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SLLV(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SRLV(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_SYNC(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_SYNCI(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_INS(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_LSA(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_CLZ(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);
		void write_PROC_CLO(jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info);

		[[nodiscard]]
		except_result write_PROC_TEQ(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TGE(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TGEU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TLT(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TLTU(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		[[nodiscard]]
		except_result write_PROC_TNE(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		[[nodiscard]]
		except_result write_PROC_SYSCALL(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex);

		[[nodiscard]]
		except_result write_PROC_BREAK(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex);

		[[nodiscard]]
		except_result write_PROC_SIGRIE(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info, const Xbyak::Label& intrinsic_ex);

		// returns empty if it was unhandled
		[[nodiscard]]
		std::optional<except_result> write_STORE(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		// returns empty if it was unhandled
		[[nodiscard]]
		std::optional<except_result> write_LOAD(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		// FPU ops
		void write_COP1_MFC1(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_MTC1(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_MFHC1(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_MTHC1(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		void write_COP1_SEL(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		// returns 'true' if compact branch patch is needed.
		[[nodiscard]]
		std::tuple<std::function<insert_function_type>, insert_location, except_result> write_compact_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);

		// returns 'true' if it was unhandled.
		[[nodiscard]]
		except_result write_delay_branch(jit1::ChunkOffset & __restrict chunk_offset, uptr_guest address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info);
		enum class branch_type : uint8 {
			none = 0,
			near_branch,				  // Branches within this chunk			
			far_branch,						 // Branches outside this chunk
			indeterminate,					 // Branches to an unknown location
			near_branch_unhandled,		  // Branches within this chunk, use pc state	
			far_branch_unhandled,			// Branches outside this chunk, use pc state
			indeterminate_unhandled		 // Branches to an unknown location, use pc state
		};
		void handle_delay_branch(
			jit1::Chunk & __restrict chunk,
			jit1::ChunkOffset & __restrict chunk_offset,
			uptr_guest address,
			instruction_t instruction,
			const mips::instructions::InstructionInfo & __restrict instruction_info,
			bool hazard
		);

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

		[[nodiscard]]
		uptr intrinsic_write_patch_jump_patcher(
			uptr_guest target_address,
			const void* start_address,
			uint16 patch_index,
			bool set_pc
		);
		void intrinsic_write_patch_jump(jit1::Chunk& __restrict chunk, uptr_guest target_address, bool set_pc);
		void intrinsic_insert_jump(const jit1::Chunk& __restrict chunk, const jit1::ChunkOffset& __restrict chunk_offset, uptr_guest address, const Xbyak::Operand& target_address);

		struct alignas(uint8) instruction_hazard_flags final
		{
			uint8 delay_branch : 1 = false;
		};

		void intrinsic_clear_execution_hazards(uptr_guest current_address);
		void intrinsic_clear_instruction_hazards(uptr_guest current_address, uptr_guest target_address, instruction_hazard_flags flags);
		void intrinsic_clear_instruction_hazards(uptr_guest current_address, const Xbyak::Reg& target_address_reg, instruction_hazard_flags flags);
		inline void intrinsic_clear_instruction_hazards(const uptr_guest address, const instruction_hazard_flags flags)
		{
			intrinsic_clear_instruction_hazards(address, address + 4, flags);
		}

		[[nodiscard]]
		_pure // not really pure, but acts like it
		_nothrow processor& get_processor() noexcept
		{
			return jit_.processor_;
		}

		[[nodiscard]]
		_pure // not really pure, but acts like it
		_nothrow const processor& get_processor() const noexcept
		{
			return jit_.processor_;
		}
	};

	MAKE_BITFLAG_ENUM(Jit1_CodeGen::except_result)
}