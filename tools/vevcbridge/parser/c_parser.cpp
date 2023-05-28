#include "buildcarbide.hpp"

#include "c_parser.hpp"
#include <cstdio>
#include <vector>
#include <cassert>

using namespace buildcarbide;

// TODO add ifdef / if defined parsing

namespace {
	class file_wrapper final {
		FILE* file_pointer_ = nullptr;

	public:
		file_wrapper(FILE* const fp) : file_pointer_(fp) {
		}

		file_wrapper(const file_wrapper&) = delete;
		file_wrapper(file_wrapper&& file) noexcept :
			file_pointer_(file.file_pointer_) {
			file.file_pointer_ = nullptr;
		}

		~file_wrapper() {
			if (file_pointer_) {
				fclose(file_pointer_);
			}
		}

		file_wrapper& operator = (const file_wrapper&) = delete;
		file_wrapper& operator = (file_wrapper&& file) noexcept {
			if (file_pointer_) {
				fclose(file_pointer_);
			}
			file_pointer_ = file.file_pointer_;
			file.file_pointer_ = nullptr;
			return *this;
		}

		operator bool() const {
			return file_pointer_ != nullptr;
		}

		operator FILE*() const {
			return file_pointer_;
		}

		[[nodiscard]]
		std::vector<char> read() const {
			check(file_pointer_ != nullptr);

			_fseeki64(file_pointer_, 0, SEEK_END);
			const size_t file_size = _ftelli64(file_pointer_);
			rewind(file_pointer_);
			std::vector<char> result(file_size); // TODO : skip initialization
			check(fread(result.data(), 1, file_size, file_pointer_) == file_size);// Now we parse the given array.

			return result;
		}
	};
}

std::unordered_map<std::wstring, file_utils::modtime_t> buildcarbide::get_c_includes(
	const configuration & __restrict configuration,
	const project & __restrict project,
	const std::wstring & __restrict filename
) {
	std::unordered_map<std::wstring, file_utils::modtime_t> out;

	const std::wstring fixed_filename = file_utils::fix_up(filename);
	std::unordered_set running_parse = { fixed_filename };
	std::unordered_set already_parsed = { fixed_filename };

	const auto insert_parse = [&](const std::wstring & __restrict insert_filename) {
		const std::wstring fixed_insert_filename = file_utils::fix_up(insert_filename);
		if (already_parsed.contains(fixed_insert_filename)) {
			return;
		}
		already_parsed.insert(fixed_insert_filename);
		running_parse.insert(fixed_insert_filename);
	};

	const project::target_pair config_pair = {
		.target = configuration.target,
		.configuration = configuration.config,
	};

	const auto include_paths = project.get_include_paths(config_pair);

	bool first = true;

	while (!running_parse.empty()) {
		auto to_parse_iterator = running_parse.begin();
		wstring to_parse = *to_parse_iterator;  // NOLINT(performance-unnecessary-copy-initialization)
		running_parse.erase(to_parse_iterator);

		const std::wstring base_path = file_utils::get_base_path(to_parse);

		// todo use memory mapping
		// parse this file.
		std::vector<char> file_contents;
		{
			const file_wrapper fp = _wfopen(to_parse.c_str(), L"rb");
			if (!fp) {
				first = false;
				continue;
			}

			if (!first) {
				out[to_parse] = file_utils::get_file_time(file_utils::filetime::modified, fp).value_or(0);
			}
			else {
				first = false;
			}

			file_contents = fp.read();
		}

		auto emit_token = [&](std::string & __restrict token) {
			if (!token.empty()) {
				// Is this a 'include' token?
				if (token.find("include<") != std::string::npos || token.find("include\"") != std::string::npos) {
					const size_t include_len = strlen("include<");
					const bool angle_include = token[include_len - 1] == '<'; // paths are different.
					// this IS an include token!
					token = token.substr(include_len, token.length() - include_len);

					// go forward and terminate it at the proper point.
					for (size_t i = 0; i < token.length(); ++i) {
						if (token[i] == '\"' || token[i] == '>') {
							token = token.substr(0, i);
							break;
						}
					}

					if (!angle_include) {
						insert_parse(file_utils::build_path(base_path, token));
					}
					if (include_paths) {
						for (const auto &include_path : *include_paths) {
							insert_parse(file_utils::build_path(include_path, token));
							// don't insert if there's a colon in the path. That makes Windows a little cranky.
							if (include_path.find(':') == std::string::npos) {
								insert_parse(file_utils::build_path(project.get_path(), include_path, token));
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
		for (char c : file_contents) {
			if (!in_escape && (c == '\"' || c == '\'')) {
				in_string = !in_string;
			}
			else if (!in_escape && in_string && c == '\\') {
				in_escape = true;
			}
			else {
				in_escape = false;
			}
			if (!in_string && (c == '\n' || c == '\r')) {
				emit_token(token);
				in_preprocessor = false;
				in_line = false;
			}
			else if (!isspace(c)) {
				if (!in_line) {
					in_line = true;
					if (c == '#') {
						in_preprocessor = true;
					}
					else {
						if (!in_string && !in_line_comment && c == '*' && prev_c == '/') {
							in_block_comment = true;
						}
						else if (!in_string && !in_line_comment && c == '/' && prev_c == '*') {
							in_block_comment = false;
						}
						else if (!in_string && !in_block_comment && c == '/' && prev_c == '/') {
							in_line_comment = true;
						}
						prev_c = c;
					}
				}
				else if (in_preprocessor) {
					if (!in_string && !in_line_comment && c == '*' && prev_c == '/') {
						in_block_comment = true;
					}
					else if (!in_string && !in_line_comment && c == '/' && prev_c == '*') {
						in_block_comment = false;
					}
					else if (!in_string && !in_block_comment && c == '/' && prev_c == '/') {
						in_line_comment = true;
					}
					if (!in_block_comment && !in_line_comment) {
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
