#pragma once

#include "pch.hpp"
#include "common.hpp"
#include "mips/coprocessor/coprocessor.hpp"

namespace mips {
	class coprocessor1 final : public coprocessor {
		friend class jit1;
		friend class Jit1_CodeGen;

	public:
		using register_type = double;
		static_assert(sizeof(double) == 8, "Need custom FPU definition for this platform");
		static_assert(sizeof(float) == 4, "Need custom FPU definition for this platform");

		struct FIR {
			uint32 Revision : 8;
			uint32 ProcessorID : 8;
			uint32 SinglePrecision : 1;
			uint32 DoublePrecision : 1;
			uint32 _Z0 : 2;
			uint32 FixedPointW : 1;
			uint32 FixedPointL : 1;
			uint32 Float64 : 1;
			uint32 Has2008 : 1;
			uint32 Impl : 4;
			uint32 _Z1 : 1;
			uint32 FREP : 1;
			uint32 _Z2 : 2;

			FIR();
			void clock() __restrict;

			operator uint32 & __restrict () __restrict {
				return *(uint32 * __restrict)this;
			}
		};
		static_assert(sizeof(FIR) == sizeof(uint32), "FIR register size mismatch");

		struct FCSR {
			uint32 RoundingMode : 2; // 0 = RN, 1 = RZ, 2 = RP, 3 = RM
			uint32 Flags : 5;
			uint32 Enables : 5;
			uint32 Cause : 6;
			uint32 NAN2008 : 1;
			uint32 ABS2008 : 1;
			uint32 _Z0 : 1;
			uint32 Impl : 2;
			uint32 _Z1 : 1;
			uint32 FlushZero : 1;
			uint32 _Z2 : 7;

			FCSR();
			void clock() __restrict;

			uint32 get_FEXR() const __restrict;
			void set_FEXR(uint32 fexr) __restrict;

			uint32 get_FENR() const __restrict;
			void set_FENR(uint32 fenr) __restrict;

			operator uint32 & __restrict () __restrict {
				return *(uint32 * __restrict)this;
			}

			operator uint32 () const __restrict {
				return *(uint32 * __restrict)this;
			}
		};
		static_assert(sizeof(FCSR) == sizeof(uint32), "FIR register size mismatch");

	private:
		static constexpr const size_t NumRegisters = 32;

		std::array<register_type, NumRegisters>	m_registers{ 0 };
		FIR													m_fir;
		FCSR												  m_fcsr;

	public:
		FIR & get_FIR() __restrict {
			return m_fir;
		}

		FIR get_FIR() const __restrict {
			return m_fir;
		}

		FCSR & get_FCSR() __restrict {
			return m_fcsr;
		}

		FCSR get_FCSR() const __restrict {
			return m_fcsr;
		}

		coprocessor1(processor & __restrict processor) : coprocessor(processor) {}
		virtual ~coprocessor1() {}

		virtual void clock() __restrict;

		// Get the register as a specific type
		template <typename T>
		T get_register(uint32 idx) const __restrict {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			return ((T * __restrict)&m_registers[idx])[0];
		}

		// Get the register as a specific type
		template <typename T>
		T get_register_upper(uint32 idx) const __restrict {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) == sizeof(register_type) / 2, "get_register_upper is casting to invalid size");
			return ((T * __restrict)&m_registers[idx])[1];
		}

		// Set the register from a given type
		template <typename T>
		void set_register(uint32 idx, T value) __restrict{
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			((T * __restrict)&m_registers[idx])[0] = value;
		}

		// Set the register from a given type
		template <typename T>
		void set_register_upper(uint32 idx, T value) __restrict {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) == sizeof(register_type) / 2, "get_register_upper is casting to invalid size");
			((T * __restrict)&m_registers[idx])[1] = value;
		}
	};
}
