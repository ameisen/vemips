#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace buildcarbide {
	class project {
	public:
		enum class output_type {
			none = 0,
			executable,
			library
		};

		struct target_pair final{
			std::wstring target;
			std::wstring configuration;

			bool operator == (const target_pair & pair) const {
				return target == pair.target && configuration == pair.configuration;
			}

			struct hash final {
				uint64_t operator () (const target_pair & pair) const {
					const auto hash0 = std::hash<std::wstring>{}(pair.target);
					const auto hash1 = std::hash<std::wstring>{}(pair.configuration);

					// Boost's hash_combine algorithm
					return hash0 ^ (hash1 + 0x9e3779b9u + (hash0 << 6) + (hash0 >> 2));
				}
			};
		};

		struct condition final {
			std::string reference;
			enum class operation {
				equals,
				not_equals,
			} op;

			bool operator ()(const std::string & value) const {
				switch (op) {
					case operation::equals:
						return value == reference;
					case operation::not_equals:
						return value != reference;
					default:
						return false;
				}
			}
		};

		struct element final {
			std::string filename;
			std::unordered_map<std::string, condition> conditions;
			bool enabled = true;
		};

	protected:
		project() = default;
		project(project&&) = default;

		std::unordered_set<std::string>	targets_;
		std::unordered_set<std::string>	configurations_;
		std::unordered_map<target_pair, std::unordered_set<std::wstring>, target_pair::hash>	include_paths_;
		// A list of all files the project file describes
		std::vector<element>	all_files_;
		// A list of all sources files in the project
		std::vector<element>	raw_source_files_;
		std::wstring			path_;
	
		// The type of output this project emits
		output_type					 output_type_ = output_type::none;
	public:
		virtual ~project() = default;

		[[nodiscard]]
		output_type get_output_type() const { return output_type_; }

		[[nodiscard]]
		const std::vector<element> & get_all_files() const { return all_files_; }
		[[nodiscard]]
		const std::vector<element> & get_raw_source_files() const { return raw_source_files_; }
		[[nodiscard]]
		const std::unordered_set<std::wstring> * get_include_paths(const target_pair & __restrict pair) const {
			const auto find_iterator = include_paths_.find(pair);
			if (find_iterator == include_paths_.end()) {
				return nullptr;
			}
			return &find_iterator->second;
		}
		[[nodiscard]]
		const std::wstring & get_path() const { return path_; }
	};

	[[nodiscard]]
	extern project * get_project(const std::wstring & __restrict filename);
}
