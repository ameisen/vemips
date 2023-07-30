# TODO : Move Me
def auto_accessor(vv)
	vv.class_variables.each { |v|
		name = v.to_s[2..-1]
		instance_symbol = "@#{name}".to_sym
		default_value = vv.class_variable_get(v)

		vv.define_method "#{name}=" do |value|
			return vv.instance_variable_set(instance_symbol, value)
		end
		vv.define_method "#{name}" do
			return vv.instance_variable_defined?(instance_symbol) ?
				vv.instance_variable_get(instance_symbol) :
				default_value
		end

		vv.remove_class_variable(v)
	}
end

module Platform
	NATIVE = 0
	WINDOWS = 1
	UNIX = 2

	module Defines
		HOST_OS = RbConfig::CONFIG['host_os']
		HOST_OS_DOWNCASE = HOST_OS.downcase
		IS_WINDOWS = ["mswin", "mingw", "cygwin"].any?{ |sig| HOST_OS_DOWNCASE.include?(sig) }
		IS_WINDOWS_NATIVE = (HOST_OS == "mswin") || Etc.uname[:sysname] == "Windows_NT"
		IS_LINUX = HOST_OS_DOWNCASE.include?('linux')
		IS_MACOS = HOST_OS_DOWNCASE.include?('darwin')
	end

	class TypeClass
		@@windows = false
		@@mingw = false
		@@cygwin = false
		@@linux = false
		@@macintosh = false
		@@unix = false

		auto_accessor(self)
	end

	def self.windows? = Defines::IS_WINDOWS
	def self.windows_native? = Defines::IS_WINDOWS_NATIVE
	def self.linux? = Defines::IS_LINUX
	def self.macos? = Defines::IS_MACOS
	def self.unix? = !Defines::IS_WINDOWS

	VALUE = proc {
		result = TypeClass.new

		case Defines::HOST_OS
		when 'mswin'
			result.windows = true
		when 'mingw'
			result.windows = true
			result.mingw = true
		when 'cygwin'
			result.windows = true
			result.cygwin = true
		when /.*linux\-.*/
			result.linux = true
			result.unix = true
		when 'darwin'
			result.macintosh = true
			result.unix = true
		else
			if Etc.uname[:sysname] == "Windows_NT"
				result.windows = true
			end
		end

		result
	}[]

	PATH_EXT = [
		'',
		*(self.windows? ?
			(ENV['PATHEXT']&.split(';') or ['.exe', '.com']) :
			[]
		)
	].freeze

	PATH_SPLIT = self.windows? ? ';' : ':'

	if self.windows?
		def self.fix_path!(path) = path.gsub!('/', '\\')
		def self.fix_path(path) = self.fix_path!(path.dup)
	else
		def self.fix_path!(path) = path
		def self.fix_path(path) = path
	end

	CYGPATH_PATH = self.windows? ? where_async?('cygpath.exe') : nil

	EXECUTABLE_EXTENSIONS = (Platform::windows? ?
		#	[ ".rb", ".cmd", ".bat", ".exe", ".com" ] :
			[ ".exe", ".com" ] :
			[]
		).freeze

	def self.cygpath(path)
		cygpath = CYGPATH_PATH.value!
		raise "Could not find 'cygpath'" if cygpath.nil?
		stdout, status = Open3.capture2e(cygpath.to_s, '--unix', '--absolute', path.to_s)
		return nil unless status.success?
		return Pathname.new(stdout.strip)
	end

	def self.normalize_path(path, platform: Platform::NATIVE)
		return nil if path.nil?

		return path.cleanpath if path.is_a?(Pathname) && platform == Platform::NATIVE

		def self.normalize_char_loop(path, char, char_extra = nil)
			path = path.to_s
			new_path = ""
			i = 0
			while i < path.length
				c = path[i]
				c = char if c == char_extra
				i += 1
				next if c == char && new_path[-1] == char
				new_path += c
			end
			return new_path
		end

		new_path = nil

		case platform
		when Platform::NATIVE
			if Platform::unix?
				new_path = normalize_char_loop(path, '/')
			else
				new_path = normalize_char_loop(path, '\\', '/')
			end
		when Platform::UNIX
			if Platform::unix?
				new_path = normalize_char_loop(path, '/')
			else
				new_path = normalize_char_loop(cygpath(path), '/')
			end
		when Platform::WINDOWS
			new_path = normalize_char_loop(path, '\\', '/')
		else
			raise "Unknown Platform: '#{platform}'"
		end

		return new_path
	end

	def self.unix_path(path)
		return normalize_path(path, platform: Platform::UNIX)
	end

	def self.resolved_tool_path(path, unix: false)
		normalized_path = Platform::normalize_path(path, platform: unix ? Platform::UNIX : Platform::NATIVE)

		normalized_path = Pathname.new(normalized_path) unless normalized_path.is_a?(Pathname)

		# try to figure out the extension since some things break otherwise
		found = false
		EXECUTABLE_EXTENSIONS.each { |ext|
			with_ext = Pathname.new(normalized_path).sub_ext(ext)
			if with_ext.file?
				normalized_path = with_ext
				found = true
				break
			end
		}
		unless found
			EXECUTABLE_EXTENSIONS.each { |ext|
				with_ext = Pathname.new(normalized_path).sub_ext(ext)
				where_path = where?(with_ext)
				unless where_path.nil?
					normalized_path = Pathname.new(where_path)
					found = true
					break
				end
			}
		end
		#begin
		#normalized_path = normalized_path.realpath
		#rescue
		#end

		return normalized_path
	end
end

class Pathname
	def normalize(platform: Platform::NATIVE) = Platform::normalize_path(self, platform: platform)
	def unix = Platform::unix_path(self)
end
