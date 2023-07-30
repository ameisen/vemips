require 'etc'

class Tool
	LOAD_AVERAGE_GOAL = 0.8

	attr_reader :name
	attr_reader :path

	def initialize(name:, path:)
		(@name = name).freeze
		(@path = path).freeze

		self.freeze
	end

	def to_s() = @name

	def load_average = LOAD_AVERAGE_GOAL * Etc.nprocessors

	def dump_command(cmd, build_path)
		puts "[", *self.class.escape_command(build_path, cmd), "]"
	end
end
