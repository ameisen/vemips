#include "tablegen.pch.hpp"
#include "writers.hpp"
#include <common.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iterator>
#include <limits>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "options.hpp"
#include "tablegen.hpp"
#include "mips/instructions/instructions_common.hpp"


using namespace vemips::tablegen;

namespace {
	constexpr const char autogen_warning[] = "/* WARNING: THIS IS AN AUTO-GENERATED FILE ('" __FILE__ "', " __DATE__ " " __TIME__ ") */";
}

#define TABLE_EXPORT
#define TABLE_IMPORT "/* __declspec(dllimport) */ extern"

#define TABLE_SEGMENT "vemips_itable"

namespace vemips::tablegen::writers {
	namespace {
		class scoped_close_file final {
			std::FILE *file_ = nullptr;

		public:
			[[nodiscard]]
			explicit scoped_close_file(std::FILE *file) : file_(file) {
			}

			scoped_close_file(const scoped_close_file&) = delete;

			[[nodiscard]]
			_nothrow scoped_close_file(scoped_close_file&& other) noexcept : scoped_close_file(other.file_) {
				other.file_ = nullptr;
			}

			_nothrow scoped_close_file& operator=(scoped_close_file&& other) noexcept
			{
				close();
				file_ = other.file_;
				other.file_ = nullptr;
				return *this;
			}

			_nothrow ~scoped_close_file() noexcept {
				close();
			}

			void operator =(const scoped_close_file&) = delete;
		private:
			_nothrow void close() noexcept
			{
				if (
					const auto file = file_;
					file && file != stdout && file != stderr && file != stdin
				) {
					std::fclose(file);  // NOLINT(cert-err33-c)
					file_ = nullptr;
				}
			}
		};

		[[nodiscard]]
		static _pure _nothrow bool CanThrow(const info_t& __restrict info)
		{
			using mips::instructions::OpFlags;
			return mips::enumeration::has_any(info.OpFlags, OpFlags::Throws, OpFlags::ThrowsMisaligned, OpFlags::ControlInstruction, OpFlags::Signals_All);
		}

		[[nodiscard]]
		static _pure _nothrow bool IsCoprocessorInstruction(const info_t& __restrict info)
		{
			return std::memcmp(info.Name, "COP1_", std::strlen("COP1_")) == 0;
		}

		[[nodiscard]]
		static _nothrow std::string BuildProcedureIdentifier(const info_t& __restrict info){
			const auto& signature = GetTypeSignature(info);

			return fmt::format(
				"{}_NS::Execute{}",
				info.Name,
				signature.signature
			);
		}

		[[nodiscard]]
		static _nothrow std::string BuildProcedureLink(const info_t& __restrict info) {
			// bool PROC_ ## InsInstruction ::Execute

			const auto& signature = GetTypeSignature(info);

			using mips::instructions::OpFlags;

			return fmt::format(
				R"(( "{}", {}, {}&{}_NS::Execute{}, static_cast<OpFlags>(0x{:08X}), {{ .control = {} }}, {}))",
				info.Name,
				info.CoprocessorIdx,
				(CanThrow(info) || !IsCoprocessorInstruction(info)) ?
					"" :
					"(const instructionexec_t)",
				info.Name,
				signature.signature,
				static_cast<uint32>(info.OpFlags),
				info.Flags.control ? "true" : "false",
				signature.name
			);
		}
	}

	void header(
		const options &options,
		const std::span<const info_t* __restrict> proc_infos
	) {
		scoped_close_file _close_file(out_stream);

		if (options.file_out)
		{
			const std::string filename = fmt::format("{}.hpp", options.file_out);
			out_stream = std::fopen(filename.c_str(), "w");
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
			"\"mips/mips_common.hpp\"",
			"\"mips/instructions/instructions_common.hpp\"",
			"<array>",
			"<functional>",
		}) {
			indented_print(indent, "#include {}", header);
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

		indented_print(indent, "/*");
		indented_print(indent, "// Workaround for Visual C++ not properly handling templated noexcept when the declaration ");
		indented_print(indent, "// uses a bool for noexcept.");
		indented_print(indent, "*/");
		indented_print(indent, "#if !__clang__ && _MSC_VER");
		indented_print(indent, "#	define VEMIPS_TEMPLATED_NOEXCEPT(...)");
		indented_print(indent, "#else");
		indented_print(indent, "#	define VEMIPS_TEMPLATED_NOEXCEPT(...) noexcept(__VA_ARGS__)");
		indented_print(indent, "#endif");

		newline();

		newline();

		{
			indented_print(indent, "namespace mips::instructions {{");

			_INDENTED{
				indented_print(indent, "static constexpr size_t num_instructions = {};", proc_infos.size());
				newline();

				std::unordered_set<proc_info_data, proc_info_data::hash> exported_procs;
				for (const auto* proc_info : proc_infos) {
					const bool is_templated = proc_info->Type != mips::instructions::instruction_type::normal;
					const crstring signature = is_templated ?
						"template <typename format_t> " : 
						"";

					if (!exported_procs.insert({ proc_info->Name, signature}).second)
					{
						continue;
					}

					using mips::instructions::OpFlags;
					const bool can_throw = CanThrow(*proc_info);
					const bool is_noreturn = (proc_info->OpFlags & OpFlags::AlwaysThrows) != OpFlags::None;

					indented_print(indent,
						"namespace {}_NS {{ {}VEMIPS_JIT_ABI VEMIPS_ITABLE_IMPORT{} void VEMIPS_JIT_ABI_INFIX Execute(instruction_t, processor& __restrict){}{}; }}",
						proc_info->Name,
						signature,
						is_noreturn ? " _noreturn_pre" : "",
						can_throw ? "" : (is_templated ? " VEMIPS_TEMPLATED_NOEXCEPT(true)" : " noexcept"),
						is_noreturn ? " _noreturn_post" : ""
					);
				}

				newline();
				indented_print(indent, "extern const std::array<const std::reference_wrapper<const InstructionInfo>, {}> AllInstructions;", proc_infos.size());

				newline();
				indented_print(indent, "extern const std::array<const instructionexec_t, {}> AllInstructionsProcs;", proc_infos.size());
			}

			indented_print(indent, "}}");
		}
		newline();
		indented_print(indent, "namespace mips::instructions {{");
		for (const auto* proc_info : proc_infos) {
			_INDENTED{
				indented_print(indent, "_segment(" TABLE_SEGMENT ") VEMIPS_ITABLE_IMPORT const InstructionInfo {}; ", GetTypeSignature(*proc_info).procedure_name);
			}
		}
		indented_print(indent, "}}");

		newline();

		indented_print(indent, "namespace mips::instructions {{");
		_INDENTED{

			indented_print(indent, "_nothrow const InstructionInfo* get_instruction(instruction_t instruction) noexcept;");

			newline();

			indented_print(indent, "bool execute_instruction(instruction_t instruction, processor& __restrict processor);");

			newline();

			indented_print(indent, "template <bool AssumeSafe = false, bool AlternateImplementation = false>");
			indented_print(indent, "std::conditional_t<AssumeSafe, void, bool> execute_instruction(");
			_INDENTED {
				indented_print(indent, "uint32 instruction_index,");
				indented_print(indent, "instruction_t instruction,");
				indented_print(indent, "processor& __restrict processor");
			}
			indented_print(indent, ");");

			newline();

			indented_print(indent, "_nothrow uint32 get_instruction_index(instruction_t instruction) noexcept;");

			newline();

			indented_print(indent, "static constexpr _nothrow _forceinline uint32 get_instruction_index(");
			_INDENTED indented_print(indent, "const instructionexec_t instruction_proc_ptr");
			indented_print(indent, ") {{");
			_INDENTED{
				indented_print(indent, "for (uint32 i = 0; i < uint32(std::size(AllInstructions)); ++i) {{");
				_INDENTED {
					indented_print(indent, "if (instruction_proc_ptr == AllInstructions[i].get().Proc) [[likely]] {{");
					_INDENTED {
						indented_print(indent, "return i;");
					}
					indented_print(indent, "}}");
				}
				indented_print(indent, "}}");
				newline();
				indented_print(indent, "xunreachable(\"Unknown Instruction\");");
			}
			indented_print(indent, "}}");
		}
		indented_print(indent, "}}");

		newline();

		indented_print(indent, "#\tundef VEMIPS_ITABLE_IMPORT");
	}

	namespace {
		template <typename T> static constexpr const T ones = std::numeric_limits<T>::max();

		template <typename T>
		static constexpr _nothrow bool is_ones(T v) noexcept { return v == ones<T>; }

		using procedure_t = std::function<std::string(const info_t& __restrict)>;

		static _nothrow uint32 reduce(
			const map_t* const __restrict parent,
			const uint32 mask
		) noexcept {
			return parent ?
				mask & ~parent->Mask :
				mask;
		}

		static _nothrow void traverse_map(
			const crstring default_value,
			const map_t* const __restrict parent,
			const map_t* const __restrict map,
			const procedure_t& procedure,
			usize indent
		) noexcept {
			if (!map->IsMap) {
				indented_print(indent, "{}", procedure(map->Info));
				return;
			}

			const usize map_size = map->Map.size();

			const bool has_simple_default =
				!map->Default ||
				(
					//map->Default->Map.size() == 1 &&
					!map->Default->Default &&
					map->Mask != map->Default->Mask
				);

			if (has_simple_default && map_size <= 1)
			{
				enum class branch_type
				{
					If = 0,
					ElseIf,
					Else
				};

				const auto print_branch = [&] (
					const map_t* root,
					const map_t* base,
					const decltype(map->Map)::const_iterator &iterator,
					const branch_type type = branch_type::If
				)
				{

					if (type == branch_type::Else)
					{
						indented_print(
							indent,
							"else"
						);
					}
					else
					{
						constexpr const char else_prefix[] = "else ";
						constexpr const char infix_space_prefix[] = "     ";
						static_assert(std::size(else_prefix) == std::size(infix_space_prefix));

						const char* const prefix = (type == branch_type::Else || type == branch_type::ElseIf) ? else_prefix : "";
						const char* const infix = (type == branch_type::If) ? infix_space_prefix : "";

						if (is_ones(base->Mask)) {
							indented_print(
								indent,
								"{}if{} (instruction == 0x{:08X})",
								prefix,
								infix,
								iterator->first
							);
						}
						else {
							indented_print(
								indent,
								"{}if{} ((instruction & 0x{:08X}) == 0x{:08X})",
								prefix,
								infix,
								reduce(root, base->Mask),
								reduce(root, iterator->first)
							);
						}
					}
					_INDENTED indented_print(
						indent, 
						"{}",
						procedure(iterator->second->Info)
					);
				};

				auto&& iterator = map->Map.begin();

				switch (map_size)
				{
					case 0:
						indented_print(indent, "return {};", default_value);
						break;

					case 2:
						print_branch(parent, map, iterator++);
						[[fallthrough]];

					case 1:
						print_branch(parent, map, iterator, (map_size > 1) ? branch_type::ElseIf : branch_type::If);
						break;

					default:
						xunreachable("Map size must be <= 2");
						break;
				}

				bool hasElse = false;

				if (map->Default)
				{
					// TODO : fix me - this is always true here
					if (has_simple_default)
					{
						print_branch(nullptr, map->Default, map->Default->Map.begin(), branch_type::ElseIf);
					}
					else
					{
						indented_print(
							indent,
							"else {{"
						);

						traverse_map(default_value, nullptr, map->Default, procedure, indent + 1);

						indented_print(
							indent,
							"}}"
						);

						hasElse = true;
					}
				}
				
				if (!hasElse)
				{
					indented_print(indent, "else");
					indented_print(indent + 1, "return {};", default_value);
					hasElse = true;
				}
			}
			else
			{
				if (
					const uint32 mask = reduce(parent, map->Mask);
					is_ones(mask)
				) {
					indented_print(indent, "switch(instruction) {{");
				}
				else {
					indented_print(indent, "switch(instruction & 0x{:08X}) {{", mask);
				}
				_INDENTED{
					for (auto&& sub_map : map->Map) {
						indented_print(indent, "case 0x{:08X}:", reduce(parent, sub_map.first));
						traverse_map(default_value, map, sub_map.second, procedure, indent + 1);
					}
					if (map->Default) {
						indented_print(indent, "default:");
						traverse_map(default_value, nullptr, map->Default, procedure, indent + 1);
					}
					else
					{
						indented_print(indent, "default:");
						indented_print(indent + 1, "return {};", default_value);
					}
					//_INDENTED indented_print(indent, "return {};", default_value);
				}
				indented_print(indent, "}}");
			}
		}
	}

	void source(
		const options& options,
		const std::span<const info_t* __restrict> proc_infos
	) {
		scoped_close_file _close_file(out_stream);

		if (options.file_out)
		{
			const std::string filename = fmt::format("{}.cpp", options.file_out);
			out_stream = fopen(filename.c_str(), "w");

			if (out_stream == nullptr) [[unlikely]]
			{
				fmt::println(stderr, "Could not open file for write: `{}`", filename);
				std::exit(-2);  // NOLINT(concurrency-mt-unsafe)
			}
		}
		else
		{
			out_stream = stdout;
		}

		size_t indent = 0;
		indented_print(indent, "// ReSharper disable CppClangTidyClangDiagnosticUndefinedFuncTemplate CppClangTidyClangDiagnosticUnreachableCodeReturn");
		indented_print(indent, "// ReSharper disable CppDefaultCaseNotHandledInSwitchStatement CppEnforceIfStatementBraces CppRedundantElseKeyword");
		indented_print(indent, "// ReSharper disable CommentTypo CppUnreachableCode IdentifierTypo StringLiteralTypo");
		newline();
		indented_print(indent, autogen_warning);

		indented_print(indent, "#include \"pch.hpp\"");
		indented_print(indent, "#include <array>");

		newline();

		for (auto&& header : {
			"<type_traits>",
			"\"mips/instructions/instructions_table.hpp\"",
			"\"mips/mips_common.hpp\"",
			"\"mips/instructions/instructions_common.hpp\"",
			"\"mips/processor/processor.hpp\"",
			//"\"mips/coprocessor/coprocessor.hpp\""
		}) {
			indented_print(indent, "#include {}", header);
		}

		newline();

		indented_print(indent, "#define VEMIPS_ITABLE_EXPORT " TABLE_EXPORT);

		newline();

		indented_print(indent, "namespace mips::instructions {{");
		_INDENTED{
			indented_print(indent, "// ReSharper disable CppCStyleCast");
			for (const auto* proc_info : proc_infos) {
				indented_print(indent, "_segment(" TABLE_SEGMENT ") VEMIPS_ITABLE_EXPORT const InstructionInfo {} {};", GetTypeSignature(*proc_info).procedure_name, BuildProcedureLink(*proc_info));
			}
			indented_print(indent, "// ReSharper restore CppCStyleCast");

			newline();

			indented_print(indent, "const std::array<const std::reference_wrapper<const InstructionInfo>, {}> AllInstructions = {{", proc_infos.size());
			_INDENTED{
				for (const auto* proc_info : proc_infos) {
					indented_print(indent, "std::ref({}),", GetTypeSignature(*proc_info).procedure_name);
				}
			}
			indented_print(indent, "}};");

			indented_print(indent, "// ReSharper disable CppCStyleCast");
			indented_print(indent, "const std::array<const instructionexec_t, {}> AllInstructionsProcs = {{", proc_infos.size());
			_INDENTED{
				for (const auto* proc_info : proc_infos) {
					indented_print(indent, "(const instructionexec_t)&{},", BuildProcedureIdentifier(*proc_info));
				}
			}
			indented_print(indent, "}};");
			indented_print(indent, "// ReSharper restore CppCStyleCast");
		}

		indented_print(indent, "}}");

		newline();

		indented_print(indent, "namespace mips::instructions {{");
		_INDENTED{
			indented_print(indent, "_nothrow const InstructionInfo* get_instruction(const instruction_t instruction) noexcept {{");
			traverse_map("nullptr", nullptr, &mips::instructions::GetStaticInitVars().g_LookupMap, static_proc, indent + 1);
			_INDENTED indented_print(indent, "return nullptr;");
			indented_print(indent, "}}");

			newline();

			indented_print(indent, "bool execute_instruction(const instruction_t instruction, processor& __restrict processor) {{");
			traverse_map("false", nullptr, &mips::instructions::GetStaticInitVars().g_LookupMap, proc_call, indent + 1);
			_INDENTED indented_print(indent, "return false;");
			indented_print(indent, "}}");

			newline();

			indented_print(indent, "// ReSharper disable CppUnreachableCode");
			indented_print(indent, "template <bool AssumeSafe, bool AlternateImplementation>");
			indented_print(indent, "std::conditional_t<AssumeSafe, void, bool> execute_instruction(");
			_INDENTED {
				indented_print(indent, "const uint32 instruction_index,");
				indented_print(indent, "const instruction_t instruction,");
				indented_print(indent, "processor& __restrict processor");
			}
			indented_print(indent, ") {{");
			
			_INDENTED {
				indented_print(indent, "if constexpr (!AlternateImplementation) {{");
				_INDENTED {
					indented_print(indent, "if constexpr (AssumeSafe) {{");
					_INDENTED indented_print(indent, "AllInstructionsProcs[instruction_index](instruction, processor);");
					_INDENTED indented_print(indent, "return;");
					indented_print(indent, "}}");
					indented_print(indent, "else {{");
					_INDENTED {
						indented_print(indent, "if _likely(instruction_index < std::size(AllInstructionsProcs)) [[likely]] {{");
						_INDENTED {
							indented_print(indent, "AllInstructionsProcs[instruction_index](instruction, processor);");
							indented_print(indent, "return true;");
						}
						indented_print(indent, "}}");
						indented_print(indent, "return false;");
					}
					indented_print(indent, "}}");
				}
				indented_print(indent, "}}");
				indented_print(indent, "else {{");
				_INDENTED {
					indented_print(indent, "switch (instruction_index) {{");
					for (usize i = 0; i < proc_infos.size(); ++i)
					{
						indented_print(indent, "case {}:", i);
						_INDENTED {
							indented_print(indent, "{}(instruction, processor);", BuildProcedureIdentifier(*proc_infos[i]));
							if (mips::enumeration::has_all(proc_infos[i]->OpFlags, mips::instructions::OpFlags::AlwaysThrows))
							{
								indented_print(indent, "xunreachable(\"Instruction is always supposed to throw.\");");
							}
							else
							{
								indented_print(indent, "if constexpr (AssumeSafe) {{");
								_INDENTED indented_print(indent, "return;");
								indented_print(indent, "}}");
								indented_print(indent, "else {{");
								_INDENTED indented_print(indent, "return true;");
								indented_print(indent, "}}");
							}
						}
					}

					indented_print(indent, "default:");
					_INDENTED {
						indented_print(indent, "if constexpr (AssumeSafe) {{");
						_INDENTED indented_print(indent, "xunreachable(\"Instruction was not recognized but AssumeSafe is true.\");");
						indented_print(indent, "}}");
						indented_print(indent, "else {{");
						_INDENTED indented_print(indent, "return false;");
						indented_print(indent, "}}");
					}

					indented_print(indent, "}}");
				}
				indented_print(indent, "}}");
			}
			indented_print(indent, "}}");
			indented_print(indent, "// ReSharper restore CppUnreachableCode");
			newline();
			const auto execute_instruction_templ_def = [&] (const bool assume_safe, const bool alternate_implementation)
			{
				indented_print(indent, "template");
				indented_print(indent,
					"{} execute_instruction<{}, {}>(",
					assume_safe ? "void" : "bool",
					assume_safe,
					alternate_implementation
				);
				_INDENTED {
					indented_print(indent, "uint32 instruction_index,");
					indented_print(indent, "instruction_t instruction,");
					indented_print(indent, "processor& __restrict processor");
				}
				indented_print(indent, ");");
			};
			execute_instruction_templ_def(false, false);
			execute_instruction_templ_def(false, true);
			execute_instruction_templ_def(true, false);
			execute_instruction_templ_def(true, true);

			newline();

			indented_print(indent, "_nothrow uint32 get_instruction_index(const instruction_t instruction) noexcept {{");
			traverse_map("std::numeric_limits<uint32>::max()", nullptr, &mips::instructions::GetStaticInitVars().g_LookupMap, [&proc_infos](const info_t& __restrict info) { return index_proc(proc_infos, info); }, indent + 1);
			_INDENTED indented_print(indent, "return std::numeric_limits<uint32>::max();");
			indented_print(indent, "}}");
		}
		indented_print(indent, "}}");
	}
}
