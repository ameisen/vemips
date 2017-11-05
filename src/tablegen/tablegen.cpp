#include "instructions/instructions.hpp"
#include <cstdio>
#include <string>

namespace mips::instructions
{
   extern void finish_map_build();
}

namespace mips::instructions
{
   extern StaticInitVars * __restrict StaticInitVarsPtr;
}

void PrintIndent (size_t indent)
{
   for (size_t i = 0; i < indent; ++i)
   {
      fwrite("  ", 2, 1, stdout);
   }
}

#define indented_print(...) do { PrintIndent(indent); printf(__VA_ARGS__); } while(false)

std::string BuildProcedureLink(const mips::instructions::InstructionInfo &info)
{
   // bool PROC_ ## InsInstruction ::SubExecute
   std::string name = info.Name;

   const char *template_str = "";
   switch (info.Type)
   {
      case 'v':
         template_str = "<void>"; break;
      case 'f':
         template_str = "<float>"; break;
      case 'd':
         template_str = "<double>"; break;
      case 'w':
         template_str = "<int32>"; break;
      case 'l':
         template_str = "<int64>"; break;
   }
   char buffer[512];
   sprintf(buffer, "mips::instructions::%s_NS::Execute%s", info.Name, template_str);

   std::string proc = buffer;
   _itoa(info.CoprocessorIdx, buffer, 10);
   std::string coprocidx = buffer;
   std::string control = info.ControlInstruction ? "true" : "false";

   _ultoa(info.OpFlags, buffer, 16);
   std::string opflags = std::string("0x") + buffer;

   const char type_str[2] = {info.Type, '\0'};
   return std::string("( \"") + name + "\", " + control + ", " + coprocidx + ", &" + proc + ", " + opflags + ", \'" + type_str + "\' )";
}

std::string BuildProcedureCall(const mips::instructions::InstructionInfo &info)
{
   std::string name = info.Name;

   const char *template_str = "";
   switch (info.Type)
   {
      case 'v':
         template_str = "<void>"; break;
      case 'f':
         template_str = "<float>"; break;
      case 'd':
         template_str = "<double>"; break;
      case 'w':
         template_str = "<int32>"; break;
      case 'l':
         template_str = "<int64>"; break;
   }
   char buffer[512];
   sprintf(buffer, "mips::instructions::%s_NS::Execute%s", info.Name, template_str);
   return buffer;
}

std::string GetStaticProcName(const mips::instructions::InstructionInfo &info)
{
   std::string name = std::string("StaticProc_") + info.Name;
   switch (info.Type)
   {
      case 'v':
         name += "_v"; break;
      case 'f':
         name += "_f"; break;
      case 'd':
         name += "_d"; break;
      case 'w':
         name += "_w"; break;
      case 'l':
         name += "_l"; break;
   }
   return name;
}

//TODO:
// Traverse the map a first time and build up just a list of procinfos. We will then have the file statically allocate them,
// so we can reference... references instead of values.
// We also need to figure out how to actually call the procedures. They are only described in source files and the symbols won't export.
// Will likely have to add simply-exported names that are pointers to the named procedures which we can extern.

std::vector<const mips::instructions::InstructionInfo *> ProcInfos;
void populate_proc_infos(const mips::instructions::MapOrInfo *map)
{
   if (!map->IsMap)
   {
      ProcInfos.push_back(&map->Info);
      return;
   }

   if (map->Map.size() == 1 && !map->Default)
   {
      ProcInfos.push_back(&map->Map.begin()->second->Info);
      return;
   }

   for (auto &sub_map : map->Map)
   {
      populate_proc_infos(sub_map.second);
   }
   if (map->Default)
   {
      populate_proc_infos(map->Default);
   }
}

void traverse_map(const mips::instructions::MapOrInfo *map, size_t indent)
{
   if (!map->IsMap)
   {
      indented_print("return &%s;\n", GetStaticProcName(map->Info).c_str());
      return;
   }

   if (map->Map.size() == 1 && !map->Default)
   {
      indented_print("if ((i & 0x%08X) == 0x%08X)\n", map->Mask, map->Map.begin()->first);
      indent++;
      indented_print("return &%s;\n", GetStaticProcName(map->Map.begin()->second->Info).c_str());
      --indent;
      return;
   }

   indented_print("switch(i & 0x%08X) {\n", map->Mask);
   ++indent;
   for (auto &sub_map : map->Map)
   {
      indented_print("case 0x%08X:\n", sub_map.first);
      traverse_map(sub_map.second, indent + 1);
   }
   if (map->Default)
   {
      indented_print("default:\n");
      traverse_map(map->Default, indent + 1);
   }
   --indent;
   indented_print("}\n");
}

void traverse_map_exec(const mips::instructions::MapOrInfo *map, size_t indent)
{
   if (!map->IsMap)
   {
      indented_print("%s(i,p); return true;\n", BuildProcedureCall(map->Info).c_str());
      return;
   }

   if (map->Map.size() == 1 && !map->Default)
   {
      indented_print("if ((i & 0x%08X) == 0x%08X)\n", map->Mask, map->Map.begin()->first);
      indent++;
      indented_print("%s(i,p); return true;\n", BuildProcedureCall(map->Map.begin()->second->Info).c_str());
      --indent;
      return;
   }

   indented_print("switch(i & 0x%08X) {\n", map->Mask);
   ++indent;
   for (auto &sub_map : map->Map)
   {
      indented_print("case 0x%08X:\n", sub_map.first);
      traverse_map_exec(sub_map.second, indent + 1);
   }
   if (map->Default)
   {
      indented_print("default:\n");
      traverse_map_exec(map->Default, indent + 1);
   }
   --indent;
   indented_print("}\n");
}


int main()
{
   mips::instructions::finish_map_build();

   populate_proc_infos(&mips::instructions::StaticInitVarsPtr->g_LookupMap);

   size_t indent = 0;
   indented_print("/* WARNING: THIS IS AN AUTOGENERATED FILE */\n\n");

   indented_print("#include \"pch.hpp\"\n");
   indented_print("#include \"mips/mips_common.hpp\"\n");
   indented_print("#include \"mips/instructions/instructions_common.hpp\"\n");
   indented_print("#include \"mips/processor/processor.hpp\"\n");
   indented_print("#include \"mips/coprocessor/coprocessor.hpp\"\n");

   indented_print("\nnamespace mips::instructions {\n");
   for (auto &Procs : ProcInfos)
   {
      ++indent;

      const char *template_str = "extern ";
      if (Procs->Type != 'n')
         template_str = "template <typename format_t> ";

      indented_print(
         "namespace %s_NS { %suint64 Execute(instruction_t, mips::processor & __restrict); }\n",
         Procs->Name,
         template_str
      );
      --indent;
   }
   indented_print("}\n");
   for (auto &Procs : ProcInfos)
   {
      indented_print("static const mips::instructions::InstructionInfo %s %s;\n", GetStaticProcName(*Procs).c_str(), BuildProcedureLink(*Procs).c_str());
   }
   //indented_print("\nstatic const mips::instructions::InstructionInfo * __restrict LookupTable[] = {\n");
   //++indent;
   //for (auto &Procs : ProcInfos)
   //{
   //   indented_print("&%s,\n", GetStaticProcName(*Procs).c_str());
   //}
   //--indent;
   //indented_print("};\n");
   indented_print("const mips::instructions::InstructionInfo * __restrict get_instruction (instruction_t i)\n{\n");
   traverse_map(&mips::instructions::StaticInitVarsPtr->g_LookupMap, indent + 1);
   indent++;
   indented_print("return nullptr;\n");
   indent--;
   indented_print("}\n\n");

   indented_print("bool execute_instruction (instruction_t i, mips::processor & __restrict p)\n{\n");
   traverse_map_exec(&mips::instructions::StaticInitVarsPtr->g_LookupMap, indent + 1);
   indent++;
   indented_print("return false;\n");
   indent--;
   indented_print("}\n");
}
