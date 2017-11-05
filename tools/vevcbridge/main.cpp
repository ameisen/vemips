#include "tinyxml2/tinyxml2.h"

#include <string>
#include <vector>
#include <Windows.h>
#include <ShlObj.h>
#include <direct.h>
#include "project/project.hpp"
#include "fileutils/fileutils.hpp"
#include "build/build.hpp"
#include "configuration.hpp"

std::string MIPSSDK = "D:\\Tools\\MIPS\\";

void replace(std::string& str, const std::string &from, const std::string &to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return;
    str.replace(start_pos, from.length(), to);
}

std::string get_dir(const std::string &file_path)
{
   std::string out = file_path;

   while (out.size() && out.back() != '/' && out.back() != '\\')
   {
      out.pop_back();
   }
   while (out.size() && (out.back() == '/' || out.back() == '\\'))
   {
      out.pop_back();
   }

   return out;
}

uint64_t get_file_time(const std::string &filename, uint64_t def = uint64_t(-1))
{
   ULARGE_INTEGER ints;
   FILETIME file_time;
   HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
   if (hFile == INVALID_HANDLE_VALUE)
   {
      return def;
   }
   
   if (GetFileTime(hFile, nullptr, nullptr, &file_time) == 0)
   {
      CloseHandle(hFile);
      return def;
   }

   CloseHandle(hFile);

   ints.HighPart = file_time.dwHighDateTime;
   ints.LowPart = file_time.dwLowDateTime;
   return ints.QuadPart;
}

std::string get_file_name(const std::string &path)
{
   std::string out;

   for (int64_t i = path.length() - 1; i >= 0; --i)
   {
      if (path[i] == '\\' || path[i] == '/')
      {
         break;
      }
      out.push_back(path[i]);
   }
   // reverse
   for (size_t i = 0; i < out.length() / 2; ++i)
   {
      std::swap(out[i], out[(out.length() - i) - 1]);
   }

   return out;
}

std::string get_compile_args(const std::string &filepath, const std::string &configuration, const std::string &intermediate, std::string &object_file)
{
   const std::string filename = get_file_name(filepath);
   const std::string filedir = get_dir(filepath);

   // TODO rework this.

   std::string cfg;

   // Get the SDK environment variable
   {
      char buffer[32'767]; // max env-variable size
      if (GetEnvironmentVariable("VEMIPS_SDK", buffer, sizeof buffer) == 0)
      {
         fprintf(stderr, "Failed to get value of environment variable VEMIPS_SDK. Please confirm that it is configured.\n");
         exit(1);
      }
      MIPSSDK = buffer;
   }

   std::string incxx = MIPSSDK + "incxx";
   std::string inc = MIPSSDK + "inc";

   if (configuration == "Debug")
   {
      cfg +=
         "mipsc -ffunction-sections -fdata-sections -mcompact-branches=always -fasynchronous-unwind-tables -funwind-tables "
         "-fexceptions -fcxx-exceptions -mips32r6 -O0 -glldb ";
   }
   else if (configuration == "Release")
   {
      cfg +=
         "mipsc -ffunction-sections -fdata-sections -mno-check-zero-division -mcompact-branches=always -fasynchronous-unwind-tables -funwind-tables "
         "-fexceptions -fcxx-exceptions -mips32r6 -O3 -Os ";
   }

   cfg += std::string("-I") + incxx + " ";
   cfg += std::string("-I") + inc + " ";
   cfg += "-c ";

   object_file = intermediate + "\\";
   object_file += filename + ".o";

   cfg += filepath + " ";
   cfg += "-o ";
   cfg += object_file;
   return cfg;
}

std::string get_link_args(const std::vector<std::string> &objectfiles, const std::string &filepath, const std::string &configuration)
{
   const std::string filename = get_file_name(filepath);
   const std::string filedir = get_dir(filepath);

   // lld --discard-all -znorelro --gc-sections --icf=all --strip-all --eh-frame-hdr -Llib -lc -lc++ -lc++abi -lllvmunwind

   // TODO rework this.

   std::string cfg;

   std::string libs = MIPSSDK + "lib";

   if (configuration == "Debug")
   {
      cfg += "mipsld --discard-none -znorelro --eh-frame-hdr -lc -lc++ -lc++abi -lllvmunwind ";
   }
   else if (configuration == "Release")
   {
      cfg += "mipsld --discard-all -znorelro --gc-sections --icf=all --strip-all --eh-frame-hdr -lc -lc++ -lc++abi -lllvmunwind ";
   }
   cfg += std::string("-L") + libs + " ";

   for (const std::string &objectfile : objectfiles)
   {
      cfg += objectfile;
      cfg += " ";
   }
   
   cfg += "-o ";
   cfg += filepath;
   return cfg;
}

int main(int argc, const char **argv)
{
   if (argc != 6)
   {
      fprintf(stderr, "Incorrect number of arguments\n");
      return 1;
   }

   const char *ProjFile = argv[1];
   const char *Configuration = argv[2];
   std::string Platform = argv[3];
   std::string BuildType = argv[4];
   std::string Outfile = argv[5];

   std::string IntermediateLocation = "obj";

   std::string OutfileStr = Outfile;

   const std::string base_dir = get_dir(ProjFile);

   _chdir(base_dir.c_str());

   if (OutfileStr.length() < 3 || (OutfileStr[2] != '/' && OutfileStr[2] != '\\'))
   {
      OutfileStr = base_dir + "/" + OutfileStr;
   }

   buildcarbide::configuration config;
   config.target = Platform;
   config.config = Configuration;
   config.base_path = buildcarbide::fileutils::fixup(base_dir);
   config.intermediate_dir = buildcarbide::fileutils::fixup(base_dir + "\\" + IntermediateLocation);

   auto *proj = buildcarbide::get_project(ProjFile);
   proj = proj;
   buildcarbide::build builder(config, *proj);
   delete proj;

   const auto IsExcluded = [&](tinyxml2::XMLElement *element) -> bool
   {
      if (!element)
      {
         return false;
      }

      auto true_false = element->FirstChild();
      if (!true_false)
      {
         return false;
      }

      auto true_false_text = true_false->ToText();
      if (!true_false_text)
      {
         return false;
      }

      if (strcmp(true_false_text->Value(), "false") == 0)
      {
         return false;
      }

      const char *condition_text = element->Attribute("Condition");
      if (!condition_text)
      {
         return false;
      }

      // parse the condition format, which looks like this:
      // $(Configuration)|$(Platform)'=='Release|Win32'
      std::string format;
      std::string values;

      // this is not a stable solution at all.
      if (*condition_text == '\'' || *condition_text == '\"')
      {
         ++condition_text;
      }

      while (*condition_text != '\'' && *condition_text != '\"')
      {
         format += *condition_text++;
      }

      ++condition_text;

      while (*condition_text == '=')
      {
         ++condition_text;
      }

      if (*condition_text == '\'' || *condition_text == '\"')
      {
         ++condition_text;
      }

      while (*condition_text != '\'' && *condition_text != '\"')
      {
         values += *condition_text++;
      }

      std::string test = format;
      replace(test, "$(Configuration)", Configuration);
      replace(test, "$(Platform)", Platform);

      return values == test;
   };

   try
   {
      std::vector<std::string> ClCompile;
      std::vector<std::string> ClInclude;

      tinyxml2::XMLDocument doc;
      doc.LoadFile(ProjFile);
      if (doc.Error())
      {
         throw 0;
      }

      const auto project = doc.FirstChildElement("Project");
      if (!project)
      {
         throw 0;
      }

      auto item_group = project->FirstChildElement("ItemGroup");
      if (!item_group)
      {
         throw 0;
      }
      while (item_group)
      {
         // Iterate over all item groups.
         // source files
         {
            auto compile_element = item_group->FirstChildElement("ClCompile");
            for (; compile_element; compile_element = compile_element->NextSiblingElement("ClCompile"))
            {
               auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
               if (IsExcluded(exclusion_element))
               {
                  continue;
               }

               const char *file_name = compile_element->Attribute("Include");
               if (!file_name)
               {
                  continue;
               }
               ClCompile.push_back(base_dir + "\\" + file_name);
            }
         }

         // header files
         {
            auto compile_element = item_group->FirstChildElement("ClInclude");
            for (; compile_element; compile_element = compile_element->NextSiblingElement("ClInclude"))
            {
               auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
               if (IsExcluded(exclusion_element))
               {
                  continue;
               }

               const char *file_name = compile_element->Attribute("Include");
               if (!file_name)
               {
                  continue;
               }
               ClInclude.push_back(base_dir + "\\" + file_name);
            }
         }

         item_group = item_group->NextSiblingElement("ItemGroup");
      }

      struct BuildFile
      {
         std::string filename;
         uint64_t modtime;
      };

      std::vector<BuildFile> FilesToBuild;

      bool DoClean = false;

      const uint64_t OutFileBuildTime = get_file_time(OutfileStr, 0);

      // If we are rebuilding, they are all built.
      // In this situation, we also clean.
      if (BuildType == "rebuild")
      {
         for (const std::string &file : ClCompile)
         {
            FilesToBuild.push_back({file, uint64_t(-1)});
         }
         DoClean = true;
      }
      else if (BuildType == "clean")
      {
         DoClean = true;
      }
      else if (BuildType == "build")
      {
         // TODO we need to do dependency checks. However, for now, we will just check if the headers have changed and flag everything as dirty.
         bool RebuildAll = false;
         for (const std::string &file : ClInclude)
         {
            if (get_file_time(file) > OutFileBuildTime)
            {
               RebuildAll = true;
               break;
            }
         }

         for (const std::string &file : ClCompile)
         {
            FilesToBuild.push_back({ file, RebuildAll ? uint64_t(-1) : get_file_time(file) });
         }
      }

      if (DoClean)
      {
         SHFILEOPSTRUCT fileStruct;
         memset(&fileStruct, 0, sizeof(fileStruct));
         fileStruct.wFunc = FO_DELETE;
         std::vector<char> InterLoc(IntermediateLocation.length() + 2, '\0');
         memcpy(InterLoc.data(), IntermediateLocation.data(), IntermediateLocation.length());
         fileStruct.pFrom = InterLoc.data();
         fileStruct.fFlags = FOF_NO_UI;

         SHFileOperation(&fileStruct);

         std::vector<char> OutLoc(OutfileStr.length() + 2, '\0');
         memcpy(OutLoc.data(), OutfileStr.data(), OutfileStr.length());
         fileStruct.pFrom = OutLoc.data();

         SHFileOperation(&fileStruct);
      }

      if (FilesToBuild.empty())
      {
         // just return
         return 0;
      }

      std::vector<std::string> object_files;

      // Do the build.
      for (const auto &build_file : FilesToBuild)
      {
         if (build_file.modtime <= OutFileBuildTime)
         {
            continue;
         }

         std::string object_file;
         const std::string compile_args = get_compile_args(build_file.filename, Configuration, IntermediateLocation, object_file);
         object_files.push_back(object_file);

         SHCreateDirectoryEx( nullptr, get_dir(base_dir + "/" + object_file).c_str(), nullptr );

         // Do the build.
         printf("%s\n", compile_args.c_str());
         int res = system(compile_args.c_str());
         if (res != 0)
         {
            fprintf(stderr, "Error compiling \'%s\'\n", build_file.filename.c_str());
            return 1;
         }
      }

      if (object_files.empty())
      {
         return 0;
      }

      SHCreateDirectoryEx( nullptr, get_dir(OutfileStr).c_str(), nullptr );

      const std::string link_args = get_link_args(object_files, OutfileStr, Configuration);
      printf("%s\n", link_args.c_str());
      int res = system(link_args.c_str());
      if (res != 0)
      {
         fprintf(stderr, "Error linking\n");
         return 1;
      }

   }
   catch (...)
   {
      fprintf(stderr, "Failed to parser project file \'%s\'\n", ProjFile);
      return 1;
   }

   return 0;
}
