#include "build.hpp"
#include "../parser/c_parser.hpp"

#include <mutex>

using namespace buildcarbide;

// TODO at some point we should integrate a build database.

build::build(const configuration & __restrict configuration, const project & __restrict project)
{
   std::mutex array_lock;

   bool any_build = false;

   // Distribute the work to multiple threads, which should help with larger projects.
   for (const auto & __restrict source_file : project.get_raw_source_files())
   {
      std::string source_path = fileutils::build_path(configuration.base_path, source_file.filename);
      auto source_modtime = fileutils::get_file_modtime(source_path);
      // TODO implement better existence check
      if (source_modtime == 0)
      {
         continue;
      }

      // TODO set to true in a rebuild
      bool do_build = false;

      // Get the modtime of the expected output file, if it exists. If it doesn't, we _must_ build.
      // TODO get a toolchain-specific object file extension
      fileutils::modtime_t obj_modtime = 0;
      std::string object_path = configuration.intermediate_dir + "/" + fileutils::strip_extension(source_file.filename) + ".o";
      if (!do_build)
      {
         obj_modtime = fileutils::get_file_modtime(object_path);
         if (obj_modtime == 0 || source_modtime > obj_modtime)
         {
            do_build = true;
         }
      }

      if (!do_build)
      {
         auto includes = get_c_includes(configuration, project, source_path);
         for (const auto &include : includes)
         {
            if (include.second > obj_modtime)
            {
               do_build = true;
               break;
            }
         }
      }

      if (!do_build)
      {
         // There is no reason to rebuild this object.
         continue;
      }

      // Kick off a compile via parocess.

      any_build = true;
   }
}

build::~build()
{
}
