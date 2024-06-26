require "FileUtils"

cfg = ARGV[0] or "Release"

CONFIG = "%_TARGET_%"

Struct.new("Object", :src, :dst)

objects = [
  Struct::Object.new(
    "./out/x64/#{CONFIG}/vevcbridge.exe",
    "./vemips_sdk/MIPS_SDK/vevcbridge.exe"
  ),
  Struct::Object.new(
    "./out/x64/#{CONFIG}/vemips.exe",
    "./vemips_sdk/MIPS_SDK/vemips.exe"
  ),
  Struct::Object.new(
    "./out/x64/#{CONFIG}/mipsdbg.exe",
    "./vemips_sdk/MIPS_SDK/mipsdbg.exe"
  ),
]

objects.each do |object|
  FileUtils.cp(
    (object[:src].gsub(CONFIG, cfg)),
    (object[:dst].gsub(CONFIG, cfg))
  )
end