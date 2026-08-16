// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include "pch.hpp"
#include <common.hpp>

#include "memory_source.hpp"

#include <optional>


_pure _nothrow std::optional<uptr_guest> mips::memory_source::get_first_unreadable(
	const uptr_guest offset,
	const usize_guest size
) const __restrict noexcept
{
	for (usize_guest current_offset = 0; current_offset < size; ++current_offset)
	{

		if (
			const uptr_guest address = offset + current_offset;
			address == 0U || !is_readable(address)
		) {
			return address;
		}
	}

	return {};
}

_pure _nothrow std::optional<uptr_guest> mips::memory_source::get_first_unwritable(
	const uptr_guest offset,
	const usize_guest size
) const __restrict noexcept
{
	for (usize_guest current_offset = 0; current_offset < size; ++current_offset)
	{

		if (
			const uptr_guest address = offset + current_offset;
			address == 0U || !is_writable(address)
		) {
			return address;
		}
	}

	return {};
}
