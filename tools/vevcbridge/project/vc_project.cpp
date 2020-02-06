#include "vc_project.hpp"

#include "../tinyxml2/tinyxml2.h"

#include "../fileutils/fileutils.hpp"

using namespace buildcarbide;

namespace
{
	static auto get_conditions(const char * __restrict condition_text)
	{
		decltype(vc_project::element::conditions) cond_out;

		// todo : consider retaining format string as it would be much simpler than extraction
		// vcxproj conditions are in the following format: '$(Configuration)|$(Platform)'=='Release|Win32'
					
		const auto get_string_side = [&]()->std::string
		{
			std::string out;

			bool escaped = false;
			char quote_char = '\0';
			if (*condition_text == '\'' || *condition_text == '\"')
			{
				quote_char = *condition_text;
				++condition_text;
			}
			while (*condition_text != '=' && *condition_text != '\0')
			{
				if (!escaped && *condition_text == quote_char)
				{
					// we are done with this string.
					++condition_text;
					break;
				}

				if (*condition_text == '\\')
				{
					if (!escaped)
					{
						escaped = true;
					}
					else
					{
						out += *condition_text;
					}
				}
				else
				{
					out += *condition_text;
				}
				++condition_text;
			}

			return out;
		};
					
		const std::string lhs = get_string_side();
		// skip '=' signs
		while (*condition_text == '=')
		{
			++condition_text;
		}
		const std::string rhs = get_string_side();

		// now we have $(Configuration)|$(Platform) and Release|Win32.
		// We will try to extract the macros from lhs: $()

		struct token
		{
			std::string value;
			std::string delim;
			std::string ref_value;
		};

		std::vector<token> macro_tokens;
		token cur_macro;
		bool in_macro = false;
		char prev_c = '\0';
		for (char c : lhs)
		{
			if (in_macro)
			{
				if (c == ')')
				{
					macro_tokens.push_back(cur_macro);
					cur_macro.value.clear();
					cur_macro.delim.clear();
					in_macro = false;
				}
				else
				{
					cur_macro.value.push_back(c);
				}
			}
			else
			{
				if (prev_c == '$' && c == '(')
				{
					if (cur_macro.delim.length())
					{
						cur_macro.delim.pop_back();
					}
					in_macro = true;
				}
				else
				{
					cur_macro.delim.push_back(c);
				}
			}

			prev_c = c;
		}
		if (cur_macro.value.length() || cur_macro.delim.length())
		{
			macro_tokens.push_back(cur_macro);
		}

		// Now, using this same data (token/delim pairs) we can extract the target values from rhs...
		// presuming there's nothing weird like macros that expand to something with a delimiter in it.
		const char * __restrict c_rhs = rhs.c_str();
		for (size_t i = 0; i < macro_tokens.size(); ++i)
		{
			std::string ref_value;

			const char *base = strstr(c_rhs, macro_tokens[i].delim.c_str());
			if (base == nullptr)
			{
				// There was a severe error parsing this.
				// todo throw
				return cond_out;
			}
			base += macro_tokens[i].delim.length();
			size_t ref_len = size_t(-1);
			if (i != (macro_tokens.size() - 1))
			{
				ref_len = uintptr_t(strstr(base, macro_tokens[i + 1].delim.c_str())) - uintptr_t(base);
				ref_value.reserve(ref_len);
			}

			if (ref_len == size_t(-1))
			{
				// end of string, use all of it.
				ref_value = base;
			}
			else
			{
				// use only part of string
				for (size_t t = 0; t < ref_len; ++t)
				{
					ref_value += base[t];
				}
			}

			c_rhs = base;

			// ref_value holds the reference value for this token.
			if (macro_tokens[i].value.length())
			{
				macro_tokens[i].ref_value = ref_value;
			}
		}

		for (const auto &token : macro_tokens)
		{
			if (!token.value.empty())
			{
				project::condition cond = { token.ref_value, project::condition::operation::not_equals };
				cond_out[token.value] = cond;
			}
		}

		return cond_out;

		return cond_out;
	}
}

project * __restrict vc_project::create_or_null(const std::string & __restrict filename)
{
	if (filename.find(".vcxproj") != std::string::npos)
	{
		return new vc_project(filename);
	}
	return nullptr;
}

vc_project::vc_project(const std::string & __restrict filename) : project()
{
	m_Path = fileutils::get_base_path(filename);

	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename.c_str());
	if (doc.Error())
	{
		throw 0;
	}

	const auto project = doc.FirstChildElement("Project");
	if (!project)
	{
		throw 0;
	}

	// Find the possible configurations.
	{
		auto configuration_group = project->FirstChildElement("ItemGroup");
		for (; configuration_group; configuration_group = configuration_group->NextSiblingElement("ItemGroup"))
		{
			// check if this is a correct item group
			auto label_element = configuration_group->Attribute("Label");
			if (!label_element || strcmp(label_element, "ProjectConfigurations") != 0)
			{
				continue;
			}

			auto configuration_element = configuration_group->FirstChildElement("ProjectConfiguration");
			for (; configuration_element; configuration_element = configuration_element->NextSiblingElement("ProjectConfiguration"))
			{
				// Get the daughter elements
				auto config_name_element = configuration_element->FirstChildElement("Configuration");
				if (config_name_element)
				{
					auto text_element = config_name_element->FirstChild();
					if (text_element)
					{
						auto text = text_element->ToText();
						if (text)
						{
							m_Configurations.insert(text->Value());
						}
					}
				}
				auto platform_name_element = configuration_element->FirstChildElement("Platform");
				if (platform_name_element)
				{
					auto text_element = platform_name_element->FirstChild();
					if (text_element)
					{
						auto text = text_element->ToText();
						if (text)
						{
							m_Targets.insert(text->Value());
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

		auto property_group = project->FirstChildElement("PropertyGroup");
		for (; property_group; property_group = property_group->NextSiblingElement("PropertyGroup"))
		{
			// See if this has an 'IncludePath' element.
			auto include_path = property_group->FirstChildElement("IncludePath");
			if (include_path)
			{
				// If so, first extract the condition. If there isn't one, just continue.
				const char *condition_text = property_group->Attribute("Condition");
				if (!condition_text)
				{
					continue;
				}

				auto conditions = get_conditions(condition_text); // We can use this to build a control structure.
				if (conditions["Configuration"].reference.empty() || conditions["Platform"].reference.empty())
				{
					continue;
				}
				const target_pair config_pair = { conditions["Platform"].reference, conditions["Configuration"].reference };

				// now extract the include paths.
				auto text_element = include_path->FirstChild();
				if (!text_element)
				{
					continue;
				}

				auto text = text_element->ToText();
				if (!text)
				{
					continue;
				}

				const char * __restrict ctext = text->Value();
				if (!ctext)
				{
					continue;
				}

				std::unordered_set<std::string> include_paths;
				std::string cur_token;
				// Now extract all the include paths.
				while (*ctext != '\0')
				{
					if (*ctext == ';')
					{
						include_paths.insert(cur_token);
						cur_token.clear();
					}
					else
					{
						cur_token.push_back(*ctext);
					}

					++ctext;
				}
				if (!cur_token.empty())
				{
					include_paths.insert(cur_token);
				}

				m_IncludePaths[config_pair] = include_paths;
			}
		}
	}

	// Find files.
	auto item_group = project->FirstChildElement("ItemGroup");
	if (!item_group)
	{
		throw 0;
	}

	const auto extract_conditions = [](
		element & element,
		const tinyxml2::XMLElement *compile_element
	) -> auto
	{
		auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");

		if (!exclusion_element)
		{
			return decltype(element::conditions){};
		}
		auto true_false = exclusion_element->FirstChild();
		if (!true_false)
		{
			return decltype(element::conditions){};
		}
		auto true_false_text = true_false->ToText();
		if (!true_false_text)
		{
			return decltype(element::conditions){};
		}
		if (strcmp(true_false_text->Value(), "false") == 0)
		{
			element.enabled = false;
			return decltype(element::conditions){};
		}

		const char *condition_text = exclusion_element->Attribute("Condition");
		if (!condition_text)
		{
			return decltype(element::conditions){};
		}

		return get_conditions(condition_text);
	};

	while (item_group)
	{
		element element;

		const auto handle_element = [&](const tinyxml2::XMLElement *compile_element, bool header)
		{
			const char *file_name = compile_element->Attribute("Include");
			if (!file_name)
			{
				return;
			}
			element.filename = file_name;
			element.conditions = extract_conditions(element, compile_element);
			m_AllFiles.push_back(element);
			if (!header)
			{
				m_RawSourceFiles.push_back(element);
			}
		};

		// Iterate over all item groups.
		// source files
		{
			auto compile_element = item_group->FirstChildElement("ClCompile");
			for (; compile_element; compile_element = compile_element->NextSiblingElement("ClCompile"))
			{
				handle_element(compile_element, false);
			}
		}
		// header files
		{
			auto compile_element = item_group->FirstChildElement("ClInclude");
			for (; compile_element; compile_element = compile_element->NextSiblingElement("ClInclude"))
			{
				handle_element(compile_element, true);
			}
		}

		item_group = item_group->NextSiblingElement("ItemGroup");
	}
}

vc_project::~vc_project()
{
}
