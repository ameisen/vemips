#include "tablegen.pch.hpp"
#include "writers.hpp"

#include <span>

using namespace vemips::tablegen;

namespace {
	constexpr const char autogen_warning[] = "/* WARNING: THIS IS AN AUTO-GENERATED FILE ('" __FILE__ "', " __DATE__ " " __TIME__ ") */";
}

#define TABLE_EXPORT "/* __declspec(dllexport) */ extern"
#define TABLE_IMPORT "/* __declspec(dllimport) */ extern"

#define TABLE_SEGMENT "vemips_itable"

namespace vemips::tablegen::writers {
	namespace {
		class scoped_close_file final {
			std::FILE * const file_;

		public:
			scoped_close_file(std::FILE *file) : file_(file) {
			}

			scoped_close_file(const scoped_close_file&) = delete;

			~scoped_close_file() {
				const auto file = file_;
				if (file && file != stdout && file != stderr && file != stdin) {
					std::fclose(file);
				}
			}

			void operator = (const scoped_close_file&) = delete;
		};

		static _nothrow std::string BuildProcedureLink(const info_t& __restrict info) {
			// bool PROC_ ## InsInstruction ::Execute

			auto&& signature = GetTypeSignature(info);

			return fmt::format(
				R"(( "{}", {}, &{}_NS::Execute{}, 0x{:08X}, {{ .control = {} }}, {}))",
				info.Name,
				info.CoprocessorIdx,
				info.Name,
				signature.signature,
				info.OpFlags,
				info.Flags.control ? "true" : "false",
				signature.name
			);
		}
	}

	void header(const options &options, std::span<const info_t * __restrict> proc_infos) {
		scoped_close_file _close_file(out_stream);

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
			indented_print(indent, "#include \"{}\"", header);
		}

		newline();

		indented_print(indent, "_define_segment(" TABLE_SEGMENT ", read)");

		newline();

		indented_print(indent, "namespace mips {{");
		_INDENTED{
			indented_print(indent, "class processor;");

			indented_print(indent, "namespace instructions {{");

			_INDENTED {
				indented_print(indent, "struct InstructionInfo;");
			}

			indented_print(indent, "}}");
		}
		indented_print(indent, "}}");

		newline();

		indented_print(indent, "#ifdef VEMIPS_ITABLE_EXPORT");
		indented_print(indent, "#\tdefine VEMIPS_ITABLE_IMPORT VEMIPS_ITABLE_EXPORT");
		indented_print(indent, "#else");
		indented_print(indent, "#\tdefine VEMIPS_ITABLE_IMPORT " TABLE_IMPORT);
		indented_print(indent, "#endif");

		newline();

		{
			indented_print(indent, "namespace mips::instructions {{");

			std::unordered_set<proc_info_data, proc_info_data::hash> exported_procs;
			for (auto* proc_info : proc_infos) {
				_INDENTED{
					const crstring signature = proc_info->Type == mips::instructions::instruction_type::normal ?
						"VEMIPS_ITABLE_IMPORT" :
						"template <typename format_t> VEMIPS_ITABLE_IMPORT";

					if (!exported_procs.insert({ proc_info->Name, signature}).second)
					{
						continue;
					}

					indented_print(indent,
						"namespace {}_NS {{ {} uint64 Execute(instruction_t, mips::processor & __restrict); }}",
						proc_info->Name,
						signature
					);
				}
			}
			indented_print(indent, "}}");
		}
		newline();
		indented_print(indent, "namespace mips::instructions {{");
		for (auto* proc_info : proc_infos) {
			_INDENTED{
				indented_print(indent, "_segment(" TABLE_SEGMENT ") VEMIPS_ITABLE_IMPORT const InstructionInfo {}; ", GetTypeSignature(*proc_info).procedure_name);
			}
		}
		indented_print(indent, "}}");

		newline();

		indented_print(indent, "namespace mips::instructions {{");
		_INDENTED{

			indented_print(indent, "const InstructionInfo * get_instruction (instruction_t i);");

			newline();

			indented_print(indent, "bool execute_instruction (instruction_t i, mips::processor & __restrict p);");

		}
		indented_print(indent, "}}");

		newline();

		indented_print(indent, "#\tundef VEMIPS_ITABLE_IMPORT");
	}

	namespace {
		template <typename T> static constexpr const T ones = T(-1);

		template <typename T>
		static constexpr _nothrow bool is_ones(T v) { return v == ones<T>; }

		using procedure_t = std::string(const info_t& __restrict);

		static _nothrow uint32_t reduce(const map_t* const __restrict parent, uint32 mask) {
			if (!parent)
				return mask;
			return mask & ~parent->Mask;
		}

		static _nothrow void traverse_map(
			const crstring default_value,
			const map_t* const __restrict parent,
			const map_t* const __restrict map,
			procedure_t procedure,
			size_t indent
		) {
			if (!map->IsMap) {
				indented_print(indent, "{}", procedure(map->Info));
				return;
			}

			if (map->Map.size() == 1 && !map->Default) {
				auto&& element = map->Map.begin();

				if (is_ones(map->Mask)) {
					indented_print(indent, "if (i == 0x{:08X})", element->first);
				}
				else {
					indented_print(indent, "if ((i & 0x{:08X}) == 0x{:08X})", reduce(parent, map->Mask), reduce(parent, element->first));
				}
				_INDENTED indented_print(indent, "{}", procedure(element->second->Info));
				return;
			}

			const uint32 mask = reduce(parent, map->Mask);

			if (is_ones(mask)) {
				indented_print(indent, "switch(i) {{");
			}
			else {
				indented_print(indent, "switch(i & 0x{:08X}) {{", mask);
			}
			_INDENTED{
				for (auto&& sub_map : map->Map) {
					indented_print(indent, "case 0x{:08X}:", reduce(parent, sub_map.first));
					traverse_map(default_value, map, sub_map.second, procedure, indent + 1);
				}
				indented_print(indent, "default:");
				if (map->Default) {
					traverse_map(default_value, nullptr, map->Default, procedure, indent + 1);
				}
				_INDENTED indented_print(indent, "return {};", default_value);
			}
			indented_print(indent, "}}");
		}
	}

	void source(const options& options, std::span<const info_t * __restrict> proc_infos) {
		scoped_close_file _close_file(out_stream);

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
			indented_print(indent, "#include \"{}\"", header);
		}

		newline();

		indented_print(indent, "namespace mips::instructions {{");
		for (auto* proc_info : proc_infos) {
			_INDENTED{
				indented_print(indent, "_segment(" TABLE_SEGMENT ") VEMIPS_ITABLE_EXPORT const InstructionInfo {} {};", GetTypeSignature(*proc_info).procedure_name, BuildProcedureLink(*proc_info));
			}
		}
		indented_print(indent, "}}");

		newline();

		indented_print(indent, "namespace mips::instructions {{");
		_INDENTED{
			indented_print(indent, "const InstructionInfo * get_instruction (instruction_t i) {{");
			traverse_map("nullptr", nullptr, &mips::instructions::GetStaticInitVars().g_LookupMap, static_proc, indent + 1);
			_INDENTED indented_print(indent, "return nullptr;");
			indented_print(indent, "}}");

			newline();

			indented_print(indent, "bool execute_instruction (instruction_t i, mips::processor & __restrict p) {{");
			traverse_map("false", nullptr, &mips::instructions::GetStaticInitVars().g_LookupMap, proc_call, indent + 1);
			_INDENTED indented_print(indent, "return false;");
			indented_print(indent, "}}");
		}
		indented_print(indent, "}}");
	}
}
