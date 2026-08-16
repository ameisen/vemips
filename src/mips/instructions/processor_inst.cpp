// ReSharper disable CommentTypo
#include "pch.hpp"
#include "processor_inst.hpp"
#include "mips/config.hpp"
#include <common.hpp>

#include <concepts>
#include <limits>
#if _MSC_VER
#	include <intrin.h>
#endif

#include "instructions_common.hpp"
#include "instructions_support.hpp"
#include "mips/exception.hpp"
#include "mips/mips_common.hpp"
#include "mips/system.hpp"
#include "mips/platform/platform.hpp"
#include "mips/processor/processor.hpp"

#define WITH_SPECIALIZED 0
#define WITH_EXP_OPTIM 0

using namespace mips;

#define WITH_COPROCESSOR_0 0
#if WITH_COPROCESSOR_0
#	define THROWS_WITHOUT_COPROCESSOR_0
#else
#	define THROWS_WITHOUT_COPROCESSOR_0 (OpFlags::Throws | OpFlags::AlwaysThrows)
#endif

#define SYSCALL_USE_EXCEPTION 0

#pragma region Boilerplate
#define ProcInstructionDef(InsInstruction, InsOperFlags, InsOpMask, InsOpRef)             \
struct PROC_ ## InsInstruction                                                            \
{                                                                                         \
    friend class PROC_Helper;                                                             \
    PROC_ ## InsInstruction() = delete;                                                   \
    static constexpr instruction_t OpMask = InsOpMask;                                    \
    static constexpr OpFlags Flags = InsOperFlags;                                        \
    static constexpr bool CanThrowInner =                                                 \
        (Flags & (OpFlags::Throws | OpFlags::ThrowsMisaligned)) != OpFlags::None;         \
    static constexpr bool CanThrow = CanThrowInner ||                                     \
        (Flags & OpFlags::ControlInstruction) != OpFlags::None;                           \
                                                                                          \
    static _forceinline void SubExecute(                                                  \
        const instruction_t instruction,                                                  \
        processor& __restrict processor                                                   \
    ) noexcept(!CanThrowInner);                                                           \
                                                                                          \
    template <typename reg_t, typename format_t>                                          \
    static _forceinline _nothrow void write_result(                                       \
        reg_t& dest,                                                                      \
        format_t value                                                                    \
    ) noexcept                                                                            \
    {                                                                                     \
        _block_forceinline                                                                \
        PROC_Helper::_write_result<reg_t, format_t, Flags>(                               \
            dest,                                                                         \
            value                                                                         \
        );                                                                                \
    }                                                                                     \
};                                                                                        \
namespace PROC_ ## InsInstruction ## _NS                                                  \
{                                                                                         \
    VEMIPS_JIT_ABI _used _flatten _forceinline void VEMIPS_JIT_ABI_INFIX Execute(         \
        const instruction_t instruction,                                                  \
        processor& __restrict processor                                                   \
    ) noexcept(!(PROC_ ## InsInstruction::CanThrow))                                      \
    {                                                                                     \
        _block_forceinline                                                                \
        PROC_Helper::Execute<PROC_ ## InsInstruction >(                                   \
            instruction,                                                                  \
            processor                                                                     \
        );                                                                                \
        if constexpr(                                                                     \
            (PROC_ ## InsInstruction::Flags & OpFlags::AlwaysThrows) != OpFlags::None     \
        )                                                                                 \
        {                                                                                 \
            xunreachable("Instruction is marked as AlwaysThrows but returned");           \
        }                                                                                 \
    }                                                                                     \
    class _StaticInit : public _instruction_initializer                                   \
    {                                                                                     \
    public:                                                                               \
        _StaticInit() :                                                                   \
            _instruction_initializer(                                                     \
                "PROC_" #InsInstruction,                                                  \
                PROC_ ## InsInstruction::OpMask,                                          \
                InsOpRef,                                                                 \
                &Execute,                                                                 \
                (PROC_ ## InsInstruction::Flags),                                         \
                uint32(PROC_ ## InsInstruction::Flags & OpFlags::ControlInstruction) != 0 \
            )                                                                             \
        {}                                                                                \
    } static _StaticInitObj;                                                              \
}                                                                                         \
_forceinline void PROC_ ## InsInstruction::SubExecute

namespace mips::instructions
{
	class _instruction_initializer
	{
	public:
		// TODO : is _nothrow safe? out-of-memory is possible
		_nothrow _instruction_initializer(
			const char* const __restrict name,
			const instruction_t instructionMask,
			const instruction_t instructionRef,
			const instructionexec_t exec,
			const OpFlags OpFlags,
			const bool control
		) noexcept
		{
#if !USE_STATIC_INSTRUCTION_SEARCH
			const InstructionInfo Procs{
				name,
				1,
				exec,
				OpFlags,
				{ .control = control }
			};
			FullProcInfo FullProc = {
				instructionMask,
				instructionRef,
				std::move(Procs)
			};
			instructions::GetStaticInitVars().g_ProcInfos.push_back(std::move(FullProc));
#else
			(void)name;
			(void)instructionMask;
			(void)instructionRef;
			(void)exec;
			(void)OpFlags;
			(void)control;
#endif
		}
	};

	// TODO : how do we compile-time conditionally apply [[noreturn]]...?
	class PROC_Helper final
	{
	public:
		PROC_Helper() = delete;

		template <typename InsT>
		_forceinline static void Execute(
			const instruction_t instruction,
			processor& __restrict processor
		) noexcept(!HasFlag(InsT::Flags, OpFlags::ControlInstruction) && !InsT::CanThrowInner)
		{
			if constexpr (HasFlag(InsT::Flags, OpFlags::ControlInstruction))
			{
				if _unlikely(processor.get_no_cti()) [[unlikely]]
				{
					CPU_Exception::throw_helper<CPU_Exception::Type::RI>(processor.get_program_counter());
				}
			}

			_block_forceinline
			InsT::SubExecute(
				instruction,
				processor
			);

			if constexpr (HasFlag(InsT::Flags, OpFlags::AlwaysThrows))
			{
				// zero register doesn't need to be cleared - will be cleared by `catch`.
				xunreachable("Should not be reachable - SubExecute should not have returned");
				return;
			}

			if constexpr (HasFlag(InsT::Flags, OpFlags::WritesGPRegister))
			{
				// Clear zero register
				_block_forceinline
				processor.set_register(0, 0U);
			}
		}

		template <typename reg_t, typename format_t, OpFlags Flags>
		_forceinline static _nothrow void _write_result(
			reg_t& __restrict dest,
			const format_t value
		) noexcept
		{
			dest.template set<format_t>(value);
		}
	};
	#pragma endregion Boilerplate

	namespace
	{
		template <uint32 Bits, CPU_Exception::Type EException>
		requires(std::has_single_bit(Bits))
		static _forceinline uint32 handle_misaligned_address(
			const processor& __restrict processor,
			const uptr_guest address
		)
		{
			static constexpr const uptr_guest mask = mips::make_bitmask<uptr_guest>((Bits - 1U) / 8U);

			const auto misaligned_address_handler = processor.get_misaligned_address_handling();
			if ((address & mask) != 0) [[unlikely]]
			{
				switch (misaligned_address_handler)
				{
					using enum processor::misaligned_address_handling;
					case exception:
						CPU_Exception::throw_helper<EException>(address);

					case align:
						return address & ~mask;

					case keep: [[likely]]
						return address;

					default: [[unlikely]]
						xunreachable("Unknown Misaligned Address Handler");
				}
			}

			return address;
		}

		namespace
		{
			[[nodiscard]]
			static _pure _nothrow bool check_llsc(const processor& __restrict processor, const uptr_guest address) noexcept
			{
				switch (processor.get_llsc_type())
				{
					case llsc::fine:
						return processor.get_load_link() == address;

					case llsc::coarse:
						return processor.get_load_link() != 0;

					case llsc::none:
						return true;

					default:
						xunreachable("unknown LLSC type");
				}
			}
		}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgcc-compat"

		namespace debug
		{
			template <uint32 RegOffset, uint32 RegSize> 
			static _forceinline void break_if_zero(const GPRegister<RegOffset, RegSize>& reg)
			{
				if (reg.is_zero())
				{
					__debugbreak();
				}
			}

			template <std::integral T>
			static _forceinline void break_if_zero(const T value)
			{
				if (value == 0)
				{
					__debugbreak();
				}
			}

			template <uint32 RegOffset, uint32 RegSize>
			static _forceinline bool is_zero(const GPRegister<RegOffset, RegSize>& reg)
			{
				return reg.is_zero();
			}

			template <std::integral T>
			static _forceinline bool is_zero(const T value)
			{
				return value == 0;
			}

			template <typename... TArgs>
			static _forceinline void break_if_all_zero(TArgs&&... args)
			{
				const bool result = (is_zero(std::forward<TArgs>(args)) && ...);
				if (result)
				{
					__debugbreak();
				}
			}
		}
	}

	ProcInstructionDef(
		ADD,
		(OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		#if __has_builtin(__builtin_sub_overflow)
		int32 result;
		const bool overflow = __builtin_add_overflow(rs.value<int32>(), rt.value<int32>(), &result);
		#elif defined(_MSC_VER) && _MSC_VER >= 1937
		int32 result;
		const bool overflow = _add_overflow_i32(0, rs.value<int32>(), rt.value<int32>(), &result);
		#else
		const int64 result = int64(rs.value<int32>()) + rt.value<int32>();
		const bool overflow = result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min();
		#endif
		if (overflow) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Ov>(processor.get_program_counter());
		}

		write_result(rd, int32(result));
	}

	// No support on MIPSr6
	/*
	ProcInstructionDef(
		ADDI,
		(OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00100000000000000000000000000000
	) (const instruction_t instruction, processor& __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 immediate = TinyInt<16>(instruction).sextend<int32>();

		const int64 result = int64(rs.value<int32>()) + immediate;
		if (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Ov>(processor.get_program_counter() );
		}

		write_result(rt, int32(result));
	}
	*/

	ProcInstructionDef(
		ADDIU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00100100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 immediate = TinyInt<16>(instruction).sextend<int32>();

		const uint32 result = rs.value<uint32>() + immediate;

		write_result(rt, result);
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		ADDIU_RSZERO,
		(OpFlags::WritesGPRegister),
		0b11111111111000000000000000000000,
		0b00100100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		GPRegister<16, 5> rt(instruction, processor);
		const int32 immediate = TinyInt<16>(instruction).sextend<int32>();

		const uint32 result = immediate;

		write_result(rt, result);
	}
#endif

	ProcInstructionDef(
		MOVE,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000001111111111111111,
		0b00100100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);

		const uint32 result = rs.value<uint32>();

		write_result(rt, result);
	}

	ProcInstructionDef(
		ADDIUPC,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000110000000000000000000,
		0b11101100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		GPRegister<21, 5> rs(instruction, processor);
		const int32 immediate = TinyInt<21>(instruction << 2).sextend<int32>();

		const uint32 result = processor.get_program_counter() + immediate;

		write_result(rs, result);
	}

	ProcInstructionDef(
		ADDU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>() + rt.value<uint32>();
		write_result(rd, result);
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		ADDU_RTZERO,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000111110000011111111111,
		0b00000000000000000000000000100001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>();

		write_result(rd, result);
	}
#endif

	ProcInstructionDef(
		ALIGN,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011100111111,
		0b01111100000000000000001000100000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 bp = TinyInt<2>(instruction).zextend<uint32>();

		const uint32 result = (rt.value<uint32>() << (8 * bp)) | (rs.value<uint32>() >> (32 - (8 * bp)));
		write_result(rd, result);
	}

	ProcInstructionDef(
		ALUIPC,
		(OpFlags::WritesGPRegister),
		0b11111100000111110000000000000000,
		0b11101100000111110000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		GPRegister<21, 5> rs(instruction, processor);
		const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

		const uint32 result =  ~0xFFFF & uint32(processor.get_program_counter() + immediate);
		write_result(rs, result);
	}

	ProcInstructionDef(
		AND,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100100
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>() & rt.value<uint32>();
		write_result(rd, result);
	}

	ProcInstructionDef(
		ANDI,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00110000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 immediate = TinyInt<16>(instruction).zextend<uint32>();

		const uint32 result = rs.value<uint32>() & immediate;
		write_result(rt, result);
	}

	ProcInstructionDef(
		AUI,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00111100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

		const uint32 result = rs.value<uint32>() + immediate;
		write_result(rt, result);
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		AUI_RSZERO,
		(OpFlags::WritesGPRegister),
		0b11111111111000000000000000000000,
		0b00111100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		GPRegister<16, 5> rt(instruction, processor);
		const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

		const uint32 result = immediate;
		write_result(rt, result);
	}

	// specialized
	ProcInstructionDef(
		AUI_IMMZERO,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000001111111111111111,
		0b00111100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		//const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

		const uint32 result = rs.value<uint32>();
		write_result(rt, result);
	}

	// specialized
	ProcInstructionDef(
		AUI_RSIMMZERO,
		(OpFlags::WritesGPRegister),
		0b11111111111000001111111111111111,
		0b00111100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		//const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		//const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

		const uint32 result = 0U;
		write_result(rt, result);
	}
#endif

	ProcInstructionDef(
		AUIPC,
		(OpFlags::WritesGPRegister),
		0b11111100000111110000000000000000,
		0b11101100000111100000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		GPRegister<21, 5> rs(instruction, processor);
		const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>(); // this is intentional

		const uint32 result = processor.get_program_counter() + immediate;
		write_result(rs, result);
	}

	ProcInstructionDef(
		BAL,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
		0b11111111111111110000000000000000,
		0b00000100000100010000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const int32 immediate = TinyInt<18>(instruction << 2).sextend<int32>();
		const uptr_guest pc = processor.get_program_counter();
		
		const uptr_guest address = pc + 4 + immediate;

		processor.set_link(pc + 8);
		processor.delay_branch(address);
	}

	ProcInstructionDef(
		BALC,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch),
		0b11111100000000000000000000000000,
		0b11101000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const int32 immediate = TinyInt<28>(instruction << 2).sextend<int32>();
		const uptr_guest pc = processor.get_program_counter();
		
		const uptr_guest address = pc + 4 + immediate;

		processor.set_link(pc + 4);
		processor.compact_branch(address);
		// BALC does not have a forbidden slot as it is unconditional compact.
	}

	ProcInstructionDef(
		BC,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch),
		0b11111100000000000000000000000000,
		0b11001000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const int32 immediate = TinyInt<28>(instruction << 2).sextend<int32>();
		const uptr_guest pc = processor.get_program_counter();
		
		const uptr_guest address = pc + 4 + immediate;

		processor.compact_branch(address);
		// BC does not have a forbidden slot as it is unconditional compact.
	}

	ProcInstructionDef(
		BEQ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b00010000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (rs.value<uint32>() == rt.value<uint32>())
		{
			const int32 immediate = TinyInt<18>(instruction << 2).sextend<int32>();
			const uptr_guest pc = processor.get_program_counter();

			const uptr_guest address = pc + 4 + immediate;
			processor.delay_branch(address);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	ProcInstructionDef(
		BGEZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000111110000000000000000,
		0b00000100000000010000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		
		if (rs.value<int32>() >= 0)
		{
			const int32 immediate = TinyInt<18>(instruction << 2).sextend<int32>();
			const uptr_guest pc = processor.get_program_counter();

			const uptr_guest address = pc + 4 + immediate;
			processor.delay_branch(address);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	// specialized - POP06
	ProcInstructionDef(
		BLEZALC,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111111111000000000000000000000,
		0b00011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<16, 5> rt(instruction, processor);
		const uptr_guest pc = processor.get_program_counter();
		
		if (!rt.is_zero()) [[likely]] // BLEZALC
		{
			processor.set_link(pc + 4);
			if (rt.value<int32>() <= 0)
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();

				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	ProcInstructionDef(
		POP06,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b00011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (!rs.is_zero() && rs == rt) // BGEZALC
		{
			const uptr_guest pc = processor.get_program_counter();
			processor.set_link(pc + 4);
			if (rt.value<int32>() >= 0)
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else if (rs != rt && !rs.is_zero() && !rt.is_zero()) // BGEUC
		{
			if (rs.value<uint32>() >= rt.value<uint32>())
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	// specialized - POP07
	ProcInstructionDef(
		BGTZALC,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111111111000000000000000000000,
		0b00011100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<16, 5> rt(instruction, processor);
		const uptr_guest pc = processor.get_program_counter();
		
		if (!rt.is_zero()) [[likely]] // BGTZALC
		{
			processor.set_link(pc + 4);
			if (rt.value<int32>() > 0)
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	ProcInstructionDef(
		POP07,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b00011100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (!rs.is_zero() && rs == rt) // BLTZALC
		{
			const uptr_guest pc = processor.get_program_counter();
			processor.set_link(pc + 4);
			if (rt.value<int32>() < 0)
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else if (rs != rt && !rs.is_zero() && !rt.is_zero()) // BLTUC
		{
			if (rs.value<uint32>() < rt.value<uint32>())
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest pc = processor.get_program_counter();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	ProcInstructionDef(
		POP10,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b00100000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);

		if (rs.is_zero() && !rt.is_zero()) // BEQZALC
		{
			const uptr_guest pc = processor.get_program_counter();
			processor.set_link(pc + 4);
			if (rt.value<int32>() == 0)
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else if (!rs.is_zero() && !rt.is_zero() && rs.get_register() < rt.get_register()) // BEQC
		{
			if (rt.value<uint32>() == rs.value<uint32>())
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else if (rs.get_register() >= rt.get_register()) // BOVC
		{
			const int64 result = int64(rs.value<int32>()) + rt.value<int32>();
			const bool overflow = (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min());

			if (overflow)
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	ProcInstructionDef(
		POP30,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b01100000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
	

		if (rs.is_zero() && !rs.is_zero()) // BNEZALC
		{
			const uptr_guest pc = processor.get_program_counter();
			processor.set_link(pc + 4);
			if (rt.value<int32>() != 0)
			{
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else if (!rs.is_zero() && !rt.is_zero() && rs.get_register() < rt.get_register()) // BNEC
		{
			if (rt.value<uint32>() != rs.value<uint32>())
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else if (rs.get_register() >= rt.get_register()) // BNVC
		{
			const int64 result = int64(rs.value<int32>()) + rt.value<int32>();
			const bool overflow = (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min());

			if (!overflow)
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	// specialized - POP26
	ProcInstructionDef(
		BLEZC,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111111111000000000000000000000,
		0b01011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (!rt.is_zero()) // BLEZC
		{
			if (rt.value<int32>() <= 0)
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	ProcInstructionDef(
		POP26,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b01011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (!rs.is_zero() && !rt.is_zero())
		{
			if (rs.get_register() == rt.get_register()) // BGEZC
			{
				if (rt.value<int32>() >= 0)
				{
					const uptr_guest pc = processor.get_program_counter();
					const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
					const uptr_guest address = pc + 4 + offset;
					processor.compact_branch(address);
				}
				else
				{
					processor.set_no_cti();
				}
			}
			else // BGEC / BLEC
			{
				if (rs.value<int32>() >= rt.value<int32>())
				{
					const uptr_guest pc = processor.get_program_counter();
					const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
					const uptr_guest address = pc + 4 + offset;
					processor.compact_branch(address);
				}
				else
				{
					processor.set_no_cti();
				}
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	// specialized - POP27
	ProcInstructionDef(
		BGTZC,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111111111000000000000000000000,
		0b01011100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (!rt.is_zero()) // BGTZC
		{
			if (rt.value<int32>() > 0)
			{
				const uptr_guest pc = processor.get_program_counter();
				const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
				const uptr_guest address = pc + 4 + offset;
				processor.compact_branch(address);
			}
			else
			{
				processor.set_no_cti();
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	ProcInstructionDef(
		POP27,
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b01011100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (!rs.is_zero() && !rt.is_zero())
		{
			if (rs.get_register() == rt.get_register()) // BLTZC
			{
				if (rt.value<int32>() < 0)
				{
					const uptr_guest pc = processor.get_program_counter();
					const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
					const uptr_guest address = pc + 4 + offset;
					processor.compact_branch(address);
				}
				else
				{
					processor.set_no_cti();
				}
			}
			else // BLTC / BGTC
			{
				if (rs.value<int32>() < rt.value<int32>())
				{
					const uptr_guest pc = processor.get_program_counter();
					const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
					const uptr_guest address = pc + 4 + offset;
					processor.compact_branch(address);
				}
				else
				{
					processor.set_no_cti();
				}
			}
		}
		else [[unlikely]]
		{
			const uptr_guest pc = processor.get_program_counter();
			CPU_Exception::throw_helper<CPU_Exception::Type::RI>(pc);
		}
	}

	// specialized - POP66
	ProcInstructionDef(
		BEQZC, // POP66 where rs != 0
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b11011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		
		const int32 value = rs.value<int32>();

		if (value == 0)
		{
			const uptr_guest pc = processor.get_program_counter();
			const int32 offset = TinyInt<23>(instruction << 2).sextend<int32>();

			const uptr_guest address = pc + 4 + offset;
			processor.compact_branch(address);
		}
		else
		{
			processor.set_no_cti();
		}
	}

	// specialized - POP66
	ProcInstructionDef(
		JIC, // PO66 where rs == 0
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::ReadsGPRegister),
		0b11111111111000000000000000000000,
		0b11011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();
		
		const uptr_guest address = rt.value<uptr_guest>() + offset;

		processor.compact_branch(address);
	}

	// specialized - POP76
	ProcInstructionDef(
		BNEZC, // POP76 where rs != 0
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b11111000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
	

		if (rs.value<int32>() != 0)
		{
			const uptr_guest pc = processor.get_program_counter();
			const int32 offset = TinyInt<23>(instruction << 2).sextend<int32>();

			const uptr_guest address = pc + 4 + offset;
			processor.compact_branch(address);
		}
		else
		{
			processor.set_no_cti();
		}
	}

	// specialized - POP76
	ProcInstructionDef(
		JIALC, // POP76 where rs == 0
		(OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::ReadsGPRegister),
		0b11111111111000000000000000000000,
		0b11111000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();
		const uptr_guest pc = processor.get_program_counter();
		
		const uptr_guest address = rt.value<uptr_guest>() + offset;

		processor.set_link(pc + 4);
		processor.compact_branch(address);
	}

	ProcInstructionDef(
		BGTZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000111110000000000000000,
		0b00011100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		
		if (rs.value<int32>() > 0)
		{
			const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
			const uptr_guest pc = processor.get_program_counter();

			const uptr_guest address = pc + 4 + offset;
			processor.delay_branch(address);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	ProcInstructionDef(
		BITSWAP,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01111100000000000000000000100000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		
		// TODO optimize
		union register_alias
		{
			uint32 reg;
			uint8 bytes[4];
		} reg {
			.reg = rt.value<uint32>()
		};

		for (uint8& byte : reg.bytes) {
			// http://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
			uint8 temp = byte;
			temp = uint8((temp & 0xF0) >> 4) | uint8((temp & 0x0F) << 4);
			temp = uint8((temp & 0xCC) >> 2) | uint8((temp & 0x33) << 2);
			temp = uint8((temp & 0xAA) >> 1) | uint8((temp & 0x55) << 1);
			byte = temp;
		}

		write_result<>(rd, reg.reg);
	}

	ProcInstructionDef(
		BLEZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000111110000000000000000,
		0b00011000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		
		if (rs.value<int32>() <= 0)
		{
			const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
			const uptr_guest pc = processor.get_program_counter();

			const uptr_guest address = pc + 4 + offset;
			processor.delay_branch(address);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	ProcInstructionDef(
		BLTZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000111110000000000000000,
		0b00000100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		
		if (rs.value<int32>() < 0)
		{
			const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
			const uptr_guest pc = processor.get_program_counter();

			const uptr_guest address = pc + 4 + offset;
			processor.delay_branch(address);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	ProcInstructionDef(
		BNE,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b00010100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		
		if (rs.value<uint32>() != rt.value<uint32>())
		{
			const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
			const uptr_guest pc = processor.get_program_counter();

			const uptr_guest address = pc + 4 + offset;
			processor.delay_branch(address);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	ProcInstructionDef(
		BREAK,
		(OpFlags::Throws | OpFlags::AlwaysThrows),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000001101
	) (const instruction_t instruction, processor & __restrict processor) _noreturn_post
	{
		const uint32 code = TinyInt<20>(instruction >> 6).zextend<uint32>();

		CPU_Exception::throw_helper<CPU_Exception::Type::Bp>(processor.get_program_counter(), code );
	}

	enum class OpCacheType : uint8_t
	{
		Instruction = 0b00,
		Data = 0b01,
		Tertiary = 0b10,
		Secondary = 0b11
	};

	ProcInstructionDef(
		CACHE,
		(OpFlags::ReadsGPRegister),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000100101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> base(instruction, processor);
		const OpCacheType op_cache = static_cast<OpCacheType>(TinyInt<2>(instruction >> 16).zextend<uint32>());
		const uint32 op_code = TinyInt<3>(instruction >> 18).zextend<uint32>();
		const uint32 offset = TinyInt<9>(instruction >> 7).sextend<uint32>();

		const uptr_guest address = base.get_register() + offset;

		processor.mem_poke<char>(address); // We still want to touch the address.

		// We don't have anything to do with anything that's not the instruction cache
		if (op_cache != OpCacheType::Instruction)
		{
			return;
		}

		switch (op_code)
		{
			// Index Invalidate
			case 0b000:
			{
				// invalidate
			} break;

			// Index Load Tag
			case 0b001:
			{
				// read tag into TagLo/TagHi
			} break;

			// Index Store Tag
			case 0b010:
			{
				// write tag from TagLo/TagHi
			} break;

			// Hit Invalidate
			case 0b100:
			{
				// invalidate
				// broadcast to all coherent caches
			} break;

			// Fill
			case 0b101:
			{
				// Fill cache from specified address
			} break;

			// Hit Writeback
			case 0b110:
			{
				// if block is valid and dirty, write block to memory
				// broadcast to all coherent caches
			} break;

			// Fetch and Lock
			case 0b111:
			{
				// if block does not contain address, fill from memory. Perform writeback if required.
				// set to valid/locked.
			} break;
		}
	}

	ProcInstructionDef(
		CACHEE,
		(OpFlags::ReadsGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000110110
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		const OpCacheType op_cache = static_cast<OpCacheType>(TinyInt<2>(instruction >> 16).zextend<uint32>());
		const uint32 op_code = TinyInt<3>(instruction >> 18).zextend<uint32>();
		const uint32 offset = TinyInt<9>(instruction >> 7).sextend<uint32>();

		const uptr_guest address = base.get_register() + offset;

		processor.mem_poke<char>(address); // We still want to touch the address.

		// We don't have anything to do with anything that's not the instruction cache
		if (op_cache != OpCacheType::Instruction)
		{
			return;
		}

		switch (op_code)
		{
			// Index Invalidate
			case 0b000:
			{
				// invalidate
			} break;

			// Index Load Tag
			case 0b001:
			{
				// read tag into TagLo/TagHi
			} break;

			// Index Store Tag
			case 0b010:
			{
				// write tag from TagLo/TagHi
			} break;

			// Hit Invalidate
			case 0b100:
			{
				// invalidate
				// broadcast to all coherent caches
			} break;

			// Fill
			case 0b101:
			{
				// Fill cache from specified address
			} break;

			// Hit Writeback
			case 0b110:
			{
				// if block is valid and dirty, write block to memory
				// broadcast to all coherent caches
			} break;

			// Fetch and Lock
			case 0b111:
			{
				// if block does not contain address, fill from memory. Perform writeback if required.
				// set to valid/locked.
			} break;
		}
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter(), 0);
#endif
	}

	ProcInstructionDef(
		GINVI,
		(OpFlags::ReadsGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'00000'1111111111111'11'111111,
		0b011111'00000'0000000000000'00'111101
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
		//const GPRegister<21, 5> rs(instruction, processor);

		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter(), 0);

		// TODO - clear instruction cache
	}

	ProcInstructionDef(
		CLO,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000111110000011111111111,
		0b00000000000000000000000001100001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 source = rs.value<uint32>();

		uint32 result;
#if _MSC_VER && !__clang__
		if (platform::get_host_features().lzcnt)
		{
			result = __lzcnt(~source);
		}
		else
#elif __has_builtin(__builtin_ia32_lzcnt_u32) && 0
		if (platform::get_host_features().lzcnt)
		{
			result = __builtin_ia32_lzcnt_u32(~source);
		}
		else
#endif
		{
			result = std::countl_one(source);
		}

		write_result<>(rd, result);
	}

	ProcInstructionDef(
		CLZ,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000111110000011111111111,
		0b00000000000000000000000001010000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 source = rs.value<uint32>();

		uint32 result;
#if _MSC_VER
		if (platform::get_host_features().lzcnt)
		{
			result = __lzcnt(source);
		}
		else
#elif __has_builtin(__builtin_ia32_lzcnt_u32) && 0
		if (platform::get_host_features().lzcnt)
		{
			result = __builtin_ia32_lzcnt_u32(source);
		}
		else
#endif
		{
			result = std::countl_zero(source);
		}

		write_result<>(rd, result);
	}

	ProcInstructionDef(
		DIV,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000010011010
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		if (rt.value<int32>() == 0) [[unlikely]]
		{
			write_result(rd, int32(0));  // UNPREDICTABLE
		}
		else
		{
			const int32 result = rs.value<int32>() / rt.value<int32>();

			write_result(rd, int32(result));
		}
	}

	ProcInstructionDef(
		MOD,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000011011010
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		if (rt.value<int32>() == 0) [[unlikely]]
		{
			write_result(rd, int32(0));  // UNPREDICTABLE
		}
		else
		{
			const int32 result = rs.value<int32>() % rt.value<int32>();

			write_result(rd, int32(result));
		}
	}

	ProcInstructionDef(
		DIVU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000010011011
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		if (rt.value<uint32>() == 0) [[unlikely]]
		{
			write_result(rd, uint32(0));  // UNPREDICTABLE
		}
		else
		{
			const uint32 result = rs.value<uint32>() / rt.value<uint32>();

			write_result(rd, uint32(result));
		}
	}

	ProcInstructionDef(
		MODU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000011011011
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		if (rt.value<uint32>() == 0) [[unlikely]]
		{
			write_result(rd, uint32(0));  // UNPREDICTABLE
		}
		else
		{
			const uint32 result = rs.value<uint32>() % rt.value<uint32>();

			write_result(rd, uint32(result));
		}
	}

	ProcInstructionDef(
		EHB,
		(OpFlags::Hazard),
		0b11111111111111111111111111111111,
		0b00000000000000000000000011000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		if (processor.handles_memory_hazards()) [[unlikely]]
		{
			processor.clear_memory_hazards(memory_hazards::all_data);
		}
		if (processor.handles_execution_hazards()) [[unlikely]]
		{
			processor.clear_execution_hazards();
		}
	}

	ProcInstructionDef(
		EXT,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000111111,
		0b01111100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 msbd = TinyInt<5>(instruction >> 11).zextend<uint32>();
		const uint32 lsb = TinyInt<5>(instruction >> 6).zextend<uint32>();

		if (lsb + msbd > 31) [[unlikely]]
		{
			write_result(rt, uint32(-1)); // UNPREDICTABLE
		}

		uint32 value = rs.value<uint32>();

		// TODO : Clang does _not_ like inlining this as it isn't being built with BMI globally-enabled
		#if !defined(__clang__)
		if (platform::get_host_features().bmi1)
		{
			value = _bextr_u32(value, lsb, msbd + 1);
		}
		else
		#endif
		{
			value >>= lsb;
			value &= ~(uint64_t(std::numeric_limits<uint32_t>::max()) << (msbd + 1));
			// value &= ~(0x1FFFFFFFEull << (msbd));
		}

		write_result(rt, value);
	}

	ProcInstructionDef(
		INS,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000111111,
		0b01111100000000000000000000000100
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 msb = TinyInt<5>(instruction >> 11).zextend<uint32>();
		const uint32 lsb = TinyInt<5>(instruction >> 6).zextend<uint32>();

		if (lsb > msb) [[unlikely]]
		{
			write_result(rt, uint32(-1)); // UNPREDICTABLE
		}

		const uint32 size = msb - lsb + 1;

		const uint32 rs_val = rs.value<uint32>();
		uint32 rt_val = rt.value<uint32>();

		/*
		if (platform::get_host_features().bmi2)
		{
			rt_val = _pdep_u32()
			value = _bextr_u32(value, lsb, msbd + 1);
		}
		else
		*/
		{
			const uint32 extracted_val = rs_val & Bits(size);
			rt_val &= ~(Bits(size) << lsb);
			rt_val |= extracted_val << lsb;
		}

		write_result(rt, rt_val);
	}

	ProcInstructionDef(
		J,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
		0b11111100000000000000000000000000,
		0b00001000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const uint32 instr_index = TinyInt<28>(instruction << 2).zextend<uint32>();
#if WITH_EXP_OPTIM
		xassert((instr_index & 0b11U) == 0U);
		xassert(instr_index > 3U);
		xassert(instr_index < (bit<uint32, 28> - 4));
#endif
		
		const uptr_guest pc = processor.get_program_counter();
#if WITH_EXP_OPTIM
		xassert((pc & 0b11U) == 0U);
#endif

		const uptr_guest address = (pc & HighBits(4)) | instr_index;

		processor.delay_branch(address);
	}

	ProcInstructionDef(
		JAL,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
		0b11111100000000000000000000000000,
		0b00001100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const uint32 instr_index = TinyInt<28>(instruction << 2).zextend<uint32>();
#if WITH_EXP_OPTIM
		xassert((instr_index & 0b11U) == 0U);
		xassert(instr_index > 3U);
		xassert(instr_index < (bit<uint32, 28> - 4));
#endif

		const uptr_guest pc = processor.get_program_counter();
#if WITH_EXP_OPTIM
		xassert((pc & 0b11U) == 0U);
#endif

		const uptr_guest address = (pc & HighBits(4)) | instr_index;

		processor.set_link(pc + 8);
		processor.delay_branch(address);
	}

	ProcInstructionDef(
		JALR,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b111111'00000'11111'00000'1'0000'111111,
		0b000000'00000'00000'00000'0'0000'001001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		rd.set<uptr_guest>(processor.get_program_counter() + 8);
		processor.delay_branch(rs.value<uptr_guest>());
	}

	ProcInstructionDef(
		JALR_HB,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::Hazard | OpFlags::InstructionHazard),
		0b111111'00000'11111'00000'1'0000'111111,
		0b000000'00000'00000'00000'1'0000'001001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		if (processor.handles_instruction_hazards())
		{
			processor.set_flags(processor::flag::instruction_hazard | processor::flag::instruction_hazard_delay_branch);
		}

		PROC_JALR::SubExecute(instruction, processor);
	}

	ProcInstructionDef(
		JR,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister),
		0b111111'00000'1111111111'1'0000'111111,
		0b000000'00000'0000000000'0'0000'001001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);

		processor.delay_branch(rs.value<uptr_guest>());
	}

	ProcInstructionDef(
		JR_HB,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI | OpFlags::ReadsGPRegister | OpFlags::Hazard | OpFlags::InstructionHazard),
		0b111111'00000'1111111111'1'0000'111111,
		0b000000'00000'0000000000'1'0000'001001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		if (processor.handles_instruction_hazards())
		{
			processor.set_flags(processor::flag::instruction_hazard | processor::flag::instruction_hazard_delay_branch);
		}

		PROC_JR::SubExecute(instruction, processor);
	}

	ProcInstructionDef(
		LB,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b10000000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;
		const int32 value = int32(processor.mem_fetch<int8>(address));
		write_result(rt, value);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LBE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000101100
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;
		const int32 value = int32(processor.mem_fetch<int8>(address));
		write_result(rt, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LBU,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b10010000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;
		const uint32 value = uint32(processor.mem_fetch<uint8>(address));
		write_result(rt, value);

#pragma message("Implement Watch")
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		LBU_OFFZERO,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000001111111111111111,
		0b10010000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);

		#if 0
		if (base.is_zero())
		{
			std::puts("\nbase = 0\n");
		}
		if (rt.is_zero())
		{
			std::puts("\nrt = 0\n");
		}
		#endif

		const uptr_guest address = base.value<uptr_guest>();
		const uint32 value = uint32(processor.mem_fetch<uint8>(address));
		write_result(rt, value);

#pragma message("Implement Watch")
	}
#endif

	ProcInstructionDef(
		LBUE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000101000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;
		const uint32 value = uint32(processor.mem_fetch<uint8>(address));
		write_result(rt, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LH,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b10000100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<16, CPU_Exception::Type::AdEL>(processor, address);

		const int32 value = int32(processor.mem_fetch<int16>(address));
		write_result(rt, value);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LHE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000101101
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<16, CPU_Exception::Type::AdEL>(processor, address);

		const int32 value = int32(processor.mem_fetch<int16>(address));
		write_result(rt, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LHU,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b10010100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<16, CPU_Exception::Type::AdEL>(processor, address);

		const uint32 value = uint32(processor.mem_fetch<uint16>(address));
		write_result(rt, value);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LHUE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000101001
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<16, CPU_Exception::Type::AdEL>(processor, address);

		const uint32 value = uint32(processor.mem_fetch<uint16>(address));
		write_result(rt, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LL,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000110110
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		// ATOMIC
		const uptr_guest address = base.value<uptr_guest>() + offset;

		// This is _required_ for LL rather than being implementation-defined
		if ((address & 0b11) != 0) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(address);
		}

		processor.set_load_link(address);

		const int32 value = processor.mem_fetch<int32>(address);
		write_result(rt, value);
#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LLE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000101110
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		// ATOMIC
		const uptr_guest address = base.value<uptr_guest>() + offset;

		// This is _required_ for LL rather than being implementation-defined
		if ((address & 0b11) != 0) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(address);
		}

		processor.set_load_link(address);
		const int32 value = processor.mem_fetch<int32>(address);
		write_result(rt, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LLWP,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b01111100000000000000000001110110
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		// ATOMIC
		const uptr_guest address = base.value<uptr_guest>();

		if ((address & 0b111U) != 0U) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(address );
		}

		processor.set_load_link(address);
		const uint64 value = processor.mem_fetch<uint64>(address);
		rt.set(uint32(value));
		rd.set(uint32(value >> 32));

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LLWPE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000011111111111,
		0b01111100000000000000000001101110
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		// ATOMIC
		const uptr_guest address = base.value<uptr_guest>();

		if ((address & 0b111U) != 0U) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(address );
		}

		processor.set_load_link(address);
		const uint64 value = processor.mem_fetch<uint64>(address);
		rt.set(uint32(value));
		rd.set(uint32(value >> 32));
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LSA,
		( OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011100111111,
		0b00000000000000000000000000000101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = TinyInt<2>(instruction >> 6).zextend<uint32>() + 1U;

		const uint32 result = (rs.value<uint32>() << sa) + rt.value<uint32>();

		write_result(rd, result);
	}

	ProcInstructionDef(
		LW,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b10001100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdEL>(processor, address);

		const int32 value = processor.mem_fetch<int32>(address);
		write_result(rt, value);

#pragma message("Implement Watch")
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		LW_OFFZERO,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000001111111111111111,
		0b10001100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);

		uptr_guest address = base.value<uptr_guest>();

		address = handle_misaligned_address<32, CPU_Exception::Type::AdEL>(processor, address);

		const int32 value = processor.mem_fetch<int32>(address);
		write_result(rt, value);

#pragma message("Implement Watch")
	}
#endif

	ProcInstructionDef(
		LWE,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000101111
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdEL>(processor, address);

		const int32 value = processor.mem_fetch<int32>(address);
		write_result(rt, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		LWPC,
		(OpFlags::Load | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000110000000000000000000,
		0b11101100000010000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		GPRegister<21, 5> rs(instruction, processor);
		const int32 offset = TinyInt<21>(instruction << 2).sextend<int32>();

		const uptr_guest address = processor.get_program_counter() + offset;
		xassert((address & 0b11U) == 0U);
		const int32 value = processor.mem_fetch<int32>(address);
		write_result(rs, value);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		MUL,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000010011000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint64 result = uint64(int64(rs.value<int32>()) * int64(rt.value<int32>()));

		write_result(rd, uint32(result));
	}

	ProcInstructionDef(
		MUH,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000011011000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint64 result = uint64(int64(rs.value<int32>()) * int64(rt.value<int32>()));

		write_result(rd, uint32(result >> 32));
	}

	ProcInstructionDef(
		MULU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000010011001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint64 result = uint64(uint64(rs.value<uint32>()) * uint64(rt.value<uint32>()));

		write_result(rd, uint32(result));
	}

	ProcInstructionDef(
		MUHU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000011011001
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint64 result = uint64(uint64(rs.value<uint32>()) * uint64(rt.value<uint32>()));

		write_result(rd, uint32(result >> 32));
	}

	ProcInstructionDef(
		NAL,
		(OpFlags::ControlInstruction | OpFlags::SetNoCTI),
		0b11111111111111110000000000000000,
		0b00000100000100000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{		
		processor.set_link(processor.get_program_counter() + 8);
		processor.set_no_cti();
	}

	ProcInstructionDef(
		NOP, // technically SLL $0, $0, 0 - given own slot for performance.
		(OpFlags::None),
		0b11111111111111111111111111111111,
		0b00000000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		// Intentionally Empty
	}

	ProcInstructionDef(
		NOR,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100111
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = ~(rs.value<uint32>() | rt.value<uint32>());

		write_result(rd, result);
	}

	ProcInstructionDef(
		OR,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>() | rt.value<uint32>();

		write_result(rd, result);
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		OR_RSZERO,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b00000000000000000000000000100101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rt.value<uint32>();

		write_result(rd, result);
	}

	// specialized
	ProcInstructionDef(
		OR_RTZERO,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000111110000011111111111,
		0b00000000000000000000000000100101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		//const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>();

		write_result(rd, result);
	}

	// specialized
	ProcInstructionDef(
		OR_RSRTZERO,
		(OpFlags::WritesGPRegister),
		0b11111111111111110000011111111111,
		0b00000000000000000000000000100101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		//const GPRegister<21, 5> rs(instruction, processor);
		//const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = 0U;

		write_result(rd, result);
	}
#endif

	ProcInstructionDef(
		ORI,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00110100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 offset = TinyInt<16>(instruction).zextend<uint32>();

		const uint32 result = rs.value<uint32>() | offset;

		write_result(rt, result);
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		ORI_RSZERO,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000000000000000,
		0b00110100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 offset = TinyInt<16>(instruction).zextend<uint32>();

		const uint32 result = offset;

		write_result(rt, result);
	}
#endif

	ProcInstructionDef(
		PAUSE,
		(OpFlags::ControlInstruction),
		0b11111111111111111111111111111111,
		0b00000000000000000000000101000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		// TODO : it is perfectly valid for the program to wait forever.
		// We just lack a mechanism to do that, as it isn't a stream of `PAUSE`s.
#pragma message ("make wait for LLBit to clear")
	}

	ProcInstructionDef(
		PREF,
		(OpFlags::ReadsGPRegister),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000110101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		//const GPRegister<21, 5> base(instruction, processor);
		//const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		//const uptr_guest address = base.value<uptr_guest>() + offset;

		//processor.mem_fetch<char>(address); // Does NOT throw address-related exceptions.
	}

	ProcInstructionDef(
		PREFE,
		(OpFlags::ReadsGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000100011
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		//const uptr_guest address = base.value<uptr_guest>() + offset;

		//processor.mem_fetch<char>(address); // Does NOT throw address-related exceptions.
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	ProcInstructionDef(
		RDHWR,
		(OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000011000111111,
		0b01111100000000000000000000111011
	) (const instruction_t instruction, processor & __restrict processor)
	{
		GPRegister<16, 5> rt(instruction, processor);
		const GPRegister<11, 5> rd(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction >> 6).zextend<uint32>();
		const uint32 reg_number = rd.get_register();

		const auto make_case = [] _forceinline_lambda((const uint32 _register, const uint32 _selector) noexcept)
		{
			return (uint64(_selector) << 32) | _register;
		};

		// only handle 0:1 and 0:29, as MUSL uses that.
		switch ([[maybe_unused]] const uint64 selected_reg = make_case(reg_number, selector))
		{
			case make_case(0, 0): // CPUNum
				write_result(rt, 0);
				return;
			case make_case(1, 0): // SYNCI_Step
				write_result(rt, processor.get_cache_line_size());
				return;
			case make_case(2, 0): // CC
				// TODO : COP `Count` register
				write_result(rt, uint32(processor.get_instruction_count()));
				return;
			case make_case(3, 0): // CCRes
				write_result(rt, 1); // CC register increments every CPU cycle
				return;
			case make_case(4, 0): // PerfCtr
				// TODO : figure out how to implement this
				// It's defined here: https://training.mips.com/cps_mips/PDF/CM_Performance_Counters.pdf
				// I'm not sure yet how this is supposed to work, though.
				break;
			case make_case(5, 0): // XNP
				write_result(rt, processor.supports_llsc() ? 1 : 0);
				return;
			case make_case(29, 0):
				write_result(rt, processor.user_value);
				return;
			default: [[unlikely]]
				break;
		}

#pragma message("Implement when COP0 is finished")
		CPU_Exception::throw_helper<CPU_Exception::Type::Impl1>(processor.get_program_counter() );
	}

	ProcInstructionDef(
		ROTR, // specialization of SRL
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000000000111111,
		0b00000000001000000000000000000010
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

		const uint32 rt_val = rt.value<uint32>();
		const uint32 result = (rt_val >> sa) | (rt_val << (32 - sa));

		write_result(rd, result);
	}

	ProcInstructionDef(
		ROTRV, // specialization of SRLV
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000001000110
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 sa = rs.value<uint32>() & Bits(5);
		const uint32 rt_val = rt.value<uint32>();
		const uint32 result = (rt_val >> sa) | (rt_val << (32 - sa));

		write_result(rd, result);
	}

	ProcInstructionDef(
		SB,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000000000,
		0b10100000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		//debug::break_if_zero(base);
		//debug::break_if_zero(rt);
		//debug::break_if_zero(offset);
		//debug::break_if_all_zero(base, rt);
		//debug::break_if_all_zero(base, offset);
		//debug::break_if_all_zero(rt, offset);
		//debug::break_if_all_zero(base, rt, offset);
		//debug::break_if_all_zero(base, rt, offset, 1);

		const uptr_guest address = base.value<uptr_guest>() + offset;
		const uint8 value = rt.value<uint8>();
		processor.mem_write(address, value);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SBE,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000011100
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;
		const uint8 value = rt.value<uint8>();
		processor.mem_write(address, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SC,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned ),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000100110
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;

		if ((address & 0b11U) != 0U) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdES>(address);
		}

		if (check_llsc(processor, address))
		{
			const uint32 value = rt.value<uint32>();
			processor.mem_write(address, value); // TODO : also conditionally clears load-link
			rt.set<uint32>(1);
		}
		else
		{
			rt.set<uint32>(0);
		}

		processor.set_load_link(0);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SCE,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::ThrowsMisaligned | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000011110
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		const uptr_guest address = base.value<uptr_guest>() + offset;

		if ((address & 0b11U) != 0U) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdES>(address);
		}

		if (check_llsc(processor, address))
		{
			const uint32 value = rt.value<uint32>();
			processor.mem_write(address, value); // TODO : also conditionally clears load-link
			rt.set<uint32>(1);
		}
		else
		{
			rt.set<uint32>(0);
		}

		processor.set_load_link(0);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SCWP,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b01111100000000000000000001100110
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const GPRegister<11, 5> rd(instruction, processor);

		const uptr_guest address = base.value<uptr_guest>();

		if ((address & 0b111U) != 0U) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdES>(address);
		}

		if (check_llsc(processor, address))
		{
			const uint64 value = rt.value<uint32>() | (uint64(rd.value<uint32>()) << 32);
			processor.mem_write(address, value);
			rt.set<uint32>(1);
		}
		else
		{
			rt.set<uint32>(0);
		}

		processor.set_load_link(0);
#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SCWPE,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000011111111111,
		0b01111100000000000000000001011110
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const GPRegister<11, 5> rd(instruction, processor);

		const uptr_guest address = base.value<uptr_guest>();

		if ((address & 0b111U) != 0U) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::AdES>(address );
		}

		if (check_llsc(processor, address))
		{
			const uint64 value = rt.value<uint32>() | (uint64(rd.value<uint32>()) << 32);
			processor.mem_write(address, value);
			rt.set<uint32>(1);
		}
		else
		{
			rt.set<uint32>(0);
		}

		processor.set_load_link(0);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SDBBP,
		(OpFlags::Throws | OpFlags::AlwaysThrows),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000001110
	) (const instruction_t instruction, processor & __restrict processor) _noreturn_post
	{
		const uint32 code = TinyInt<20>(instruction >> 6).zextend<uint32>();

#pragma message("should do special things with SRBI bit in COP0, or Debug bit")

		CPU_Exception::throw_helper<CPU_Exception::Type::Bp>(processor.get_program_counter(), code );
	}

	ProcInstructionDef(
		SEB, // specialization of BSHFL
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01111100000000000000010000100000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const int32 result = rt.value<int8>();

		write_result(rd, result);
	}

	ProcInstructionDef(
		SEH, // specialization of BSHFL
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01111100000000000000011000100000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const int32 result = rt.value<int16>();

		write_result(rd, result);
	}

	ProcInstructionDef(
		SELEQZ,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000110101
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 rt_val = rt.value<uint32>();
		const uint32 result = (rt_val == 0) ? rs.value<uint32>() : 0;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SELNEZ,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000110111
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 rt_val = rt.value<uint32>();
		const uint32 result = (rt_val != 0) ? rs.value<uint32>() : 0;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SH,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b10100100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<16, CPU_Exception::Type::AdES>(processor, address);

		const uint16 value = rt.value<uint16>();
		processor.mem_write(address, value);

#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SHE,
		(OpFlags::Store | OpFlags::Throws | OpFlags::ReadsGPRegister | OpFlags::ThrowsMisaligned | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000011101
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<16, CPU_Exception::Type::AdES>(processor, address);

		const uint16 value = rt.value<uint16>();
		processor.mem_write(address, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
#pragma message("Implement Watch")
	}

	ProcInstructionDef(
		SIGRIE,
		(OpFlags::Throws),
		0b11111111111111110000000000000000,
		0b00000100000101110000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const uint32 code = TinyInt<16>(instruction).zextend<uint32>();

		CPU_Exception::throw_helper<CPU_Exception::Type::RI>(processor.get_program_counter(), code );
	}

	ProcInstructionDef(
		SLL,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000000000111111,
		0b00000000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

		// this would be EHB, which is handled by another case
		_assume(!(rt.is_zero() && rd.is_zero() && sa == 0b00011));

		const uint32 result = rt.value<uint32>() << sa;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SLLV,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000000100
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = rs.value<uint32>() & Bits(5);

		const uint32 result = rt.value<uint32>() << sa;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SLT,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000101010
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const int32 rt_val = rt.value<int32>();
		const int32 rs_val = rs.value<int32>();
		const uint32 result = (rs_val < rt_val) ? 1 : 0;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SLTI,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00101000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const int32 immediate = TinyInt<16>(instruction).sextend<int32>();

		const int32 rs_val = rs.value<int32>();
		const uint32 result = (rs_val < immediate) ? 1 : 0;

		write_result(rt, result);
	}

	ProcInstructionDef(
		SLTIU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00101100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 immediate = uint32(TinyInt<16>(instruction).sextend<int32>()); // intended sign-extend to unsigned

		const uint32 rs_val = rs.value<uint32>();
		const uint32 result = (rs_val < immediate) ? 1 : 0;

		write_result(rt, result);
	}

	ProcInstructionDef(
		SLTU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000101011
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 rt_val = rt.value<uint32>();
		const uint32 rs_val = rs.value<uint32>();
		const uint32 result = (rs_val < rt_val) ? 1 : 0;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SRA,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000000000111111,
		0b00000000000000000000000000000011
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

		const int32 result = rt.value<int32>() >> sa;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SRAV,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000000111
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = rs.value<uint32>() & Bits(5);

		const int32 result = rt.value<int32>() >> sa;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SRL,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000000000111111,
		0b00000000000000000000000000000010
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

		const uint32 result = rt.value<uint32>() >> sa;

		write_result(rd, result);
	}

	ProcInstructionDef(
		SRLV,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000000110
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);
		const uint32 sa = rs.value<uint32>() & Bits(5);

		const uint32 result = rt.value<uint32>() >> sa;

		write_result(rd, result);
	}

	// specialized
	ProcInstructionDef(
		SSNOP, // technically SLL $0, $0, 1 - given own slot for performance.
		(OpFlags::None),
		0b11111111111111111111111111111111,
		0b00000000000000000000000001000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{		
		// intentionally empty
	}

	ProcInstructionDef(
		SUB,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister | OpFlags::Throws),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100010
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		#if __has_builtin(__builtin_sub_overflow)
		int32 result;
		const bool overflow = __builtin_sub_overflow(rs.value<int32>(), rt.value<int32>(), &result);
		#elif defined(_MSC_VER) && _MSC_VER >= 1937
		int32 result;
		const bool overflow = _sub_overflow_i32(0, rs.value<int32>(), rt.value<int32>(), &result);
		#else
		const int64 result = int64(rs.value<int32>()) - rt.value<int32>();
		const bool overflow = result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min();
		#endif
		if (overflow) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Ov>(processor.get_program_counter() );
		}

		write_result(rd, int32(result));
	}

	ProcInstructionDef(
		SUBU,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100011
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>() - rt.value<uint32>();

		write_result(rd, int32(result));
	}

	ProcInstructionDef(
		SW,
		(OpFlags::Store | OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::ThrowsMisaligned ),
		0b11111100000000000000000000000000,
		0b10101100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdES>(processor, address);

		const uint32 value = rt.value<uint32>();
		processor.mem_write(address, value);

#pragma message("Implement Watch")
	}

#if WITH_SPECIALIZED
	// specialized
	ProcInstructionDef(
		SW_RTZERO,
		(OpFlags::Store | OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::ThrowsMisaligned ),
		0b11111100000111110000000000000000,
		0b10101100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdES>(processor, address);

		const uint32 value = 0U;
		processor.mem_write(address, value);

#pragma message("Implement Watch")
	}

	// specialized
	ProcInstructionDef(
		SW_OFFZERO,
		(OpFlags::Store | OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::ThrowsMisaligned ),
		0b11111100000000001111111111111111,
		0b10101100000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		//const int32 offset = TinyInt<16>(instruction).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>();

		address = handle_misaligned_address<32, CPU_Exception::Type::AdES>(processor, address);

		const uint32 value = rt.value<uint32>();
		processor.mem_write(address, value);

#pragma message("Implement Watch")
	}
#endif

	ProcInstructionDef(
		SWE,
		(OpFlags::Store | OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::ThrowsMisaligned | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b11111100000000000000000001111111,
		0b01111100000000000000000000011111
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<21, 5> base(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

		uptr_guest address = base.value<uptr_guest>() + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdES>(processor, address);

		const uint32 value = rt.value<uint32>();
		processor.mem_write(address, value);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
#pragma message("Implement Watch")

	}

	ProcInstructionDef(
		SYNC,
		(OpFlags::None | OpFlags::Hazard),
		0b11111111111111111111100000111111,
		0b00000000000000000000000000001111
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const uint8 stype = TinyInt<5>(instruction >> 6).zextend<uint8>();

		const mips::memory_hazards hazards = instructions::parse_sync_type(stype);

		if (hazards != memory_hazards::none) [[likely]]
		{
			if (processor.handles_memory_hazards()) [[unlikely]]
			{
				processor.clear_memory_hazards(hazards);
			}
		}
	}

	ProcInstructionDef(
		SYNCI,
		(OpFlags::ReadsGPRegister),
		0b11111100000111110000000000000000,
		0b00000100000111110000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> base(instruction, processor);
		const int32 offset = TinyInt<16>(instruction).sextend<int32>();
		
		const uptr_guest address = base.value<uptr_guest>() + offset;

		//processor.mem_poke<char>(address); // We still want to touch the address.

		processor.invalidate_instruction_cache(address);

		// TODO : operates on a cache line

		// TODO : there must be a SYNC following this, followed by an instruction that clears hazards:
		// * jalr.hb, jr.hb, deret, eret
	}

	static constexpr OpFlags SysCallOpFlags =
#if SYSCALL_USE_EXCEPTION
		OpFlags::OpFlags::AlwaysThrows;
#else
		OpFlags::WritesGPRegister | OpFlags::ReadsGPRegister;
#endif

#if SYSCALL_USE_EXCEPTION
#	define _VEMIPS_SYSCALL_NORETURN _noreturn_post
#else
#	define _VEMIPS_SYSCALL_NORETURN
#endif

	ProcInstructionDef(
		SYSCALL,
		(OpFlags::Throws | SysCallOpFlags),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000001100
	) (const instruction_t instruction, processor & __restrict processor) _VEMIPS_SYSCALL_NORETURN
	{
		const uint32 code = TinyInt<20>(instruction >> 6).zextend<uint32>();

#if !SYSCALL_USE_EXCEPTION
		const CPU_Exception sys_ex{ CPU_Exception::Type::Sys, processor.get_program_counter(), code };

		uint32 result = 0;

#if SET_RESULT_REGISTER_ON_SYSCALL_EXIT
		try {
#endif
			result = processor.get_guest_system()->handle_exception(sys_ex);
#if SET_RESULT_REGISTER_ON_SYSCALL_EXIT
		}
		catch (const ExecutionEndedException&) {
			processor.set_register<uint32>(4, 1);
			throw;
		}
#endif

		processor.set_register<uint32>(4, result);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::Sys>(processor.get_program_counter(), code );
#endif
	}

#undef _VEMIPS_SYSCALL_NORETURN

	ProcInstructionDef(
		TEQ,
		(OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000110100
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

		if (rs.value<int32>() == rt.value<int32>()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Tr>(processor.get_program_counter(), code );
		}
	}

	ProcInstructionDef(
		TGE,
		(OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000110000
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

		if (rs.value<int32>() >= rt.value<int32>()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Tr>(processor.get_program_counter(), code );
		}
	}

	ProcInstructionDef(
		TGEU,
		(OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000110001
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

		if (rs.value<uint32>() >= rt.value<uint32>()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Tr>(processor.get_program_counter(), code );
		}
	}

	ProcInstructionDef(
		TLT,
		(OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000110010
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

		if (rs.value<int32>() < rt.value<int32>()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Tr>(processor.get_program_counter(), code );
		}
	}

	ProcInstructionDef(
		TLTU,
		(OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000110011
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

		if (rs.value<uint32>() < rt.value<uint32>()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Tr>(processor.get_program_counter(), code);
		}
	}

	ProcInstructionDef(
		TNE,
		(OpFlags::Throws | OpFlags::ReadsGPRegister),
		0b11111100000000000000000000111111,
		0b00000000000000000000000000110110
	) (const instruction_t instruction, processor & __restrict processor)
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

		if (rs.value<int32>() != rt.value<int32>()) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::Tr>(processor.get_program_counter(), code );
		}
	}

	ProcInstructionDef(
		WSBH,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01111100000000000000000010100000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		// Swap bytes _within each halfword_
		union register_alias
		{
			uint32 reg;
			uint8 bytes[4];
		} reg {
			.reg = rt.value<uint32>()
		};

#if __clang__
		std::swap(reg.bytes[0], reg.bytes[1]);
		std::swap(reg.bytes[2], reg.bytes[3]);
#else // Neither GCC nor MSVC appear to be able to optimize the std::swaps into this, but LLVM does it fine.
		reg.reg = std::byteswap(reg.reg);
		reg.reg = std::rotr(reg.reg, 16);
#endif

		write_result(rd, reg.reg);
	}

	ProcInstructionDef(
		XOR,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000011111111111,
		0b00000000000000000000000000100110
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		const GPRegister<16, 5> rt(instruction, processor);
		GPRegister<11, 5> rd(instruction, processor);

		const uint32 result = rs.value<uint32>() ^ rt.value<uint32>();

		write_result(rd, result);
	}

	ProcInstructionDef(
		XORI,
		(OpFlags::ReadsGPRegister | OpFlags::WritesGPRegister),
		0b11111100000000000000000000000000,
		0b00111000000000000000000000000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept
	{
		const GPRegister<21, 5> rs(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 offset = TinyInt<16>(instruction).zextend<uint32>();

		const uint32 result = rs.value<uint32>() ^ offset;

		write_result(rt, result);
	}

	// TODO : move to cop0
	ProcInstructionDef(
		MFC0,
		(OpFlags::WritesGPRegister | OpFlags::Throws | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'11111'00000'00000'11111111'000,
		0b010000'00000'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		// TODO : if we ever support non-r6, it is undefined if we don't implement said register. R6 defines it as zero.
		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	// TODO : move to cop0
	ProcInstructionDef(
		MFHC0,
		(OpFlags::WritesGPRegister | OpFlags::Throws | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'11111'00000'00000'11111111'000,
		0b010000'00010'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		// TODO : if we ever support non-r6, it is undefined if we don't implement said register. R6 defines it as zero.
		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	// TODO : move to cop0
	ProcInstructionDef(
		MTC0,
		(OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'11111'00000'00000'11111111'000,
		0b010000'00100'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	// TODO : move to cop0
	ProcInstructionDef(
		MTHC0,
		(OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'11111'00000'00000'11111111'000,
		0b010000'00110'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	// TODO : move to cop0
	ProcInstructionDef(
		ERET,
		(OpFlags::Throws | OpFlags::Hazard | OpFlags::InstructionHazard | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'1'111111111111111111'1'011000,
		0b010000'1'000000000000000000'0'000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		if (processor.handles_memory_hazards()) [[unlikely]]
		{
			processor.clear_memory_hazards(processor::memory_hazards::all);
		}
		if (processor.handles_instruction_hazards()) [[unlikely]]
		{
			processor.flush_instruction_hazards(); // TODO : not a function
		}
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	// TODO : move to cop0
	ProcInstructionDef(
		ERETNC,
		(OpFlags::Throws | OpFlags::Hazard | OpFlags::InstructionHazard | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'1'111111111111111111'1'011000,
		0b010000'1'000000000000000000'1'000000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		if (processor.handles_memory_hazards()) [[unlikely]]
		{
			processor.clear_memory_hazards(processor::memory_hazards::all_data);
		}
		if (processor.handles_instruction_hazards()) [[unlikely]]
		{
			processor.flush_instruction_hazards(); // TODO : not a function
		}
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	// TODO : move to cop2
	ProcInstructionDef(
		MFC2,
		(OpFlags::WritesGPRegister | OpFlags::Throws | OpFlags::AlwaysThrows),
		0b111111'11111'00000'00000'11111111'000,
		0b010010'00000'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) _noreturn_post
	{
		#if 0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		// TODO : if we ever support non-r6, it is undefined if we don't implement said register. R6 defines it as zero.
		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
		#endif

		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
	}

	// TODO : move to cop2
	ProcInstructionDef(
		MFHC2,
		(OpFlags::WritesGPRegister | OpFlags::Throws | OpFlags::AlwaysThrows),
		0b111111'11111'00000'00000'11111111'000,
		0b010010'00011'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) _noreturn_post
	{
		#if 0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		// TODO : if we ever support non-r6, it is undefined if we don't implement said register. R6 defines it as zero.
		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
		#endif

		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
	}

	// TODO : move to cop2
	ProcInstructionDef(
		MTC2,
		(OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::AlwaysThrows),
		0b111111'11111'00000'00000'11111111'000,
		0b010010'00100'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) _noreturn_post
	{
		#if 0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
		#endif

		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
	}

	// TODO : move to cop2
	ProcInstructionDef(
		MTHC2,
		(OpFlags::ReadsGPRegister | OpFlags::Throws | OpFlags::AlwaysThrows),
		0b111111'11111'00000'00000'11111111'000,
		0b010010'00111'00000'00000'00000000'000
	) (const instruction_t instruction, processor & __restrict processor) _noreturn_post
	{
		#if 0
		const GPRegister<11, 5> rd(instruction, processor);
		GPRegister<16, 5> rt(instruction, processor);
		const uint32 selector = TinyInt<3>(instruction).zextend<uint32>();

		uint32 result = 0;

		switch (rd.value<uint32>())
		{
			
		}

		write_result(rt, result);
		#endif

		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
	}

	// TODO : move to cop0
	ProcInstructionDef(
		WAIT,
		(OpFlags::Throws | OpFlags::Hazard /* TODO : is it? */ | OpFlags::InstructionHazard /* TODO : is it? */ | OpFlags::RequiresCoP0 | THROWS_WITHOUT_COPROCESSOR_0),
		0b111111'1'000000000000000000'1'111111,
		0b010000'1'000000000000000000'1'100000
	) (const instruction_t instruction, processor & __restrict processor) noexcept(WITH_COPROCESSOR_0) _noreturn_post
	{
#if WITH_COPROCESSOR_0
		// implementation-dependent. We don't honor it for the same reason we don't honor `PAUSE`.
#else
		CPU_Exception::throw_helper<CPU_Exception::Type::CpU>(processor.get_program_counter() );
#endif
	}

	/*** TODO: COP0 instructions...
	**** DERET
	**** DI
	**** DVP
	**** EI
	**** ERET
	**** ERETNC
	**** EVP
	**** ~MFC0
	**** ~MFHC0
	**** ~MTC0
	**** ~MTHC0
	**** RDPGPR
	**** TLBINV
	**** TLBINVF
	**** TLBP
	**** TLBR
	**** TLBWI
	**** TLBWR
	**** ~WAIT
	**** WRPGPR
	***/
}

#pragma clang diagnostic pop

#undef THROWS_WITHOUT_COPROCESSOR_0
