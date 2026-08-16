#pragma once

#include <common.hpp>

#include <cstddef>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>

#include "mips/llsc.hpp"
#include "mips/mmu.hpp"
#include "mips/processor/jit/jit.hpp"
#include "mips/system.hpp"


namespace vemips::options
{
	struct version final {
		uint32 major;
		uint32 minor;
		uint32 build;
	};

	template <typename T>
	struct unique_span final {
		std::unique_ptr<T[]> data;
		std::size_t size = 0;

		_nothrow _forceinline unique_span() noexcept = default;

		_forceinline unique_span(const std::size_t size) noexcept(std::is_nothrow_constructible_v<T[]>) :
			data(std::make_unique<T[]>(size)),
			size(size) {
		}

		_nothrow _forceinline void resize (const std::size_t new_size) noexcept(std::is_nothrow_constructible_v<T[]>) {
			if (size == new_size) {
				return;
			}

			// TODO : aligned_realloc
			data = std::make_unique<T[]>(new_size);
			size = new_size;
		}

		[[nodiscard]]
		_pure _nothrow _forceinline operator std::span<T>() const & noexcept {
			return { data.get(), data.get() + size };
		}

		[[nodiscard]]
		_pure _nothrow _forceinline operator std::span<T>() const && noexcept = delete;
	};

	struct argument_data final {
		const tchar* __restrict binary_file = nullptr;
		unique_span<char> binary_data;
		uint64 ticks = 0;
		uint32 available_memory = 0x20'0000;
		uint32 stack_memory = uint32(-1);
		mips::JitType jit = mips::JitType::Jit;
		mips::llsc llsc_type = mips::llsc::fine;
		mips::mmu mmu_type = mips::mmu::none;
		struct {
			uint16 port = 0;
			bool enabled = false;
		} debug;
		bool collect_statistics = false;
		bool disable_cti = false;
		bool use_rox = false;
		bool side_by_side = false;
		mips::system::options::policy self_modifying_code = mips::system::options::policy::enabled;
	};

#ifndef EMSCRIPTEN
	struct option final {
		std::vector<tstring_view> option_str;
		tstring_view description;
		std::function<
			void(
				argument_data & __restrict argument_data,
				std::span<const tchar*> args,
				std::size_t& __restrict i,
				std::optional<tstring_view>&& value
			)
		> procedure;
	};
#endif

	_nothrow void print_version() noexcept;

	[[nodiscard]]
	std::expected<argument_data, int> parse(const std::span<const tchar*> args);
}
