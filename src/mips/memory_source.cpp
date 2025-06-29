// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include "pch.hpp"

#include "memory_source.hpp"


std::optional<uint32> mips::memory_source::get_first_unreadable(const uint32 offset, const uint32 size) const __restrict
{
	for (uint32 current_offset = 0; current_offset < size; ++current_offset)
	{

		if (
			const uint32 address = offset + current_offset;
			address == 0U || !is_readable(address)
		) {
			return address;
		}
	}

	return {};
}

std::optional<uint32> mips::memory_source::get_first_unwritable(const uint32 offset, const uint32 size) const __restrict
{
	for (uint32 current_offset = 0; current_offset < size; ++current_offset)
	{

		if (
			const uint32 address = offset + current_offset;
			address == 0U || !is_writable(address)
		) {
			return address;
		}
	}

	return {};
}
