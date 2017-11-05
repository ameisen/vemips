#pragma once

#if defined(_WIN32)
#  include "win32/fileutils_win32.hpp"
#else
#  error "Unknown Platform"
#endif

namespace buildcarbide::fileutils
{
   template <typename... Ts>
   static std::string build_path(Ts... args)
   {
      std::string out;

      const std::array<std::string, sizeof...(args)> elements = { {args...} };
   
      size_t i = 0;
      for (const std::string &str : elements)
      {
         if (str.length())
         {
            out += str;
            if ((i != elements.size() - 1) && str.back() != '\\' && str.back() != '/')
            {
               out += "/";
            }
         }
         ++i;
      }

      return out;
   }

   static std::string get_base_path(const std::string & __restrict path)
   {
      std::string out = path;

      while (out.size() && (out.back() == '\\' || out.back() == '/'))
      {
         out.pop_back();
      }

      // Now, pop until we hit another directory separator.

      while (out.size() && (out.back() != '\\' && out.back() != '/'))
      {
         out.pop_back();
      }

      return out;
   }

   static std::string fixup(const std::string & __restrict path)
   {
      auto replace = [](std::string& str, const std::string & __restrict from, const std::string & __restrict to) -> bool {
         size_t start_pos = str.find(from);
         if (start_pos == std::string::npos)
            return false;
         str.replace(start_pos, from.length(), to);
         return true;
      };
      std::string out = path;
      while (replace(out, "\\", "/")) {}
      while (replace(out, "//", "/")) {}
      return out;
   }

   static std::string strip_extension(const std::string & __restrict filename)
   {
      // Is there a filename?
      if (filename.size() == 0)
      {
         return "";
      }
      size_t offset = filename.size() - 1;
      for (size_t i = offset; i >= 1; --i)
      {
         if (filename[i] == '/' || filename[i] == '\\')
         {
            break;
         }
         if (filename[i] == '.')
         {
            return filename.substr(0, i);
         }
      }
      return filename;
   }
}
