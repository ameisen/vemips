require_relative 'common.rb'

class AutoConfTool < Tool
	def has_define_override? = false

	private
	def self.check_executable(file)
		result = true
		if Platform::windows?
			if [".cmd", ".bat"].include?(file.extname.downcase)
				result = false
			else
				#skip WSL stuff since it can break stdout
				system32 = (Pathname.new(ENV['WINDIR']) + 'System32').expand_path
				if file.expand_path.to_s.downcase.start_with?(system32.to_s.downcase)
					result = false
				else
					begin
						output, err, status = Open3.capture2(file.to_s, "--version")
						#result = output.lines(remove_empty: true)[0].end_with?("msys)")
						result = true
					rescue
						result = false
					end
				end
			end
		end
		result
	end

	SH_PATH = where_async?('bash', check: method(:check_executable))
	MAKE_PATH = where_async?('make', check: method(:check_executable))

	def self.define(key, value, delimiter = ' ') = AutoConf::define(key, value, delimiter)

	TEST_LINES = [
		"checking ",
	].map!{ |s| "-- #{s} "}.freeze

	def self.escape_command(build_path, cmd)
		#cmd = cmd.map(&:escape)
		#cmd = cmd.dup

		if Options::debug_build
			return ["  #{cmd.join(' ')}"]
		else
			return cmd.map{ |t| "  #{t.light_cyan}"}
		end
	end

	def self.configure_system_parse(*cmd)
		return smart_system_simple(*cmd) { |stdin, lines|
			lines.each { |stream, line|
				if stream == lines.stdout
					if !line.include?(" - ") && TEST_LINES.any?{ |l| line.start_with?(l) }
						next
					end
					line.rstrip!
					split_line = line.split(' - ', 2)
					status = split_line[1]
					if !status.nil?
						split_color = nil
						case status.downcase
						when 'failed'
							split_color = :light_red
						when 'no', 'not found'
							split_color = :light_yellow
						when 'success', 'ok', 'true'
							split_color = :light_green
						when 'skipped'
							split_color = :light_cyan
						end
						unless split_color.nil?
							line = "#{split_line[0]} - #{status.send(split_color)}"
							line.sub!('--', '--'.send(split_color))
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
					puts line
				else
					line.rstrip!
					eputs line
				end
			}
		}
	end

	public
	def initialize(name:, path:) = super(name: name, path: path)

	def configure(build_path, source_path, target)
		build_path.mkpath

=begin
		Dir.entries(path).each { |p|
			next if p[0] == '.'
			p = File.combine(path, p)
			FileUtils.cp_r(p, build_path)
		}
=end

		FileUtils.rm_f(build_path + 'Makefile')

		target_out_host = target.out_host || target.host?

		install_path = nil
		if target.out_path.nil?
			install_path = Directories::Intermediate::get(target_out_host)::OUT_ROOT.mkpath
		else
			install_path = (Directories::Intermediate::get(target_out_host)::ROOT + target.out_path).mkpath
		end

		install_path.mkpath

		result = nil
		Dir.chdir(build_path.to_s) do
			sub_cmd = [
				source_path + 'configure',
				*Environment::current::AUTOCONF_FLAGS,
				"--srcdir=#{source_path.unix}",
				"--prefix=#{install_path.unix}",
				*target.configure_flags,
			]

			cmd = [
				SH_PATH.value!,
				"-c",
				sub_cmd.map!(&:escape_unix).join(' ')
			].map!(&:to_s)

			dump_command(cmd, build_path)

			result = AutoConfTool::configure_system_parse(*cmd)
		end

		begin
			FileUtils.cp(source_path + 'Makefile', build_path + 'Makefile') unless (build_path + 'Makefile').file?
		rescue
			raise if result
		end

		return result
	end

	def build(build_path, source_path, target)
		Dir.chdir(build_path.to_s) do
			sub_cmd = [
				Platform::unix_path(MAKE_PATH.value!),
				"-j", Options::concurrency,
				"-l", self.load_average,
				*target.make_flags
			].map!(&:to_s)

			cmd = [
				SH_PATH.value!,
				"-c",
				sub_cmd.map!(&:escape_unix).join(' ')
			].map!(&:to_s)

			dump_command(cmd, build_path)

			result = smart_system(*cmd, do_yield: false)

			return result if !result || !target.install

			return smart_system(*[*cmd, 'install'], do_yield: false)
		end
	end
end

module Tools
	AUTOCONF = AutoConfTool.new(name: 'autoconf', path: nil)
end
