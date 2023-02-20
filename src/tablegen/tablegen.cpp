// ReSharper disable CppClangTidyCertErr33C
#include "common.hpp"
#include "instructions/instructions.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include <cstdio>
#include <queue>
#include <span>
#include <optional>

namespace mips::instructions {
	extern void finish_map_build();
}

namespace mips::instructions {
	extern StaticInitVars * __restrict StaticInitVarsPtr;
}

namespace {
	static constexpr const size_t max_signature_length = 4096;

	static FILE* out_stream = stdout;

	using crstring = const char* __restrict;

	static void PrintIndent (size_t indent) _nothrow {
		while (indent--) {
			fputc('\t', out_stream);
		}
	}

	// This isn't a comprehensive version a-la Alexandrescu's. This is just a quick hack.
	namespace formatize {
		template <typename T>
		static std::remove_reference_t<T> get(T&& v) _nothrow { return v; }

		static const char* get(std::string&& v) _nothrow { return v.c_str(); }
		static const char* get(const std::string& v) _nothrow { return v.c_str(); }
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
	template <size_t Fn, size_t length = max_signature_length, typename... Args>
	_forceinline
	static std::string format(const char(&__restrict format)[Fn], Args&&... args) _nothrow {
		std::array<char, max_signature_length> buffer;  // NOLINT(cppcoreguidelines-pro-type-member-init)
		sprintf(buffer.data(), format, formatize::get(std::forward<Args>(args))...);  // NOLINT(clang-diagnostic-format-nonliteral, cert-err33-c)
		return buffer.data();
	}

	template <size_t Fn, typename... Args>
	static int printft(const char (& __restrict format)[Fn], Args&&... args) _nothrow {
		return fprintf(out_stream, format, formatize::get(std::forward<Args>(args))...);  // NOLINT(clang-diagnostic-format-nonliteral)
	}
#pragma clang diagnostic pop

	static void newline() _nothrow {
		fputc('\n', out_stream);
	}

	#define _INDENTED for (bool once = true; once && ++indent; --indent, once = false)

	template <size_t N>
	struct char_array_wrapper final {
		const char data[N];
	};

	template <size_t N, size_t... Indices>
	static constexpr char_array_wrapper<sizeof...(Indices) + 2> append_newline(
		const char(&str)[N],
		std::index_sequence<Indices...>
	) _nothrow {
		return { str[Indices]..., '\n', '\0' };
	}

	template <size_t N, typename... Tt>
	static void indented_print(const size_t indent, const char(&format)[N], Tt... args) _nothrow {
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

	static const type_signature& GetTypeSignature(const info_t& __restrict info) _nothrow {
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

	static std::string BuildProcedureLink(const info_t & __restrict info) _nothrow {
		// bool PROC_ ## InsInstruction ::Execute

		auto&& signature = GetTypeSignature(info);

		return format(
			R"(( "%s", %d, &%s_NS::Execute%s, 0x%08Xu, { .control = %s }, %s))",
			info.Name,
			info.CoprocessorIdx,
			info.Name,
			signature.signature,
			info.OpFlags,
			info.Flags.control ? "true" : "false",
			signature.name
		);
	}

	static std::string BuildProcedureCall(const info_t & __restrict info) _nothrow {
		return format(
			"%s_NS::Execute%s",
			info.Name,
			GetTypeSignature(info).signature
		);
	}

	//TODO:
	// Traverse the map a first time and build up just a list of procinfos. We will then have the file statically allocate them,
	// so we can reference... references instead of values.
	// We also need to figure out how to actually call the procedures. They are only described in source files and the symbols won't export.
	// Will likely have to add simply-exported names that are pointers to the named procedures which we can extern.

	using map_t = mips::instructions::MapOrInfo;

	static std::vector<const info_t*> populate_proc_infos(const map_t * const map) _nothrow {
		std::vector<const info_t*> result;

		std::queue<std::remove_cv_t<decltype(map)>> pending;
		pending.push(map);

		while (!pending.empty()) {
			const auto current = pending.front();
			pending.pop();

			if (!current->IsMap) {
				result.push_back(&current->Info);
				continue;
			}

			if (current->Map.size() == 1 && !current->Default) {
				result.push_back(&current->Map.begin()->second->Info);
				continue;
			}

			for (auto&& sub_map : current->Map) {
				pending.push(sub_map.second);
			}

			if (current->Default) {
				pending.push(current->Default);
			}
		}

		return result;
	}

	template <typename T> static constexpr const T ones = T(-1);

	template <typename T>
	static constexpr bool is_ones(T v) _nothrow { return v == ones<T>; }

	using procedure_t = std::string (const info_t & __restrict);

	static uint32_t reduce(const map_t* const __restrict parent, uint32 mask) _nothrow {
		if (!parent)
			return mask;
		return mask & ~parent->Mask;
	}

	static void traverse_map(
		const crstring default_value,
		const map_t * const __restrict parent,
		const map_t * const __restrict map,
		procedure_t procedure,
		size_t indent
	) _nothrow {
		if (!map->IsMap) {
			indented_print(indent, "%s", procedure(map->Info));
			return;
		}

		if (map->Map.size() == 1 && !map->Default) {
			auto&& element = map->Map.begin();

			if (is_ones(map->Mask)) {
				indented_print(indent, "if (i == 0x%08Xu)", element->first);
			}
			else {
				indented_print(indent, "if ((i & 0x%08Xu) == 0x%08Xu)", reduce(parent, map->Mask), reduce(parent, element->first));
			}
			_INDENTED indented_print(indent, "%s", procedure(element->second->Info));
			return;
		}

		const uint32 mask = reduce(parent, map->Mask);

		if (is_ones(mask)) {
			indented_print(indent, "switch(i) {");
		}
		else {
			indented_print(indent, "switch(i & 0x%08Xu) {", mask);
		}
		_INDENTED{
			for (auto&& sub_map : map->Map) {
				indented_print(indent, "case 0x%08Xu:", reduce(parent, sub_map.first));
				traverse_map(default_value, map, sub_map.second, procedure, indent + 1);
			}
			indented_print(indent, "default:");
			if (map->Default) {
				traverse_map(default_value, nullptr, map->Default, procedure, indent + 1);
			}
			_INDENTED indented_print(indent, "return %s;", default_value);
		}
		indented_print(indent, "}");
	}

	static std::string static_proc(const info_t & __restrict info) _nothrow {
		return format("return &%s;", GetTypeSignature(info).procedure_name);
	}

	static std::string proc_call(const info_t & __restrict info) _nothrow {
		return format("{ %s(i,p); return true; }", BuildProcedureCall(info));
	}

	struct instruction_compare final {
		using type = const info_t* __restrict;

		bool operator() (const type lhs, const type rhs) const __restrict _nothrow{
			return strcmp(lhs->Name, rhs->Name) < 0;
		}
	};
}

namespace
{
	template <typename HashType>
	struct fnv_constants;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"
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
#pragma clang diagnostic pop

	static constexpr size_t fnv_hash(crstring str) _nothrow
	{
		auto hash = fnv_constants<size_t>::offset_basis;

		for (char c; (c = *str) != '\0'; ++str)
		{
			hash *= fnv_constants<size_t>::prime;
			hash ^= uint8(c);
		}

		return hash;
	}

	struct proc_info_data final
	{
		const crstring name;
		const crstring signature;

		bool operator ==(const proc_info_data& __restrict other) const __restrict _nothrow
		{
			return strcmp(name, other.name) == 0 && signature == other.signature;
		}

		bool operator !=(const proc_info_data& __restrict other) const __restrict _nothrow
		{
			return !(*this == other);
		}

		struct hash
		{
			std::size_t operator()(proc_info_data const& __restrict s) const __restrict _nothrow
			{
				const auto hash0 = fnv_hash(s.name);
				const auto hash1 = fnv_hash(s.signature);
				return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
			}
		};
	};
}

namespace
{
	template <size_t N>
	static bool is_binary_option(crstring arg, const char (&__restrict ref)[N], bool &has_operand) _nothrow
	{
		if (strstr(arg, ref) != arg)
		{
			return false;
		}

		switch (arg[N - 1])
		{
		case '=':
		case ':':
			has_operand = true;
			return true;
		case '\0':
			has_operand = false;
			return true;
		default:
			return false;
		}
	}

	struct option final {
		const crstring tag;
		crstring value = nullptr;
	};

	struct options final {
		const crstring file_out = nullptr;
		const bool write_header = false;
		const bool write_source = false;
	};

	static std::optional<options> parse_options(const std::span<char * const> args) {
		crstring file_out = nullptr;
		bool write_header = false;
		bool write_source = false;

		const auto extract_option = [&](char* __restrict arg) -> option {
			{
				crstring arg_temp = arg;
				for (char c; (c = *arg_temp) != '\0'; ++arg_temp) {
					if (c == '=' || c == ':') {
						const size_t operand_index = size_t(arg_temp - arg);
						arg[operand_index] = '\0';
						return {
							.tag = arg,
							.value = arg + operand_index + 1
						};
					}
				}
			}

			return {
				.tag = arg
			};
		};

		for (size_t i = 1; i < args.size(); ++i)
		{
			auto&& arg = extract_option(args[i]);

			bool has_operand = false;

			switch (fnv_hash(arg.tag)) {
			case fnv_hash("--header"):
				if (arg.value) {
					fprintf(stderr, "Argument does not take value: '%s' with '%s'", arg.tag, arg.value);
					return {};
				}
				write_header = true;
				break;
			case fnv_hash("--source"):
				if (arg.value) {
					fprintf(stderr, "Argument does not take value: '%s' with '%s'", arg.tag, arg.value);
					return {};
				}
				write_source = true;
				break;
			case fnv_hash("--file"):
				if (!arg.value) {
					// then the next arg
					if (args.size() <= i + 1)
					{
						fprintf(stderr, "'%s' provided but no argument passed", arg.tag);
						return {};
					}

					arg.value = args[++i];
				}

				file_out = arg.value;
				break;
			default:
				fprintf(stderr, "Unknown Argument: '%s'", arg.tag);
				return {};
			}
		}

		return options{
			.file_out = file_out,
			.write_header = write_header,
			.write_source = write_source
		};
	}
}

#define TABLE_EXPORT "/* __declspec(dllexport) */ extern"
#define TABLE_IMPORT "/* __declspec(dllimport) */ extern"

int main(const int argc, char * const * const argv) {
	xassert(argc >= 0);

	auto&& options_optional = parse_options({ argv, size_t(argc) });
	if (!options_optional) {
		return -1;
	}
	const options options = options_optional.value();

	mips::instructions::finish_map_build();

	auto&& proc_infos = populate_proc_infos(&mips::instructions::StaticInitVarsPtr->g_LookupMap);

	std::stable_sort(proc_infos.begin(), proc_infos.end(), instruction_compare{});

	constexpr const char autogen_warning[] = "/* WARNING: THIS IS AN AUTO-GENERATED FILE ('" __FILE__ "', " __DATE__ " " __TIME__ ") */";

	if (options.write_source) {
		if (options.file_out)
		{
			const std::string filename = std::string(options.file_out) + ".cpp";
			out_stream = fopen(filename.c_str(), "w");
		}
		else
		{
			out_stream = stdout;
		}

		size_t indent = 0;
		indented_print(indent, autogen_warning);
		indented_print(indent, "// ReSharper disable CppClangTidyClangDiagnosticUndefinedFuncTemplate CppClangTidyClangDiagnosticUnreachableCodeReturn IdentifierTypo CommentTypo CppUnreachableCode");

		indented_print(indent, "#include \"pch.hpp\"");

		newline();

		indented_print(indent, "#define VEMIPS_ITABLE_EXPORT " TABLE_EXPORT);

		newline();

		for (auto&& header : {
			"mips/instructions/instructions_table.hpp",
			"mips/mips_common.hpp",
			"mips/instructions/instructions_common.hpp",
			"mips/processor/processor.hpp",
			//"mips/coprocessor/coprocessor.hpp"
			}) {
			indented_print(indent, "#include \"%s\"", header);
		}

		newline();

		indented_print(indent, "namespace mips::instructions {");
		for (auto* proc_info : proc_infos) {
			_INDENTED{
				indented_print(indent, "VEMIPS_ITABLE_EXPORT const InstructionInfo %s %s;", GetTypeSignature(*proc_info).procedure_name, BuildProcedureLink(*proc_info));
			}
		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "namespace mips::instructions {");
		_INDENTED{
			indented_print(indent, "const InstructionInfo * get_instruction (instruction_t i) {");
			traverse_map("nullptr", nullptr, &mips::instructions::StaticInitVarsPtr->g_LookupMap, static_proc, indent + 1);
			_INDENTED indented_print(indent, "return nullptr;");
			indented_print(indent, "}");

			newline();

			indented_print(indent, "bool execute_instruction (instruction_t i, mips::processor & __restrict p) {");
			traverse_map("false", nullptr, &mips::instructions::StaticInitVarsPtr->g_LookupMap, proc_call, indent + 1);
			_INDENTED indented_print(indent, "return false;");
			indented_print(indent, "}");
		}
		indented_print(indent, "}");

		if (options.file_out)
		{
			fclose(out_stream);
		}
	}

	if (options.write_header)
	{
		if (options.file_out)
		{
			const std::string filename = std::string(options.file_out) + ".hpp";
			out_stream = fopen(filename.c_str(), "w");
		}
		else
		{
			out_stream = stdout;
		}

		size_t indent = 0;
		indented_print(indent, "#pragma once");
		indented_print(indent, autogen_warning);
		indented_print(indent, "// ReSharper disable CppClangTidyClangDiagnosticUnreachableCodeReturn IdentifierTypo CommentTypo");

		for (auto&& header : {
			"mips/mips_common.hpp"
		}) {
			indented_print(indent, "#include \"%s\"", header);
		}

		newline();

		indented_print(indent, "namespace mips {");
		_INDENTED{
			indented_print(indent, "class processor;");

			indented_print(indent, "namespace instructions {");

			_INDENTED {
				indented_print(indent, "struct InstructionInfo;");
			}

			indented_print(indent, "}");
		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "#ifdef VEMIPS_ITABLE_EXPORT");
		indented_print(indent, "#\tdefine VEMIPS_ITABLE_IMPORT VEMIPS_ITABLE_EXPORT");
		indented_print(indent, "#else");
		indented_print(indent, "#\tdefine VEMIPS_ITABLE_IMPORT " TABLE_IMPORT);
		indented_print(indent, "#endif");

		newline();

		{
			indented_print(indent, "namespace mips::instructions {");

			std::unordered_set<proc_info_data, proc_info_data::hash> exported_procs;
			for (auto* proc_info : proc_infos) {
				_INDENTED{
					const crstring signature = proc_info->Type == mips::instructions::instruction_type::normal ?
						"VEMIPS_ITABLE_IMPORT " :
						"template <typename format_t> VEMIPS_ITABLE_IMPORT ";

					if (!exported_procs.insert({ proc_info->Name, signature}).second)
					{
						continue;
					}

					indented_print(indent,
						"namespace %s_NS { %suint64 Execute(instruction_t, mips::processor & __restrict); }",
						proc_info->Name,
						signature
					);
				}
			}
			indented_print(indent, "}");
		}
		newline();
		indented_print(indent, "namespace mips::instructions {");
		for (auto* proc_info : proc_infos) {
			_INDENTED{
				indented_print(indent, "VEMIPS_ITABLE_IMPORT const InstructionInfo %s;", GetTypeSignature(*proc_info).procedure_name);
			}
		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "namespace mips::instructions {");
		_INDENTED{

			indented_print(indent, "const InstructionInfo * get_instruction (instruction_t i);");

			newline();

			indented_print(indent, "bool execute_instruction (instruction_t i, mips::processor & __restrict p);");

		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "#\tundef VEMIPS_ITABLE_IMPORT");

		if (options.file_out)
		{
			fclose(out_stream);
		}
	}
}
