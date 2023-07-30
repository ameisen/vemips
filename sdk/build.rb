#!/usr/bin/env ruby

ALTERNATE_COMPILER_SETTING = false

require 'set'
require 'fileutils'
require 'etc'
require 'find'
require 'pathname'

require_relative 'rb.build/common.rb'

module Options
	# using Ninja rules as per `GuessParallelism`
	DEFAULT_CONCURRENCY = proc {
		case Etc.nprocessors
		when 0, 1
			2
		when 2
			3
		else
			Etc.nprocessors + 2
		end
	}[]

	LEGAL_CONFIGURATIONS = Set[
		'Release',
		'Debug',
		'MinSizeRel',
		'RelWithDebInfo'
	].freeze

	def self.find_configuration(config) = LEGAL_CONFIGURATIONS.find { |c| c.downcase == config.downcase }

	class << self
		self.attr_accessor :build_targets
		self.attr_accessor :skip_targets
		self.attr_accessor :build_deps
		self.attr_accessor :concurrency
		self.attr_accessor :tasks
		self.attr_accessor :configuration
		self.attr_accessor :debug_build
		self.attr_accessor :quick_fail
		self.attr_accessor :with_environment
		self.attr_accessor :ccache
		self.attr_accessor :build_directory
	end

	@build_targets = Set.new
	@skip_targets = Set.new
	@build_deps = false
	@concurrency = DEFAULT_CONCURRENCY
	@tasks = Set.new
	@configuration = find_configuration('Release')
	@debug_build = false
	@quick_fail = true
	@with_environment = true
	@ccache = true
	@build_directory = nil
end

require_relative 'rb.build/option.rb'

OPTIONS = Hash.new
[
	BooleanOption.new(
		"clear",
		matches: ["clear", "cls"],
		short_matches: 'c',
		callback: proc { |arg|
			if Platform::windows?
				system("cls") rescue nil if arg
			else
				system("clear") rescue nil if arg
			end
		}
	),
	BooleanOption.new(
		"build dependencies",
		matches: ["build-deps", "build-dependencies"],
		short_matches: nil,
		callback: proc { |arg|
			Options::build_deps = arg
		}
	),
	BooleanOption.new(
		"concurrent",
		matches: ["concurrent", "mt"],
		short_matches: ['C'],
		callback: proc { |arg|
			if !arg
				Options::concurrency = 1
			elsif $concurrent <= 1
				Options::concurrency = Options::DEFAULT_CONCURRENCY
			end
		}
	),
	BooleanOption.new(
		"debug build tool",
		matches: ["debug-build", "build-debug"],
		short_matches: ['D'],
		callback: proc { |arg|
			Options::debug_build = arg
		}
	),
	BooleanOption.new(
		"quick fail",
		matches: ["quickfail", "quick-fail"],
		short_matches: [],
		callback: proc { |arg|
			Options::quick_fail = arg
		}
	),
	BooleanOption.new(
		"with environment variables",
		matches: ["with-env", "with-environment"],
		short_matches: ['E'],
		callback: proc { |arg|
			Options::with_environment = arg
		}
	),
	BooleanOption.new(
		"with ccache",
		matches: ["ccache", "with-ccache"],
		short_matches: [],
		callback: proc { |arg|
			Options::ccache = arg
		}
	),
	ArgumentOption.new(
		"targets",
		matches: ["target", "targets"],
		short_matches: nil,
		callback: proc { |arg|
			Options::build_targets.merge(arg.downcase.split(';'))
		}
	),
	ArgumentOption.new(
		"skip",
		matches: ["skip", "skips"],
		short_matches: nil,
		callback: proc { |arg|
			Options::skip_targets.merge(arg.downcase.split(';'))
		}
	),
	ArgumentOption.new(
		"tasks",
		matches: ["task", "tasks"],
		short_matches: 't',
		callback: proc { |arg|
			Options::tasks.merge(arg.downcase.split(';'))
		}
	),
	ArgumentOption.new(
		"configuration",
		matches: ["configuration", "config"],
		short_matches: 'C',
		callback: proc { |arg|
			found = find_configuration(arg)
			fail "Configuration '#{arg}' is not valid (valid configurations: [ #{Options::LEGAL_CONFIGURATIONS.join('; ')} ])" if found.nil?
			Options::configuration = found
		}
	),
	ArgumentOption.new(
		"jobs (concurrency)",
		matches: ["jobs"],
		short_matches: ['j', 'J'],
		callback: proc { |arg|
			begin
			Options::concurrency = arg.to_i
			rescue
			fail "Concurrency value is not a valid positive integer (#{arg})"
			end
			Options::concurrency = Options::DEFAULT_CONCURRENCY if Options::concurrency == 0
			fail "Concurrency must not be negative (#{Options::concurrency})" if Options::concurrency < 0
		}
	),
	ArgumentOption.new(
		"build directory",
		matches: ["build-directory", "build-dir"],
		short_matches: ['B'],
		callback: proc { |arg|
			Options::build_directory = Pathname.new(arg)
		}
	),
	Option.new(
		"clean",
		matches: ["clean"],
		short_matches: [],
		callback: proc { |arg|
			Options::tasks << 'clean'
		}
	),
	Option.new(
		"build",
		matches: ["build"],
		short_matches: [],
		callback: proc { |arg|
			Options::tasks << 'build'
		}
	),
	Option.new(
		"rebuild",
		matches: ["rebuild"],
		short_matches: [],
		callback: proc { |arg|
			Options::tasks << 'clean'
			Options::tasks << 'build'
		}
	),
	Option.new(
		"help",
		matches: ["help"],
		short_matches: ['?'],
		callback: proc { |arg|
			def tputs(*msg)
				puts *msg.map{ |m| "\t#{m}" }
			end

			OPTIONS.each { |key, option|
				puts option.to_s
				if option.boolean?
					unless option.matches.empty?
						tputs "#{option.matches.map { |m| "--[no-]#{m}" }.join(', ')}"
					end
					unless option.short_matches.empty?
						tputs "#{option.short_matches.map { |m| "-#{m}[-]" }.join(', ')}"
					end
				elsif option.takes_arg?
					unless option.matches.empty?
						tputs "#{option.matches.map { |m| "--#{m}=value" }.join(', ')}"
					end
					unless option.short_matches.empty?
						tputs "#{option.short_matches.map { |m| "-#{m}=value" }.join(', ')}"
					end
				else
					unless option.matches.empty?
						tputs "#{option.matches.map { |m| "--#{m}" }.join(', ')}"
					end
					unless option.short_matches.empty?
						tputs "#{option.short_matches.map { |m| "-#{m}" }.join(', ')}"
					end
				end
			}

			puts "", "Targets:"

			target_len = TARGETS.keys.max_by{ |name| name.length }.length

			TARGETS.each { |name, target|
				if target.depends.empty?
					tputs name
				else
					spacing = target_len - name.length
					tputs "#{name}#{' ' * spacing} [#{target.depends.join(', ')}]"
				end
			}

			exit 0
		}
	)
].each { |option|
	option.matches.each { |match|
		OPTIONS[match] = option
	}
	option.short_matches.each { |match|
		OPTIONS[match] = option
	}
}
OPTIONS.freeze

proc {
	post_flags = false
	arg_index = 0

	pop_arg = lambda { |arg|
		fail "No argument supplied for '#{arg}'" if arg_index >= ARGV.length
		result = ARGV[arg_index]
		arg_index += 1
		return result
	}

	while arg_index < ARGV.length
		arg = ARGV[arg_index]
		arg_index += 1

		unless post_flags
			if arg == "--"
				post_flags = true
				next
			end

			orig_arg = arg

			is_flag = false
			is_short_flag = false
			if arg[0] == '/' && arg.length > 1
				is_flag = true
				is_short_flag = (arg.length == 2)
				arg = arg[1..-1]
			elsif arg[0] == '-'
				if arg[1] == '-'
					arg = arg[2..-1]
				else
					arg = arg[1..-1]
					is_short_flag = true
				end
				is_flag = true
			end

			if is_flag
				flags = nil
				if is_short_flag
					temp_flags = arg.each_char.to_a
					flags = []
					append_all = false
					temp_flags.each { |c|
						if append_all
							flags[-1] += c
							next
						end

						if flags.empty?
							case c
							when '-'
								flags[-1] += '-'
								next
							when '=', ':'
								flags[-1] += c
								append_all = true
								next
							end
						end

						flags << c
					}
				else
					flags = [arg]
				end
				flags.each { |flag|
					orig_flag = flag

					bool_value = true
					if !is_short_flag && flag.downcase.start_with?('no-')
						bool_value = !bool_value
						flag = flag[3..-1]
					end
					if flag.end_with?('-')
						bool_value = !bool_value
						flag = flag[0..-2]
					end

					flag_value = nil

					indices = [':', '='].map{ |c| flag.index(c) }.compact!
					eq_index = indices.min
					unless eq_index.nil?
						flag_value = flag[eq_index + 1..-1]
						flag = flag[0...eq_index]
					end

					flag_match = is_short_flag ? flag : flag.downcase

					option = OPTIONS[flag_match]
					fail "Unknown Argument: '#{orig_arg}'" if option.nil?
					if !flag_value.nil? && !option.takes_arg?
						fail "Argument '#{orig_arg}' does not take an argument, but '#{flag_value}' provided"
					end

					if option.boolean?
						option.invoke(bool_value)
					elsif option.takes_arg?
						flag_value = pop_arg[orig_flag] if flag_value.nil?
						option.invoke(flag_value)
					else
						option.invoke(nil)
					end
				}
				next
			end
		end

		Options::build_targets << arg.downcase
	end
}.call

require_relative 'rb.build/platform.rb'

if Options::build_directory.nil?
	Options::build_directory = Pathname.new(proc {
		if Platform::VALUE.windows
			".build.windows"
		elsif Platform::VALUE.linux
			".build.linux"
		elsif Platform::VALUE.macintosh
			".build.macos"
		else
			".build"
		end
	}[])
end

require_relative 'rb.build/common_post.rb'

Options::build_targets.each { |target|
	fail "Unknown Target requested: #{target}" unless TARGETS.has_key?(target)
}

VALID_TASKS = Set['clean', 'build', 'rebuild'].freeze

proc {
	Options::tasks.map!(&:downcase)
	task_fail = false
	Options::tasks.each { |task|
		unless VALID_TASKS.include?(task)
			eputs "Unknown Task: '#{task}'"
			task_fail = true
		end
	}
	exit -1 if task_fail
	Options::tasks = Set.new(['clean', 'build']) if Options::tasks.empty?

	fail "No tasks provided" if Options::tasks.empty?
}[]

puts "Building into '#{Directories::Intermediate::ROOT}'".light_cyan

Directories::Intermediate::ROOT.rmtree rescue nil if Options::tasks.include?('clean') && Options::build_targets.empty?
Directories::Intermediate::ROOT.mkpath rescue nil

built = Set.new
to_build = nil
if Options::build_targets.empty?
	to_build = TARGETS.to_a.map { |t| t[1] }
else
	to_build = Options::build_targets.map { |t| TARGETS[t] }

	if Options::build_deps
		dependencies = Set.new(to_build)

		queue = Queue.new
		to_build.each { |target|
			queue << target
		}

		while !queue.empty?
			target = queue.pop

			dependencies << target

			target.depends.each { |depend|
				depend_target = TARGETS[depend]

				fail "Target '#{target}' dependency '#{depend}' does not exist" if depend_target.nil?

				queue << depend_target
			}
		end

		queue.close

		to_build.append(*dependencies)

		to_build.uniq!
	end
end

unless Options::skip_targets.empty?
	to_build.reject! { |target| Options::skip_targets.include?(target.to_s) }
end

while !to_build.empty?
	item = to_build.shift
	if Options::tasks.include?('build') && Options::build_deps
		unless item.depends.all?{ |depend| built.include?(depend) }
			to_build << item
			next
		end
	end

	puts "Processing '#{item.to_s.light_green}'".light_cyan

	Environment::configure(item, item.options, ALTERNATE_COMPILER_SETTING || !item.tool.has_define_override?,unix_path: item.unix) {
		if Options::tasks.include?('clean')
			puts "Cleaning '#{item.to_s.light_cyan}'"
			item.clean
		end

		if Options::tasks.include?('build')
			if !Options::tasks.include?('rebuild') && item.built?
				built << item.name
				puts "Skipping '#{item.to_s.light_cyan}' (#{"Already Built".light_cyan})"
				next
			end

			puts "Building '#{item.to_s.light_cyan}'"
			result = item.build(Options::tasks)
			qfail "failure: #{item}" unless result

			dirs_module = Directories::Intermediate::get(item.host?)
			libs_dir = dirs_module::LIBS
			includes_dir = dirs_module::INCLUDES

			copy_artifact = lambda { |artifact|
				fail "Copy Artifact '#{artifact}' could not be found" unless File.exist?(artifact)

				def header?(path)
					return false unless File.file?(path)
					ext = File.extname(path.to_s)
					return ['.h', '.hpp', '.inc'].include?(ext.downcase)
				end

				def header_dir?(path)
					return Dir.glob("#{path}/**/*").all? { |e|
						File.directory?(e) || header?(e)
					}
				end

				dest_dir = nil
				if header?(artifact)
					dest_dir = includes_dir
				else
					if File.directory?(artifact.to_s) && header_dir?(artifact)
						dest_dir = includes_dir
					else
						dest_dir = libs_dir
					end
				end

				dest_dir.mkpath rescue nil
				fail "Build Target directory '#{dest_dir}' does not exist and could not be created" unless dest_dir.directory?

				# TODO : just strip item + build_path from full artifact path to get copy location
				FileUtils.cp_r(artifact, dest_dir + item.copy_target + File.basename(artifact))
			}

			copy_item = lambda { |artifact|
				if artifact == TARGET_ROOT
					artifact = item.source_path.to_s
				elsif artifact.root == TARGET_ROOT
					artifact = item.source_path + artifact.relative_path_from(artifact.root)
				end

				whole_directory = ['include', '*', '**'].include?(File.basename(artifact))
				if ['*', '**'].include?(File.basename(artifact))
					artifact = artifact.parent
				end

				fail "Copy Item '#{artifact}' could not be found" unless File.exist?(artifact)

				includes_dir.mkpath rescue nil
				fail "Build Target directory '#{includes_dir}' does not exist and could not be created" unless includes_dir.directory?

				target = includes_dir + item.copy_target + File.basename(artifact)
				target = target.dirname if (artifact.directory? && target.directory?) || whole_directory
				if whole_directory
					artifact.each_child { |child|
						subtarget = target + child.basename
						if child.directory? && subtarget.directory?
							FileUtils.cp_r(child, subtarget.parent)
						else
							FileUtils.cp_r(child, subtarget)
						end
					}
				else
					FileUtils.cp_r(artifact, target)
				end
			}

			item.artifacts.each { |artifact|
				artifact_path = item.build_path + artifact

				if artifact_path.basename.to_s == '*'
					artifact_path = artifact_path.parent
					artifact_path.each_child { |child|
						copy_artifact[child]
					}
				else
					copy_artifact[artifact_path]
				end
			}

			item.copy.each { |artifact|
				copy_item[artifact]
			}

			built << item.name
		end
	}

end
