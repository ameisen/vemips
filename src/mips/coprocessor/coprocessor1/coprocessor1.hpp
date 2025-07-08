#pragma once

#include "pch.hpp"
#include "common.hpp"
#include "mips/coprocessor/coprocessor.hpp"

namespace mips {
	class _empty_bases coprocessor1 final : public coprocessor {
		friend class jit1;
		friend class Jit1_CodeGen;

	public:
		using register_type = double;
		static_assert(sizeof(double) == 8, "Need custom FPU definition for this platform");
		static_assert(sizeof(float) == 4, "Need custom FPU definition for this platform");

		struct alignas(uint32) FIR final {
			uint32 Revision : 8;
			uint32 ProcessorID : 8;
			uint32 SinglePrecision : 1;
			uint32 DoublePrecision : 1;
			uint32 : 2;
			uint32 FixedPointW : 1;
			uint32 FixedPointL : 1;
			uint32 Float64 : 1;
			uint32 Has2008 : 1;
			uint32 Impl : 4;
			uint32 : 1;
			uint32 FREP : 1;
			uint32 : 2;

			_nothrow FIR() noexcept;
			_nothrow void clock() noexcept;

			_nothrow operator uint32 () const __restrict noexcept {
				return std::bit_cast<uint32>(*this);
			}
		};
		static_assert(sizeof(FIR) == sizeof(uint32), "FIR register size mismatch");

		struct alignas(uint32) FCSR final {
			uint32 RoundingMode : 2; // 0 = RN, 1 = RZ, 2 = RP, 3 = RM
			uint32 Flags : 5;
			uint32 Enables : 5;
			uint32 Cause : 6;
			uint32 NAN2008 : 1;
			uint32 ABS2008 : 1;
			uint32 _z_padding0 : 1;
			uint32 Impl : 2;
			uint32 _z_padding1 : 1;
			uint32 FlushZero : 1;
			uint32 _z_padding2 : 7;

			_nothrow FCSR() noexcept;
			_nothrow void clock() noexcept;

			_nothrow uint32 get_FEXR() const noexcept;
			_nothrow void set_FEXR(uint32 fexr) noexcept;

			_nothrow uint32 get_FENR() const noexcept;
			_nothrow void set_FENR(uint32 fenr) noexcept;

			_nothrow bool set_flag(const uint32_t flag) __restrict noexcept
			{
				Flags |= flag;
				return (Enables & flag) != 0;
			}

			_nothrow operator uint32 () const __restrict noexcept {
				return std::bit_cast<uint32>(*this);
			}
		};
		static_assert(sizeof(FCSR) == sizeof(uint32), "FIR register size mismatch");

	private:
		static constexpr const size_t NumRegisters = 32;

		// TODO : make pointer, adjust jit
		// TODO: execution unit work
		alignas(64) std::array<register_type, NumRegisters>	m_registers{ 0 };
		FIR													m_fir;
		FCSR												m_fcsr;

	public:
		_nothrow FIR & get_FIR() noexcept {
			return m_fir;
		}

		_nothrow FIR get_FIR() const noexcept {
			return m_fir;
		}

		_nothrow FCSR & get_FCSR() noexcept {
			return m_fcsr;
		}

		_nothrow FCSR get_FCSR() const noexcept {
			return m_fcsr;
		}

		_nothrow coprocessor1(processor & __restrict processor) noexcept : coprocessor(processor) {}
		virtual _nothrow ~coprocessor1() noexcept override = default;

		virtual _nothrow void clock() override;

		// Get the register as a specific type
		template <typename T>
		_nothrow T get_register(const uint32 idx) const {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			return std::bit_cast<T * __restrict>(&m_registers[idx])[0];
		}

		// Get the register as a specific type
		template <typename T>
		_nothrow T get_register_upper(const uint32 idx) const {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) == sizeof(register_type) / 2, "get_register_upper is casting to invalid size");
			return std::bit_cast<T * __restrict>(&m_registers[idx])[1];
		}

		// Set the register from a given type
		template <typename T>
		_nothrow void set_register(const uint32 idx, const T value) {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			std::bit_cast<T * __restrict>(&m_registers[idx])[0] = value;
		}

		// Set the register from a given type
		template <typename T>
		_nothrow void set_register_upper(const uint32 idx, const T value) {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) == sizeof(register_type) / 2, "get_register_upper is casting to invalid size");
			std::bit_cast<T * __restrict>(&m_registers[idx])[1] = value;
		}

	public:
#pragma region dynamic recompiler support
		enum class offset_type : uint8
		{
			registers
		};

		static constexpr intptr get_address_offset_static (const offset_type type)
		{
			switch (type)
			{
				case offset_type::registers:
					return offsetof(coprocessor1, m_registers);
			}

			xassert(false);
		}

#pragma endregion
	};
}
