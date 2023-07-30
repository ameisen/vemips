class Pathname
	def self.combine(*args)
		return Pathname.new(args[0]).join(*args[1..-1])
	end

	def self.check_combine(*args)
		result = Pathname.combine(*args)
		raise "Path '#{result}' does not exist" unless result.exist?
		return result
	end

	def root
		self.descend { |v|
			return v
		}
		return self
	end

	def start_with?(element)
		return root.to_s == element
	end
end

class File
	class <<self
		alias_method :combine, :join
	end

	def self.check_combine(*args)
		result = File.combine(*args)
		raise "Path '#{result}' does not exist" unless File.exist?(result)
		return result
	end
end

class Array
	def normalize()
		result = self.flatten
		result.compact!
		return result
	end

	def normalize!()
		self.flatten!
		self.compact!
		return self
	end

	def escape_join(delim) = self.map { |v| v.to_s.gsub(delim, "\\#{delim}") }.join(delim)
end

class Object
	def evaluate(sym)
		return self.const_get(sym) if self.const_defined?(sym) rescue nil
		return self.instance_variable_get(sym) if self.instance_variable_defined?(sym) rescue nil
		return self.method(sym).call if self.methods.include?(sym) rescue nil

		return eval("self.#{sym}") rescue nil
	end
end

class Module
	def parent
		return @__parent if @__cached_parent

		self_str = self.to_s
		idx = self_str.rindex('::')
		if idx.nil?
			@__cached_parent = true
			@__parent = nil
			return nil
		end
		module_str = self_str[0...idx]
		parent_module = Object.const_get(module_str)
		@__cached_parent = true
		@__parent = parent_module
		return parent_module
	end

	def has_parent? = !self.parent.nil?
end

class EnumValue
	attr_reader :name
	attr_reader :value

	protected
	def initialize(name, value)
		begin
			value = value.to_i
		rescue ex
			raise "EnumValue value '#{value}' is not integral (#{value.class} :: #{ex})"
		end

		(@name = name).freeze
		(@value = value).freeze
		self.freeze
	end

	public
	def to_s = @name
	def to_i = @value

	def ==(other) = (self.class == other.class) && (@value == other.value)
	def !=(other) = (self.class != other.class) || (@value != other.value)
end

class String
	# https://stackoverflow.com/a/9743074/5055153
  def rsub!(pattern,replacement=nil)
    if n=rindex(pattern)
			found = nil
			if pattern.is_a?(Regexp)
				found = match(pattern,n)
			else
				found = [pattern]
			end
      self[n,found[0].length] = if replacement
        replacement.gsub(/\\\d+/){ |s| found[s[1..-1].to_i] || s }
      else
        yield(*found).to_s
      end
    end
  end
  def rsub(pattern,replacement=nil,&block) = dup.tap{ |s| s.rsub!(pattern,replacement,&block) }

	def lines(remove_empty: false)
		value = self.dup
		value.gsub!("\r\n", "\n") if Platform::windows?
		value.gsub!("\r", "\n")
		result = value.split("\n")
		result.reject!(&:empty?) if remove_empty
		return result
end

=begin
	module EscapeType
		class InnerEnumValue < EnumValue
			def initialize(*args) = super(*args)
		end

		NONE = InnerEnumValue.new(0)
		ESC = InnerEnumValue.new(1)
		CSI = InnerEnumValue.new(2)
		DCS = InnerEnumValue.new(3)
		OSC = InnerEnumValue.new(4)
	end

	module EscapeCSType
		class InnerEnumValue < EnumValue
			def initialize(*args) = super(*args)
		end

		NONE = InnerEnumValue.new(0)
		COLOR = InnerEnumValue.new(1)
		OTHER = InnerEnumValue.new(2)
	end

	private
	def parse_escape(str, index)
		escape_type = EscapeType::NONE

		while index < self.length
			c = str[index]
			index += 1

			case escape_type
			when EscapeType::NONE
				case c
				when '[', "\x9B"
					escape_type = EscapeType::CSI
				when 'P', "\x90"
					escape_type = EscapeType::DCS
				when ']', "\x9D"
					escape_type = EscapeType::OSC
				else
					escape_type = EscapeType::ESC
					index -= 1
				end

				next
			when EscapeType::CSI
			when EscapeType::DCS
			when EscapeType::OSC
			when EscapeType::ESC
			else
				raise "Unknown Escape Type: #{escape_type}"
			end
		end
	end

	public

	def strip_codes!
		return self unless self.include?("\e")

		result = ""

		in_escape = false
		escape_type = EscapeType::NONE

		index = 0
		while index < self.length
			c = self[index]
			index += 1

			if in_escape
				if escape_type == EscapeType::NONE
					case c
						''
					end
				else
				end
			else
				if c == "\e"
					in_escape = true
					next
				end
				result += c
			end
		end

		return result
	end
	def strip_codes = dup.strip_codes!
=end
end

class Object
	def is_bool? = self.is_a?(FalseClass) || self.is_a?(TrueClass)
end

class IO
	private
	def readline_nonblock_impl
		@readline_nb_buffer = "" if @readline_nb_buffer.nil?

		result = @readline_nb_buffer

		newline = result.index("\n")
		unless newline.nil?
			result_line = result[0..newline]
			@readline_nb_buffer = result[newline+1..-1]
			return result_line
		end

		while result[-1] != "\n"
			# read_nonblock doesn't always work on all platforms, so use select first
			begin
				return nil if self.closed?

				temp_buffer = self.read_nonblock(512)
				newline = temp_buffer.index("\n")
				if newline.nil?
					result += temp_buffer
				else
					result += temp_buffer[0..newline]
					@readline_nb_buffer = temp_buffer[newline+1..-1]
					break
				end
			rescue => ex
				if ex.is_a?(EOFError)
					return result unless result.empty?
					raise
				end
				@readline_nb_buffer = result unless result.empty?
				return nil
			end
		end

		return result
	end

	public
	def readline_nonblock
		result = readline_nonblock_impl
		if result.nil?
			raise IO.WaitReadable.new
		else
			return result
		end
	end

	def readline_nonblock_safe
		begin
			return readline_nonblock
		rescue
			return nil
		end
	end

	def each_nonblock
		if block_given?
			loop do
				begin
				result = readline_nonblock_impl
				rescue EOFError
					break
				end
				break if result.nil?
				yield result
			end
		else
			return to_enum(:each_nonblock)
		end
	end
end
