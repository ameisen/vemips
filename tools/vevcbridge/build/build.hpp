#pragma once

#include "../project/project.hpp"
#include "../fileutils/fileutils.hpp"
#include "../configuration.hpp"

#include <unordered_set>
#include <string>

namespace buildcarbide
{
   class build
   {
      struct build_element
      {
         project::element proj_element;
         std::unordered_set<std::string> includes;
         fileutils::modtime_t modtime;
      };

      std::vector<build_element> m_build_elements;

   public:
      build(const configuration & __restrict configuration, const project & __restrict project);
      ~build();
   };
}
