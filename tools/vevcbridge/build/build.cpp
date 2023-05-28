#include "buildcarbide.hpp"

#include "build.hpp"
#include "parser/c_parser.hpp"

#include <mutex>

using namespace buildcarbide;

// TODO at some point we should integrate a build database.

build::build(const configuration & __restrict configuration, const project & __restrict project) :
	build(configuration, project, false)
{
}

build::build(const configuration & __restrict configuration, std::function<project * ()>&& project_getter) :
	build(configuration, *project_getter(), true)
{
}

namespace {
	struct file_info final {
		std::wstring path;
		file_utils::modtime_t mod_time = 0;

		file_info() = default;
		file_info(std::wstring && __restrict path) :
			path(path),
			mod_time(file_utils::get_file_time(file_utils::filetime::modified, path).value_or(0))
		{}

		file_info(std::wstring&& __restrict path, const file_utils::modtime_t modification_time) :
			path(path),
			mod_time(modification_time)
		{}

		void update_mod_time() {
			mod_time = file_utils::get_file_time(file_utils::filetime::modified, path).value_or(0);
		}
	};
}

build::build(const configuration & __restrict configuration, const project & __restrict project, const bool owned)
try {
	// Distribute the work to multiple threads, which should help with larger projects.
	for (const auto& __restrict source_file : project.get_raw_source_files()) {
		auto filename = to_wstring(source_file.filename);

		const file_info source = file_utils::build_path(configuration.base_path, filename);
		// TODO implement better existence check
		if (source.mod_time == 0) {
			continue;
		}

		// TODO set to true in a rebuild
		bool do_build = false;

		// Get the modification time of the expected output file, if it exists. If it doesn't, we _must_ build.
		// TODO get a toolchain-specific object file extension
		file_info object = { file_utils::build_path(configuration.intermediate_dir, file_utils::strip_extension(filename) + L".o"), 0 };
		if (!do_build) {
			object.update_mod_time();
			if (object.mod_time == 0 || source.mod_time > object.mod_time) {
				do_build = true;
			}
		}

		if (!do_build) {
			for (
				auto includes = get_c_includes(configuration, project, source.path);
				const auto& __restrict include : includes
			) {
				if (include.second > object.mod_time) {
					do_build = true;
					break;
				}
			}
		}

		if (!do_build) {
			// There is no reason to rebuild this object.
			continue;
		}

		// Kick off a compile via process.
	}
}
catch(...) {
	if (owned) {
		delete &project;
	}
	throw;
}

build::~build() = default;
