require_relative 'extensions.rb'

require 'open3'
require 'rbconfig'
require 'colorize'
require_relative 'hue.rb'
require 'concurrent'
require 'shellwords'

def eputs(*msg) = STDERR.puts(*msg.map{ |s| s.to_s.light_red })

def fail(*msg, code: -1)
	eputs(*msg)
	exit code
end

def qfail(*msg, code: -1)
	eputs(*msg)
	exit code if Options::quick_fail
end

def where?(target, default = nil, check: nil)
	begin
		if default&.respond_to?(:file?)
			return default if default&.executable? && !default&.directory?
		elsif default&.respond_to?(:to_s)
			return default if File.executable?(default&.to_s) && !File.directory?(default&.to_s)
		end

		target_ext = File.extname(target)

		search_paths = [
			Dir.pwd,
			*ENV['PATH'].split(Platform::PATH_SPLIT)
		].uniq

		search_paths.each { |path|
			path_root = Pathname.new(path)

			Platform::PATH_EXT.each { |ext|
				test_file = path_root + "#{target}#{ext}"
				if test_file.executable? && !test_file.directory?
					unless check.nil?
						next unless check[test_file]
					end

					test_file = test_file.to_s

					Platform::fix_path!(test_file)
					if Platform::windows? && test_file[1] == ':'
						test_file[0].upcase!
					end

					return Pathname.new(test_file)
				end
			}
		}

		return nil
	rescue => ex
		return nil
	end
end

def where_async?(target, default = nil, check: nil)
	return Concurrent::Promises.future(target, default, check) { |t, d, c|
		where?(t, d, check: c) or fail "Could not find target: '#{t}'"
	}
end

require_relative 'platform.rb'

class String
	def include_any?(*values)
		return self.include?(values) unless values.respond_to?('each')

		if values.all?{ |v| v.length <= 1 }
			self.each_char { |c|
				values.each { |v|
					next if v.empty?
					return true if c == v
				}
			}
			return false
		else
			return values.any?{ |v| self.include?(v) }
		end
	end

	private
	def escape_windows(quote_force: false, force_escape: false, no_quotes: false)
		# https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
		if (!quote_force && !self.empty? && !self.include_any?(' ', "\t", "\n", "\v", '"'))
			return self.dup
		elsif !quote_force && force_escape
			result = ""

			self.each_char { |c|
				case c
				when ' '
					result += '^ '
				when "\t"
					result += '^t'
				when "\n"
					result += '^n'
				when "\v"
					result += '^v'
				when '"'
					result += '^"'
				else
					result += c
				end
			}

			return result
		else
			result = ""

			iter = 0
			iter_end = self.length

			loop do
				begin
					num_backslashes = 0

					while iter < iter_end && self[iter] == "\\"
						iter += 1
						num_backslashes += 1
					end

					if iter >= iter_end
						result += "\\" * (num_backslashes * 2)
						break
					elsif self[iter] == '"'
						result += "\\" * (num_backslashes * 2 + 1)
						result += self[iter]
					else
						result += "\\" * num_backslashes
						result += self[iter]
					end
				ensure
					iter += 1
				end
			end

			if no_quotes
				return result
			else
				return "\"#{result}\""
			end
		end
	end
	public
	def escape(unix: false, windows: false, force_escape: false, no_quotes: false)
		if !unix && !windows
			if Platform::windows?
				windows = true
			else
				unix = true
			end
		elsif unix && windows
			raise "Cannot specify 'unix' and 'windows' for String.escape"
		end

		if windows
			return self.escape_windows(quote_force: false, force_escape: force_escape, no_quotes: no_quotes)
		else
			return Shellwords.escape(self)
		end
	end

	def escape_unix = self.escape(unix: true)
end

class NilClass
	def escape(unix: false, windows: false) = nil
	def escape_unix = self.escape(unix: true)
end

class Pathname
	def escape(unix: false, windows: false) = self.to_s.escape(unix: unix, windows: windows)
	def escape_unix = self.to_s.escape_unix
end

class Lazy
	@functor = nil
	@initialized = false
	@value = nil

	def value
		@value = @functor[] unless @initialized
		@functor = nil
		@initialized = true
		return @value
	end

	def initialized? = @initialized
	alias_method :has_value?, :initialized?

	def initialize(&block)
		block = proc { nil } if block.nil?
		@functor = block
		@initialized = false
	end

	def to_s = self.value.to_s

	def [](index) = self.value[index]
end

class LazyArray < Lazy
	include Enumerable

	def initialize(&block) = super(&block)

	def to_a = self.value
	def each(&block) =
			block_given? ?
				self.value.each(&block) :
				to_enum(:each)
	def <<(item) = self.value << item
	def inspect = self.value.inspect

	def join(separator = $,) = self.value.join(separator)

	def escape_join(delim) = self.value.escape_join(delim)

	def map(&block) = LazyArray.new{to_a.map(&block)}
	def map!(&block)
		to_a.map!(&block)
		return self
	end
end

class Integer
	@@NumBytes = [42].pack('i').size
	@@NumBits = @@NumBytes * 8
	@@Max = 2 ** (@@NumBits - 2) - 1
	@@Min = -@@Max - 1
end

def recursive_require(path)
	queue = Queue.new
	queue << path

	while !queue.empty?
		queued_path = queue.pop

		Dir.each_child(queued_path) { |req|
			full_path = File.combine(queued_path, req)
			if File.directory?(full_path)
				queue << full_path
			else
				require full_path
			end
		}
	end

	queue.close
end
