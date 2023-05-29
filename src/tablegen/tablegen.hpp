#pragma once

#include "common.hpp"
#include "instructions/instructions.hpp"

#include <cstdio>
#include <unordered_map>

namespace vemips::tablegen {
	static constexpr const size_t max_signature_length = 4096;

	extern FILE* out_stream;

	using crstring = const char* __restrict;

	static _nothrow void PrintIndent(size_t indent) {
		while (indent--) {
			std::fputc('\t', out_stream);
		}
	}

	static _nothrow void newline() {
		std::fputc('\n', out_stream);
	}

#define _INDENTED for (bool once = true; once && ++indent; --indent, once = false)

	template <typename... Tt>
	static _nothrow void indented_print(const size_t indent, fmt::format_string<Tt...> format, Tt... args) {
		PrintIndent(indent);

		fmt::println(out_stream, format, std::forward<Tt>(args)...);
	}

	using info_t = mips::instructions::InstructionInfo;

	struct type_signature final {
		std::string procedure_name;
		crstring name;
		crstring signature;
		char suffix;
	};

	static _nothrow const type_signature& GetTypeSignature(const info_t& __restrict info) {
		static std::unordered_map<const info_t*, type_signature> signature_map;

		if (const auto iter = signature_map.find(&info); iter != signature_map.end()) {
			return iter->second;
		}

		crstring signature;
		crstring type_append;
		crstring name;
		char suffix;

		switch (info.Type) {
		case mips::instructions::instruction_type::void_fp:
			signature = "<void>";
			type_append = "_v";
			name = "instruction_type::void_fp";
			suffix = 'v';
			break;
		case mips::instructions::instruction_type::single_fp:
			signature = "<float>";
			type_append = "_f";
			name = "instruction_type::single_fp";
			suffix = 'f';
			break;
		case mips::instructions::instruction_type::double_fp:
			signature = "<double>";
			type_append = "_d";
			name = "instruction_type::double_fp";
			suffix = 'd';
			break;
		case mips::instructions::instruction_type::word_fp:
			signature = "<int32>";
			type_append = "_w";
			name = "instruction_type::word_fp";
			suffix = 'w';
			break;
		case mips::instructions::instruction_type::long_fp:
			signature = "<int64>";
			type_append = "_l";
			name = "instruction_type::long_fp";
			suffix = 'l';
			break;
		case mips::instructions::instruction_type::normal:
			signature = "";
			type_append = "";
			name = "instruction_type::normal";
			suffix = 'n';
			break;
		default:  // NOLINT(clang-diagnostic-covered-switch-default)
			xassert(false);
		}

		const auto result = signature_map.emplace(
			std::make_pair(
				&info,
				type_signature{
					.procedure_name = fmt::format("StaticProc_{}{}", info.Name, type_append),
					.name = name,
					.signature = signature,
					.suffix = suffix
				}
			)
		);

		return result.first->second;
	}

	static _nothrow std::string BuildProcedureCall(const info_t& __restrict info) {
		return fmt::format(
			"{}_NS::Execute{}",
			info.Name,
			GetTypeSignature(info).signature
		);
	}

	static _nothrow std::string static_proc(const info_t& __restrict info) {
		return fmt::format("return &{};", GetTypeSignature(info).procedure_name);
	}

	static _nothrow std::string proc_call(const info_t& __restrict info) {
		return fmt::format("{{ {}(i,p); return true; }}", BuildProcedureCall(info));
	}

	namespace _hash_detail {
		template <typename HashType>
		struct fnv_constants;

#if __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wunused-const-variable"
#endif
		// ReSharper disable once CppExplicitSpecializationInNonNamespaceScope
		template<> struct fnv_constants<uint64>
		{
			static constexpr const uint64 offset_basis = 0xcbf29ce484222325ull;
			static constexpr const uint64 prime = 0x00000100000001B3ull;
		};

		// ReSharper disable once CppExplicitSpecializationInNonNamespaceScope
		template<> struct fnv_constants<uint32>
		{
			static constexpr const uint32 offset_basis = 0x811c9dc5u;  // NOLINT(clang-diagnostic-unused-const-variable)
			static constexpr const uint32 prime = 0x01000193u; // NOLINT(clang-diagnostic-unused-const-variable)
		};
#if __clang__
#	pragma clang diagnostic pop
#endif
	}

	static constexpr _nothrow size_t fnv_hash(crstring str)
	{
		auto hash = _hash_detail::fnv_constants<size_t>::offset_basis;

		for (char c; (c = *str) != '\0'; ++str)
		{
			hash *= _hash_detail::fnv_constants<size_t>::prime;
			hash ^= uint8(c);
		}

		return hash;
	}

	struct proc_info_data final
	{
		const crstring name;
		const crstring signature;

		_nothrow bool operator ==(const proc_info_data& __restrict other) const __restrict
		{
			return std::strcmp(name, other.name) == 0 && signature == other.signature;
		}

		_nothrow bool operator !=(const proc_info_data& __restrict other) const __restrict
		{
			return !(*this == other);
		}

		struct hash
		{
			_nothrow std::size_t operator()(proc_info_data const& __restrict s) const __restrict
			{
				const auto hash0 = fnv_hash(s.name);
				const auto hash1 = fnv_hash(s.signature);
				return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
			}
		};
	};

	using map_t = mips::instructions::MapOrInfo;
}
