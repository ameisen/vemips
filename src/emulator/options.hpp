#pragma once

#include <cstddef>

#include <expected>
#include <functional>
#include <memory>
#include <vector>

#include "common.hpp"
#include "mips/mmu.hpp"
#include "mips/processor/jit/jit.hpp"


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

		_nothrow _forceinline unique_span() = default;

		_nothrow _forceinline unique_span(const std::size_t size) :
			data(std::make_unique<T[]>(size)),
			size(size) {
		}

		_nothrow _forceinline void resize (const std::size_t new_size) {
			if (size == new_size) {
				return;
			}

			data = std::make_unique<T[]>(new_size);
			size = new_size;
		}

		_nothrow _forceinline operator std::span<T>() const {
			return { data.get(), data.get() + size };
		}
	};

	struct argument_data final {
		const tchar * __restrict binary_file = nullptr;
		unique_span<char> binary_data;
		uint64 ticks = 0;
		uint32 available_memory = 0x100000 * 2;
		uint32 stack_memory = uint32(-1);
		mips::JitType jit = mips::JitType::Jit;
		mips::mmu mmu_type = mips::mmu::none;
		struct {
			uint16 port = 0;
			bool enabled = false;
		} debug;
		bool collect_statistics = false;
		bool disable_cti = false;
		bool instruction_cache = true;
		bool use_rox = false;
		bool strict_noncoherence = false;
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

	void print_version();

	std::expected<argument_data, int> parse(const std::span<const tchar*> args);
}
