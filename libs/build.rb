require 'yaml'
require 'fileutils'

# get all subdirectories, look for a config file.

if (ARGV[0] == nil || ARGV[0] == "")
  puts "Must provide configuration file as parameter."
  exit 1
end

build_only = ""
if (ARGV[1] != nil)
  build_only = ARGV[1]
end

# load the common file.
common_fp = nil
begin
  common_fp = File.open(ARGV[0], "r")
rescue
  puts "Could not open configuration file \'#{ARGV[0]}\'"
  exit 1
end

@cwd = Dir.pwd
tool_paths = []
cpp_cmd = nil
c_cmd = nil
asm_cmd = nil
cpp_tool = nil
c_tool = nil
asm_tool = nil
ar_tool = nil
intermediate = nil
includes = nil

begin
  common_str = common_fp.read
  common_yml_data = YAML.load(common_str)
  
  puts "Building configuration \'#{common_yml_data["name"]}\'"
  tool_paths = common_yml_data["tool_paths"]
  tool_paths.each do |path|
    puts "Additional Tool Path: \'#{path}\'"
  end
  cpp_cmd = common_yml_data["cpp_cmd"]
  c_cmd = common_yml_data["c_cmd"]
  asm_cmd = common_yml_data["asm_cmd"]
  cpp_tool = common_yml_data["cpp_tool"]
  c_tool = common_yml_data["c_tool"]
  intermediate = common_yml_data["intermediate"]
  includes = common_yml_data["includes"]
  ar_tool = common_yml_data["ar_tool"]
  asm_tool = common_yml_data["asm_tool"]
  puts "C++ Command Line Options: " + cpp_tool + " " + cpp_cmd
  puts "C Command Line Options: " + c_tool + " " + c_cmd
rescue
  puts "Could not parse configuration file \'#{ARGV[0]}\'"
  exit 1
end

if (asm_tool == nil)
  puts "No assembly tool provided (asm_tool)."
  exit 1
end

# find cpp_tool and c_tool in paths.
tool_paths.each do |path|
  if File.exist? File.join(path, cpp_tool)
    cpp_tool = File.join(path, cpp_tool)
    break
  end
end
tool_paths.each do |path|
  if File.exist? File.join(path, c_tool)
    c_tool = File.join(path, c_tool)
    break
  end
end
tool_paths.each do |path|
  if File.exist? File.join(path, ar_tool)
    ar_tool = File.join(path, ar_tool)
    break
  end
end
tool_paths.each do |path|
  if File.exist? File.join(path, asm_tool)
    asm_tool = File.join(path, asm_tool)
    break
  end
end

def pathexpand (file)
  return File.expand_path(file, @cwd).gsub('/', '\\')
end

cpp_tool = pathexpand(cpp_tool)
c_tool = pathexpand(c_tool)
ar_tool = pathexpand(ar_tool)
asm_tool = pathexpand(asm_tool)

build_files = []

Dir['*/'].each do |subdir|
  Dir.entries(subdir).each do |file|
    file_path = File.join(subdir, file)
    if File.file? file_path
      if file == "rbbuild.yml"
        build_files << file_path
      end
    end
  end
end

if build_files.size == 0
  puts "No build configuration files found!"
  exit 1
end

# iterate over all build files and parse them

Struct.new("BuildConfig", :name, :path, :include, :source, :exclude_dir_token, :exclude_file_token, :cpp_cmd, :c_cmd);
build_configs = []

build_files.each do |file|
  fp = File.open(file, "r")
  filestr = fp.read
  yml_data = YAML.load(filestr)
  if yml_data == nil
    puts "Configuration file \'#{file}\' is invalid"
    exit 1
  end
  if (build_only != "" && build_only != yml_data["name"])
    puts "Skipping #{yml_data["name"]}"
    next
  end
  puts "Found #{yml_data["name"]}"
  new_config = Struct::BuildConfig.new(
    yml_data["name"],
    File.dirname(file),
    yml_data["include"],
    yml_data["source"],
    yml_data["exclude_dir_token"],
    yml_data["exclude_file_token"],
    yml_data["cpp_cmd"],
    yml_data["c_cmd"],
  );
  build_configs << new_config
end

def is_excluded_dir (config, dir)
  if (config[:exclude_dir_token] != nil)
    config[:exclude_dir_token].each do |token|
      if (dir.include? token)
        return true
      end
    end
  end
  return false
end

def is_excluded_file (config, file)
  if (config[:exclude_file_token] != nil)
    config[:exclude_file_token].each do |token|
      if (file.include? token)
        return true
      end
    end
  end
  return false
end

# iterate over the configs.

c_endings = ["c", "C"]
cpp_endings = ["CC", "cc", "CXX", "cxx", "CPP", "cpp"]
asm_endings = ["asm", "s", "ASM", "S"]

build_configs.each do |config|
  puts "Building " + config[:name]
  
  # iterate over all source files.
  source_files = []
  source_dirs = config[:source]
  iter = 0
  while source_dirs.size > iter
    source_dir = source_dirs[iter]
    if (!File.exist? source_dir)
      source_dir = File.join(config[:path], source_dir)
    end
    Dir.glob(source_dir + "/*").select {|entry|
      if File.file? entry
        if (!is_excluded_file(config, entry))
          puts entry
          source_files << entry
        end
      end
      if File.directory? entry
        if (entry != "." && entry != ".." && !is_excluded_dir(config, entry))
          source_dirs << entry
        end
      end
    }
    iter += 1
  end
  
  object_files = []
  
  # build C++ files
  source_files.each do |file|
    ext = File.extname(file).split('.').last
    if (!cpp_endings.include? ext)
      next
    end
    cmdline = cpp_tool + " " + cpp_cmd;
    # add include paths
    config[:include].each do |inc_path|
      inc_path = File.join(config[:path], inc_path)
      cmdline += " -I" + pathexpand(inc_path)
    end
    includes.each do |inc_path|
      cmdline += " -I" + pathexpand(inc_path)
    end
    cmdline += " " + config[:cpp_cmd]
    cmdline += " " + file + " -o " + pathexpand(intermediate + "/" + File.join(File.dirname(file), File.basename(file, ".*")) + ".o")
    puts cmdline
    object_files << pathexpand(intermediate + "/" + File.join(File.dirname(file), File.basename(file, ".*")) + ".o")
    FileUtils.mkdir_p pathexpand(intermediate + "/" + File.dirname(file))
    success = system cmdline
    if (!success)
      puts "Failed to compile \'#{file}\'"
      exit 1
    end
  end
  
  # build C files
  source_files.each do |file|
    ext = File.extname(file).split('.').last
    if (!c_endings.include? ext)
      next
    end
    cmdline = c_tool + " " + c_cmd;
    # add include paths
    config[:include].each do |inc_path|
      inc_path = File.join(config[:path], inc_path)
      cmdline += " -I" + pathexpand(inc_path)
    end
    includes.each do |inc_path|
      cmdline += " -I" + pathexpand(inc_path)
    end
    cmdline += " " + config[:c_cmd]
    cmdline += " " + file + " -o " + pathexpand(intermediate + "/" + File.join(File.dirname(file), File.basename(file, ".*")) + ".o")
    puts cmdline
    object_files << pathexpand(intermediate + "/" + File.join(File.dirname(file), File.basename(file, ".*")) + ".o")
    FileUtils.mkdir_p pathexpand(intermediate + "/" + File.dirname(file))
    success = system cmdline
    if (!success)
      puts "Failed to compile \'#{file}\'"
      exit 1
    end
  end
  
  # build asm files
  source_files.each do |file|
    ext = File.extname(file).split('.').last
    if (!asm_endings.include? ext)
      next
    end
    cmdline = asm_tool + " " + asm_cmd;
    # add include paths
    config[:include].each do |inc_path|
      inc_path = File.join(config[:path], inc_path)
      cmdline += " -I" + pathexpand(inc_path)
    end
    includes.each do |inc_path|
      cmdline += " -I" + pathexpand(inc_path)
    end
    cmdline += " " + file + " -o " + pathexpand(intermediate + "/" + File.join(File.dirname(file), File.basename(file, ".*")) + ".o")
    puts cmdline
    object_files << pathexpand(intermediate + "/" + File.join(File.dirname(file), File.basename(file, ".*")) + ".o")
    FileUtils.mkdir_p pathexpand(intermediate + "/" + File.dirname(file))
    success = system cmdline
    if (!success)
      puts "Failed to compile \'#{file}\'"
      exit 1
    end
  end

  # link
  ar_cmd = ar_tool + " rcs #{config[:name]}.a "
  object_files.each do |obj|
    ar_cmd += " " + obj
  end
  puts ar_cmd
  success = system ar_cmd
  if (!success)
    puts "Failed to archive \'#{config[:name]}.a\'"
    exit 1
  end
end
