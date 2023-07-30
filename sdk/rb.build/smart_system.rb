require 'open3'
require 'thread'
require 'shellwords'
require 'io/console'
require 'fileutils'
require 'concurrent'

SIGNALS = ["INT", "TERM", "ABRT"].freeze #"KILL", "STOP"

def smart_system(*cmd, do_yield: true)
	if Platform::windows?
		case File.extname(cmd[0]).downcase
		when '.cmd', '.bat'
			cmd = ['cmd', '/C', cmd.join(' ').escape(windows: true, no_quotes: true)]
		end
	end

	begin
		Open3.popen3(*cmd.map(&:to_s)) { |stdin, stdout, stderr, wait_thr|
			begin
			SIGNALS.each { |signal|
				Signal.trap(signal) {
					Process.kill(signal, wait_thr.pid)
					Signal.trap(signal, "DEFAULT")
					Process.kill(signal, 0)
				}
			}

			in_thread = Thread.new {
				begin
				while wait_thr.alive?
					c = STDIN.getch(:intr => true)
					next if c.nil?
					c = c.ord
					if c == 24 || c == 3
						Process.kill("KILL", wait_thr.pid) rescue nil
						eputs " ## Process terminated by user ## "
						exit(-1000)
					end
				end
				rescue
				end
			}

			if do_yield
				yield stdin, stdout, stderr, wait_thr
			else
				streams = [stderr, stdout]

				loop do
					selected = IO.select(streams)&.[](0)
					unless selected.nil?
						if selected[1] == stderr
							selected = [stderr, selected[0]]
						end

						#any = true
						#while any
						#	any = false
							selected.each { |stream|
								begin
									begin
										line = stream.readline_nonblock
									rescue EOFError
										streams.delete(stream)
										line = nil
									end
									unless line.nil?
										if stream == stdout
											puts line.rstrip
										elsif stream == stderr
											eputs line.rstrip
										end
						#				any = true
									end
									#streams.delete(stream) if selected.include?(stream) && stream.eof?
								rescue
								end
							}
						#end
					end

					break if streams.empty?
				end
			end

			stdin.close

			result = wait_thr.value.success?

			in_thread.raise
			in_thread.join

			return result
			rescue
			wait_thr.terminate
			raise
			end
		}
	ensure
		SIGNALS.each { |signal|
			Signal.trap(signal, "DEFAULT")
		}
	end
end

class SmartSystemLines
	attr_reader :stdout
	attr_reader :stderr

	def initialize(stdout, stderr)
		@stdout = stdout
		@stderr = stderr

		self.freeze
	end

	def each
=begin
		lines = Concurrent::Array.new
		event = Concurrent::Event.new

		def process_stream(stream, lines, event)
			if stream.nil?
				lines << nil
				event.set
				return nil
			end

			return Thread.new {
				return if stream.closed?

				begin
				stream.each { |line|
					lines << [stream, line.rstrip!]
					event.set
				}
				rescue
				end
				stream.close rescue nil

				lines << nil
				event.set
			}
		end
=end

	streams = [@stderr, @stdout]

	loop do
		selected = IO.select(streams)&.[](0)
		unless selected.nil?
			if selected[1] == @stderr
				selected = [@stderr, selected[0]]
			end

			#any = true
			#while any
			#	any = false
				selected.each { |stream|
					begin
						begin
						line = stream.readline_nonblock
						rescue EOFError
							streams.delete(stream)
							line = nil
						end
						unless line.nil?
							if line.end_with?("\r\n")
								line.chomp!("\r\n")
								line += "\n"
							end
							yield stream, line.rstrip
			#				any = true
						end
						#streams.delete(stream) if selected.include?(stream) && stream.eof?
					rescue
					end
				}
			#end
		end
		break if streams.empty?
	end

=begin
		threads = [
			process_stream(@stdout, lines, event),
			process_stream(@stderr, lines, event)
		]

		completions = 0
		loop do
			event.wait
			next if lines.empty?
			line = lines.shift
			if line.nil?
				completions += 1
				break if completions == threads.length
			else
				yield *line
			end
		end

		threads.each { |t| t&.join }
=end
	end
end

def smart_system_simple(*cmd)
	return smart_system(*cmd, do_yield: true) { |stdin, stdout, stderr, wait_thr|
		yield stdin, SmartSystemLines.new(stdout, stderr)
	}
end
