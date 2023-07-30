require 'pathname'

CMAKE_GENERATOR = 'Ninja'

module Directories
	ROOT = Pathname.new(File.expand_path("..", __dir__))

	module Intermediate
		ROOT = Options::build_directory.absolute? ? Options::build_directory : (Directories::ROOT + Options::build_directory)

		module Host
			ROOT = Directories::Intermediate::ROOT + 'host'
			OUT_ROOT = ROOT + 'out'
			LIBS = OUT_ROOT + 'libs'
			INCLUDES = OUT_ROOT + 'include'

			PREFIX = [LIBS, INCLUDES]
		end

		module Target
			ROOT = Directories::Intermediate::ROOT + 'target'
			OUT_ROOT = ROOT + 'out'
			LIBS = OUT_ROOT + 'libs'
			INCLUDES = OUT_ROOT + 'include'

			PREFIX = [LIBS, INCLUDES]
		end

		def self.get(host) = host ? Host : Target
	end
end

COMMON_INCLUDE = Directories::ROOT + 'force_include.h'
fail "Common Include '#{COMMON_INCLUDE}' could not be found" unless COMMON_INCLUDE.file?
