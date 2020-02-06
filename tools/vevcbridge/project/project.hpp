#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace buildcarbide
{
	class project
	{
	public:
		enum class output_type : uint32_t
		{
			none = 0,
			executable,
			library
		};

		struct target_pair
		{
			std::string target;
			std::string configuration;

			bool operator == (const target_pair & __restrict pair) const __restrict
			{
				return (const std::string & __restrict)target == pair.target && (const std::string & __restrict)configuration == pair.configuration;
			}

			struct hash
			{
				uint64_t operator () (const target_pair & __restrict pair) const __restrict
				{
					return std::hash<std::string>{}(pair.target) ^ std::hash<std::string>{}(pair.configuration);
				}
			};
		};

		struct condition
		{
			std::string reference;
			enum class operation : uint32_t
			{
				equals,
				not_equals,
			} op;

			bool operator ()(const std::string & __restrict value) const __restrict
			{
				switch (op)
				{
					case operation::equals:
						return value == (const std::string & __restrict)reference;
					case operation::not_equals:
						return value != (const std::string & __restrict)reference;
					default:
						return false;
				}
			}
		};

		struct element
		{
			std::string filename;
			std::unordered_map<std::string, condition> conditions;
			bool enabled = true;
		};

	protected:
		project() = default;

		std::unordered_set<std::string>	m_Targets;
		std::unordered_set<std::string>	m_Configurations;
		std::unordered_map<target_pair, std::unordered_set<std::string>, target_pair::hash>	m_IncludePaths;
		// A list of all files the project file describes
		std::vector<element>	m_AllFiles;
		// A list of all sources files in the project
		std::vector<element>	m_RawSourceFiles;
		std::string				m_Path;
	
		// The type of output this project emits
		output_type					 m_OutputType = output_type::none;
	public:
		virtual ~project() {}

		output_type get_output_type() const __restrict { return m_OutputType; }

		const std::vector<element> & __restrict get_all_files() const __restrict { return (const std::vector<element> & __restrict)m_AllFiles; }
		const std::vector<element> & __restrict get_raw_source_files() const __restrict { return (const std::vector<element> & __restrict)m_RawSourceFiles; }
		const std::unordered_set<std::string> * __restrict get_include_paths(const target_pair & __restrict pair) const /*__restrict*/
		{
			auto fiter = m_IncludePaths.find(pair);
			if (fiter == m_IncludePaths.end())
			{
				return nullptr;
			}
			return &fiter->second;
		}
		const std::string & __restrict get_path() const __restrict { return (const std::string & __restrict)m_Path; }
	};

	extern project * __restrict get_project(const std::string & __restrict filename);
}
