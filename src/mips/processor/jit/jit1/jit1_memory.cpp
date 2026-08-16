#include "pch.hpp"

#include <bit>
#include <limits>
#include <optional>
#include <tuple>

#include "jit1.hpp"
#include "jit1_xbyak.hpp"
#include "mips/mips_common.hpp"
#include "mips/mmu.hpp"
#include "mips/processor//processor.hpp"
#include "mips/instructions/coprocessor1_support.hpp"
#include "mips/instructions/instructions_common.hpp"
#include "codegen.hpp"

using namespace mips;

namespace
{
	[[nodiscard]]
	VEMIPS_JIT_ABI static _nothrow uintptr VEMIPS_JIT_ABI_INFIX mem_write_jit(
		processor& __restrict proc,
		const uptr_guest address,
		const usize_guest size
	) noexcept {
		return proc.get_mem_write_jit(address, size);
	}

	[[nodiscard]]
	VEMIPS_JIT_ABI static _nothrow uintptr VEMIPS_JIT_ABI_INFIX mem_read_jit(
		processor& __restrict proc,
		const uptr_guest address,
		const usize_guest size
	) noexcept
	{
		return proc.get_mem_read_jit(address, size);
	}

	[[nodiscard]]
	_func_const
	static constexpr _nothrow _forceinline uint32 make_size_mask(const uint8 size) noexcept
	{
		switch (size)
		{
			case 1:
				return 0U;
			case 2:
				return 0b1U;
			case 4:
				return 0b11U;
			case 8:
				return 0b111U;
			default:
				xunreachable("Unrecognized Size");
		}
	}
}

std::optional<Jit1_CodeGen::except_result> Jit1_CodeGen::write_STORE(
	jit1::ChunkOffset& __restrict chunk_offset,
	const uptr_guest address,
	const instruction_t instruction,
	const mips::instructions::InstructionInfo& __restrict instruction_info
)
{
	// rd = rs + rt
	const instructions::GPRegister<21, 5> base(instruction, jit_.processor_);

	auto&& op_base = get_register_op32(base);

	uint8 store_size;
	bool fpu = false;
	bool e = false;

	if (IS_INSTRUCTION(instruction_info, PROC_SB)) {
		store_size = 1;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SBE)) {
		e = true;
		store_size = 1;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SC)) {
		return {};
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SCE)) {
		e = true;
		return {};
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SCWP)) {
		return {};
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SCWPE)) {
		e = true;
		return {};
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SH)) {
		store_size = 2;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SHE)) {
		e = true;
		store_size = 2;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SW)) {
		store_size = 4;
	}
	else if(IS_INSTRUCTION(instruction_info, PROC_SWE)) {
		e = true;
		store_size = 4;
	}
	else if(IS_INSTRUCTION(instruction_info, COP1_SDC1_v)) {
		store_size = 8;
		fpu = true;
	}
	else if(IS_INSTRUCTION(instruction_info, COP1_SWC1_v)) {
		store_size = 4;
		fpu = true;
	}
	else {
		return {};
	}

	xassert(store_size >= 1 && store_size <= 8);

	const auto mmu_type = jit_.processor_.mmu_type_;
	
	ssize_guest offset;
	if (!e) {
		offset = instructions::TinyInt<16>(instruction).sextend<ssize_guest>();
	}
	else {
		offset = instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>();
	}

	const auto misaligned_address_handler = jit_.processor_.get_misaligned_address_handling();

	const bool static_offset = base.is_zero();

	bool is_ll_aligned = static_offset ?
		((offset & ~(processor::load_link_align - 1)) == 0) :
		false;

	if (mmu_type == mmu::emulated)
	{
		if (static_offset && (offset & make_size_mask(store_size)) != 0U)
		{
			switch (misaligned_address_handler)
			{
				using enum processor::misaligned_address_handling;
				case exception:
					set(eax, address);
					set(ecx, offset);
					jmp(intrinsics_.ades, T_NEAR);
					return except_result::always_throw;

				case align:
					offset &= ~make_size_mask(store_size);
					break;

				case keep:
					break;
			}
		}

		const Xbyak::Label valid_ptr;

		lea(rcx, dword[rbp - 128]);
		// 'rcx' is the first parameter (processor ptr)

		if (!static_offset) {
			// The effective address is '[base] + offset'
			if (offset && op_base.isREG())
			{
				xassert(!is_same(op_base, edx)); // TODO : handle this case
				lea(edx, dword[op_base.as_reg() + offset]);
			}
			else
			{
				xassert(!is_same(op_base, edx)); // TODO : handle this case
				mov(edx, op_base);
				add_ex(edx, offset);
			}
			switch (misaligned_address_handler)
			{
				using enum processor::misaligned_address_handling;
				case exception:
				{
					const Xbyak::Label no_misalign;
					test(edx, make_size_mask(store_size));
					jz(no_misalign, T_SHORT);
					set(eax, address);
					set(ecx, offset);
					jmp(intrinsics_.ades, T_NEAR);
					L(no_misalign);
				}
				break;

				case align:
					and_(edx, ~make_size_mask(store_size));
					break;

				case keep:
					break;
			}
		}
		else {
			set(edx, offset);
		}

		// 'edx' is the second parameter (address)
		set(r8d, store_size); // TODO = store_size - 1 would be more efficient
		mov(r13d, edx); // store to non-volatile for after call if there's an exception.
		// 'r8' is the third parameter (size)
		std::ignore = call_ex<JumpFlags::ExternalCall>(ptr_cast(&mem_write_jit), rax);
		// rax now has our destination pointer.
		mov(r13, rax); // save the pointer off to non-volatile r13.
		test(rax, rax);
		jnz(valid_ptr, T_SHORT);
		set(eax, address); // TODO : was 'ecx' src, why?
		mov(ecx, r13d);
		jmp(intrinsics_.ades, T_NEAR);
		L(valid_ptr);
	}
	else
	{
		// This would be far faster if we could memory map easily. Then we could just wrap the address range around.
		if (static_offset)
		{
			if ((offset & make_size_mask(store_size)) != 0U)
			{
				switch (misaligned_address_handler)
				{
					using enum processor::misaligned_address_handling;
					case exception:
						set(eax, address);
						set(ecx, offset);
						jmp(intrinsics_.ades, T_NEAR);
						return except_result::always_throw;

					case align:
						offset = static_cast<uptr_guest>(offset) & ~make_size_mask(store_size);
						if (store_size >= processor::load_link_align)
						{
							is_ll_aligned = true;
						}
						break;

					case keep:
						break;
				}
			}

			// If base is 0, the address is just offset. This simplifies things, though we need to treat the offset as unsigned.
			// Check for basic range things.
			const uptr_guest start_address = uptr_guest(offset);
			using uptr_guest_bigger = uint_fitted<usize(std::numeric_limits<uptr_guest>::max()) + std::numeric_limits<usize_guest>::max()>;
			const uptr_guest_bigger end_address = uptr_guest_bigger(offset) + store_size;

			// quick error checking
			if (mmu_type == mmu::none) {
				// this checks if it is in range, or if it overflows and thus overwrites '0'
				if (
					(end_address + jit_.processor_.stack_size_) > jit_.processor_.memory_size_ ||
					(uptr(offset) + usize(jit_.processor_.stack_size_) + usize(store_size)) > 0x1'0000'0000ull ||
					start_address == 0u
				)
				{
					set(eax, address);
					set(ecx, start_address);
					jmp(intrinsics_.ades, T_NEAR);
					return except_result::always_throw;
				}
			}
			else if (mmu_type == mmu::host) {
				// I'm going to do this the dumb way.
				for (uptr addr = start_address; addr < end_address; ++addr) {
					if (
						addr == 0 ||
						(
							jit_.processor_.stack_size_ &&
							addr >= jit_.processor_.memory_size_ &&
							addr < uint32(0x1'0000'0000ull - jit_.processor_.stack_size_)
						)
					)
					{
						set(eax, address);
						set(ecx, addr);
						jmp(intrinsics_.ades, T_NEAR);
						return except_result::always_throw;
					}
				}
			}

			mov(rdx, qword[rbp + offsets.memory_ptr]);
			set(eax, offset);
		}
		else {
			if (offset && op_base.isREG())
			{
				lea(eax, dword[op_base.as_reg() + offset]);
			}
			else
			{
				mov(eax, op_base);
				add_ex(eax, offset);
			}

			switch (misaligned_address_handler)
			{
				using enum processor::misaligned_address_handling;
				case exception:
				{
					const Xbyak::Label no_misalign;
					test(eax, make_size_mask(store_size));
					jz(no_misalign, T_SHORT);
					mov(ecx, eax);
					set(eax, address);
					jmp(intrinsics_.ades, T_NEAR);
					L(no_misalign);
					
					if (store_size >= processor::load_link_align)
					{
						is_ll_aligned = true;
					}
				}
				break;

				case align:
					and_(eax, ~make_size_mask(store_size));
					if (store_size >= processor::load_link_align)
					{
						is_ll_aligned = true;
					}
					break;

				case keep:
					break;
			}

			// error checking
			if (mmu_type == mmu::none) {
				const Xbyak::Label ades;

				if (jit_.processor_.stack_size_)
				{
					// zero test
					if (store_size == 1)
					{
						test(eax, eax);
						jz(ades, T_SHORT);
					}
					else
					{
						lea(edx, dword[eax + (store_size - 1)]);
						cmp(edx, store_size - 1);
						jbe(ades, T_SHORT);
					}

					// Offset for stack
					add_ex(eax, jit_.processor_.stack_size_);

					// check for range
					cmp(eax, uint32((jit_.processor_.memory_size_ - store_size) /* - offset */));
				}
				else
				{
					// check for range
					const intptr pow2_memsize = intptr(std::bit_ceil(jit_.processor_.memory_size_));
					const ssize_guest addend = ssize_guest(pow2_memsize + (store_size - 1));
					const ssize_guest cmpand = ssize_guest(pow2_memsize + store_size);
					lea(edx, dword[eax + addend]);
					cmp(edx, cmpand);
					jae(ades, T_SHORT);
				}
				const Xbyak::Label no_ades;
				jbe(no_ades, T_SHORT);
				if (jit_.processor_.stack_size_)
				{
					lea(eax, dword[eax - jit_.processor_.stack_size_]);
				}
				L(ades);
				mov(ecx, eax);
				set(eax, address);
				jmp(intrinsics_.ades, T_NEAR);
				L(no_ades);
			}

			mov(rdx, qword[rbp + offsets.memory_ptr]);
		}
		// rdx == address
	}

	const auto get_sized_address = [this](const Xbyak::RegExp& address, const uint8 size)
	{
		switch (size)
		{
			case 1:
				return byte[address];
			case 2:
				return word[address];
			case 4:
				return dword[address];
			case 8:
				return qword[address];
			default:
				xunreachable("undefined size");
		}
	};

	const auto get_sized_register_op = [this](const auto& register_entry, const uint8 size)
	{
		switch (size)
		{
			case 1:
				return get_register_op8(register_entry);
			case 2:
				return get_register_op16(register_entry);
			case 4:
				return get_register_op32(register_entry);
			case 8:
				return get_register_op64(register_entry);
			default:
				xunreachable("undefined size");
		}
	};

	const auto get_sized_register = [](const Xbyak::Reg& reg, const uint8 size) -> Xbyak::Reg
	{
		switch (size)
		{
			case 1:
				return reg.cvt8();
			case 2:
				return reg.cvt16();
			case 4:
				return reg.cvt32();
			case 8:
				return reg.cvt64();
			default:
				xunreachable("undefined size");
		}
	};

	// perform the actual store.
	if (!fpu) {
		const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

		if (mmu_type != mmu::emulated) {
			auto&& target_operand = rdx + rax;

			if (rt.is_zero()) {
				switch (store_size) {
				case 1:
					mov(byte[target_operand], 0); break;
				case 2:
					mov(word[target_operand], 0); break;
				case 4:
					xor_(ecx, ecx);
					mov(dword[target_operand], ecx); break;
				case 8:
					xor_(ecx, ecx);
					mov(qword[target_operand], rcx); break;
				default:
					_assume(0);
				}
			}
			else {
				std::ignore = mov_ex(
					get_sized_address(target_operand, store_size),
					get_sized_register_op(rt, store_size),
					get_sized_register(rcx, store_size)
				);
			}
		}
		else {
			if (rt.is_zero()) {
				switch (store_size) {
				case 1:
					mov(byte[r13], 0); break;
				case 2:
					mov(word[r13], 0); break;
				case 4:
					xor_(ecx, ecx);
					mov(dword[r13], ecx); break;
				case 8:
					xor_(ecx, ecx);
					mov(qword[r13], rcx); break;
				default:
					_assume(0);
				}
			}
			else {
				const auto& src = get_sized_register_op(rt, store_size);
				const auto& dst = get_sized_address(r13, store_size);
				std::ignore = mov_ex(dst, src, get_sized_register(cl, store_size));
			}
		}
	}
	else
	{
		// TODO fix offsets.
		const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
		const int16 ft_offset = ft.get_offset();

		if (mmu_type != mmu::emulated) {
			auto&& target_operand = rdx + rax;

			const auto& tmp = get_sized_register(cl, store_size);
			mov(tmp, get_sized_address(r12 + ft_offset, store_size));
			mov(get_sized_address(target_operand, store_size), tmp);
		}
		else {
			const auto& tmp = get_sized_register(cl, store_size);
			mov(tmp, get_sized_address(r12 + ft_offset, store_size));
			mov(get_sized_address(r13, store_size), tmp);
		}
	}
	
	// emulated MMU already handles load-link stuff
	if (mmu_type != mmu::emulated)
	{
		if (
			jit_.processor_.llsc_type_ != llsc::none &&
			jit_.processor_.llsc_type_ != llsc::coarse
		)
		{
			xassert(jit_.processor_.llsc_type_ == llsc::fine);

			if (static_offset)
			{
				const Xbyak::Label no_ll_match;

				if (is_ll_aligned)
				{
					cmp(eax, dword[rbp + offsets.ll]);
					jne(no_ll_match);
				}
				else
				{
					const uptr_guest aligned_offset = align_mask<processor::load_link_align>(offset);

					cmp(dword[rbp + offsets.ll], aligned_offset);
					jne(no_ll_match);
				}

				mov(dword[rbp + offsets.ll], 0);
				L(no_ll_match);
			}
			else
			{
				const Xbyak::Label no_ll_match;

				if (is_ll_aligned)
				{
					cmp(eax, dword[rbp + offsets.ll]);
					jne(no_ll_match);
				}
				else
				{
					mov(ecx, dword[rbp + offsets.ll]);
					xor_(ecx, eax); // non-matching bits are 1
					and_(ecx, ~(processor::load_link_align - 1)); // will be zero if they're the same (after accounting for alignment)
					test(ecx, ecx);
					jnz(no_ll_match);
				}

				mov(dword[rbp + offsets.ll], 0);
				L(no_ll_match);
			}
		}
		else if (jit_.processor_.llsc_type_ == llsc::coarse)
		{
			mov(dword[rbp + offsets.ll], 0);
		}
	}

	if (mmu_type == mmu::emulated && !jit_.processor_.readonly_exec_) {
		const Xbyak::Label no_flush;
		//cmp(eax, 0);
		//je(no_flush);
		test(eax, eax);
		jz(no_flush, T_SHORT);
		set(eax, address);
		jmp(intrinsics_.save_return, T_NEAR);
		L(no_flush);
	}

	return except_result::can_throw;
}

std::optional<Jit1_CodeGen::except_result> Jit1_CodeGen::write_LOAD(
	jit1::ChunkOffset& __restrict chunk_offset,
	const uptr_guest address,
	const instruction_t instruction,
	const mips::instructions::InstructionInfo& __restrict instruction_info
)
{
	// rd = rs + rt
	const instructions::GPRegister<21, 5> base(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto&& op_base = get_register_op32(base);
	auto&& op_rt = get_register_op32(rt);

	const auto misaligned_address_handler = jit_.processor_.get_misaligned_address_handling();

	if (rt.is_zero() && misaligned_address_handler != processor::misaligned_address_handling::exception) {
		// nop
		return except_result::none;
	}

	const auto mmu_type = jit_.processor_.mmu_type_;

	const auto get_address = [&](ssize_guest offset, const uint8 load_size, const bool alignment_check = true) {
		const Xbyak::Label valid_ptr;

		if (mmu_type == mmu::emulated) {
			if (alignment_check && base.is_zero() && (offset & make_size_mask(load_size)) != 0U)
			{
				switch (misaligned_address_handler)
				{
					using enum processor::misaligned_address_handling;
					case exception:
						set(eax, address);
						set(ecx, offset);
						jmp(intrinsics_.adel, T_NEAR);
						return false;

					case align:
						offset &= ~make_size_mask(load_size);
						break;

					case keep:
						break;
				}
			}

			lea(rcx, qword[rbp - 128]);
			// 'rcx' is the first parameter (processor ptr)

			if (!base.is_zero()) {
				// The effective address is '[base] + offset'
				if (offset && op_base.isREG())
				{
					lea(edx, dword[op_base.as_reg() + offset]);
				}
				else
				{
					mov(edx, op_base);
					add_ex(edx, offset);
				}

				if (alignment_check)
				{
					switch (misaligned_address_handler)
					{
						using enum processor::misaligned_address_handling;
						case exception:
						{
							const Xbyak::Label no_misalign;
							test(edx, make_size_mask(load_size));
							jz(no_misalign, T_SHORT);
							set(eax, address);
							mov(ecx, edx);
							jmp(intrinsics_.adel, T_NEAR);
							L(no_misalign);
						}
						break;

						case align:
							and_(edx, ~make_size_mask(load_size));
							break;

						case keep:
							break;
					}
				}
			}
			else {
				set(edx, offset);
			}

			// 'edx' is the second parameter (address)
			set(r8d, load_size);
			mov(r13d, edx); // store to non-volatile for after call if there's an exception.
			// 'r8' is the third parameter (size)
			std::ignore = call_ex<JumpFlags::ExternalCall>(ptr_cast(&mem_read_jit), rax);
			// rax now has our destination pointer.
			mov(r13, rax); // save the pointer off to non-volatile r13.
			//cmp(rax, 0);
			//jne(valid_ptr);
			test(rax, rax);
			jnz(valid_ptr, T_SHORT);
			set(eax, address); // was ecx source - why?
			mov(ecx, r13d);
			jmp(intrinsics_.adel, T_NEAR);
			L(valid_ptr);
		}
		else {
			// This would be far faster if we could memory map easily. Then we could just wrap the address range around.
			if (base.is_zero()) {
				if (alignment_check && (offset & make_size_mask(load_size)) != 0U)
				{
					switch (misaligned_address_handler)
					{
						using enum processor::misaligned_address_handling;
						case exception:
							set(eax, address);
							set(ecx, offset);
							jmp(intrinsics_.adel, T_NEAR);
							return false;

						case align:
							offset = static_cast<uint32>(offset) & ~make_size_mask(load_size);
							break;

						case keep:
							break;
					}
				}

				// If base is 0, the address is just offset. This simplifies things, though we need to treat the offset as unsigned.
				// Check for basic range things.
				const uptr_guest start_address = uptr_guest(offset);
				using uptr_guest_bigger = uint_fitted<usize(std::numeric_limits<uptr_guest>::max()) + std::numeric_limits<usize_guest>::max()>;
				const uptr_guest_bigger end_address = uptr_guest_bigger(offset) + load_size;

				// quick error checking
				if (mmu_type == mmu::none) {
					// this checks if it is in range, or if it overflows and thus overwrites '0'
					if (
						(end_address + jit_.processor_.stack_size_) > jit_.processor_.memory_size_ ||
						(uptr(offset) + usize(jit_.processor_.stack_size_) + usize(load_size)) > 0x100000000ull ||
						start_address == 0u
					)
					{
						set(eax, address);
						set(ecx, start_address);
						jmp(intrinsics_.adel, T_NEAR);
						return false;
					}
				}
				else if (mmu_type == mmu::host) {
					// I'm going to do this the dumb way.
					for (uptr addr = start_address; addr < end_address; ++addr) {
						if (
							addr == 0 ||
							(
								jit_.processor_.stack_size_ &&
								addr >= jit_.processor_.memory_size_ &&
								addr < uptr(0x1'0000'0000ull - jit_.processor_.stack_size_)
							)
						) {
							set(eax, address);
							set(ecx, addr);
							jmp(intrinsics_.adel, T_NEAR);
							return false;
						}
					}
				}

				mov(rdx, qword[rbp + offsets.memory_ptr]);
				set(eax, offset);
			}
			else {
				if (offset && op_base.isREG())
				{
					lea(eax, dword[op_base.as_reg() + offset]);
				}
				else
				{
					mov(eax, op_base);
					add_ex(eax, offset);
				}

				if (alignment_check)
				{
					switch (misaligned_address_handler)
					{
						using enum processor::misaligned_address_handling;
						case exception:
						{
							const Xbyak::Label no_misalign;
							test(eax, make_size_mask(load_size));
							jz(no_misalign, T_SHORT);
							mov(ecx, eax);
							set(eax, address);
							jmp(intrinsics_.adel, T_NEAR);
							L(no_misalign);
						}
						break;

						case align:
							and_(eax, ~make_size_mask(load_size));
							break;

						case keep:
							break;
					}
				}

				// error checking
				if (mmu_type == mmu::none) {
					const Xbyak::Label adel;

					// zero test
					if (load_size == 1)
					{
						test(eax, eax);
						jz(adel, T_SHORT);
					}
					else
					{
						lea(edx, dword[eax + (load_size - 1)]);
						cmp(edx, load_size - 1);
						jbe(adel, T_SHORT);
					}

					// Offset for stack
					if (jit_.processor_.stack_size_)
					{
						add_ex(eax, jit_.processor_.stack_size_);
					}

					// check for range
					// address 1000
					// offset 500
					// eax = 1500
					// memory size = 1250
					// load size = 4
					// first = 1246
					// second = 1246 - 500 = 746
					cmp(eax, uint32((jit_.processor_.memory_size_ - load_size) /* - offset */));
					const Xbyak::Label no_adel;
					jbe(no_adel, T_SHORT);
					if (jit_.processor_.stack_size_)
					{
						lea(eax, dword[eax - jit_.processor_.stack_size_]);
					}
					L(adel);
					mov(ecx, eax);
					set(eax, address);
					jmp(intrinsics_.adel, T_NEAR);
					L(no_adel);
				}

				mov(rdx, qword[rbp + offsets.memory_ptr]);
			}
			// rdx == address
		}
		return true;
	};

	// get_offset: r13 = mem pointer

	if (mmu_type == mmu::emulated) {
		try {
			if (IS_INSTRUCTION(instruction_info, PROC_LB)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, byte[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LH)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, word[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LL)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLE)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWP)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWPE)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LW)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				std::ignore = mov_ex(op_rt, dword[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				std::ignore = mov_ex(op_rt, dword[r13], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LDC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(rax, qword[r13]);
				mov(qword[r12 + ft_offset], rax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LWC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(eax, dword[r13]);
				mov(dword[r12 + ft_offset], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWPC)) {
				// load word PC relative - gets special offset handling.
				const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
				const ssize_guest offset = instructions::TinyInt<21>(instruction << 2).sextend<ssize_guest>();

				auto&& op_rs = get_register_op32(rs);

				if (!get_address(address + offset, 4)) {
					return except_result::always_throw;
				}

				std::ignore = mov_ex(op_rs, dword[r13], eax);
			}
			else {
				return {};
			}
		}
		catch (...) {
			// TODO : eh?
			return except_result::always_throw;
		}
	}
	else { // no mmu
		try {
			auto&& src = rdx + rax;

			if (IS_INSTRUCTION(instruction_info, PROC_LB)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LBUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 1)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, byte[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LH)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movsx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHU)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LHUE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 2)) {
					return except_result::always_throw;
				}

				std::ignore = movzx_ex(op_rt, word[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LL)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLE)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWP)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LLWPE)) {
				return {};
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LW)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				std::ignore = mov_ex(op_rt, dword[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWE)) {
				if (!get_address(instructions::TinyInt<9>(instruction >> 7).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				std::ignore = mov_ex(op_rt, dword[src], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LDC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(rax, qword[src]);
				mov(qword[r12 + ft_offset], rax);
			}
			else if (IS_INSTRUCTION(instruction_info, COP1_LWC1_v)) {
				if (!get_address(instructions::TinyInt<16>(instruction).sextend<ssize_guest>(), 4)) {
					return except_result::always_throw;
				}

				const instructions::FPRegister<16, 5> ft(instruction, jit_.processor_.get_fpu_coprocessor());
				const int16 ft_offset = ft.get_offset();

				mov(eax, dword[src]);
				mov(dword[r12 + ft_offset], eax);
			}
			else if (IS_INSTRUCTION(instruction_info, PROC_LWPC)) {
				// load word PC relative - gets special offset handling.
				const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
				const ssize_guest offset = instructions::TinyInt<21>(instruction << 2).sextend<ssize_guest>();

				auto&& op_rs = get_register_op32(rs);

				if (!get_address(address + offset, 4, false)) {
					return except_result::always_throw;
				}

				std::ignore = mov_ex(op_rs, dword[src], eax);
			}
			else {
				return {};
			}
		}
		catch (...) {
			// TODO : eh?
			return except_result::always_throw;
		}
	}

	return except_result::can_throw;
}
