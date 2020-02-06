#include "instructions/instructions.hpp"
#include <cstdio>
#include <string>
#include <array>
#include <functional>
#include <type_traits>
#include <utility>

#if _MSC_VER
#	define always_inline __forceinline
#else
#	define always_ilnine __attribute__((always_inline))
#endif

namespace mips::instructions {
	extern void finish_map_build();
}

namespace mips::instructions {
	extern StaticInitVars * __restrict StaticInitVarsPtr;
}

namespace {
	static constexpr const size_t max_signature_length = 4096;

	using crstring = const char* __restrict;

	static void PrintIndent (size_t indent) {
		while (indent--) {
			fwrite("\t", 1, 1, stdout);
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
		return printf(format, formatize::get(std::forward<Args>(args))...);
	}

	#define indented for (bool once = true; once && ++indent; --indent, once = false)

	#define indented_print(_format, ...) [&] { PrintIndent(indent); printft(_format "\n", __VA_ARGS__); }()

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

	static std::vector<const info_t * __restrict> ProcInfos;
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
			indented_print("%s", procedure(map->Info));
			return;
		}

		if (map->Map.size() == 1 && !map->Default) {
			auto&& element = map->Map.begin();

			if (is_ones(map->Mask)) {
				indented_print("if (i == 0x%08Xu)", element->first);
			}
			else {
				indented_print("if ((i & 0x%08Xu) == 0x%08Xu)", reduce(parent, map->Mask), reduce(parent, element->first));
			}
			indented indented_print("%s", procedure(element->second->Info));
			return;
		}

		uint32 mask = reduce(parent, map->Mask);

		if (is_ones(mask)) {
			indented_print("switch(i) {");
		}
		else {
			indented_print("switch(i & 0x%08Xu) {", mask);
		}
		indented{
			for (auto&& sub_map : map->Map) {
				indented_print("case 0x%08Xu:", reduce(parent, sub_map.first));
				traverse_map(default_value, map, sub_map.second, procedure, indent + 1);
			}
			indented_print("default:");
			if (map->Default) {
				traverse_map(default_value, nullptr, map->Default, procedure, indent + 1);
			}
			indented indented_print("return %s;", default_value);
		}
		indented_print("}");
	}

	static std::string static_proc(const info_t & __restrict info) {
		return format("return &%s;", GetStaticProcName(info));
	}

	static std::string proc_call(const info_t & __restrict info) {
		return format("%s(i,p); return true;", BuildProcedureCall(info));
	}

	struct InstructionCompare final {
		bool operator() (const info_t* __restrict lhs, const info_t* __restrict rhs) const {
			return strcmp(lhs->Name, rhs->Name) < 0;
		}
	};

	static void newline() {
		puts("");
	}
}

int main() {
	mips::instructions::finish_map_build();

	populate_proc_infos(&mips::instructions::StaticInitVarsPtr->g_LookupMap);

	std::stable_sort(ProcInfos.begin(), ProcInfos.end(), InstructionCompare{});

	size_t indent = 0;
	indented_print("/* WARNING: THIS IS AN AUTO-GENERATED FILE ('" __FILE__ "', " __DATE__ " " __TIME__ ") */");

	for (auto&& header : {
		"pch.hpp",
		"mips/mips_common.hpp",
		"mips/instructions/instructions_common.hpp",
		"mips/processor/processor.hpp",
		"mips/coprocessor/coprocessor.hpp"
	}) {
		indented_print("#include \"%s\"", header);
	}

	newline();

	indented_print("namespace mips::instructions {");
	for (auto&& Procs : ProcInfos) {
		indented {
			const crstring signature = (Procs->Type == 'n') ?
				"extern " :
				"template <typename format_t> ";

			indented_print(
				"namespace %s_NS { %suint64 Execute(instruction_t, mips::processor & __restrict); }",
				Procs->Name,
				signature
			);
		}
	}
	indented_print("}");
	for (auto&& Procs : ProcInfos) {
			indented_print("__declspec(dllexport) const mips::instructions::InstructionInfo %s %s;", GetStaticProcName(*Procs), BuildProcedureLink(*Procs));
	}

	newline();

	indented_print("const mips::instructions::InstructionInfo * __restrict get_instruction (instruction_t i) {");
	traverse_map("nullptr", nullptr, &mips::instructions::StaticInitVarsPtr->g_LookupMap, static_proc, indent + 1);
	indented indented_print("return nullptr;");
	indented_print("}");

	newline();

	indented_print("bool execute_instruction (instruction_t i, mips::processor & __restrict p) {");
	traverse_map("false", nullptr, &mips::instructions::StaticInitVarsPtr->g_LookupMap, proc_call, indent + 1);
	indented indented_print("return false;");
	indented_print("}");
}
