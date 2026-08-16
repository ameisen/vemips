#pragma once
#include <common.hpp>

#include <array>
#include <bit>
#include <expected>
#include <limits>
#include <memory>
#include <type_traits>

#include "mips/mips_common.hpp"
#include "mips/exception.hpp"
#include "mips/instructions/instructions_common.hpp"
#include "mips/instructions/instructions_table.hpp"


namespace mips
{
	class processor;
}

namespace mips::interpreter::cache
{
	namespace detail_hash_cache
	{
		template <typename F>
		concept get_instruction_concept = std::is_nothrow_invocable_r_v<std::expected<const instruction_t, uptr_guest>, F, const uptr_guest>;

		template <typename T, typename I = std::size_t>
		concept subscriptable = requires (T& t, const I& i)
		{
			{t[i]};
		};

		template <typename T, bool Indirect = true>
		struct _empty_bases ref_obj final
		{
			_no_unique
			std::unique_ptr<T> obj;

			_forceinline ref_obj() : obj(std::make_unique<T>())
			{}

			
			_pure
			_nothrow _forceinline T* operator->() noexcept {
				return obj.get();
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline const T* operator->() const noexcept {
				return obj.get();
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline T* operator*() noexcept {
				return obj.get();
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline const T* operator*() const noexcept {
				return obj.get();
			}
		};

		template <typename T>
		struct _empty_bases ref_obj<T, false>
		{
			_no_unique
			T obj;

			[[nodiscard]]
			_pure
			_nothrow _forceinline T* operator->() noexcept {
				return &obj;
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline const T* operator->() const noexcept {
				return &obj;
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline T* operator*() noexcept {
				return &obj;
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline const T* operator*() const noexcept {
				return &obj;
			}
		};

		template <usize_guest LineSizeBytes, uint32 NumBuckets, uint32 Associativity>
		class _empty_bases hash_cache_base
		{
		protected:
			hash_cache_base() = default;

			template <typename T, bool Indirect = true>
			using bucket_ref = ref_obj<T, Indirect && true>;

			template <typename T, bool Indirect = true>
			using bucket_array_ref = ref_obj<T, Indirect && true>;

			using index_type = uint_fitted<instructions::num_instructions + 2U>;

			static constexpr index_type max_valid_index = std::numeric_limits<index_type>::max() - 2;

			static constexpr usize_guest instruction_size = sizeof(instruction_t);
			static constexpr int instruction_shift = std::countr_zero(instruction_size);
			static constexpr usize_guest line_size_bytes = LineSizeBytes;
			static constexpr usize_guest line_size_instructions = line_size_bytes / instruction_size;
			static constexpr uptr_guest line_mask = line_size_bytes - 1u;

			static constexpr int bucket_shift = std::countr_zero(LineSizeBytes); //std::countr_zero(sizeof(instruction_t));
			static constexpr uptr_guest bucket_mask = NumBuckets - 1U;
			static_assert((line_mask | bucket_mask) == line_mask);

			using instruction_exec_ptr = instructions::instructionexec_t;

		public:
			static constexpr uptr_guest invalid_ptr = std::numeric_limits<uptr_guest>::max();
			static constexpr index_type ri_index = max_valid_index + 2U;
			static constexpr index_type adel_index = max_valid_index + 1U;

			[[nodiscard]]
			static constexpr
			_pure
			_nothrow _forceinline bool is_valid_index(const index_type index) noexcept
			{
				return index != ri_index && index != adel_index;
			}

			[[nodiscard]]
			static constexpr
			_pure
			_nothrow _forceinline uint32 get_index_from_ptr(const uptr_guest ptr) noexcept
			{
				return (ptr & line_mask) >> instruction_shift;
			}

			struct entry final
			{
				union
				{
					uptr_guest error_address = invalid_ptr;
					instruction_t instruction;
				};
				index_type index = ri_index;

				_forceinline void execute_instruction(
					const uptr_guest ptr,
					processor& __restrict processor
				) const __restrict
				{

					if (
						const index_type index_local = index;
						is_valid_index(index_local)
					) [[likely]]
					{
						instructions::execute_instruction<true, false>(index_local, instruction, processor);
					}
					else
					{
						if (index_local == adel_index)
						{
							CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(ptr, error_address);
						}
						else
						{
							CPU_Exception::throw_helper<CPU_Exception::Type::RI>(ptr);
						}
					}
				}
			};

			struct line final
			{
				using entry = hash_cache_base::entry;

				static constexpr usize_guest instruction_count = line_size_instructions;

				uptr_guest base_address = invalid_ptr;
				std::array<entry, instruction_count> data;
				
				[[nodiscard]]
				static
				_pure _nothrow _forceinline bool contains_static(const uptr_guest base_ptr, const uptr_guest ptr) noexcept {
					return base_ptr == get_base_ptr(ptr);
				}

				[[nodiscard]]
				_pure
				_nothrow _forceinline bool contains(const uptr_guest ptr) const noexcept {
					return contains_static(base_address, ptr);
				}

				[[nodiscard]]
				static constexpr _pure _nothrow _forceinline uptr_guest get_base_ptr(const uptr_guest ptr) noexcept {
					return ptr & ~line_mask;
				}
			};

		protected:
			template <get_instruction_concept GetInstructionF>
			static _nothrow _noinline void populate_line(
				line& __restrict line,
				const uptr_guest base_address,
				const GetInstructionF& get_instruction
			) noexcept
			{
				uptr_guest current_address = base_address;

				line.base_address = current_address;
				for (entry& __restrict entry : line.data)
				{
					if (
						const std::expected<instruction_t, uptr_guest> instruction = get_instruction(current_address);
						instruction.has_value()
					) [[likely]]
					{
						const uint32 raw_index = instructions::get_instruction_index(instruction.value());
						xassert(raw_index != adel_index);

						entry = {
							.instruction = instruction.value(),
							.index = static_cast<index_type>(raw_index)
						};
					}
					else
					{
						entry = {
							.error_address = instruction.error(),
						    .index = adel_index
						};
					}

					current_address += sizeof(instruction_t);
				}
			}
		};

		template <usize_guest LineSizeBytes, uint32 NumBuckets, uint32 Associativity>
		class _empty_bases hash_cache_base_templ : public hash_cache_base<LineSizeBytes, NumBuckets, Associativity>
		{
			using base = hash_cache_base<LineSizeBytes, NumBuckets, Associativity>;

			template <typename T, bool Indirect = true>
			using bucket_ref = base::template bucket_ref<T, Indirect>;
			template <typename T, bool Indirect = true>
			using bucket_array_ref = base::template bucket_array_ref<T, Indirect>;

		protected:
			hash_cache_base_templ() = default;

			struct bucket final
			{
				using line = base::line;

				std::array<line, Associativity> lines;
				uint_fitted<Associativity> write_offset = 0U;

				template <get_instruction_concept GetInstructionF>
				[[nodiscard]]
				_nothrow _forceinline const line& fetch_line(
					const uptr_guest ptr,
					const GetInstructionF& get_instruction
				) noexcept
				{
					const uptr_guest base_ptr = line::get_base_ptr(ptr);

					line* __restrict line;

					for (auto& __restrict current_line : lines)
					{
						if (current_line.base_address == base_ptr) [[likely]]
						{
							line = &current_line;
							goto found_line;  // NOLINT(cppcoreguidelines-avoid-goto, hicpp-avoid-goto)
						}
					}

					// Was not found - need to populate the line.
					[&] _forceinline_lambda(() noexcept) {
						const auto line_index = write_offset++;
						write_offset %= Associativity;
						line = &lines[line_index];
						base::populate_line(*line, base_ptr, get_instruction);
					}();

					found_line:

					return *line;
				}

				template <get_instruction_concept GetInstructionF>
				[[nodiscard]]
				_nothrow _forceinline const line::entry& fetch_line_entry(
					const uptr_guest ptr,
					const GetInstructionF& get_instruction
				) noexcept
				{
					const line& __restrict line = fetch_line(ptr, get_instruction);

					const uint32 entry_index = base::get_index_from_ptr(ptr);
					return line.data[entry_index];
				}

				_nothrow _forceinline void invalidate(const uptr_guest base_ptr) noexcept
				{
					for (auto& current_line : lines)
					{
						if (current_line.base_address == base_ptr)
						{
							current_line = line{};
							break;
						}
					}
				}
			};

			_no_unique
			bucket_array_ref<std::array<bucket_ref<bucket>, NumBuckets>, (NumBuckets > 1) && (Associativity > 0)> buckets_;
		};

		template <usize_guest LineSizeBytes, uint32 NumBuckets>
		class _empty_bases hash_cache_base_templ<LineSizeBytes, NumBuckets, 1U> : public hash_cache_base<LineSizeBytes, NumBuckets, 1U>
		{
			using base = hash_cache_base<LineSizeBytes, NumBuckets, 1U>;

			template <typename T, bool Indirect = true>
			using bucket_ref = base::template bucket_ref<T, Indirect>;
			template <typename T, bool Indirect = true>
			using bucket_array_ref = base::template bucket_array_ref<T, Indirect>;

		protected:
			hash_cache_base_templ() = default;

			struct bucket final
			{
				using line = base::line;

				line single_line;

				template <get_instruction_concept GetInstructionF>
				[[nodiscard]]
				_nothrow _forceinline const line& fetch_line(
					const uptr_guest ptr,
					const GetInstructionF& get_instruction
				) noexcept
				{
					if (
						const uptr_guest base_ptr = line::get_base_ptr(ptr);
						single_line.base_address == base_ptr
					) [[unlikely]]
					{
						base::populate_line(single_line, base_ptr, get_instruction);
					}

					return single_line;
				}

				template <get_instruction_concept GetInstructionF>
				[[nodiscard]]
				_nothrow _forceinline const line::entry& fetch_line_entry(
					const uptr_guest ptr,
					const GetInstructionF& get_instruction
				) noexcept
				{
					const uint32 entry_index = base::get_index_from_ptr(ptr);
					return fetch_line(ptr, get_instruction).data[entry_index];
				}

				_nothrow _forceinline void invalidate(const uptr_guest base_ptr) noexcept
				{
					if (single_line.base_address == base_ptr)
					{
						single_line = line{};
					}
				}
			};

			_no_unique
			bucket_array_ref<std::array<bucket_ref<bucket>, NumBuckets>, (NumBuckets > 1)> buckets_;
		};

		template <usize_guest LineSizeBytes, uint32 NumBuckets>
		class _empty_bases hash_cache_base_templ<LineSizeBytes, NumBuckets, 0U> : public hash_cache_base<LineSizeBytes, NumBuckets, 0U>
		{
			using base = hash_cache_base<LineSizeBytes, NumBuckets, 0U>;

		protected:
			hash_cache_base_templ() = default;

			struct bucket final {};
		};
	}

	template <
		usize_guest LineSizeBytes,
		uint32 NumBuckets,
		uint32 Associativity = 1
	> requires (
		std::has_single_bit(LineSizeBytes) &&
		std::has_single_bit(NumBuckets)
	)
	class _empty_bases hash_cache final : public detail_hash_cache::hash_cache_base_templ<LineSizeBytes, NumBuckets, Associativity>
	{
		using base = detail_hash_cache::hash_cache_base_templ<LineSizeBytes, NumBuckets, Associativity>;

		using typename base::index_type;

		using base::max_valid_index;
		using base::instruction_size;
		using base::instruction_shift;
		using base::line_size_bytes;
		using base::line_size_instructions;


		using typename base::instruction_exec_ptr;

		static_assert(std::has_single_bit(sizeof(instruction_t)));

		using base::bucket_shift;
		using base::bucket_mask;

	public:
		using base::invalid_ptr;
		using base::ri_index;
		using base::adel_index;

		using typename base::entry;
		using typename base::line;

		static constexpr bool is_enabled = NumBuckets != 0 && Associativity != 0;
		static constexpr bool is_indexable = true && is_enabled;

		[[nodiscard]]
		static constexpr _pure _nothrow _forceinline uint32 get_bucket_index(const uptr_guest ptr) noexcept
		{
			if constexpr (NumBuckets == 1)
			{
				return 0;
			}
			else
			{
				const uint32 cast_ptr = checked_cast<uint32>(ptr);
				const uint32 shifted_ptr = cast_ptr >> bucket_shift;
				const uint32 bucket_index = shifted_ptr & bucket_mask;

				return bucket_index;
			}
		}

	public:
		enum class execution_state
		{
			success = 0,
			fail_ri,
			fail_adel
		};

		template <detail_hash_cache::get_instruction_concept GetInstructionF>
		[[nodiscard]]
		_nothrow _forceinline entry get_entry(
			const uptr_guest ptr,
			const GetInstructionF& get_instruction
		) noexcept //requires(is_enabled)
		{
			if constexpr (!is_enabled)
			{
				entry result;

				if (
					const std::expected<instruction_t, uptr_guest> instruction = get_instruction(ptr);
					instruction.has_value()
				)
				{
					const uint32 raw_index = instructions::get_instruction_index(instruction.value());
					xassert(raw_index != adel_index);

					result = {
						.instruction = instruction.value(),
						.index = static_cast<index_type>(raw_index)
					};
				}
				else
				{
					result = {
						.error_address = instruction.error(),
					    .index = adel_index
					};
				}

				return result;
			}
			else
			{
				const uint32 bucket_index = get_bucket_index(ptr);

				const auto& __restrict entry = (**base::buckets_)[bucket_index]->fetch_line_entry(
					ptr,
					get_instruction
				);
				return entry;
			}
		}

		template <detail_hash_cache::get_instruction_concept GetInstructionF> requires (is_enabled)
		[[nodiscard]]
		_nothrow _forceinline const line& get_cache_line(
			const uptr_guest ptr,
			const GetInstructionF& get_instruction
		) noexcept
		{
			const uint32 bucket_index = get_bucket_index(ptr);

			return (**base::buckets_)[bucket_index]->fetch_line(
				ptr,
				get_instruction
			);
		}

		template <detail_hash_cache::get_instruction_concept GetInstructionF>
		_forceinline void execute_instruction(
			const uptr_guest ptr,
			processor& __restrict processor,
			const GetInstructionF& get_instruction
		)
		{
			if constexpr (!is_enabled)
			{
				if (
					const std::expected<instruction_t, uptr_guest> instruction = get_instruction(ptr);
					!(instruction.has_value() && instructions::execute_instruction(instruction.value(), processor))
				) [[unlikely]]
				{
					if (!instruction.has_value())
					{
						CPU_Exception::throw_helper<CPU_Exception::Type::RI>(ptr);
					}
					else
					{
						CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(ptr, instruction.error());
					}
				}
			}
			else
			{
				const uint32 bucket_index = get_bucket_index(ptr);

				const auto& __restrict entry = (**base::buckets_)[bucket_index]->fetch_line_entry(
					ptr,
					get_instruction
				);

				entry.execute_instruction(ptr, processor);
			}
		}

		template <detail_hash_cache::get_instruction_concept GetInstructionF>
		_forceinline instruction_t execute_instruction_ret(
			const uptr_guest ptr,
			processor& __restrict processor,
			const GetInstructionF& get_instruction
		) {
			if constexpr (!is_enabled)
			{
				if (
					const std::expected<instruction_t, uptr_guest> instruction = get_instruction(ptr);
					instruction.has_value() && instructions::execute_instruction(instruction.value(), processor)
				) [[likely]]
				{
					return instructions::get_instruction_index(instruction.value());
				}
				else
				{
					if (!instruction.has_value())
					{
						CPU_Exception::throw_helper<CPU_Exception::Type::RI>(ptr);
					}
					else
					{
						CPU_Exception::throw_helper<CPU_Exception::Type::AdEL>(ptr, instruction.error());
					}
				}
			}
			else
			{
				const uint32 bucket_index = get_bucket_index(ptr);

				const auto& __restrict entry = (**base::buckets_)[bucket_index]->fetch_line_entry(
					ptr,
					get_instruction
				);

				entry.execute_instruction(ptr, processor);
				return entry.index;
			}
		}

		_nothrow _forceinline void invalidate(
			const uptr_guest ptr,
			const usize_guest size
		) noexcept
		{
			if constexpr (is_enabled)
			{
				// TODO : this is suboptimal when we have associativity > 1, as multiple of the addresses could be in a single bucket
				const uint32 base_bucket_index = get_bucket_index(ptr);
				const usize num_buckets = (usize(size) + (LineSizeBytes - 1)) / LineSizeBytes;

				for (
					uint32 i = 0;
					i < num_buckets;
					++i
				)
				{
					const uint32 bucket_index = (base_bucket_index + i) % NumBuckets;
					(**base::buckets_)[bucket_index]->invalidate(ptr & ~bucket_mask);
				}
			}
		}
	};
}
