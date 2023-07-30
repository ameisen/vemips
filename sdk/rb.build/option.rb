class Option
	attr_reader :name
	attr_reader :callback
	attr_reader :matches
	attr_reader :short_matches

	private
	@boolean = false
	@takes_arg = false

	def self.make_set(value)
		return Set.new if value.nil?
		return value if value.kind_of?(Set)
		if value.respond_to?('each')
			if (value.respond_to?('compact'))
				return Set.new(value.compact)
			else
				result = Set.new
				value.each { |v|
					result << v unless v.nil?
				}
				return result
			end
		end
		return Set.new([value])
	end

	public
	def initialize(name, callback:, matches:, short_matches: nil, boolean: false, takes_arg: false)
		raise "Option '#{name}' takes boolean and argument" if boolean && takes_arg

		(@name = name).freeze
		(@callback = callback).freeze
		(@matches = Option.make_set(matches)).freeze
		(@short_matches = Option.make_set(short_matches)).freeze
		@boolean = boolean
		@takes_arg = takes_arg

		self.freeze
	end

	def to_s = @name

	def boolean? = @boolean
	def takes_arg? = @takes_arg

	def invoke(arg) = @callback[arg]
end

class BooleanOption < Option
	def initialize(name, callback:, matches:, short_matches: nil)
		super(name, callback: callback, matches: matches, short_matches: short_matches, boolean: true, takes_arg: false)
	end
end

class ArgumentOption < Option
	def initialize(name, callback:, matches:, short_matches: nil)
		super(name, callback: callback, matches: matches, short_matches: short_matches, boolean: false, takes_arg: true)
	end
end
