#include "buildcarbide.hpp"

#include "vc_project.hpp"

#include "tinyxml2/tinyxml2.h"

#include "file_utils/file_utils.hpp"

using namespace buildcarbide;

namespace {
	static auto get_conditions(const char * __restrict condition_text)
	{
		decltype(vc_project::element::conditions) cond_out;

		// todo : consider retaining format string as it would be much simpler than extraction
		// vcxproj conditions are in the following format: '$(Configuration)|$(Platform)'=='Release|Win32'
					
		const auto get_string_side = [&]()->std::string {
			std::string out;

			bool escaped = false;
			char quote_char = '\0';
			if (*condition_text == '\'' || *condition_text == '\"') {
				quote_char = *condition_text;
				++condition_text;
			}
			while (*condition_text != '=' && *condition_text != '\0') {
				if (!escaped && *condition_text == quote_char) {
					// we are done with this string.
					++condition_text;
					break;
				}

				if (*condition_text == '\\') {
					if (!escaped) {
						escaped = true;
					}
					else {
						out += *condition_text;
					}
				}
				else {
					out += *condition_text;
				}
				++condition_text;
			}

			return out;
		};
					
		const std::string lhs = get_string_side();
		// skip '=' signs
		while (*condition_text == '=') {
			++condition_text;
		}
		const std::string rhs = get_string_side();

		// now we have $(Configuration)|$(Platform) and Release|Win32.
		// We will try to extract the macros from lhs: $()

		struct token final {
			std::string value;
			std::string delimiter;
			std::string ref_value;
		};

		std::vector<token> macro_tokens;
		token cur_macro;
		bool in_macro = false;
		char prev_c = '\0';
		for (char c : lhs) {
			if (in_macro) {
				if (c == ')') {
					macro_tokens.push_back(cur_macro);
					cur_macro.value.clear();
					cur_macro.delimiter.clear();
					in_macro = false;
				}
				else {
					cur_macro.value.push_back(c);
				}
			}
			else {
				if (prev_c == '$' && c == '(') {
					if (cur_macro.delimiter.length()) {
						cur_macro.delimiter.pop_back();
					}
					in_macro = true;
				}
				else {
					cur_macro.delimiter.push_back(c);
				}
			}

			prev_c = c;
		}
		if (cur_macro.value.length() || cur_macro.delimiter.length()) {
			macro_tokens.push_back(cur_macro);
		}

		// Now, using this same data (token/delimiter pairs) we can extract the target values from rhs...
		// presuming there's nothing weird like macros that expand to something with a delimiter in it.
		const char * __restrict c_rhs = rhs.c_str();
		for (size_t i = 0; i < macro_tokens.size(); ++i) {
			std::string ref_value;

			const char *base = strstr(c_rhs, macro_tokens[i].delimiter.c_str());
			if (base == nullptr) {
				// There was a severe error parsing this.
				// todo throw
				return cond_out;
			}
			base += macro_tokens[i].delimiter.length();
			size_t ref_len = size_t(-1);
			if (i != (macro_tokens.size() - 1)) {
				ref_len = uintptr_t(strstr(base, macro_tokens[i + 1].delimiter.c_str())) - uintptr_t(base);
				ref_value.reserve(ref_len);
			}

			if (ref_len == size_t(-1)) {
				// end of string, use all of it.
				ref_value = base;
			}
			else {
				// use only part of string
				for (size_t t = 0; t < ref_len; ++t) {
					ref_value += base[t];
				}
			}

			c_rhs = base;

			// ref_value holds the reference value for this token.
			if (macro_tokens[i].value.length()) {
				macro_tokens[i].ref_value = ref_value;
			}
		}

		for (const auto &macro_token : macro_tokens) {
			if (!macro_token.value.empty()) {
				project::condition cond = { macro_token.ref_value, project::condition::operation::not_equals };
				cond_out[macro_token.value] = cond;
			}
		}

		return cond_out;
	}
}

project * vc_project::create_or_null(const std::wstring & __restrict filename)
{
	if (filename.find(L".vcxproj") != std::string::npos) {
		return new vc_project(filename);
	}
	return nullptr;
}

vc_project::vc_project(const std::wstring& __restrict filename) : project() {
	path_ = file_utils::get_base_path(filename);

	tinyxml2::XMLDocument doc;
	doc.LoadFile(to_string(filename).c_str());
	if (doc.Error()) {
		throw 0;
	}

	const auto project = doc.FirstChildElement("Project");
	if (!project) {
		throw 0;
	}

	// Find the possible configurations.
	{
		for (
			auto configuration_group = project->FirstChildElement("ItemGroup");
			configuration_group;
			configuration_group = configuration_group->NextSiblingElement("ItemGroup")
		) {
			// check if this is a correct item group
			if (
				const auto label_element = configuration_group->Attribute("Label");
				!label_element || strcmp(label_element, "ProjectConfigurations") != 0
			) {
				continue;
			}

			for (
				auto configuration_element = configuration_group->FirstChildElement("ProjectConfiguration");
				configuration_element;
				configuration_element = configuration_element->NextSiblingElement("ProjectConfiguration")
			) {
				// Get the daughter elements
				if (
					const auto config_name_element = configuration_element->FirstChildElement("Configuration")
				) {
					if (const auto text_element = config_name_element->FirstChild()) {
						if (const auto text = text_element->ToText()) {
							configurations_.insert(text->Value());
						}
					}
				}
				if (const auto platform_name_element = configuration_element->FirstChildElement("Platform")) {
					if (const auto text_element = platform_name_element->FirstChild()) {
						if (const auto text = text_element->ToText()) {
							targets_.insert(text->Value());
						}
					}
				}
			}
		}
	}

	// Get the include directories
	// TODO handle normal vcxproj, not nmake
	{
		// This happens on a per config/platform basis.
		// get_conditions(condition_text)

		for (
			auto property_group = project->FirstChildElement("PropertyGroup");
			property_group;
			property_group = property_group->NextSiblingElement("PropertyGroup")
		) {
			// See if this has an 'IncludePath' element.
			if (const auto include_path = property_group->FirstChildElement("IncludePath")) {
				// If so, first extract the condition. If there isn't one, just continue.
				const char *condition_text = property_group->Attribute("Condition");
				if (!condition_text) {
					continue;
				}

				auto conditions = get_conditions(condition_text); // We can use this to build a control structure.
				if (conditions["Configuration"].reference.empty() || conditions["Platform"].reference.empty()) {
					continue;
				}
				const target_pair config_pair = {
					to_wstring(conditions["Platform"].reference),
					to_wstring(conditions["Configuration"].reference)
				};

				// now extract the include paths.
				const auto text_element = include_path->FirstChild();
				if (!text_element) {
					continue;
				}

				const auto text = text_element->ToText();
				if (!text) {
					continue;
				}

				const char * __restrict text_cstring = text->Value();
				if (!text_cstring) {
					continue;
				}

				std::unordered_set<std::wstring> include_paths;
				std::string cur_token;
				// Now extract all the include paths.
				while (*text_cstring != '\0') {
					if (*text_cstring == ';') {
						include_paths.insert(to_wstring(cur_token));
						cur_token.clear();
					}
					else {
						cur_token.push_back(*text_cstring);
					}

					++text_cstring;
				}
				if (!cur_token.empty()) {
					include_paths.insert(to_wstring(cur_token));
				}

				include_paths_[config_pair] = include_paths;
			}
		}
	}

	const auto extract_conditions = [](
		element & element,
		const tinyxml2::XMLElement * const compile_element
	) -> decltype(element::conditions) {
		const auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
		if (!exclusion_element) {
			return {};
		}
		const auto true_false = exclusion_element->FirstChild();
		if (!true_false) {
			return {};
		}
		const auto true_false_text = true_false->ToText();
		if (!true_false_text) {
			return {};
		}
		if (strcmp(true_false_text->Value(), "false") == 0) {
			element.enabled = false;
			return {};
		}

		const char * const condition_text = exclusion_element->Attribute("Condition");
		if (!condition_text) {
			return {};
		}

		return get_conditions(condition_text);
	};

	// Find files.
	auto item_group = project->FirstChildElement("ItemGroup");
	if (!item_group) {
		throw 0;
	}

	for (; item_group; item_group = item_group->NextSiblingElement("ItemGroup")) {
		element element;

		const auto handle_element = [&](const tinyxml2::XMLElement * const compile_element, const bool header) {
			const char * const file_name = compile_element->Attribute("Include");
			if (!file_name) {
				return;
			}
			element.filename = file_name;
			element.conditions = extract_conditions(element, compile_element);
			all_files_.push_back(element);
			if (!header) {
				raw_source_files_.push_back(element);
			}
		};

		// Iterate over all item groups.
		// source files
		{
			for (
				auto compile_element = item_group->FirstChildElement("ClCompile");
				compile_element;
				compile_element = compile_element->NextSiblingElement("ClCompile")
			) {
				handle_element(compile_element, false);
			}
		}
		// header files
		{
			for (
				auto compile_element = item_group->FirstChildElement("ClInclude");
				compile_element;
				compile_element = compile_element->NextSiblingElement("ClInclude")
			) {
				handle_element(compile_element, true);
			}
		}
	}
}

vc_project::~vc_project() = default;
