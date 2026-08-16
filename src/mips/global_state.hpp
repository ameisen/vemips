#pragma once

#include <common.hpp>

#include <type_traits>


namespace mips
{
	class jit_base;
	class processor;
}

namespace mips::global_state
{
	class _no_vtable global_state_base
	{
	public:
		global_state_base() = delete;
	};

	template <typename T, bool ResetToNull = false> requires std::is_base_of_v<global_state_base, T>
	class scoped_global_state final
	{
		using type = T::type;

		_no_unique
		type original_value;

	public:
		[[nodiscard]]
		_nothrow _forceinline scoped_global_state(const type value) noexcept
		{
			if constexpr (!ResetToNull)
			{
				original_value = T::get_current();
			}

			T::set_current(value);
		}

		scoped_global_state(const scoped_global_state&) = delete;
		scoped_global_state(scoped_global_state&&) = delete;

		scoped_global_state& operator=(const scoped_global_state&) = delete;
		scoped_global_state& operator=(scoped_global_state&&) = delete;

		_nothrow _forceinline ~scoped_global_state() noexcept
		{
			if constexpr (ResetToNull)
			{
				T::set_current(nullptr);
			}
			else
			{
				T::set_current(original_value);
			}
		}
	};

	class _empty_bases processor final : global_state_base
	{
	public:
		processor() = delete;

		using type = mips::processor*;

		[[nodiscard]]
		static _pure _nothrow type get_current() noexcept;
		static _nothrow void set_current(type processor) noexcept;
		
		template <bool ResetToNull = false>
		[[nodiscard]]
		static _pure _nothrow scoped_global_state<processor, ResetToNull> get_scoped(const type processor) noexcept
		{
			return {
				processor
			};
		} 
	};

	class _empty_bases jit final : global_state_base
	{
	public:
		jit() = delete;

		using type = mips::jit_base*;

		[[nodiscard]]
		static _pure _nothrow type get_current() noexcept;
		static _nothrow void set_current(type jit) noexcept;

		template <bool ResetToNull = false>
		[[nodiscard]]
		static _pure _nothrow scoped_global_state<jit, ResetToNull> get_scoped(const type jit) noexcept
		{
			return {
				jit
			};
		} 
	};
}
