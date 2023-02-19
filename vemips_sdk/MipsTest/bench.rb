#!/usr/bin/env ruby

require 'benchmark'

#EXEC_PATH = File.join("d:", "Projects", "vemips", "out", "x64", "Release", "vemips.exe");
#EXEC_PATH = File.join("d:", "Projects", "vemips", "out", "x64", "Release-LLVM", "vemips.exe");
# D:\Projects\vemips\out\x64\Release
# D:\Projects\vemips\out\x64\Release-LLVM

DEFAULT_SDK_PATH = ENV["VEMIPS_SDK"] || File.join("d:", "SDKs", "MIPS");

class String
	def sanitize() = self.empty? ? nil : self
end

class ParameterError < RuntimeError
end

class ExpectedParameterError < ParameterError
	def initialize(base)
		super(base.nil? ? "Expected Parameter" : "Expected Parameter for '#{base}'")
	end
end

module Options
	module Paths
		class << self;
			attr_accessor :sdk_path
			attr_accessor :exec_path
			attr_accessor :bin_path

			def finalize!
				@sdk_path = DEFAULT_SDK_PATH if @sdk_path.nil?
				@exec_path = File.join(@sdk_path, "vemips.exe") if @exec_path.nil?
				@bin_path = File.join(Dir.pwd, "MipsTest.bin") if @bin_path.nil?
				
				raise IOError.new("SDK Path not found: '#{@sdk_path}'") unless Dir.exist?(@sdk_path)
				raise IOError.new("Executable Path not found: '#{@exec_path}'") unless File.executable?(@exec_path)
				raise IOError.new("Binary Path not found: '#{@bin_path}'") unless File.exists?(@bin_path)
			end
		end
	end
	
	def self.finalize!
		Paths::finalize!
	end
end

def binary_arg(name)
	return [/^(\/|-|--)#{name}(?<operator>)(?<value>)$/i, /^(\/|-|--)#{name}(?<operator>[=:])(?<value>.+)$/i]
end

def strip_arg(arg)
	return arg[2..] if arg.start_with?('--')
	return arg[1..] if arg.start_with?('/') || arg.start_with?('-')
	return arg
end

def parse_options(argv)
	index = 0
	in_args = false
	
	pop_arg = lambda { |current = nil|
		raise ExpectedParameterError.new(strip_arg(current)) unless index < argv.length
		result = argv[index]
		index += 1
		return result
	}
	
	get_binary_value = lambda { |arg, results|
		if results[:operator].length == 0
			return pop_arg.(arg)
		else
			return results[:value].sanitize
		end
	}
	
	while (index < argv.length)
		arg = pop_arg.()
		
		if (!in_args && arg == '--')
			in_args = true
			next
		end
		
		case arg
			when *binary_arg('sdk')
				Options::Paths::sdk_path = get_binary_value.(arg, $~)
			when *binary_arg('exec'), *binary_arg('emulator')
				Options::Paths::exec_path = get_binary_value.(arg, $~)
			when *binary_arg('bin'), *binary_arg('binary')
				Options::Paths::bin_path = get_binary_value.(arg, $~)
			else
				raise ParameterError.new("Unrecognized Parameter '#{strip_arg(arg)}'")
		end
	end
end

parse_options(ARGV)
Options::finalize!

module JIT
	JIT0 = 0
	JIT1 = 1
end

module MMU
	Emulated = "emulated"
	None = "none"
	Host = "host"
end

class Config
	attr_reader :jit
	attr_reader :mmu
	attr_reader :rox
	attr_reader :icache
	
	def initialize(jit, mmu, rox, icache)
		raise "ROX is incompatible with MMU::None" if (mmu == MMU::None && rox)
		
		@jit = jit
		@mmu = mmu
		@rox = rox
		@icache = icache
		self.freeze
	end
	
	def to_s
		return "jit:#{@jit} mmu:#{@mmu} rox:#{@rox ? "on" : "off"} icache:#{@icache ? "on" : "off"}"
	end
	
	def args
		return ["--jit#{@jit}", "--mmu", @mmu, @rox ? "--rox" : nil, @icache ? "--icache" : nil].compact
	end
end

benchmarks = [
	Config.new(JIT::JIT0, MMU::None, false, false),
	Config.new(JIT::JIT1, MMU::None, false, true),
	Config.new(JIT::JIT0, MMU::Emulated, false, false),
	Config.new(JIT::JIT1, MMU::Emulated, false, false),
	#Config.new(JIT::JIT0, MMU::Emulated, true),
	#Config.new(JIT::JIT1, MMU::Emulated, true)
]

max_len = benchmarks.map(&:to_s).map(&:length).max

Benchmark.bm(max_len) { |bm|
	results = []
	benchmarks.each { |bench|
		results << bm.report(bench.to_s) {
			args = [Options::Paths::exec_path, *bench.args, Options::Paths::bin_path]
			puts args.join(' ')
			puts system(*args)
		}
	}
	results << bm.report("Native") {
		args = ["MipsTest.exe"]
		puts args.join(' ')
		puts system(*args)
	}
	results
}
