#include "instructions/instructions.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include <cstdio>

#if _MSC_VER
#	define always_inline __forceinline
#else
#	define always_inline __attribute__((always_inline))
#endif

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

	static void PrintIndent (size_t indent) {
		while (indent--) {
			fwrite("\t", 1, 1, out_stream);
		}
	}

	// This isn't a comprehensive version a-la Alexandrescu's. This is just a quick hack.
	namespace formatize {
		template <typename T>
		static std::remove_reference_t<T> get(T&& v) { return v; }

		static const char* get(std::string&& v) { return v.c_str(); }
	}

	template <size_t length = max_signature_length, typename... Args>
	always_inline
	static std::string format(const crstring format, Args&&... args) {
		std::array<char, max_signature_length> buffer;
		sprintf(buffer.data(), format, formatize::get(std::forward<Args>(args))...);
		return buffer.data();
	}

	template <typename... Args>
	static int printft(const crstring format, Args&&... args) {
		return fprintf(out_stream, format, formatize::get(std::forward<Args>(args))...);
	}

	static void newline() {
		fputs("\n", out_stream);
	}

	#define indented for (bool once = true; once && ++indent; --indent, once = false)

	template <size_t N, size_t... Indices>
	static constexpr std::array<char, sizeof...(Indices) + 2> append_newline(
		const char(&str)[N],
		std::index_sequence<Indices...>
	) {
		return { str[Indices]..., '\n', '\0' };
	}

	template <size_t N, typename... Tt>
	static void indented_print(size_t indent, const char(&format)[N], Tt... args) {
		PrintIndent(indent);

		printft(append_newline(format, std::make_index_sequence<N - 1>()).data(), std::forward<Tt>(args)...);
	}

	using info_t = mips::instructions::InstructionInfo;

	static crstring GetTypeSignature(const decltype(info_t::Type) type) {
		switch (type) {
		case 'v':
			return "<void>";
		case 'f':
			return "<float>";
		case 'd':
			return "<double>";
		case 'w':
			return "<int32>";
		case 'l':
			return "<int64>";
		default:
			return "";
		}
	}

	static std::string BuildProcedureLink(const info_t & __restrict info) {
		// bool PROC_ ## InsInstruction ::Execute

		return format(
			"( \"%s\", %s, %d, &mips::instructions::%s_NS::Execute%s, 0x%08Xu, \'%c\' )",
			info.Name,
			info.ControlInstruction ? "true" : "false",
			info.CoprocessorIdx,
			info.Name,
			GetTypeSignature(info.Type),
			info.OpFlags,
			info.Type
		);
	}

	static std::string BuildProcedureCall(const info_t & __restrict info) {
		return format(
			"mips::instructions::%s_NS::Execute%s",
			info.Name,
			GetTypeSignature(info.Type)
		);
	}

	static std::string GetStaticProcName(const info_t & __restrict info) {
		crstring typeAppend = "";
		switch (info.Type) {
		case 'v':
			typeAppend = "_v"; break;
		case 'f':
			typeAppend = "_f"; break;
		case 'd':
			typeAppend = "_d"; break;
		case 'w':
			typeAppend = "_w"; break;
		case 'l':
			typeAppend = "_l"; break;
		}
		return format("StaticProc_%s%s", info.Name, typeAppend);
	}

	//TODO:
	// Traverse the map a first time and build up just a list of procinfos. We will then have the file statically allocate them,
	// so we can reference... references instead of values.
	// We also need to figure out how to actually call the procedures. They are only described in source files and the symbols won't export.
	// Will likely have to add simply-exported names that are pointers to the named procedures which we can extern.

	using map_t = mips::instructions::MapOrInfo;

	static std::vector<const info_t *> ProcInfos;
	static void populate_proc_infos(const map_t * const __restrict map) {
		if (!map->IsMap) {
				ProcInfos.push_back(&map->Info);
				return;
		}

		if (map->Map.size() == 1 && !map->Default) {
				ProcInfos.push_back(&map->Map.begin()->second->Info);
				return;
		}

		for (auto &&sub_map : map->Map) {
				populate_proc_infos(sub_map.second);
		}

		if (map->Default) {
				populate_proc_infos(map->Default);
		}
	}

	template <typename T> static constexpr const T ones = T(-1);

	template <typename T>
	static bool is_ones(T v) { return v == ones<T>; }

	using procedure_t = std::string (const info_t & __restrict);

	static uint32_t reduce(const map_t* const __restrict parent, uint32 mask) {
		if (!parent)
			return mask;
		return mask & ~parent->Mask;
	}

	static void traverse_map(const crstring default_value, const map_t * const __restrict parent, const map_t * const __restrict map, procedure_t procedure, size_t indent) {
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
			indented indented_print(indent, "%s", procedure(element->second->Info));
			return;
		}

		const uint32 mask = reduce(parent, map->Mask);

		if (is_ones(mask)) {
			indented_print(indent, "switch(i) {");
		}
		else {
			indented_print(indent, "switch(i & 0x%08Xu) {", mask);
		}
		indented{
			for (auto&& sub_map : map->Map) {
				indented_print(indent, "case 0x%08Xu:", reduce(parent, sub_map.first));
				traverse_map(default_value, map, sub_map.second, procedure, indent + 1);
			}
			indented_print(indent, "default:");
			if (map->Default) {
				traverse_map(default_value, nullptr, map->Default, procedure, indent + 1);
			}
			indented indented_print(indent, "return %s;", default_value);
		}
		indented_print(indent, "}");
	}

	static std::string static_proc(const info_t & __restrict info) {
		return format("return &mips::instructions::%s;", GetStaticProcName(info));
	}

	static std::string proc_call(const info_t & __restrict info) {
		return format("{ %s(i,p); return true; }", BuildProcedureCall(info));
	}

	struct InstructionCompare final {
		bool operator() (const info_t* __restrict lhs, const info_t* __restrict rhs) const {
			return strcmp(lhs->Name, rhs->Name) < 0;
		}
	};
}

namespace
{
	struct proc_info_data
	{
		const crstring name;
		const crstring signature;

		bool operator ==(const proc_info_data& __restrict other) const __restrict noexcept
		{
			return strcmp(name, other.name) == 0 && signature == other.signature;
		}

		bool operator !=(const proc_info_data& __restrict other) const __restrict noexcept
		{
			return !(*this == other);
		}

		struct hash
		{
			std::size_t operator()(proc_info_data const& __restrict s) const __restrict noexcept
			{
				const auto hash0 = fnv_hash(s.name);
				const auto hash1 = fnv_hash(s.signature);
				return hash0 ^ (hash1 + 0x9e3779b9 + (hash0 << 6) + (hash0 >> 2));
			}

		private:
			template <typename HashType>
			struct fnv_constants;

			template<> struct fnv_constants<uint64>
			{
				static constexpr const uint64 offset_basis = 0xcbf29ce484222325ull;
				static constexpr const uint64 prime = 0x00000100000001B3ull;
			};

			template<> struct fnv_constants<uint32>
			{
				static constexpr const uint32 offset_basis = 0x811c9dc5u;
				static constexpr const uint32 prime = 0x01000193u;
			};

			static size_t fnv_hash(crstring str)
			{
				auto hash = fnv_constants<size_t>::offset_basis;

				for (char c; (c = *str) != '\0'; ++str)
				{
					hash *= fnv_constants<size_t>::prime;
					hash ^= uint8(c);
				}

				return hash;
			}
		};
	};
}

namespace
{
	template <size_t N>
	static bool is_binary_option(const char * __restrict arg, const char (&__restrict ref)[N], bool &has_operand)
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
}

int main(int argc, char* const* const argv) {
	const char* file_out = nullptr;
	bool write_header = false;
	bool write_source = false;

	for (int i = 1; i < argc; ++i)
	{
		auto arg = argv[i];
		bool has_operand = false;

		if (strcmp(arg, "--header") == 0) {
			write_header = true;
		}
		else if (strcmp(arg, "--source") == 0) {
			write_source = true;
		}
		else if (is_binary_option(arg, "--file", has_operand))
		{
			if (!has_operand)
			{
				// then the next arg
				if (argc <= i + 1)
				{
					fputs("--file provided but no argument passed", stderr);
					return -1;
				}

				file_out = argv[++i];
			}
			else
			{
				const char* sub_arg = arg + strlen("--file") + 1;
				if (strlen(sub_arg) == 0)
				{
					fputs("--file provided but no argument passed", stderr);
					return -1;
				}
				file_out = sub_arg;
			}
		}
	}

	mips::instructions::finish_map_build();

	populate_proc_infos(&mips::instructions::StaticInitVarsPtr->g_LookupMap);

	std::stable_sort(ProcInfos.begin(), ProcInfos.end(), InstructionCompare{});

	constexpr const char autogen_warning[] = "/* WARNING: THIS IS AN AUTO-GENERATED FILE ('" __FILE__ "', " __DATE__ " " __TIME__ ") */";

	if (write_source) {
		if (file_out)
		{
			std::string filename = std::string(file_out) + ".cpp";
			out_stream = fopen(filename.c_str(), "w");
		}
		else
		{
			out_stream = stdout;
		}

		size_t indent = 0;
		indented_print(indent, autogen_warning);

		for (auto&& header : {
			"pch.hpp",
			"mips/instructions/instructions_table.hpp",
			"mips/mips_common.hpp",
			"mips/instructions/instructions_common.hpp",
			"mips/processor/processor.hpp",
			"mips/coprocessor/coprocessor.hpp"
			}) {
			indented_print(indent, "#include \"%s\"", header);
		}

		newline();

		indented_print(indent, "#define _export /* __declspec(dllexport) */ extern");

		newline();

		{
			indented_print(indent, "namespace mips::instructions {");

			std::unordered_set<proc_info_data, proc_info_data::hash> exported_procs;
			for (auto* proc_info : ProcInfos) {
				indented{
					const crstring signature = (proc_info->Type == 'n') ?
						"extern " :
						"template <typename format_t> ";

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
		for (auto* proc_info : ProcInfos) {
			indented{
				indented_print(indent, "_export const mips::instructions::InstructionInfo %s %s;", GetStaticProcName(*proc_info), BuildProcedureLink(*proc_info));
			}
		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "const mips::instructions::InstructionInfo * __restrict get_instruction (instruction_t i) {");
		traverse_map("nullptr", nullptr, &mips::instructions::StaticInitVarsPtr->g_LookupMap, static_proc, indent + 1);
		indented indented_print(indent, "return nullptr;");
		indented_print(indent, "}");

		newline();

		indented_print(indent, "bool execute_instruction (instruction_t i, mips::processor & __restrict p) {");
		traverse_map("false", nullptr, &mips::instructions::StaticInitVarsPtr->g_LookupMap, proc_call, indent + 1);
		indented indented_print(indent, "return false;");
		indented_print(indent, "}");

		if (file_out)
		{
			fclose(out_stream);
		}
	}

	if (write_header)
	{
		if (file_out)
		{
			std::string filename = std::string(file_out) + ".hpp";
			out_stream = fopen(filename.c_str(), "w");
		}
		else
		{
			out_stream = stdout;
		}

		size_t indent = 0;
		indented_print(indent, "#pragma once");
		indented_print(indent, autogen_warning);

		for (auto&& header : {
			"mips/mips_common.hpp"
		}) {
			indented_print(indent, "#include \"%s\"", header);
		}

		newline();

		indented_print(indent, "namespace mips {");
		indented{
			indented_print(indent, "class processor;");

			indented_print(indent, "namespace instructions {");

			indented {
				indented_print(indent, "struct InstructionInfo;");
			}

			indented_print(indent, "}");
		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "#define _import /* __declspec(dllexport) */ extern");

		newline();

		{
			indented_print(indent, "namespace mips::instructions {");

			std::unordered_set<proc_info_data, proc_info_data::hash> exported_procs;
			for (auto* proc_info : ProcInfos) {
				indented{
					const crstring signature = (proc_info->Type == 'n') ?
						"extern " :
						"template <typename format_t> ";

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
		for (auto* proc_info : ProcInfos) {
			indented{
				indented_print(indent, "_import const mips::instructions::InstructionInfo %s;", GetStaticProcName(*proc_info));
			}
		}
		indented_print(indent, "}");

		newline();

		indented_print(indent, "const mips::instructions::InstructionInfo * __restrict get_instruction (instruction_t i);");

		newline();

		indented_print(indent, "bool execute_instruction (instruction_t i, mips::processor & __restrict p);");

		if (file_out)
		{
			fclose(out_stream);
		}
	}
}
