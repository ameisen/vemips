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
			fputc('\t', out_stream);
		}
	}

	// This isn't a comprehensive version a-la Alexandrescu's. This is just a quick hack.
	namespace formatize {
		template <typename T>
		static _nothrow std::remove_reference_t<T> get(T&& v) { return v; }

		static _nothrow const char* get(std::string&& v) { return v.c_str(); }
		static _nothrow const char* get(const std::string& v) { return v.c_str(); }
	}

#if __clang__
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wformat-security"
#endif
	template <size_t Fn, size_t length = max_signature_length, typename... Args>
	_forceinline
		static _nothrow std::string format(const char(&__restrict format)[Fn], Args&&... args) {
		std::array<char, max_signature_length> buffer;  // NOLINT(cppcoreguidelines-pro-type-member-init)
		sprintf(buffer.data(), format, formatize::get(std::forward<Args>(args))...);  // NOLINT(clang-diagnostic-format-nonliteral, cert-err33-c)
		return buffer.data();
	}

	template <size_t Fn, typename... Args>
	static _nothrow int printft(const char(&__restrict format)[Fn], Args&&... args) {
		return fprintf(out_stream, format, formatize::get(std::forward<Args>(args))...);  // NOLINT(clang-diagnostic-format-nonliteral)
	}
#if __clang__
#	pragma clang diagnostic pop
#endif

	static _nothrow void newline() {
		fputc('\n', out_stream);
	}

#define _INDENTED for (bool once = true; once && ++indent; --indent, once = false)

	template <size_t N>
	struct char_array_wrapper final {
		const char data[N];
	};

	template <size_t N, size_t... Indices>
	static constexpr _nothrow char_array_wrapper<sizeof...(Indices) + 2> append_newline(
		const char(&str)[N],
		std::index_sequence<Indices...>
	) {
		return { str[Indices]..., '\n', '\0' };
	}

	template <size_t N, typename... Tt>
	static _nothrow void indented_print(const size_t indent, const char(&format)[N], Tt... args) {
		PrintIndent(indent);

		printft(append_newline(format, std::make_index_sequence<N - 1>()).data, std::forward<Tt>(args)...);
	}

	using info_t = mips::instructions::InstructionInfo;

	struct type_signature final {
		const std::string procedure_name;
		const crstring name;
		const crstring signature;
		const char suffix;
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
					.procedure_name = format("StaticProc_%s%s", info.Name, type_append),
					.name = name,
					.signature = signature,
					.suffix = suffix
				}
			)
		);

		return result.first->second;
	}

	static _nothrow std::string BuildProcedureCall(const info_t& __restrict info) {
		return format(
			"%s_NS::Execute%s",
			info.Name,
			GetTypeSignature(info).signature
		);
	}

	static _nothrow std::string static_proc(const info_t& __restrict info) {
		return format("return &%s;", GetTypeSignature(info).procedure_name);
	}

	static _nothrow std::string proc_call(const info_t& __restrict info) {
		return format("{ %s(i,p); return true; }", BuildProcedureCall(info));
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
			return strcmp(name, other.name) == 0 && signature == other.signature;
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
