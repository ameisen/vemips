#include "c_parser.hpp"
#include <cstdio>
#include <vector>

using namespace buildcarbide;

// TODO add ifdef / if defined parsing

std::unordered_map<std::string, fileutils::modtime_t> buildcarbide::get_c_includes(const configuration & __restrict configuration, const project & __restrict project, const std::string & __restrict filename)
{
	std::unordered_map<std::string, fileutils::modtime_t> out;

	const std::string fixed_filename = fileutils::fixup(filename);
	std::unordered_set<std::string> running_parse = { fixed_filename };
	std::unordered_set<std::string> already_parsed = { fixed_filename };

	const auto insert_parse = [&](const std::string & __restrict filename)
	{
		std::string fixed_filename = fileutils::fixup(filename);
		if (already_parsed.find(fixed_filename) != already_parsed.end())
		{
			return;
		}
		already_parsed.insert(fixed_filename);
		running_parse.insert(fixed_filename);
	};

	project::target_pair config_pair;
	config_pair.configuration = configuration.config;
	config_pair.target = configuration.target;

	const auto include_paths = project.get_include_paths(config_pair);

	bool first = true;

	while (running_parse.size())
	{
		auto to_parse_iter = running_parse.begin();
		auto to_parse = *to_parse_iter;
		running_parse.erase(to_parse_iter);

		const std::string base_path = fileutils::get_base_path(to_parse);

		// todo use memory mapping
		// parse this file.
		FILE * __restrict fp = fopen(to_parse.c_str(), "rb");
		if (!fp)
		{
			first = false;
			continue;
		}

		if (!first)
		{
			out[to_parse] = fileutils::get_file_modtime(fp);
		}
		else
		{
			first = false;
		}

		fseek(fp, 0, SEEK_END);
		size_t filesize = ftell(fp);
		rewind(fp);
		std::vector<char> file_contents(filesize);
		fread(file_contents.data(), 1, filesize, fp);
		fclose(fp);

		// Now we parse the given array.
		
		auto emit_token = [&](std::string & __restrict token)
		{
			if (!token.empty())
			{
				// Is this a 'include' token?
				if (token.find("include<") != std::string::npos || token.find("include\"") != std::string::npos)
				{
					const size_t include_len = strlen("include<");
					const bool angle_include = token[include_len - 1] == '<'; // paths are different.
					// this IS an include token!
					token = token.substr(include_len, token.length() - include_len);

					// go forward and terminate it at the proper point.
					for (size_t i = 0; i < token.length(); ++i)
					{
						if (token[i] == '\"' || token[i] == '>')
						{
							token = token.substr(0, i);
							break;
						}
					}

					if (!angle_include)
					{
						insert_parse(fileutils::build_path(base_path, token));
					}
					if (include_paths)
					{
						for (const auto &include_path : *include_paths)
						{
							insert_parse(fileutils::build_path(include_path, token));
							// don't insert if there's a colon in the path. That makes Windows a little cranky.
							if (include_path.find(':') == std::string::npos)
							{
								insert_parse(fileutils::build_path(project.get_path(), include_path, token));
							}
						}
					}
				}
				token.clear();
			}
		};
		
		std::string token;
		bool in_preprocessor = false;
		bool in_line = false;
		bool in_escape = false;
		bool in_string = false;
		bool in_line_comment = false;
		bool in_block_comment = false;
		char prev_c = '\0';
		for (char c : file_contents)
		{
			if (!in_escape && (c == '\"' || c == '\''))
			{
				in_string = !in_string;
			}
			else if (!in_escape && in_string && c == '\\')
			{
				in_escape = true;
			}
			else
			{
				in_escape = false;
			}
			if (!in_string && (c == '\n' || c == '\r'))
			{
				emit_token(token);
				in_preprocessor = false;
				in_line = false;
			}
			else if (!isspace(c))
			{
				if (!in_line)
				{
					in_line = true;
					if (c == '#')
					{
						in_preprocessor = true;
					}
					else
					{
						if (!in_string && !in_line_comment && c == '*' && prev_c == '/')
						{
							in_block_comment = true;
						}
						else if (!in_string && !in_line_comment && c == '/' && prev_c == '*')
						{
							in_block_comment = false;
						}
						else if (!in_string && !in_block_comment && c == '/' && prev_c == '/')
						{
							in_line_comment = true;
						}
						prev_c = c;
					}
				}
				else if (in_preprocessor)
				{
					if (!in_string && !in_line_comment && c == '*' && prev_c == '/')
					{
						in_block_comment = true;
					}
					else if (!in_string && !in_line_comment && c == '/' && prev_c == '*')
					{
						in_block_comment = false;
					}
					else if (!in_string && !in_block_comment && c == '/' && prev_c == '/')
					{
						in_line_comment = true;
					}
					if (!in_block_comment && !in_line_comment)
					{
						token.push_back(c);
					}
				}
			}
			prev_c = c;
		}
		emit_token(token);
	}

	return out;
}
