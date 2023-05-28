#pragma once

#include <span>
#include <string>

using std::wstring;

namespace buildcarbide {
	struct options final {
		wstring project_file;
		wstring configuration;
		wstring platform;
		wstring command;
		wstring out_file;
		wstring sdk_path;

		options(wstring &&project_file, wstring &&configuration, wstring &&platform, wstring &&command, wstring &&out_file, wstring &&sdk_path) : 
			project_file(std::move(project_file)),
			configuration(std::move(configuration)),
			platform(std::move(platform)),
			command(std::move(command)),
			out_file(std::move(out_file)),
			sdk_path(std::move(sdk_path))
		{
		}

		static options build(std::span<const wchar_t*> args);
	};
}
