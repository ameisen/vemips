require 'open3'
require 'thread'
require 'shellwords'
require 'io/console'
require 'fileutils'

GENERIC_SYSTEM_NAME_FULL = 'Generic'
EMULATED_SYSTEM_NAME_FULL = 'VeMIX'
GENERIC_SYSTEM_NAME = GENERIC_SYSTEM_NAME_FULL.downcase
EMULATED_SYSTEM_NAME = EMULATED_SYSTEM_NAME_FULL.downcase

TRIPLE = "mipsisa32r6el-vemips-generic-musl" #mipsisa32r6el
BASIC_TRIPLE = "mipsisa32r6el-vemips-generic-musl"
CMAKE_SYSTEM_NAME = 'Generic'

TARGET_ROOT = Pathname.new('[[target_root]]').freeze

class CMakeFlags
	include Enumerable

	def initialize(hash, *extra)
		(@array = hash.map { |k, v| CMake::define(k, v) })

		unless extra.empty?
			extra.flatten.each { |e|
				@array << e
				key, value = e.split('=', 2)
				key = key[2..-1] if key.start_with?('-D')
				hash[key] = value
			}
		end
		(@hash = hash).freeze
		@array.freeze
		self.freeze
	end

	def to_h = @hash
	def to_a = @array

	def merged(other)
		return CMakeFlags.new(@hash.merge(other.to_h))
	end

	def [](index) = @array[index]

		def each(&block) =
			block_given? ?
				@array.each(&block) :
				to_enum(:each)
	def <<(item) = @array << item
	def inspect = @array.inspect

	def join(separator = $,) = @array.join(separator)
end

class Target
	attr_reader :name
	attr_reader :path
	attr_reader :tool
	attr_reader :configure_flags
	attr_reader :make_flags
	attr_reader :artifacts
	attr_reader :copy
	attr_reader :copy_target
	attr_reader :depends
	attr_reader :options
	attr_reader :unix
	attr_reader :extra
	attr_reader :no_msys
	attr_reader :skip_tools
	attr_reader :build_option
	attr_reader :artifact_path
	attr_reader :out_path
	attr_reader :out_host
	attr_reader :install

	def initialize(
		name:,
		root: nil,
		path:,
		tool:,
		artifacts:,
		copy: [],
		copy_target: ".",
		depends: [],
		options:,
		configure_flags: nil,
		make_flags: nil,
		unix: false,
		host: true,
		extra: nil,
		no_msys: true,
		skip_tools: false,
		build_option: nil,
		artifact_path: nil,
		out_path: nil,
		out_host: nil,
		install: false,
		clean: true
	)
		unless path.nil?
			path = Pathname.new(path)
			fail "Target Path '#{path}' does not exist" unless path.directory?
		end

		extra = Hash.new if extra.nil?

		root = path if root.nil?
		path = root if path.nil?

		artifacts.map! { |item|
			if item.instance_of?(Array)
				item.map { |i|
					Pathname.new(i)
				}
			else
				Pathname.new(item)
			end
		}

		copy.map! { |item|
			Pathname.new(item)
		}

		@name = name
		(@root = root)&.freeze
		(@path = path)&.freeze
		(@tool = tool)&.freeze
		@configure_flags = configure_flags or []
		(@make_flags = make_flags or []).freeze
		(@artifacts = artifacts.to_set).freeze
		(@copy = copy.to_set)
		(@copy_target = copy_target).freeze
		(@depends = depends.to_set).freeze
		(@options = options).freeze
		@unix = unix
		@host = host
		(@extra = extra).freeze
		@no_msys = no_msys
		@skip_tools = skip_tools
		@build_option = build_option
		(@artifact_path = artifact_path).freeze
		(@out_path = out_path).freeze
		@out_host = out_host
		@install = install
		@do_clean = clean

		self.freeze
	end

	def to_s() = @name

	def build_path = (Directories::Intermediate::get(@host)::ROOT + @name)
	alias_method :intermediate_path, :build_path

	def source_path = @root

	def clean()
		return unless @do_clean
		build_path.rmtree if build_path.exist?
	end

	def build(jobs)
		build_path.mkpath if build_path.exist?
		result = @tool.configure(build_path, @path, self)
		return result unless result
		result = @tool.build(build_path, @path, self)
		return result
	end

	def built? = false
	def host? = @host
end

class CommandTarget < Target
	def initialize(
		name:,
		command:,
		depends: [],
		clean: true
	)
		@command = command

		super(
			name: name,
			root: nil,
			path: nil,
			tool: nil,
			artifacts: [],
			options: [],
			depends: depends,
			clean: clean
		)
	end

	def build(jobs)
		build_path.mkpath if build_path.exist?

		begin
			result = self.instance_exec(build_path, @depends, &@command)
		rescue => ex
			STDERR.puts("Exception building `#{@name}`: #{ex}")
			result = false
		end
		return result
	end
end

class LLVMTarget < Target
	def initialize(
		name:,
		root: nil,
		path: Directories::ROOT + 'llvm-project' + 'llvm',
		subpath: nil,
		tool: Tools::CMAKE,
		artifacts: [],
		copy: [],
		copy_target: ".",
		depends: [],
		options: ToolchainOptions::Host::Executable,
		configure_flags: nil,
		make_flags: nil,
		unix: false,
		host: true,
		extra: nil,
		no_msys: true,
		skip_tools: false,
		build_option: nil,
		artifact_path: nil,
		out_path: nil,
		out_host: nil,
		install: false,
		clean: true
	)
		default_configure_flags = CMakeFlags.new({
				'LLVM_ENABLE_PROJECTS' => "",
				'LLVM_ENABLE_RUNTIMES' => "",
				'LLVM_HOST_TRIPLE' => 'x86_64',
				'C_INCLUDE_DIRS' => Directories::Intermediate::Target::INCLUDES.normalize,
			},
			*LLVMCommonFlags::COMMON
		)

		if configure_flags.nil?
			configure_flags = default_configure_flags
		else
			configure_flags = CMakeFlags.new(
				default_configure_flags.to_h
					.merge(configure_flags.to_h)
			)
		end

		super(
			name: name,
			root: root,
			path: subpath.nil? ? path : path + subpath,
			tool: tool,
			artifacts: artifacts,
			copy: copy,
			copy_target: copy_target,
			depends: [
				'libzstd',
				'liblzma',
				'zlib',
				'libxml2',
				*depends
			],
			options: options,
			configure_flags: configure_flags,
			make_flags: make_flags,
			unix: unix,
			host: host,
			extra: extra,
			no_msys: no_msys,
			skip_tools: skip_tools,
			build_option: build_option,
			artifact_path: artifact_path,
			out_path: out_path,
			out_host: out_host,
			install: install,
			clean: clean
		)
	end
end

module LLVMCommonFlags
	ALWAYS_DISABLE = [
		'COMPILER_RT_BUILD_SANITIZERS',
		'COMPILER_RT_BUILD_XRAY',
		'COMPILER_RT_BUILD_LIBFUZZER',
		'COMPILER_RT_BUILD_PROFILE',
		'COMPILER_RT_BUILD_MEMPROF',
		'COMPILER_RT_BUILD_XRAY_NO_PREINIT',
		'COMPILER_RT_BUILD_ORC',
		'COMPILER_RT_BUILD_GWP_ASAN',
		'COMPILER_RT_CAN_EXECUTE_TESTS',
		'COMPILER_RT_EXCLUDE_ATOMIC_BUILTIN',
		'COMPILER_RT_BUILD_STANDALONE_LIBATOMIC',

		'LIBCXX_HAS_GCC_LIB',
		'LIBCXX_ENABLE_SHARED',

		'LIBCXXABI_ENABLE_SHARED',

		'LIBUNWIND_ENABLE_SHARED',
		'LIBUNWIND_ENABLE_ASSERTIONS',
		'LIBUNWIND_ENABLE_THREADS',

		'LIBOMP_ENABLE_SHARED',

		'BUILD_SHARED_LIBS',

		'LLVM_BUILD_LLVM_DYLIB',
		'LLVM_INCLUDE_EXAMPLES',
		'LLVM_INCLUDE_BENCHMARKS',
		'LLVM_INCLUDE_TESTS',

		# threads aren't currently supported...
		'LIBCXX_ENABLE_THREADS',
		'LIBCXX_HAS_PTHREAD_API',
		'LIBCXX_HAS_PTHREAD_LIB',
		'LIBCXX_ENABLE_FILESYSTEM',

		'LIBCXXABI_ENABLE_THREADS',
		'LIBCXXABI_HAS_PTHREAD_API',
		'LIBCXXABI_HAS_PTHREAD_LIB',
	]

	ALWAYS_ENABLE = [
		'LLVM_ENABLE_PER_TARGET_RUNTIME_DIR',

		'LIBCXX_ENABLE_LOCALIZATION',
		'COMPILER_RT_BUILD_BUILTINS',
		#'COMPILER_RT_USE_LLVM_UNWINDER',
		#'COMPILER_RT_ENABLE_STATIC_UNWINDER',
		'COMPILER_RT_STATIC_CXX_LIBRARY',
		'COMPILER_RT_BAREMETAL_BUILD',
		'COMPILER_RT_BUILD_CRT',
		'COMPILER_RT_HAS_CRT',

		'SANITIZER_USE_STATIC_CXX_ABI',
		'SANITIZER_USE_STATIC_LLVM_UNWINDER',

		'LIBCXX_HAS_MUSL_LIBC',
		#'LIBCXX_HAS_PTHREAD_API',
		'LIBCXX_ENABLE_FILESYSTEM',
		'LIBCXX_USE_COMPILER_RT',
		'LIBCXX_ENABLE_STATIC_ABI_LIBRARY',
		'LIBCXX_ENABLE_STATIC',

		'LIBCXXABI_USE_LLVM_UNWINDER',
		'LIBCXXABI_ENABLE_STATIC_UNWINDER',
		'LIBCXXABI_USE_COMPILER_RT',
		'LIBCXXABI_ENABLE_STATIC',

		'LIBUNWIND_USE_COMPILER_RT',
		'LIBUNWIND_ENABLE_STATIC',

		'LIBOMP_ENABLE_STATIC',

		'LLVM_USE_STATIC_ZSTD',
		'LLVM_OPTIMIZED_TABLEGEN',
		'LLVM_ENABLE_LIBCXX',
		'LLVM_ENABLE_MODULES',
		'LLVM_ENABLE_LLD',
		'LLVM_BUILD_EXTERNAL_COMPILER_RT',

		'ZLIB_USE_STATIC_LIBS',
	]

	COMMON = CMakeFlags.new(
		{
			'LLVM_TARGETS_TO_BUILD' => 'Mips',
			'LLVM_TARGET_ARCH' => 'Mips',
			'LLVM_DEFAULT_TARGET_TRIPLE' => TRIPLE,
			'LLVM_TARGET_TRIPLE' => TRIPLE,
			'LLVM_CCACHE_BUILD' => Options::ccache,

			'CLANG_DEFAULT_CXX_STDLIB' => 'libc++',
			'CLANG_DEFAULT_LINKER' => 'lld',
			'CLANG_DEFAULT_OBJCOPY' => 'llvm-objcopy',
			'CLANG_DEFAULT_RTLIB' => 'compiler-rt',
			'CLANG_DEFAULT_UNWINDLIB' => 'libunwind',

			'LLDB_ENABLE_PYTHON' => false,
			'LLDB_ENABLE_LUA' => false,

			'COMPILER_RT_DEFAULT_TARGET_ARCH' => 'mips',
			'COMPILER_RT_DEFAULT_TARGET_TRIPLE' => TRIPLE,
			'COMPILER_RT_OS_DIR' => TRIPLE,
			#'COMPILER_RT_DEFAULT_TARGET_ONLY' => true,
			'LLVM_STATIC_LINK_CXX_STDLIB' => true,

			'LIBCXX_CXX_ABI' => "libcxxabi",

			'TOOLCHAIN_SHARED_LIBS' => false,
			'TOOLCHAIN_USE_STATIC_LIBS' => true,

			#'CMAKE_C_COMPILER_TARGET' => TRIPLE,
			#'CMAKE_CXX_COMPILER_TARGET' => TRIPLE,
			'LLVM_RUNTIME_TARGETS' => TRIPLE,
			'LLVM_BUILTIN_TARGETS' => TRIPLE,
			'TOOLCHAIN_TARGET_TRIPLE' => TRIPLE,
			#"BUILTINS_#{TRIPLE}_CMAKE_SYSROOT" => ???,
			##"BUILTINS_#{TRIPLE}_CMAKE_SYSTEM_NAME" => CMAKE_SYSTEM_NAME, # Generic
			##"BUILTINS_#{TRIPLE}_COMPILER_RT_BAREMETAL_BUILD" => true,
			##"BUILTINS_#{TRIPLE}_COMPILER_RT_OS_DIR" => 'baremetal',
			##"BUILTINS_#{TRIPLE}_CMAKE_SYSTEM_NAME" => CMAKE_SYSTEM_NAME, # generic
			#'LIBCXX_ADDITIONAL_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'LIBCXXABI_ADDITIONAL_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'COMPILER_RT_COMMON_CFLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'LIBCXX_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'LIBCXXABI_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'COMPILER_RT_COMMON_CFLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",

			#'COMPILER_RT_USE_BUILTINS_LIBRARY' => true,
			#COMPILER_RT_CXX_LIBRARY
			#'LLVM_ENABLE_PIC' => false,

			#'LLVM_INTEGRATED_CRT_ALLOC' => Directories::ROOT + 'rpmalloc',
			#'LLVM_USE_CRT_RELEASE' => 'MT',
			#'LLVM_USE_RELATIVE_PATHS_IN_FILES' => true,
			#'LLVM_USE_RELATIVE_PATHS_IN_DEBUG_INFO' => true,
			#'LLVM_USE_SPLIT_DWARF' => true,
			# CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY
			# CMAKE_FIND_USE_PACKAGE_REGISTRY
			# CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH
		} .merge!(
			ALWAYS_DISABLE.to_h{|v| [v, false]}
		) .merge!(
			ALWAYS_ENABLE.to_h{|v| [v, true]}
		) .sort_by{|k,v| k}.to_h
	)

	HOST = CMakeFlags.new({
		'LLVM_ENABLE_LTO' => ToolchainOptions::Host::lto?.casename,
		#'CMAKE_MSVC_RUNTIME_LIBRARY' => "MultiThreaded",
	})

	TARGET = CMakeFlags.new({
		'LLVM_ENABLE_LTO' => ToolchainOptions::Target::lto?.casename,
		'LLVM_HOST_TRIPLE' => BASIC_TRIPLE,
		'CMAKE_SYSTEM_NAME' => CMAKE_SYSTEM_NAME, # Generic not used because LLVM generates shared targets even if we do not use them
		'CMAKE_SYSTEM_VERSION' => '1',
		'CMAKE_SYSTEM_PROCESSOR' => 'Mips',
		'CMAKE_STATIC_LIBRARY_SUFFIX' => '.a',
		'CMAKE_C_OUTPUT_EXTENSION' => '.o',
		'CMAKE_CXX_OUTPUT_EXTENSION' => '.o',
		'CMAKE_ASM_OUTPUT_EXTENSION' => '.o',
	})
end

class StageGroup
	include Enumerable

	attr_reader :number

	def initialize(targets, number:)
		@array = targets
		@array.each { |target|
			target.name += ".#{number}"
		}
		@number = number
	end

	def to_a = @array
	def to_i = @number

	def [](index) = @array[index]

	def each(&block) =
		block_given? ?
			@array.each(&block) :
			to_enum(:each)
	def <<(item) = @array << item
	def inspect = @array.inspect
end

LIB_SUFFIX = Platform::windows? ?
	'.lib' :
	'.a'

TARGETS = [
	Target.new(
		name: "libzstd",
		root: Directories::ROOT + 'zstd',
		path: Directories::ROOT + 'zstd' + 'build' + 'cmake',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
			'ZSTD_BUILD_SHARED' => false,
			'ZSTD_LEGACY_SUPPORT' => true,
			'ZSTD_BUILD_PROGRAMS' => false
		}),
		artifacts: [
			Platform::windows? ?
				Pathname.new('lib') + "zstd_static#{LIB_SUFFIX}" :
				Pathname.new('lib') + "libzstd#{LIB_SUFFIX}"
		],
		copy: LazyArray.new{[
			TARGET_ROOT + 'lib' + 'zdict.h',
			TARGET_ROOT + 'lib' + 'zstd.h',
			TARGET_ROOT + 'lib' + 'zstd_errors.h'
		]},
		options: ToolchainOptions::Host::Library
	),
	Target.new(
		name: "liblzma",
		path: Directories::ROOT + 'xz',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
			'BUILD_SHARED_LIBS' => false,
			'BUILD_TESTING' => false,
			'HAVE_DECODERS' => true,
			'HAVE_ENCODERS' => true,
			'CREATE_XZ_SYMLINKS' => false,
			'CREATE_LZMA_SYMLINKS' => false,
		}),
		artifacts: [
			Platform::windows? ? "lzma#{LIB_SUFFIX}" : "liblzma#{LIB_SUFFIX}"
		],
		copy: LazyArray.new{[
			TARGET_ROOT + 'src' + 'liblzma' + 'api' + 'lzma.h',
			TARGET_ROOT + 'src' + 'liblzma' + 'api' + 'lzma',
		]},
		options: ToolchainOptions::Host::Library,
		extra: {
		}
	),
	Target.new(
		name: "zlib",
		path: Directories::ROOT + 'zlib-ng',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
			# WITH_NATIVE_INSTRUCTIONS
			# WITH_INFLATE_STRICT
			# WITH_INFLATE_ALLOW_INVALID_DIST
			'ZLIB_COMPAT' => true,
			'ZLIB_ENABLE_TESTS' => false,
			'ZLIBNG_ENABLE_TESTS' => false,
			'BUILD_SHARED_LIBS' => false,
			'WITH_SANITIZER' => false,
			'WITH_GTEST' => false,
		}),
		artifacts: [
			Platform::windows? ? "z#{LIB_SUFFIX}" : "libz#{LIB_SUFFIX}",
			'zconf.h',
			'zlib.h',
			'zlib_name_mangling.h'
		],
		options: ToolchainOptions::Host::Library
	),
	Target.new(
		name: "libxml2",
		path: Directories::ROOT + 'libxml2',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
			'BUILD_SHARED_LIBS' => false,
			'LIBXML2_WITH_ICONV' => false,
			'LIBXML2_WITH_FTP' => false,
			'LIBXML2_WITH_ICU' => false,
			'LIBXML2_WITH_LEGACY' => true,
			'LIBXML2_WITH_THREAD_ALLOC' => true,
			'LIBXML2_WITH_XPTR_LOCS' => true,
			'LIBXML2_WITH_TESTS' => false,
			'LIBXML2_WITH_PROGRAMS' => false
		}),
		artifacts: [
			"libxml2#{LIB_SUFFIX}",
			'config.h',
			'libxml'
		],
		copy: LazyArray.new{[
			TARGET_ROOT + 'include' + 'libxml'
		]},
		depends: [
			'liblzma',
			'zlib',
		],
		options: ToolchainOptions::Host::Library
	),
	LLVMTarget.new(
		name: "llvm-exe.stage1",
		configure_flags: CMakeFlags.new({
			'LLVM_ENABLE_PROJECTS' => "clang;lld",
			'LLVM_BUILD_EXTERNAL_COMPILER_RT' => true,
		}, *LLVMCommonFlags::HOST),
		depends: [
			'libzstd',
			'liblzma',
			'zlib',
			'libxml2',
		],
		out_path: 'llvm.stage1',
		out_host: true,
		install: true
	),
	Target.new(
		name: "musl.stage1",
		path: Directories::ROOT + 'musl',
		tool: Tools::AUTOCONF,
		configure_flags: LazyArray.new{
			bin_path = TARGETS['llvm-exe.stage1'].intermediate_path + 'bin'
			[
				#"--prefix=",
				#"--exec-prefix=",
				"--target=#{TRIPLE}", #originally had 'linux' as the system
				#"--host=",
				"--disable-shared",
				"--with-malloc=oldmalloc",
			].normalize!
		},
		artifacts: [
			'lib/*',
			'obj/include/bits',
		],
		copy: [
			TARGET_ROOT + 'include',
			TARGET_ROOT + 'arch' + 'generic' + '*',
			TARGET_ROOT + 'arch' + 'mips' + '*',
		],
		depends: [
			'llvm-exe.stage1'
		],
		options: ToolchainOptions::Target::Library,
		unix: true,
		host: false,
		no_msys: false,
		out_path: 'llvm.stage1',
		out_host: true,
		install: true
	),
	LLVMTarget.new(
		name: "llvm-crt.stage1",
		subpath: "../compiler-rt",
		configure_flags: CMakeFlags.new({
			'LLVM_ENABLE_RUNTIMES' => "compiler-rt",
			'CMAKE_FIND_ROOT_PATH' => Directories::Intermediate::get(false)::PREFIX.concat(
				[Directories::Intermediate::get(true)::ROOT + 'llvm-exe.stage1' + 'lib']
			).uniq.join(';'),
		}, *LLVMCommonFlags::TARGET),
		depends: [
			'llvm-exe.stage1',
			'libzstd',
			'liblzma',
			'zlib',
			'libxml2',
			"musl.stage1",
		],
		options: ToolchainOptions::Target::LibraryBuiltin,
		host: false,
		out_path: 'llvm.stage1',
		out_host: true,
		install: true
	),
	LLVMTarget.new(
		name: "llvm-lib.stage1",
		subpath: "../runtimes",
		configure_flags: CMakeFlags.new({
			'LLVM_ENABLE_RUNTIMES' => "libcxx;libcxxabi;libunwind",
			'CMAKE_FIND_ROOT_PATH' => Directories::Intermediate::get(false)::PREFIX.concat(
				[Directories::Intermediate::get(true)::ROOT + 'llvm-exe.stage1' + 'lib']
			).uniq.join(';'),
		}, *LLVMCommonFlags::TARGET),
		depends: [
			'llvm-exe.stage1',
			'libzstd',
			'liblzma',
			'zlib',
			'libxml2',
			"musl.stage1",
		],
		artifacts: [
			#'lib/libc++.a',
			#'lib/libc++.modules.json',
			#'lib/libc++abi.a',
			#'lib/libc++experimental.a',
			#'lib/libunwind.a',
			#'modules',
		],
		options: ToolchainOptions::Target::LibraryBuiltin,
		host: false,
		out_path: 'llvm.stage1',
		out_host: true,
		install: true
	),
	CommandTarget.new(
		name: "llvm.stage1",
		depends: [
			'llvm-exe.stage1',
			'llvm-crt.stage1',
			'llvm-lib.stage1',
		],
		clean: false,
		command: lambda { |path, depends|
			dst_root = self.build_path

			dst = Directories::Intermediate::get(true)::ROOT + 'llvm.stage1'

			src_dst_pairs = {
				Directories::Intermediate::get(false)::ROOT + 'out' + 'include' => dst + 'include_c' + TRIPLE, # C include dst
				Directories::Intermediate::get(false)::ROOT + 'out' + 'libs' => dst + 'lib' + TRIPLE
			}

			src_dst_pairs.each { |src, dst|
				dst.mkpath

				src.glob('**/*').each { |item|
					next unless item.file?

					relative_item = item.relative_path_from(src)

					item_dst = dst + relative_item

					item_dst.dirname.mkpath

					puts "#{item} -> #{item_dst}"
					item_dst.unlink if item_dst.exist?
					begin
					item_dst.make_link(item)
					rescue
					FileUtils.cp(item.to_s, item_dst.to_s)
					end
				}
			}

			return true
		}
	),
	CommandTarget.new(
		name: "llvm.out",
		depends: [
			'llvm.stage1',
		],
		clean: false,
		command: lambda { |path, depends|
			dst_root = self.build_path

			src = Directories::Intermediate::get(true)::ROOT + 'llvm.stage1'
			dst = Directories::Intermediate::ROOT + 'vemips-llvm'

			FileUtils.cp_r(src.to_s, dst.to_s)

			puts "LLVM toolchain copied to `#{dst}`"

			return true
		}
	),
=begin
	LLVMTarget.new(
		name: "llvm-exe.stage2",
		configure_flags: CMakeFlags.new({
			'LLVM_ENABLE_PROJECTS' => "clang;lld",  # ;lldb;clang-tools-extra
			#'LLVM_ENABLE_RUNTIMES' => "compiler-rt;libcxx;libcxxabi;libunwind",
		}, *LLVMCommonFlags::HOST),
		depends: [
			'llvm-exe.stage1',
			'musl.stage2',
		]#,
		#skip_tools: true
	),
=end
=begin
	LLVMTarget.new(
		name: "llvm-libs.stage3",
		configure_flags: CMakeFlags.new({
			'LLVM_ENABLE_RUNTIMES' => "compiler-rt;libcxx;libcxxabi",
			#'LIBCXX_ADDITIONAL_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'LIBCXXABI_ADDITIONAL_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'COMPILER_RT_COMMON_CFLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'LIBCXX_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'LIBCXXABI_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
			#'COMPILER_RT_COMMON_CFLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
		}, *LLVMCommonFlags::TARGET),
		depends: [
			'musl',
			'llvm-exe.stage2',
		],
	),
	Target.new(
		name: "llvm-compiler-rt",
		path: Directories::ROOT + 'llvm-project' + 'llvm',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
				'LLVM_ENABLE_PROJECTS' => "",
				'LLVM_ENABLE_RUNTIMES' => "compiler-rt;libcxx;libcxxabi",
				'LLVM_HOST_TRIPLE' => TRIPLE,
				#'LIBCXX_ADDITIONAL_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
				#'LIBCXXABI_ADDITIONAL_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
				#'COMPILER_RT_COMMON_CFLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
				#'LIBCXX_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
				#'LIBCXXABI_COMPILE_FLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
				#'COMPILER_RT_COMMON_CFLAGS' => "-I#{Directories::Intermediate::Target::INCLUDES}",
				'C_INCLUDE_DIRS' => Directories::Intermediate::Target::INCLUDES,
				'CMAKE_SYSTEM_NAME' => 'Generic',
				'CMAKE_HOST_SYSTEM_NAME' => 'Generic',
				'CMAKE_SYSTEM_PROCESSOR' => 'mipsisa32r6el',
				'LLVM_CMAKE_DIR' => Directories::ROOT + 'llvm-project' + 'llvm',
				#DEFAULT_SYSROOT
			},
			*LLVMCommonFlags::COMMON,
			*LLVMCommonFlags::TARGET,
		),
		artifacts: [
		],
		copy: [
		],
		copy_target: ".",
		depends: [
			'libzstd',
			'liblzma',
			'zlib',
			'libxml2',
			'llvm-exe.stage1'
		],
		options: ToolchainOptions::Target::LibraryBuiltin
	),
=end
=begin
	Target.new(
		name: "llvm-compiler-rt",
		path: Directories::ROOT + 'llvm-project' + 'llvm',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
				'LLVM_ENABLE_PROJECTS' => "",
				'LLVM_ENABLE_RUNTIMES' => "compiler-rt",
			},
			*LLVMCommonFlags::TARGET,
			*LLVMCommonFlags::COMMON
		),
		artifacts: [
		],
		copy: [
		],
		copy_target: ".",
		depends: [
			'musl',
			'llvm-exe'
		],
		options: ToolchainOptions::Target::LibraryBuiltin,
		host: false
	),
=end
=begin
	Target.new(
		name: "llvm-libunwind",
		path: Directories::ROOT + 'llvm-project' + 'llvm',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
				'LLVM_ENABLE_PROJECTS' => "",
				'LLVM_ENABLE_RUNTIMES' => "libunwind",
			},
			*LLVMCommonFlags::TARGET,
			*LLVMCommonFlags::COMMON
		),
		artifacts: [
		],
		copy: [
		],
		copy_target: ".",
		depends: [
			'musl',
			'llvm-exe',
			'llvm-compiler-rt'
		],
		options: ToolchainOptions::Target::LibraryBuiltin,
		host: false
	),
	Target.new(
		name: "llvm-libs",
		path: Directories::ROOT + 'llvm-project' + 'llvm',
		tool: Tools::CMAKE,
		configure_flags: CMakeFlags.new({
				'LLVM_ENABLE_PROJECTS' => "",
				'LLVM_ENABLE_RUNTIMES' => "libcxx;libcxxabi",
			},
			*LLVMCommonFlags::TARGET,
			*LLVMCommonFlags::COMMON
		),
		artifacts: [
		],
		copy: [
		],
		copy_target: ".",
		depends: [
			'musl',
			'llvm-exe',
			'llvm-libunwind',
			'llvm-compiler-rt'
		],
		options: ToolchainOptions::Target::Library,
		host: false
	)
=end
].map! { |target| [target.to_s.downcase, target] }.to_h.freeze

# validate no circular dependencies
TARGETS.each { |kt, vt|
	pending = [vt]

	while !pending.empty?
		parse_target = pending.shift

		parse_target.depends.each { |depend_name|
			depend = TARGETS.find { |k, v| v.name == depend_name }[1] rescue nil
			fail "Dependency '#{depend_name}' was not defined" if depend.nil?

			if depend == vt
				fail "There is a circular dependency in target '#{vt}' from '#{depend}'"
			end

			pending << depend
		}
	end
}
