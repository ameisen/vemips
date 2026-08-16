#pragma once

#include <common.hpp>

#include <limits>
#include <type_traits>
#include <utility>


namespace mips {
	// TODO refactor

	struct _empty_bases base_exception
	{
	protected:
		_pragma_small_code
		_cold _nothrow base_exception() noexcept = default;
		_pragma_default_code
	};

	template <typename TException>
	struct _empty_bases base_exception_templated : base_exception
	{
	protected:
		_pragma_small_code
		_cold _nothrow base_exception_templated() noexcept = default;
		_pragma_default_code

	public:
		template <typename... ArgsT>
		_pragma_small_code [[noreturn]]
		static _cold _noinline void throw_helper(ArgsT&&... args)
		{
			static_assert(std::is_base_of_v<base_exception, TException>);

			throw TException(std::forward<ArgsT>(args)...);
		}
		_pragma_default_code
	};

	struct CPU_Exception final : public base_exception_templated<CPU_Exception> {
	public:
		enum class Type : uint32 {
			Interrupt = 0,
			Mod = 1,
			TLBL = 2,
			TLBS = 3,
			AdEL = 4,
			AdES = 5,
			IBE = 6,
			DBE = 7,
			Sys = 8,
			Bp = 9,
			RI = 10,
			CpU = 11,
			Ov = 12,
			Tr = 14,
			FPE = 15,
			Impl1 = 16,
			Impl2 = 17,
			C2E = 18,
			TLBRI = 19,
			TLBXI = 20,
			MDMX = 22,
			WATCH = 23,
			MCheck = 24,
			Thread = 25,
			DPSPDis = 26,
			GE = 27,
			Prot = 29,
			CacheErr = 30,

			// Internal exceptions used for JIT interactions and such
			InternalFlow = std::numeric_limits<uint32>::max() - 0U,
		} m_ExceptionType = {};
		uptr_guest m_InstructionAddress = 0;
		uint32 m_Code = 0;

		_pragma_small_code
		_cold _nothrow CPU_Exception() noexcept = default;
		_pragma_default_code

		_pragma_small_code
		_cold _nothrow CPU_Exception(const Type type, const uptr_guest address, const uint32 code = 0) noexcept
			: m_ExceptionType(type)
			, m_InstructionAddress(address)
			, m_Code(code)
		{}
		_pragma_default_code

		_pragma_small_code [[noreturn]]
		_cold _noinline void rethrow_helper() const _noreturn_post
		{
			throw *this;
		}
		_pragma_default_code
			
		// TODO : add overload that pulls program counter from processor directly - forward declaration is insufficient
		_pragma_small_code
		template <Type ExceptionType>
		[[noreturn]]
		static _cold _noinline void throw_helper(const uptr_guest address, const uint32 code) _noreturn_post
		{
			throw CPU_Exception(ExceptionType, address, code);
		}
		_pragma_default_code

		_pragma_small_code
		template <Type ExceptionType>
		[[noreturn]]
		static _cold _noinline void throw_helper(const uptr_guest address) _noreturn_post
		{
			throw CPU_Exception(ExceptionType, address, 0);
		}
		_pragma_default_code
	};

	struct _empty_bases ExecutionEndedException : base_exception
	{
	public:
		_pragma_small_code
		_cold _nothrow ExecutionEndedException() noexcept = default;
		_pragma_default_code
	};

	struct ExecutionCompleteException final : base_exception_templated<ExecutionCompleteException>, ExecutionEndedException
	{
		int32 result_code = 0;

		_pragma_small_code
		_cold _nothrow ExecutionCompleteException() noexcept = default;
		_cold _nothrow ExecutionCompleteException(const int32 code) noexcept :
			result_code(code)
		{}
		_pragma_default_code
	};
	struct ExecutionFailException final : base_exception_templated<ExecutionFailException>, ExecutionEndedException
	{};

	namespace exceptions
	{
		_pragma_small_code
		template <typename TException, typename... Tt>
		[[noreturn]]
		static _cold _noinline void throw_helper(Tt&&... args)
		{
			throw TException(std::forward<Tt>(args)...);
		}
		_pragma_default_code

		_pragma_small_code
		template <typename TException, typename... Tt>
		[[noreturn]]
		static _cold _noinline void throw_assert(Tt&&... args)
		{
			xassert(false);
			// ReSharper disable once CppUnreachableCode
			throw TException(std::forward<Tt>(args)...);
		}
		_pragma_default_code
	}
}
