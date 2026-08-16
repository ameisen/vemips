#pragma once

#include "mips/pch.hpp"
#include <common.hpp>

#include <array>
#include <bit>
#include <cstddef>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "mips/coprocessor/coprocessor.hpp"


namespace mips {
	class processor;

	class _empty_bases coprocessor1 final : public coprocessor {
		friend class jit1;
		friend class Jit1_CodeGen;

	public:
		using register_type = double;
		using cr_type = uint32;
		static_assert(sizeof(double) == 8, "Need custom FPU definition for this platform");
		static_assert(sizeof(float) == 4, "Need custom FPU definition for this platform");
		static_assert(sizeof(cr_type) == 4, "Need custom FPU definition for this platform");

		struct alignas(cr_type) FIR final {
			cr_type Revision        : 8 = 0b00000001;
			cr_type ProcessorID     : 8 = 0b11111111;
			cr_type SinglePrecision : 1 = 1;
			cr_type DoublePrecision : 1 = 1;
			cr_type _z_padding0     : 2 = 0;
			cr_type FixedPointW     : 1 = 0;
			cr_type FixedPointL     : 1 = 0;
			cr_type Float64         : 1 = 1;
			cr_type Has2008         : 1 = 1;
			cr_type Impl            : 4 = 0;
			cr_type _z_padding1     : 1 = 0;
			cr_type FREP            : 1 = 0;
			cr_type _z_padding2     : 2 = 0;

			_nothrow void clock() noexcept;

			_pure
			_nothrow operator cr_type () const __restrict noexcept {
				return std::bit_cast<cr_type>(*this);
			}
		};
		static_assert(sizeof(FIR) == sizeof(cr_type), "FIR register size mismatch");

		enum class RoundingMode : cr_type
		{
			ToNearest = 0,
			ToZero = 1,
			ToPositive = 2,
			ToNegative = 3,

			Default = ToNearest,
		};

		struct alignas(cr_type) FCSR final {
			RoundingMode RoundingMode : 2 = RoundingMode::Default; // 0 = RN, 1 = RZ, 2 = RP, 3 = RM
			cr_type Flags              : 5 = 0;
			cr_type Enables            : 5 = 0;
			cr_type Cause              : 6 = 0;
			cr_type NAN2008            : 1 = 1;
			cr_type ABS2008            : 1 = 1;
			cr_type _z_padding0        : 1 = 0;
			cr_type Impl               : 2 = 0;
			cr_type _z_padding1        : 1 = 0;
			cr_type FlushZero          : 1 = 0;
			cr_type _z_padding2        : 7 = 0;

			_nothrow void clock() noexcept;

			_pure
			_nothrow cr_type get_FEXR() const noexcept;
			_nothrow void set_FEXR(cr_type fexr) noexcept;

			_pure
			_nothrow cr_type get_FENR() const noexcept;
			_nothrow void set_FENR(cr_type fenr) noexcept;

			_nothrow bool set_flag(const cr_type flag) __restrict noexcept
			{
				Flags |= flag;
				return (Enables & flag) != 0;
			}

			_pure
			_nothrow operator cr_type () const __restrict noexcept {
				return std::bit_cast<cr_type>(*this);
			}
		};
		static_assert(sizeof(FCSR) == sizeof(cr_type), "FIR register size mismatch");

		static constexpr const size_t num_registers = 32;
		using register_file = std::array<register_type, num_registers /*- 1*/>;

	private:
		// TODO : make pointer, adjust jit
		// TODO: execution unit work
		FIR                       fir_;
		FCSR                      fcsr_;
		alignas(64) register_file registers_{ 0 };

	public:
		using coprocessor::get_processor;

		/*
		_pure
		_nothrow FIR & get_FIR() noexcept {
			return fir_;
		}
		*/

		_pure
		_nothrow FIR get_FIR() const noexcept {
			return fir_;
		}

		_pure
		_nothrow FCSR & get_FCSR() noexcept {
			return fcsr_;
		}

		_pure
		_nothrow FCSR get_FCSR() const noexcept {
			return fcsr_;
		}

		_nothrow coprocessor1(processor& __restrict processor) noexcept : coprocessor(processor) {}
		virtual _nothrow ~coprocessor1() noexcept override = default;

		virtual _nothrow void clock() override;

		// Get the register as a specific type
		template <typename T>
		_pure
		_nothrow T get_register(const uint32 idx) const noexcept {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			return std::bit_cast<T* __restrict>(&registers_[idx])[0];
		}

		// Get the register as a specific type
		template <typename T>
		_pure
		_nothrow T get_register_upper(const uint32 idx) const noexcept {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) == sizeof(register_type) / 2, "get_register_upper is casting to invalid size");
			return std::bit_cast<T* __restrict>(&registers_[idx])[1];
		}

		// Set the register from a given type
		template <typename T>
		_nothrow void set_register(const uint32 idx, const T value) noexcept {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			std::bit_cast<T* __restrict>(&registers_[idx])[0] = value;
		}

		// Set the register from a given type
		template <typename T>
		_nothrow void set_register_upper(const uint32 idx, const T value) noexcept {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) == sizeof(register_type) / 2, "get_register_upper is casting to invalid size");
			std::bit_cast<T* __restrict>(&registers_[idx])[1] = value;
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline const register_file& get_register_file() const noexcept
		{
			return registers_;
		}

		[[nodiscard]]
		_pure
		virtual _nothrow std::vector<std::string> compare(const coprocessor& __restrict other) const noexcept override;

	public:
#pragma region dynamic recompiler support
		template <typename TType = int16>
		requires (std::is_integral_v<TType>)
		struct recompiler_offsets final
		{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
			// floating-point implementation and revision register
			const TType fir       = value_assert<int16>(offsetof(coprocessor1, fir_       ));
			// floating-point control and status register
			const TType fcsr      = value_assert<int16>(offsetof(coprocessor1, fcsr_      ));
			// registers
			const TType registers = value_assert<int16>(offsetof(coprocessor1, registers_ ));
#pragma clang diagnostic pop

			template <typename TIntegerType>
			requires (std::is_integral_v<TIntegerType>)
			_pure _nothrow
			static constexpr recompiler_offsets<TIntegerType> get(const std::make_signed_t<TIntegerType> offset = -128) noexcept
			{
				return {
					.fir       = value_assert<TIntegerType>(recompiler_offsets{}.fir       + offset),
					.fcsr      = value_assert<TIntegerType>(recompiler_offsets{}.fcsr      + offset),
					.registers = value_assert<TIntegerType>(recompiler_offsets{}.registers + offset),
				};
			}
		};
#pragma endregion
	};
}
