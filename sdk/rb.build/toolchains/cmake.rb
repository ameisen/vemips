require_relative 'common.rb'

require 'word_wrap'

class CMakeTool < Tool
	def has_define_override? = true

private
	CMAKE_PATH = where_async?('cmake')
	NINJA_PATH = where_async?('ninja')

	TEST_LINES = [
		"Performing Test",
		"Looking for",
		"Detecting",
		"Check if",
		"Check for",
	].map!{ |s| "-- #{s} "}.freeze

	def self.escape_command(build_path, cmd)
		tab = Options::debug_build ? '' : '  ';

		escaped_eq = '='.escape

		cmd = cmd.map(&:strip)
		cmd.map!(&:escape) if Options::debug_build
		result = []
		i = 0
		while i < cmd.length
			arg = cmd[i]
			i += 1

			tabs = i == 1 ? tab : tab * 2

			if arg[0] == '-' && arg[1] != '-'
				eq_index = arg.index(escaped_eq)
				sub_arg = nil
				mid_arg = ' '
				if eq_index.nil?
					sub_arg = cmd[i]
					i += 1
				else
					sub_arg = arg[eq_index+escaped_eq.length..-1]
					arg = arg[0...eq_index]
					mid_arg = '='
				end

				if ['d', 'D'].include?(arg[1])
					tok = arg[1]
					arg = "-D#{arg[2..-1].light_yellow}".light_cyan
				end

				sub_arg.gsub!(';', ';'.light_red)

				result << "#{tabs}#{arg.light_cyan}#{mid_arg}#{sub_arg.light_green}"
			else
				result << "#{tabs}#{arg.light_cyan}"
			end
		end

		if Options::debug_build
			return ['  ' + result.join(' ')]
		else
			return result
		end
	end

	def self.build_header(header, message)
		if message.start_with?("FAILED:")
			return "#{header.light_red}  #{"FAILED:".light_red}#{message["FAILED:".length..-1].light_yellow}"
		else
			return "#{header.light_red}  #{message.light_yellow}"
		end
	end

	def self.failure_processor(buffer)
		Enumerator.new { |y|
			return if buffer.length <= 0
			y.yield build_header('╔', buffer[0])
			return if buffer.length <= 1
			command_line = []
			buffer[1].split.each { |elem|
				if (command_line.length & 1) == 0
					command_line << elem.light_cyan
				else
					command_line << elem.light_yellow
				end
			}

			continue_prefix = '║'.light_red
			end_prefix = '╚'.light_red

			y.yield "#{continue_prefix}    #{command_line.join(' ')}"
			last_included_from = false
			first_included_from = true

			idx = 2
			end_idx = buffer.length - 1

			buffer[2..-1].each { |line|
				is_end = idx == end_idx
				idx += 1

				prefix = is_end ? end_prefix : continue_prefix

				included_from = "In file included from "
				if !line.delete_prefix!(included_from).nil?
					y.yield continue_prefix unless first_included_from || last_included_from
					first_included_from = false
					last_included_from = true
					y.yield "#{prefix}    #{included_from.light_red}#{line.light_yellow}"
				elsif last_included_from
					last_included_from = false
					file, line, column, error = line.split(':', 4)
					y.yield "#{prefix}    #{file.light_yellow}:#{line}:#{column}:#{error.light_red}"
				else
					y.yield "#{prefix}    #{line}"
				end
			}
		}
	end

	def self.common_system_parse(*cmd)
		return smart_system_simple(*cmd) { |stdin, lines|
			in_warning = false
			in_error = false
			last_err_line = ""

			stdout_cache = []
			failure_buffer = []

			push_failure = lambda {
				unless failure_buffer.empty?
					eputs *failure_processor(failure_buffer)
					failure_buffer = []
				end
			}

			handle_stdout = lambda { |line|
				line.rstrip!

				if line.start_with?("FAILED:")
					push_failure[]
					failure_buffer << line
					return
				elsif !failure_buffer.empty?
					if line[0] == '['
						push_failure[]
					else
						failure_buffer << line
						return
					end
				end

				split_line = line.split(' - ', 2)
				if split_line.length == 2
					split_line[0] = split_line[0].split(' ')
					split_line[0][-1] = split_line[0][-1].light_yellow
					split_line[0] = split_line[0].join(' ')
					status = split_line[1]
					if !status.nil?
						case status.downcase
						when 'failed'
							line = "#{split_line[0]} - #{status.light_red}"
							line.sub!('--', '--'.light_red)
						when 'no', 'not found'
							line = "#{split_line[0]} - #{status.light_yellow}"
							line.sub!('--', '--'.light_yellow)
						when 'success', 'ok', 'true', 'found'
							line = "#{split_line[0]} - #{status.light_green}"
							line.sub!('--', '--'.light_green)
						when 'skipped'
							line = "#{split_line[0]} - #{status.light_cyan}"
							line.sub!('--', '--'.light_cyan)
						end
					else
						if line.start_with?("-- Could NOT")
							line = line.light_red
						elsif line.end_with?(" is disabled")
							line = line[0...-("disabled".length)]
							line += "disabled".light_red
						elsif line.end_with?(" is enabled")
							line = line[0...-("enabled".length)]
							line += "enabled".light_green
						elsif line.include?(" is ")
							split_line = line.split(" is ", 2)
							if split_line.length > 1
								line = "#{split_line[0]} is #{split_line[1].light_yellow}"
								line.sub!('--', '--'.light_yellow)
							end
						elsif line.include?(": ")
							split_line = line.split(": ", 2)
							if split_line.length > 1
								line = "#{split_line[0]}: #{split_line[1].light_yellow}"
								line.sub!('--', '--'.light_yellow)
							end
						end
					end
				else
					prefix = nil

					if line[0] == '['
						close_index = line.index(']')
						unless close_index.nil?
							counter = line[1..close_index - 1]
							line = line[close_index+1..-1]

							current, max = *counter.split('/', 2)
							prefix = "[#{current.ljust(max.length).light_cyan}/#{max.light_cyan}]"
						end
					end

					if line[0] == ' '
						line = line[1..-1]
					end

					line_elem = line.split(' ')
					if line_elem.length > 0
						line_elem[0] = line_elem[0].light_green

						line_elem[1...-1].map!(&:light_cyan)

						line_elem[-1] = line_elem[-1].split('/').map!(&:light_yellow).join('/')

						line = "#{prefix} #{line_elem.join(' ')}" unless prefix.nil?
					end
				end
				puts line
			}

			found_cmake_callstack = false

			lines.each { |stream, line|
				if !in_warning && !in_error && !stdout_cache.empty?
					stdout_cache.each { |line|
						handle_stdout[line]
					}
					stdout_cache.clear
				end

				if stream == lines.stdout
					if !line.include?(" - ") && TEST_LINES.any?{ |l| line.start_with?(l) }
						next
					end
					#if in_warning || in_error
					#	stdout_cache << line
					#else
						handle_stdout[line]
					#end
				else
					line.rstrip!

					if (in_error || in_warning) && !found_cmake_callstack
						found_cmake_callstack = true if line.start_with?("Call Stack ")
					end

					if in_error
						if line.empty? && (found_cmake_callstack || last_err_line.empty?)
							found_cmake_callstack = false
							in_error = false
						end
					elsif in_warning
						if line.empty? && (found_cmake_callstack || last_err_line.empty?)
							found_cmake_callstack = false
							in_warning = false
						end
					else
						if line.start_with?("FAILED:")
							push_failure[]
							failure_buffer << line
							next
						elsif !failure_buffer.empty?
							failure_buffer << line
							next
						end

						if line.start_with?("CMake Warning")
							in_warning = true
						elsif line.start_with?("CMake Error")
							in_error = true
						end
					end

					if in_warning
						STDERR.puts line.light_yellow
					elsif in_error
						STDERR.puts line.light_red
					else
						eputs line
					end

					last_err_line = line
				end
			}

			push_failure[]
		}
	end

	public
	def initialize(name:, path:) = super(name: name, path: path)

	def configure(build_path, path, target)
		ccache = []
		if Options::ccache
			ccache_opt = CCACHE_PATH.value!.normalize
			ccache = [
				CMake::define('CMAKE_C_COMPILER_LAUNCHER', ccache_opt),
				CMake::define('CMAKE_CXX_COMPILER_LAUNCHER', ccache_opt),
			]
		end

		flags = *Environment::current::CMAKE_FLAGS

		if (target.skip_tools)
			flags.delete_if { |flag|
				if !flag.start_with?('-D') && !flag.include?('=')
					false
				else
					f = flag.split('=', 2)[0]
					f = f[2..-1]
					f.strip!

					found = false

					CMake::TOOLS_MAP.each { |k, v|
						if (k.to_s == f)
							found = true
							break
						end
					}

					found
				end
			}
		end

		target_out_host = target.out_host || target.host?

		install_path = nil
		if target.out_path.nil?
			install_path = Directories::Intermediate::get(target_out_host)::OUT_ROOT.mkpath
		else
			install_path = (Directories::Intermediate::get(target_out_host)::ROOT + target.out_path).mkpath
		end

		install_path.mkpath

		cmd = [
			(@path or CMAKE_PATH.value!),
			"--no-warn-unused-cli",
			"-Wno-dev",
			*flags,
			*target.configure_flags,
			CMake::define('CMAKE_INSTALL_PREFIX', install_path),
			CMake::define('CMAKE_POLICY_WARNING_CMP0116', false),
			*ccache,
			#"-DMSVC_RUNTIME_LIBRARY=MultiThreaded",
			"-B", build_path,
			"-S", path
		].compact.map!(&:to_s)

		dump_command(cmd, build_path)

		return CMakeTool::common_system_parse(*cmd)
	end

	def build(build_path, path, target)
		build_option = nil
		begin
			build_option = target.build_option
		rescue
		end

		cmd = [
			NINJA_PATH.value!,
			"-j", Options::concurrency,
			"-l", self.load_average,
			'-C', build_path,
			build_option,
		].compact.map!(&:to_s)

		dump_command(cmd, build_path)

		result = CMakeTool::common_system_parse(*cmd)

		return result if !result || !target.install

		return CMakeTool::common_system_parse(*cmd, 'install')
	end
end

module Tools
	CMAKE = CMakeTool.new(name: 'cmake', path: 'cmake')
end
