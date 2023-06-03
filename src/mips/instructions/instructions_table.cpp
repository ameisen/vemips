/* WARNING: THIS IS AN AUTO-GENERATED FILE ('D:\Projects\vemips\src\tablegen\writers.cpp', May 29 2023 13:17:49) */
// ReSharper disable CppClangTidyClangDiagnosticUndefinedFuncTemplate CppClangTidyClangDiagnosticUnreachableCodeReturn IdentifierTypo CommentTypo CppUnreachableCode
#include "pch.hpp"

#define VEMIPS_ITABLE_EXPORT /* __declspec(dllexport) */ extern

#include "mips/instructions/instructions_table.hpp"
#include "mips/mips_common.hpp"
#include "mips/instructions/instructions_common.hpp"
#include "mips/processor/processor.hpp"

namespace mips::instructions {
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ABS_f ( "COP1_ABS", 1, &COP1_ABS_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ABS_d ( "COP1_ABS", 1, &COP1_ABS_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ADD_f ( "COP1_ADD", 1, &COP1_ADD_NS::Execute<float>, 0x00800077, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ADD_d ( "COP1_ADD", 1, &COP1_ADD_NS::Execute<double>, 0x00800077, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_BC1EQZ_v ( "COP1_BC1EQZ", 1, &COP1_BC1EQZ_NS::Execute<void>, 0x00972000, { .control = true }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_BC1NEZ_v ( "COP1_BC1NEZ", 1, &COP1_BC1NEZ_NS::Execute<void>, 0x00972000, { .control = true }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_L_f ( "COP1_CEIL_L", 1, &COP1_CEIL_L_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_L_d ( "COP1_CEIL_L", 1, &COP1_CEIL_L_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_W_f ( "COP1_CEIL_W", 1, &COP1_CEIL_W_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_W_d ( "COP1_CEIL_W", 1, &COP1_CEIL_W_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CFC1_v ( "COP1_CFC1", 1, &COP1_CFC1_NS::Execute<void>, 0x0081207F, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CLASS_f ( "COP1_CLASS", 1, &COP1_CLASS_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CLASS_d ( "COP1_CLASS", 1, &COP1_CLASS_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CMP_condn_d_v ( "COP1_CMP_condn_d", 1, &COP1_CMP_condn_d_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CMP_condn_s_v ( "COP1_CMP_condn_s", 1, &COP1_CMP_condn_s_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CTC1_v ( "COP1_CTC1", 1, &COP1_CTC1_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_f ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<float>, 0x00800047, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_d ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<double>, 0x00800047, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_w ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<int32>, 0x00800047, { .control = false }, instruction_type::word_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_l ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<int64>, 0x00800047, { .control = false }, instruction_type::long_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_L_f ( "COP1_CVT_L", 1, &COP1_CVT_L_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_L_d ( "COP1_CVT_L", 1, &COP1_CVT_L_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_f ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<float>, 0x00800047, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_d ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<double>, 0x00800047, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_w ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<int32>, 0x00800047, { .control = false }, instruction_type::word_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_l ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<int64>, 0x00800047, { .control = false }, instruction_type::long_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_W_f ( "COP1_CVT_W", 1, &COP1_CVT_W_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_W_d ( "COP1_CVT_W", 1, &COP1_CVT_W_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_DIV_f ( "COP1_DIV", 1, &COP1_DIV_NS::Execute<float>, 0x0080007F, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_DIV_d ( "COP1_DIV", 1, &COP1_DIV_NS::Execute<double>, 0x0080007F, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_L_f ( "COP1_FLOOR_L", 1, &COP1_FLOOR_L_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_L_d ( "COP1_FLOOR_L", 1, &COP1_FLOOR_L_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_W_f ( "COP1_FLOOR_W", 1, &COP1_FLOOR_W_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_W_d ( "COP1_FLOOR_W", 1, &COP1_FLOOR_W_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_LDC1_v ( "COP1_LDC1", 1, &COP1_LDC1_NS::Execute<void>, 0x00A12000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_LWC1_v ( "COP1_LWC1", 1, &COP1_LWC1_NS::Execute<void>, 0x00A12000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MADDF_f ( "COP1_MADDF", 1, &COP1_MADDF_NS::Execute<float>, 0x00800077, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MADDF_d ( "COP1_MADDF", 1, &COP1_MADDF_NS::Execute<double>, 0x00800077, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAX_f ( "COP1_MAX", 1, &COP1_MAX_NS::Execute<float>, 0x00812007, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAX_d ( "COP1_MAX", 1, &COP1_MAX_NS::Execute<double>, 0x00812007, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAXA_f ( "COP1_MAXA", 1, &COP1_MAXA_NS::Execute<float>, 0x00812007, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAXA_d ( "COP1_MAXA", 1, &COP1_MAXA_NS::Execute<double>, 0x00812007, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MFC1_v ( "COP1_MFC1", 1, &COP1_MFC1_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MFHC1_v ( "COP1_MFHC1", 1, &COP1_MFHC1_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MIN_f ( "COP1_MIN", 1, &COP1_MIN_NS::Execute<float>, 0x00812007, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MIN_d ( "COP1_MIN", 1, &COP1_MIN_NS::Execute<double>, 0x00812007, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MINA_f ( "COP1_MINA", 1, &COP1_MINA_NS::Execute<float>, 0x00812007, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MINA_d ( "COP1_MINA", 1, &COP1_MINA_NS::Execute<double>, 0x00812007, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MOV_f ( "COP1_MOV", 1, &COP1_MOV_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MOV_d ( "COP1_MOV", 1, &COP1_MOV_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MSUBF_f ( "COP1_MSUBF", 1, &COP1_MSUBF_NS::Execute<float>, 0x00800077, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MSUBF_d ( "COP1_MSUBF", 1, &COP1_MSUBF_NS::Execute<double>, 0x00800077, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MTC1_v ( "COP1_MTC1", 1, &COP1_MTC1_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MTHC1_v ( "COP1_MTHC1", 1, &COP1_MTHC1_NS::Execute<void>, 0x00812000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MUL_f ( "COP1_MUL", 1, &COP1_MUL_NS::Execute<float>, 0x00800077, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MUL_d ( "COP1_MUL", 1, &COP1_MUL_NS::Execute<double>, 0x00800077, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_NEG_f ( "COP1_NEG", 1, &COP1_NEG_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_NEG_d ( "COP1_NEG", 1, &COP1_NEG_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RECIP_f ( "COP1_RECIP", 1, &COP1_RECIP_NS::Execute<float>, 0x0080007F, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RECIP_d ( "COP1_RECIP", 1, &COP1_RECIP_NS::Execute<double>, 0x0080007F, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RINT_f ( "COP1_RINT", 1, &COP1_RINT_NS::Execute<float>, 0x00800077, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RINT_d ( "COP1_RINT", 1, &COP1_RINT_NS::Execute<double>, 0x00800077, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_L_f ( "COP1_ROUND_L", 1, &COP1_ROUND_L_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_L_d ( "COP1_ROUND_L", 1, &COP1_ROUND_L_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_W_f ( "COP1_ROUND_W", 1, &COP1_ROUND_W_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_W_d ( "COP1_ROUND_W", 1, &COP1_ROUND_W_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RSQRT_f ( "COP1_RSQRT", 1, &COP1_RSQRT_NS::Execute<float>, 0x0080007F, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RSQRT_d ( "COP1_RSQRT", 1, &COP1_RSQRT_NS::Execute<double>, 0x0080007F, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SDC1_v ( "COP1_SDC1", 1, &COP1_SDC1_NS::Execute<void>, 0x00C12000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SEL_f ( "COP1_SEL", 1, &COP1_SEL_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SEL_d ( "COP1_SEL", 1, &COP1_SEL_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELEQZ_f ( "COP1_SELEQZ", 1, &COP1_SELEQZ_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELEQZ_d ( "COP1_SELEQZ", 1, &COP1_SELEQZ_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELNEZ_f ( "COP1_SELNEZ", 1, &COP1_SELNEZ_NS::Execute<float>, 0x00812000, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELNEZ_d ( "COP1_SELNEZ", 1, &COP1_SELNEZ_NS::Execute<double>, 0x00812000, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SQRT_f ( "COP1_SQRT", 1, &COP1_SQRT_NS::Execute<float>, 0x00800047, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SQRT_d ( "COP1_SQRT", 1, &COP1_SQRT_NS::Execute<double>, 0x00800047, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SUB_f ( "COP1_SUB", 1, &COP1_SUB_NS::Execute<float>, 0x00800077, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SUB_d ( "COP1_SUB", 1, &COP1_SUB_NS::Execute<double>, 0x00800077, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SWC1_v ( "COP1_SWC1", 1, &COP1_SWC1_NS::Execute<void>, 0x00C12000, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_L_f ( "COP1_TRUNC_L", 1, &COP1_TRUNC_L_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_L_d ( "COP1_TRUNC_L", 1, &COP1_TRUNC_L_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_W_f ( "COP1_TRUNC_W", 1, &COP1_TRUNC_W_NS::Execute<float>, 0x008000C7, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_W_d ( "COP1_TRUNC_W", 1, &COP1_TRUNC_W_NS::Execute<double>, 0x008000C7, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADD ( "PROC_ADD", 1, &PROC_ADD_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDIU ( "PROC_ADDIU", 1, &PROC_ADDIU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDIUPC ( "PROC_ADDIUPC", 1, &PROC_ADDIUPC_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDU ( "PROC_ADDU", 1, &PROC_ADDU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ALIGN ( "PROC_ALIGN", 1, &PROC_ALIGN_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ALUIPC ( "PROC_ALUIPC", 1, &PROC_ALUIPC_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AND ( "PROC_AND", 1, &PROC_AND_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ANDI ( "PROC_ANDI", 1, &PROC_ANDI_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AUI ( "PROC_AUI", 1, &PROC_AUI_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AUIPC ( "PROC_AUIPC", 1, &PROC_AUIPC_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BAL ( "PROC_BAL", 1, &PROC_BAL_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BALC ( "PROC_BALC", 1, &PROC_BALC_NS::Execute, 0x000A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BC ( "PROC_BC", 1, &PROC_BC_NS::Execute, 0x000A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BEQ ( "PROC_BEQ", 1, &PROC_BEQ_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BEQZC ( "PROC_BEQZC", 1, &PROC_BEQZC_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGEZ ( "PROC_BGEZ", 1, &PROC_BGEZ_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZ ( "PROC_BGTZ", 1, &PROC_BGTZ_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZALC ( "PROC_BGTZALC", 1, &PROC_BGTZALC_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZC ( "PROC_BGTZC", 1, &PROC_BGTZC_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BITSWAP ( "PROC_BITSWAP", 1, &PROC_BITSWAP_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZ ( "PROC_BLEZ", 1, &PROC_BLEZ_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZALC ( "PROC_BLEZALC", 1, &PROC_BLEZALC_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZC ( "PROC_BLEZC", 1, &PROC_BLEZC_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLTZ ( "PROC_BLTZ", 1, &PROC_BLTZ_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BNE ( "PROC_BNE", 1, &PROC_BNE_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BNEZC ( "PROC_BNEZC", 1, &PROC_BNEZC_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BREAK ( "PROC_BREAK", 1, &PROC_BREAK_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CACHE ( "PROC_CACHE", 1, &PROC_CACHE_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CLO ( "PROC_CLO", 1, &PROC_CLO_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CLZ ( "PROC_CLZ", 1, &PROC_CLZ_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_DIV ( "PROC_DIV", 1, &PROC_DIV_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_DIVU ( "PROC_DIVU", 1, &PROC_DIVU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_EHB ( "PROC_EHB", 1, &PROC_EHB_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_EXT ( "PROC_EXT", 1, &PROC_EXT_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_INS ( "PROC_INS", 1, &PROC_INS_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_J ( "PROC_J", 1, &PROC_J_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JAL ( "PROC_JAL", 1, &PROC_JAL_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JALR ( "PROC_JALR", 1, &PROC_JALR_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JIALC ( "PROC_JIALC", 1, &PROC_JIALC_NS::Execute, 0x000A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JIC ( "PROC_JIC", 1, &PROC_JIC_NS::Execute, 0x000A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JR ( "PROC_JR", 1, &PROC_JR_NS::Execute, 0x00160000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LB ( "PROC_LB", 1, &PROC_LB_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBE ( "PROC_LBE", 1, &PROC_LBE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBU ( "PROC_LBU", 1, &PROC_LBU_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBUE ( "PROC_LBUE", 1, &PROC_LBUE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LH ( "PROC_LH", 1, &PROC_LH_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHE ( "PROC_LHE", 1, &PROC_LHE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHU ( "PROC_LHU", 1, &PROC_LHU_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHUE ( "PROC_LHUE", 1, &PROC_LHUE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LL ( "PROC_LL", 1, &PROC_LL_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLE ( "PROC_LLE", 1, &PROC_LLE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLWP ( "PROC_LLWP", 1, &PROC_LLWP_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLWPE ( "PROC_LLWPE", 1, &PROC_LLWPE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LSA ( "PROC_LSA", 1, &PROC_LSA_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LW ( "PROC_LW", 1, &PROC_LW_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LWE ( "PROC_LWE", 1, &PROC_LWE_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LWPC ( "PROC_LWPC", 1, &PROC_LWPC_NS::Execute, 0x00200000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MOD ( "PROC_MOD", 1, &PROC_MOD_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MODU ( "PROC_MODU", 1, &PROC_MODU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MOVE ( "PROC_MOVE", 1, &PROC_MOVE_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUH ( "PROC_MUH", 1, &PROC_MUH_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUHU ( "PROC_MUHU", 1, &PROC_MUHU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUL ( "PROC_MUL", 1, &PROC_MUL_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MULU ( "PROC_MULU", 1, &PROC_MULU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NAL ( "PROC_NAL", 1, &PROC_NAL_NS::Execute, 0x00120000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NOP ( "PROC_NOP", 1, &PROC_NOP_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NOR ( "PROC_NOR", 1, &PROC_NOR_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_OR ( "PROC_OR", 1, &PROC_OR_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ORI ( "PROC_ORI", 1, &PROC_ORI_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PAUSE ( "PROC_PAUSE", 1, &PROC_PAUSE_NS::Execute, 0x00020000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP06 ( "PROC_POP06", 1, &PROC_POP06_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP07 ( "PROC_POP07", 1, &PROC_POP07_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP10 ( "PROC_POP10", 1, &PROC_POP10_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP26 ( "PROC_POP26", 1, &PROC_POP26_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP27 ( "PROC_POP27", 1, &PROC_POP27_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP30 ( "PROC_POP30", 1, &PROC_POP30_NS::Execute, 0x001A0000, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PREF ( "PROC_PREF", 1, &PROC_PREF_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PREFE ( "PROC_PREFE", 1, &PROC_PREFE_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_RDHWR ( "PROC_RDHWR", 1, &PROC_RDHWR_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ROTR ( "PROC_ROTR", 1, &PROC_ROTR_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ROTRV ( "PROC_ROTRV", 1, &PROC_ROTRV_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SB ( "PROC_SB", 1, &PROC_SB_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SBE ( "PROC_SBE", 1, &PROC_SBE_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SC ( "PROC_SC", 1, &PROC_SC_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCE ( "PROC_SCE", 1, &PROC_SCE_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCWP ( "PROC_SCWP", 1, &PROC_SCWP_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCWPE ( "PROC_SCWPE", 1, &PROC_SCWPE_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SDBBP ( "PROC_SDBBP", 1, &PROC_SDBBP_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SEB ( "PROC_SEB", 1, &PROC_SEB_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SEH ( "PROC_SEH", 1, &PROC_SEH_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SELEQZ ( "PROC_SELEQZ", 1, &PROC_SELEQZ_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SELNEZ ( "PROC_SELNEZ", 1, &PROC_SELNEZ_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SH ( "PROC_SH", 1, &PROC_SH_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SHE ( "PROC_SHE", 1, &PROC_SHE_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SIGRIE ( "PROC_SIGRIE", 1, &PROC_SIGRIE_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLL ( "PROC_SLL", 1, &PROC_SLL_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLLV ( "PROC_SLLV", 1, &PROC_SLLV_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLT ( "PROC_SLT", 1, &PROC_SLT_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTI ( "PROC_SLTI", 1, &PROC_SLTI_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTIU ( "PROC_SLTIU", 1, &PROC_SLTIU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTU ( "PROC_SLTU", 1, &PROC_SLTU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRA ( "PROC_SRA", 1, &PROC_SRA_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRAV ( "PROC_SRAV", 1, &PROC_SRAV_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRL ( "PROC_SRL", 1, &PROC_SRL_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRLV ( "PROC_SRLV", 1, &PROC_SRLV_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SSNOP ( "PROC_SSNOP", 1, &PROC_SSNOP_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SUB ( "PROC_SUB", 1, &PROC_SUB_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SUBU ( "PROC_SUBU", 1, &PROC_SUBU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SW ( "PROC_SW", 1, &PROC_SW_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SWE ( "PROC_SWE", 1, &PROC_SWE_NS::Execute, 0x00400000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYNC ( "PROC_SYNC", 1, &PROC_SYNC_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYNCI ( "PROC_SYNCI", 1, &PROC_SYNCI_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYSCALL ( "PROC_SYSCALL", 1, &PROC_SYSCALL_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TEQ ( "PROC_TEQ", 1, &PROC_TEQ_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TGE ( "PROC_TGE", 1, &PROC_TGE_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TGEU ( "PROC_TGEU", 1, &PROC_TGEU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TLT ( "PROC_TLT", 1, &PROC_TLT_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TLTU ( "PROC_TLTU", 1, &PROC_TLTU_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TNE ( "PROC_TNE", 1, &PROC_TNE_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_WSBH ( "PROC_WSBH", 1, &PROC_WSBH_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_XOR ( "PROC_XOR", 1, &PROC_XOR_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_XORI ( "PROC_XORI", 1, &PROC_XORI_NS::Execute, 0x00000000, { .control = false }, instruction_type::normal);
}

namespace mips::instructions {
	const InstructionInfo * get_instruction (instruction_t i) {
		switch(i) {
			case 0x00000040:
				return &StaticProc_PROC_SSNOP;
			case 0x00000140:
				return &StaticProc_PROC_PAUSE;
			case 0x00000000:
				return &StaticProc_PROC_NOP;
			case 0x000000C0:
				return &StaticProc_PROC_EHB;
			default:
				switch(i & 0xFC000000) {
					case 0x38000000:
						return &StaticProc_PROC_XORI;
					case 0xAC000000:
						return &StaticProc_PROC_SW;
					case 0x2C000000:
						return &StaticProc_PROC_SLTIU;
					case 0x28000000:
						return &StaticProc_PROC_SLTI;
					case 0xA4000000:
						return &StaticProc_PROC_SH;
					case 0xA0000000:
						return &StaticProc_PROC_SB;
					case 0x34000000:
						return &StaticProc_PROC_ORI;
					case 0x8C000000:
						return &StaticProc_PROC_LW;
					case 0x94000000:
						return &StaticProc_PROC_LHU;
					case 0x84000000:
						return &StaticProc_PROC_LH;
					case 0x90000000:
						return &StaticProc_PROC_LBU;
					case 0x80000000:
						return &StaticProc_PROC_LB;
					case 0x0C000000:
						return &StaticProc_PROC_JAL;
					case 0x08000000:
						return &StaticProc_PROC_J;
					case 0x14000000:
						return &StaticProc_PROC_BNE;
					case 0x60000000:
						return &StaticProc_PROC_POP30;
					case 0x20000000:
						return &StaticProc_PROC_POP10;
					case 0x10000000:
						return &StaticProc_PROC_BEQ;
					case 0xC8000000:
						return &StaticProc_PROC_BC;
					case 0xE8000000:
						return &StaticProc_PROC_BALC;
					case 0x3C000000:
						return &StaticProc_PROC_AUI;
					case 0x30000000:
						return &StaticProc_PROC_ANDI;
					case 0xE4000000:
						return &StaticProc_COP1_SWC1_v;
					case 0xF4000000:
						return &StaticProc_COP1_SDC1_v;
					case 0xC4000000:
						return &StaticProc_COP1_LWC1_v;
					case 0xD4000000:
						return &StaticProc_COP1_LDC1_v;
					case 0xF8000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_JIALC;
							default:
								if ((i & 0xFC000000) == 0xF8000000)
									return &StaticProc_PROC_BNEZC;
								return nullptr;
						}
					case 0xD8000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_JIC;
							default:
								if ((i & 0xFC000000) == 0xD8000000)
									return &StaticProc_PROC_BEQZC;
								return nullptr;
						}
					case 0x5C000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_BGTZC;
							default:
								if ((i & 0xFC000000) == 0x5C000000)
									return &StaticProc_PROC_POP27;
								return nullptr;
						}
					case 0x58000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_BLEZC;
							default:
								if ((i & 0xFC000000) == 0x58000000)
									return &StaticProc_PROC_POP26;
								return nullptr;
						}
					case 0x00000000:
						switch(i & 0x0000003F) {
							case 0x0000000F:
								return &StaticProc_PROC_SYNC;
							case 0x00000009:
								switch(i & 0x001FF800) {
									case 0x00000000:
										return &StaticProc_PROC_JR;
									default:
										if ((i & 0xFC1F003F) == 0x00000009)
											return &StaticProc_PROC_JALR;
										return nullptr;
								}
							case 0x00000021:
								switch(i & 0x001F07C0) {
									case 0x00000040:
										return &StaticProc_PROC_CLO;
									default:
										if ((i & 0xFC0007FF) == 0x00000021)
											return &StaticProc_PROC_ADDU;
										return nullptr;
								}
							case 0x00000020:
								return &StaticProc_PROC_ADD;
							case 0x00000010:
								return &StaticProc_PROC_CLZ;
							case 0x00000024:
								return &StaticProc_PROC_AND;
							case 0x0000001A:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										return &StaticProc_PROC_MOD;
									case 0x00000080:
										return &StaticProc_PROC_DIV;
									default:
										return nullptr;
								}
							case 0x0000001B:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										return &StaticProc_PROC_MODU;
									case 0x00000080:
										return &StaticProc_PROC_DIVU;
									default:
										return nullptr;
								}
							case 0x00000018:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										return &StaticProc_PROC_MUH;
									case 0x00000080:
										return &StaticProc_PROC_MUL;
									default:
										return nullptr;
								}
							case 0x00000019:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										return &StaticProc_PROC_MUHU;
									case 0x00000080:
										return &StaticProc_PROC_MULU;
									default:
										return nullptr;
								}
							case 0x00000027:
								return &StaticProc_PROC_NOR;
							case 0x00000025:
								return &StaticProc_PROC_OR;
							case 0x00000002:
								switch(i & 0x03E00000) {
									case 0x00000000:
										return &StaticProc_PROC_SRL;
									case 0x00200000:
										return &StaticProc_PROC_ROTR;
									default:
										return nullptr;
								}
							case 0x00000006:
								switch(i & 0x000007C0) {
									case 0x00000000:
										return &StaticProc_PROC_SRLV;
									case 0x00000040:
										return &StaticProc_PROC_ROTRV;
									default:
										return nullptr;
								}
							case 0x00000035:
								return &StaticProc_PROC_SELEQZ;
							case 0x00000037:
								return &StaticProc_PROC_SELNEZ;
							case 0x00000000:
								return &StaticProc_PROC_SLL;
							case 0x00000004:
								return &StaticProc_PROC_SLLV;
							case 0x0000002A:
								return &StaticProc_PROC_SLT;
							case 0x0000002B:
								return &StaticProc_PROC_SLTU;
							case 0x00000003:
								return &StaticProc_PROC_SRA;
							case 0x00000007:
								return &StaticProc_PROC_SRAV;
							case 0x00000022:
								return &StaticProc_PROC_SUB;
							case 0x00000023:
								return &StaticProc_PROC_SUBU;
							case 0x00000026:
								return &StaticProc_PROC_XOR;
							case 0x00000005:
								return &StaticProc_PROC_LSA;
							case 0x0000000D:
								return &StaticProc_PROC_BREAK;
							case 0x0000000E:
								return &StaticProc_PROC_SDBBP;
							case 0x0000000C:
								return &StaticProc_PROC_SYSCALL;
							case 0x00000034:
								return &StaticProc_PROC_TEQ;
							case 0x00000030:
								return &StaticProc_PROC_TGE;
							case 0x00000031:
								return &StaticProc_PROC_TGEU;
							case 0x00000032:
								return &StaticProc_PROC_TLT;
							case 0x00000033:
								return &StaticProc_PROC_TLTU;
							case 0x00000036:
								return &StaticProc_PROC_TNE;
							default:
								return nullptr;
						}
					case 0x44000000:
						switch(i & 0x03E00000) {
							case 0x01A00000:
								return &StaticProc_COP1_BC1NEZ_v;
							case 0x01200000:
								return &StaticProc_COP1_BC1EQZ_v;
							case 0x00600000:
								return &StaticProc_COP1_MFHC1_v;
							case 0x00E00000:
								return &StaticProc_COP1_MTHC1_v;
							case 0x02000000:
								switch(i & 0x001F003F) {
									case 0x00000025:
										return &StaticProc_COP1_CVT_L_f;
									case 0x00000005:
										return &StaticProc_COP1_ABS_f;
									case 0x0000001B:
										return &StaticProc_COP1_CLASS_f;
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_f;
									case 0x00000020:
										return &StaticProc_COP1_CVT_S_f;
									case 0x00000024:
										return &StaticProc_COP1_CVT_W_f;
									case 0x00000006:
										return &StaticProc_COP1_MOV_f;
									case 0x00000007:
										return &StaticProc_COP1_NEG_f;
									case 0x00000015:
										return &StaticProc_COP1_RECIP_f;
									case 0x0000001A:
										return &StaticProc_COP1_RINT_f;
									case 0x00000016:
										return &StaticProc_COP1_RSQRT_f;
									case 0x00000004:
										return &StaticProc_COP1_SQRT_f;
									case 0x00000008:
										return &StaticProc_COP1_ROUND_L_f;
									case 0x0000000C:
										return &StaticProc_COP1_ROUND_W_f;
									case 0x0000000A:
										return &StaticProc_COP1_CEIL_L_f;
									case 0x0000000E:
										return &StaticProc_COP1_CEIL_W_f;
									case 0x0000000B:
										return &StaticProc_COP1_FLOOR_L_f;
									case 0x0000000F:
										return &StaticProc_COP1_FLOOR_W_f;
									case 0x00000009:
										return &StaticProc_COP1_TRUNC_L_f;
									case 0x0000000D:
										return &StaticProc_COP1_TRUNC_W_f;
									default:
										switch(i & 0xFFE0003F) {
											case 0x46000018:
												return &StaticProc_COP1_MADDF_f;
											case 0x46000000:
												return &StaticProc_COP1_ADD_f;
											case 0x46000003:
												return &StaticProc_COP1_DIV_f;
											case 0x46000019:
												return &StaticProc_COP1_MSUBF_f;
											case 0x4600001E:
												return &StaticProc_COP1_MAX_f;
											case 0x4600001C:
												return &StaticProc_COP1_MIN_f;
											case 0x4600001F:
												return &StaticProc_COP1_MAXA_f;
											case 0x4600001D:
												return &StaticProc_COP1_MINA_f;
											case 0x46000002:
												return &StaticProc_COP1_MUL_f;
											case 0x46000010:
												return &StaticProc_COP1_SEL_f;
											case 0x46000014:
												return &StaticProc_COP1_SELEQZ_f;
											case 0x46000017:
												return &StaticProc_COP1_SELNEZ_f;
											case 0x46000001:
												return &StaticProc_COP1_SUB_f;
											default:
												return nullptr;
										}
										return nullptr;
								}
							case 0x02200000:
								switch(i & 0x001F003F) {
									case 0x00000025:
										return &StaticProc_COP1_CVT_L_d;
									case 0x00000005:
										return &StaticProc_COP1_ABS_d;
									case 0x0000001B:
										return &StaticProc_COP1_CLASS_d;
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_d;
									case 0x00000020:
										return &StaticProc_COP1_CVT_S_d;
									case 0x00000024:
										return &StaticProc_COP1_CVT_W_d;
									case 0x00000006:
										return &StaticProc_COP1_MOV_d;
									case 0x00000007:
										return &StaticProc_COP1_NEG_d;
									case 0x00000015:
										return &StaticProc_COP1_RECIP_d;
									case 0x0000001A:
										return &StaticProc_COP1_RINT_d;
									case 0x00000016:
										return &StaticProc_COP1_RSQRT_d;
									case 0x00000004:
										return &StaticProc_COP1_SQRT_d;
									case 0x00000008:
										return &StaticProc_COP1_ROUND_L_d;
									case 0x0000000C:
										return &StaticProc_COP1_ROUND_W_d;
									case 0x0000000A:
										return &StaticProc_COP1_CEIL_L_d;
									case 0x0000000E:
										return &StaticProc_COP1_CEIL_W_d;
									case 0x0000000B:
										return &StaticProc_COP1_FLOOR_L_d;
									case 0x0000000F:
										return &StaticProc_COP1_FLOOR_W_d;
									case 0x00000009:
										return &StaticProc_COP1_TRUNC_L_d;
									case 0x0000000D:
										return &StaticProc_COP1_TRUNC_W_d;
									default:
										switch(i & 0xFFE0003F) {
											case 0x46200018:
												return &StaticProc_COP1_MADDF_d;
											case 0x46200000:
												return &StaticProc_COP1_ADD_d;
											case 0x46200003:
												return &StaticProc_COP1_DIV_d;
											case 0x46200019:
												return &StaticProc_COP1_MSUBF_d;
											case 0x4620001E:
												return &StaticProc_COP1_MAX_d;
											case 0x4620001C:
												return &StaticProc_COP1_MIN_d;
											case 0x4620001F:
												return &StaticProc_COP1_MAXA_d;
											case 0x4620001D:
												return &StaticProc_COP1_MINA_d;
											case 0x46200002:
												return &StaticProc_COP1_MUL_d;
											case 0x46200010:
												return &StaticProc_COP1_SEL_d;
											case 0x46200014:
												return &StaticProc_COP1_SELEQZ_d;
											case 0x46200017:
												return &StaticProc_COP1_SELNEZ_d;
											case 0x46200001:
												return &StaticProc_COP1_SUB_d;
											default:
												return nullptr;
										}
										return nullptr;
								}
							case 0x02800000:
								switch(i & 0x001F003F) {
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_w;
									case 0x00000020:
										return &StaticProc_COP1_CVT_S_w;
									default:
										if ((i & 0xFFE00020) == 0x46800000)
											return &StaticProc_COP1_CMP_condn_s_v;
										return nullptr;
								}
							case 0x02A00000:
								switch(i & 0x001F003F) {
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_l;
									case 0x00000020:
										return &StaticProc_COP1_CVT_S_l;
									default:
										if ((i & 0xFFE00020) == 0x46A00000)
											return &StaticProc_COP1_CMP_condn_d_v;
										return nullptr;
								}
							case 0x00C00000:
								return &StaticProc_COP1_CTC1_v;
							case 0x00400000:
								return &StaticProc_COP1_CFC1_v;
							case 0x00800000:
								return &StaticProc_COP1_MTC1_v;
							case 0x00000000:
								return &StaticProc_COP1_MFC1_v;
							default:
								return nullptr;
						}
					case 0x24000000:
						switch(i & 0x0000FFFF) {
							case 0x00000000:
								return &StaticProc_PROC_MOVE;
							default:
								if ((i & 0xFC000000) == 0x24000000)
									return &StaticProc_PROC_ADDIU;
								return nullptr;
						}
					case 0x7C000000:
						switch(i & 0x03E007FF) {
							case 0x000000A0:
								return &StaticProc_PROC_WSBH;
							case 0x00000620:
								return &StaticProc_PROC_SEH;
							case 0x00000420:
								return &StaticProc_PROC_SEB;
							case 0x00000020:
								return &StaticProc_PROC_BITSWAP;
							default:
								switch(i & 0xFC00003F) {
									case 0x7C00003B:
										return &StaticProc_PROC_RDHWR;
									case 0x7C00001E:
										switch(i & 0x000007C0) {
											case 0x00000040:
												return &StaticProc_PROC_SCWPE;
											default:
												if ((i & 0xFC00007F) == 0x7C00001E)
													return &StaticProc_PROC_SCE;
												return nullptr;
										}
									case 0x7C000026:
										switch(i & 0x000007C0) {
											case 0x00000040:
												return &StaticProc_PROC_SCWP;
											default:
												if ((i & 0xFC00007F) == 0x7C000026)
													return &StaticProc_PROC_SC;
												return nullptr;
										}
									case 0x7C00002E:
										switch(i & 0x000007C0) {
											case 0x00000040:
												return &StaticProc_PROC_LLWPE;
											default:
												if ((i & 0xFC00007F) == 0x7C00002E)
													return &StaticProc_PROC_LLE;
												return nullptr;
										}
									case 0x7C000036:
										switch(i & 0x000007C0) {
											case 0x00000040:
												return &StaticProc_PROC_LLWP;
											default:
												if ((i & 0xFC00007F) == 0x7C000036)
													return &StaticProc_PROC_LL;
												return nullptr;
										}
									case 0x7C000020:
										return &StaticProc_PROC_ALIGN;
									case 0x7C000025:
										return &StaticProc_PROC_CACHE;
									case 0x7C00002C:
										return &StaticProc_PROC_LBE;
									case 0x7C000028:
										return &StaticProc_PROC_LBUE;
									case 0x7C00002D:
										return &StaticProc_PROC_LHE;
									case 0x7C000029:
										return &StaticProc_PROC_LHUE;
									case 0x7C00002F:
										return &StaticProc_PROC_LWE;
									case 0x7C000035:
										return &StaticProc_PROC_PREF;
									case 0x7C000023:
										return &StaticProc_PROC_PREFE;
									case 0x7C00001C:
										return &StaticProc_PROC_SBE;
									case 0x7C00001D:
										return &StaticProc_PROC_SHE;
									case 0x7C00001F:
										return &StaticProc_PROC_SWE;
									case 0x7C000000:
										return &StaticProc_PROC_EXT;
									case 0x7C000004:
										return &StaticProc_PROC_INS;
									default:
										return nullptr;
								}
								return nullptr;
						}
					case 0x04000000:
						switch(i & 0x03FF0000) {
							case 0x00170000:
								return &StaticProc_PROC_SIGRIE;
							case 0x00100000:
								return &StaticProc_PROC_NAL;
							case 0x00110000:
								return &StaticProc_PROC_BAL;
							default:
								switch(i & 0xFC1F0000) {
									case 0x041F0000:
										return &StaticProc_PROC_SYNCI;
									case 0x04000000:
										return &StaticProc_PROC_BLTZ;
									case 0x04010000:
										return &StaticProc_PROC_BGEZ;
									default:
										return nullptr;
								}
								return nullptr;
						}
					case 0xEC000000:
						switch(i & 0x001F0000) {
							case 0x001E0000:
								return &StaticProc_PROC_AUIPC;
							case 0x001F0000:
								return &StaticProc_PROC_ALUIPC;
							default:
								switch(i & 0xFC180000) {
									case 0xEC080000:
										return &StaticProc_PROC_LWPC;
									case 0xEC000000:
										return &StaticProc_PROC_ADDIUPC;
									default:
										return nullptr;
								}
								return nullptr;
						}
					case 0x18000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_BLEZALC;
							default:
								switch(i & 0xFC1F0000) {
									case 0x18000000:
										return &StaticProc_PROC_BLEZ;
									default:
										if ((i & 0xFC000000) == 0x18000000)
											return &StaticProc_PROC_POP06;
										return nullptr;
								}
								return nullptr;
						}
					case 0x1C000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_BGTZALC;
							default:
								switch(i & 0xFC1F0000) {
									case 0x1C000000:
										return &StaticProc_PROC_BGTZ;
									default:
										if ((i & 0xFC000000) == 0x1C000000)
											return &StaticProc_PROC_POP07;
										return nullptr;
								}
								return nullptr;
						}
					default:
						return nullptr;
				}
				return nullptr;
		}
		return nullptr;
	}

	bool execute_instruction (instruction_t i, mips::processor & __restrict p) {
		switch(i) {
			case 0x00000040:
				{ PROC_SSNOP_NS::Execute(i,p); return true; }
			case 0x00000140:
				{ PROC_PAUSE_NS::Execute(i,p); return true; }
			case 0x00000000:
				{ PROC_NOP_NS::Execute(i,p); return true; }
			case 0x000000C0:
				{ PROC_EHB_NS::Execute(i,p); return true; }
			default:
				switch(i & 0xFC000000) {
					case 0x38000000:
						{ PROC_XORI_NS::Execute(i,p); return true; }
					case 0xAC000000:
						{ PROC_SW_NS::Execute(i,p); return true; }
					case 0x2C000000:
						{ PROC_SLTIU_NS::Execute(i,p); return true; }
					case 0x28000000:
						{ PROC_SLTI_NS::Execute(i,p); return true; }
					case 0xA4000000:
						{ PROC_SH_NS::Execute(i,p); return true; }
					case 0xA0000000:
						{ PROC_SB_NS::Execute(i,p); return true; }
					case 0x34000000:
						{ PROC_ORI_NS::Execute(i,p); return true; }
					case 0x8C000000:
						{ PROC_LW_NS::Execute(i,p); return true; }
					case 0x94000000:
						{ PROC_LHU_NS::Execute(i,p); return true; }
					case 0x84000000:
						{ PROC_LH_NS::Execute(i,p); return true; }
					case 0x90000000:
						{ PROC_LBU_NS::Execute(i,p); return true; }
					case 0x80000000:
						{ PROC_LB_NS::Execute(i,p); return true; }
					case 0x0C000000:
						{ PROC_JAL_NS::Execute(i,p); return true; }
					case 0x08000000:
						{ PROC_J_NS::Execute(i,p); return true; }
					case 0x14000000:
						{ PROC_BNE_NS::Execute(i,p); return true; }
					case 0x60000000:
						{ PROC_POP30_NS::Execute(i,p); return true; }
					case 0x20000000:
						{ PROC_POP10_NS::Execute(i,p); return true; }
					case 0x10000000:
						{ PROC_BEQ_NS::Execute(i,p); return true; }
					case 0xC8000000:
						{ PROC_BC_NS::Execute(i,p); return true; }
					case 0xE8000000:
						{ PROC_BALC_NS::Execute(i,p); return true; }
					case 0x3C000000:
						{ PROC_AUI_NS::Execute(i,p); return true; }
					case 0x30000000:
						{ PROC_ANDI_NS::Execute(i,p); return true; }
					case 0xE4000000:
						{ COP1_SWC1_NS::Execute<void>(i,p); return true; }
					case 0xF4000000:
						{ COP1_SDC1_NS::Execute<void>(i,p); return true; }
					case 0xC4000000:
						{ COP1_LWC1_NS::Execute<void>(i,p); return true; }
					case 0xD4000000:
						{ COP1_LDC1_NS::Execute<void>(i,p); return true; }
					case 0xF8000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								{ PROC_JIALC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000) == 0xF8000000)
									{ PROC_BNEZC_NS::Execute(i,p); return true; }
								return false;
						}
					case 0xD8000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								{ PROC_JIC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000) == 0xD8000000)
									{ PROC_BEQZC_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x5C000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								{ PROC_BGTZC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000) == 0x5C000000)
									{ PROC_POP27_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x58000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								{ PROC_BLEZC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000) == 0x58000000)
									{ PROC_POP26_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x00000000:
						switch(i & 0x0000003F) {
							case 0x0000000F:
								{ PROC_SYNC_NS::Execute(i,p); return true; }
							case 0x00000009:
								switch(i & 0x001FF800) {
									case 0x00000000:
										{ PROC_JR_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC1F003F) == 0x00000009)
											{ PROC_JALR_NS::Execute(i,p); return true; }
										return false;
								}
							case 0x00000021:
								switch(i & 0x001F07C0) {
									case 0x00000040:
										{ PROC_CLO_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC0007FF) == 0x00000021)
											{ PROC_ADDU_NS::Execute(i,p); return true; }
										return false;
								}
							case 0x00000020:
								{ PROC_ADD_NS::Execute(i,p); return true; }
							case 0x00000010:
								{ PROC_CLZ_NS::Execute(i,p); return true; }
							case 0x00000024:
								{ PROC_AND_NS::Execute(i,p); return true; }
							case 0x0000001A:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										{ PROC_MOD_NS::Execute(i,p); return true; }
									case 0x00000080:
										{ PROC_DIV_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x0000001B:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										{ PROC_MODU_NS::Execute(i,p); return true; }
									case 0x00000080:
										{ PROC_DIVU_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000018:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										{ PROC_MUH_NS::Execute(i,p); return true; }
									case 0x00000080:
										{ PROC_MUL_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000019:
								switch(i & 0x000007C0) {
									case 0x000000C0:
										{ PROC_MUHU_NS::Execute(i,p); return true; }
									case 0x00000080:
										{ PROC_MULU_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000027:
								{ PROC_NOR_NS::Execute(i,p); return true; }
							case 0x00000025:
								{ PROC_OR_NS::Execute(i,p); return true; }
							case 0x00000002:
								switch(i & 0x03E00000) {
									case 0x00000000:
										{ PROC_SRL_NS::Execute(i,p); return true; }
									case 0x00200000:
										{ PROC_ROTR_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000006:
								switch(i & 0x000007C0) {
									case 0x00000000:
										{ PROC_SRLV_NS::Execute(i,p); return true; }
									case 0x00000040:
										{ PROC_ROTRV_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000035:
								{ PROC_SELEQZ_NS::Execute(i,p); return true; }
							case 0x00000037:
								{ PROC_SELNEZ_NS::Execute(i,p); return true; }
							case 0x00000000:
								{ PROC_SLL_NS::Execute(i,p); return true; }
							case 0x00000004:
								{ PROC_SLLV_NS::Execute(i,p); return true; }
							case 0x0000002A:
								{ PROC_SLT_NS::Execute(i,p); return true; }
							case 0x0000002B:
								{ PROC_SLTU_NS::Execute(i,p); return true; }
							case 0x00000003:
								{ PROC_SRA_NS::Execute(i,p); return true; }
							case 0x00000007:
								{ PROC_SRAV_NS::Execute(i,p); return true; }
							case 0x00000022:
								{ PROC_SUB_NS::Execute(i,p); return true; }
							case 0x00000023:
								{ PROC_SUBU_NS::Execute(i,p); return true; }
							case 0x00000026:
								{ PROC_XOR_NS::Execute(i,p); return true; }
							case 0x00000005:
								{ PROC_LSA_NS::Execute(i,p); return true; }
							case 0x0000000D:
								{ PROC_BREAK_NS::Execute(i,p); return true; }
							case 0x0000000E:
								{ PROC_SDBBP_NS::Execute(i,p); return true; }
							case 0x0000000C:
								{ PROC_SYSCALL_NS::Execute(i,p); return true; }
							case 0x00000034:
								{ PROC_TEQ_NS::Execute(i,p); return true; }
							case 0x00000030:
								{ PROC_TGE_NS::Execute(i,p); return true; }
							case 0x00000031:
								{ PROC_TGEU_NS::Execute(i,p); return true; }
							case 0x00000032:
								{ PROC_TLT_NS::Execute(i,p); return true; }
							case 0x00000033:
								{ PROC_TLTU_NS::Execute(i,p); return true; }
							case 0x00000036:
								{ PROC_TNE_NS::Execute(i,p); return true; }
							default:
								return false;
						}
					case 0x44000000:
						switch(i & 0x03E00000) {
							case 0x01A00000:
								{ COP1_BC1NEZ_NS::Execute<void>(i,p); return true; }
							case 0x01200000:
								{ COP1_BC1EQZ_NS::Execute<void>(i,p); return true; }
							case 0x00600000:
								{ COP1_MFHC1_NS::Execute<void>(i,p); return true; }
							case 0x00E00000:
								{ COP1_MTHC1_NS::Execute<void>(i,p); return true; }
							case 0x02000000:
								switch(i & 0x001F003F) {
									case 0x00000025:
										{ COP1_CVT_L_NS::Execute<float>(i,p); return true; }
									case 0x00000005:
										{ COP1_ABS_NS::Execute<float>(i,p); return true; }
									case 0x0000001B:
										{ COP1_CLASS_NS::Execute<float>(i,p); return true; }
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<float>(i,p); return true; }
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<float>(i,p); return true; }
									case 0x00000024:
										{ COP1_CVT_W_NS::Execute<float>(i,p); return true; }
									case 0x00000006:
										{ COP1_MOV_NS::Execute<float>(i,p); return true; }
									case 0x00000007:
										{ COP1_NEG_NS::Execute<float>(i,p); return true; }
									case 0x00000015:
										{ COP1_RECIP_NS::Execute<float>(i,p); return true; }
									case 0x0000001A:
										{ COP1_RINT_NS::Execute<float>(i,p); return true; }
									case 0x00000016:
										{ COP1_RSQRT_NS::Execute<float>(i,p); return true; }
									case 0x00000004:
										{ COP1_SQRT_NS::Execute<float>(i,p); return true; }
									case 0x00000008:
										{ COP1_ROUND_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000C:
										{ COP1_ROUND_W_NS::Execute<float>(i,p); return true; }
									case 0x0000000A:
										{ COP1_CEIL_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000E:
										{ COP1_CEIL_W_NS::Execute<float>(i,p); return true; }
									case 0x0000000B:
										{ COP1_FLOOR_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000F:
										{ COP1_FLOOR_W_NS::Execute<float>(i,p); return true; }
									case 0x00000009:
										{ COP1_TRUNC_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000D:
										{ COP1_TRUNC_W_NS::Execute<float>(i,p); return true; }
									default:
										switch(i & 0xFFE0003F) {
											case 0x46000018:
												{ COP1_MADDF_NS::Execute<float>(i,p); return true; }
											case 0x46000000:
												{ COP1_ADD_NS::Execute<float>(i,p); return true; }
											case 0x46000003:
												{ COP1_DIV_NS::Execute<float>(i,p); return true; }
											case 0x46000019:
												{ COP1_MSUBF_NS::Execute<float>(i,p); return true; }
											case 0x4600001E:
												{ COP1_MAX_NS::Execute<float>(i,p); return true; }
											case 0x4600001C:
												{ COP1_MIN_NS::Execute<float>(i,p); return true; }
											case 0x4600001F:
												{ COP1_MAXA_NS::Execute<float>(i,p); return true; }
											case 0x4600001D:
												{ COP1_MINA_NS::Execute<float>(i,p); return true; }
											case 0x46000002:
												{ COP1_MUL_NS::Execute<float>(i,p); return true; }
											case 0x46000010:
												{ COP1_SEL_NS::Execute<float>(i,p); return true; }
											case 0x46000014:
												{ COP1_SELEQZ_NS::Execute<float>(i,p); return true; }
											case 0x46000017:
												{ COP1_SELNEZ_NS::Execute<float>(i,p); return true; }
											case 0x46000001:
												{ COP1_SUB_NS::Execute<float>(i,p); return true; }
											default:
												return false;
										}
										return false;
								}
							case 0x02200000:
								switch(i & 0x001F003F) {
									case 0x00000025:
										{ COP1_CVT_L_NS::Execute<double>(i,p); return true; }
									case 0x00000005:
										{ COP1_ABS_NS::Execute<double>(i,p); return true; }
									case 0x0000001B:
										{ COP1_CLASS_NS::Execute<double>(i,p); return true; }
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<double>(i,p); return true; }
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<double>(i,p); return true; }
									case 0x00000024:
										{ COP1_CVT_W_NS::Execute<double>(i,p); return true; }
									case 0x00000006:
										{ COP1_MOV_NS::Execute<double>(i,p); return true; }
									case 0x00000007:
										{ COP1_NEG_NS::Execute<double>(i,p); return true; }
									case 0x00000015:
										{ COP1_RECIP_NS::Execute<double>(i,p); return true; }
									case 0x0000001A:
										{ COP1_RINT_NS::Execute<double>(i,p); return true; }
									case 0x00000016:
										{ COP1_RSQRT_NS::Execute<double>(i,p); return true; }
									case 0x00000004:
										{ COP1_SQRT_NS::Execute<double>(i,p); return true; }
									case 0x00000008:
										{ COP1_ROUND_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000C:
										{ COP1_ROUND_W_NS::Execute<double>(i,p); return true; }
									case 0x0000000A:
										{ COP1_CEIL_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000E:
										{ COP1_CEIL_W_NS::Execute<double>(i,p); return true; }
									case 0x0000000B:
										{ COP1_FLOOR_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000F:
										{ COP1_FLOOR_W_NS::Execute<double>(i,p); return true; }
									case 0x00000009:
										{ COP1_TRUNC_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000D:
										{ COP1_TRUNC_W_NS::Execute<double>(i,p); return true; }
									default:
										switch(i & 0xFFE0003F) {
											case 0x46200018:
												{ COP1_MADDF_NS::Execute<double>(i,p); return true; }
											case 0x46200000:
												{ COP1_ADD_NS::Execute<double>(i,p); return true; }
											case 0x46200003:
												{ COP1_DIV_NS::Execute<double>(i,p); return true; }
											case 0x46200019:
												{ COP1_MSUBF_NS::Execute<double>(i,p); return true; }
											case 0x4620001E:
												{ COP1_MAX_NS::Execute<double>(i,p); return true; }
											case 0x4620001C:
												{ COP1_MIN_NS::Execute<double>(i,p); return true; }
											case 0x4620001F:
												{ COP1_MAXA_NS::Execute<double>(i,p); return true; }
											case 0x4620001D:
												{ COP1_MINA_NS::Execute<double>(i,p); return true; }
											case 0x46200002:
												{ COP1_MUL_NS::Execute<double>(i,p); return true; }
											case 0x46200010:
												{ COP1_SEL_NS::Execute<double>(i,p); return true; }
											case 0x46200014:
												{ COP1_SELEQZ_NS::Execute<double>(i,p); return true; }
											case 0x46200017:
												{ COP1_SELNEZ_NS::Execute<double>(i,p); return true; }
											case 0x46200001:
												{ COP1_SUB_NS::Execute<double>(i,p); return true; }
											default:
												return false;
										}
										return false;
								}
							case 0x02800000:
								switch(i & 0x001F003F) {
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<int32>(i,p); return true; }
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<int32>(i,p); return true; }
									default:
										if ((i & 0xFFE00020) == 0x46800000)
											{ COP1_CMP_condn_s_NS::Execute<void>(i,p); return true; }
										return false;
								}
							case 0x02A00000:
								switch(i & 0x001F003F) {
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<int64>(i,p); return true; }
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<int64>(i,p); return true; }
									default:
										if ((i & 0xFFE00020) == 0x46A00000)
											{ COP1_CMP_condn_d_NS::Execute<void>(i,p); return true; }
										return false;
								}
							case 0x00C00000:
								{ COP1_CTC1_NS::Execute<void>(i,p); return true; }
							case 0x00400000:
								{ COP1_CFC1_NS::Execute<void>(i,p); return true; }
							case 0x00800000:
								{ COP1_MTC1_NS::Execute<void>(i,p); return true; }
							case 0x00000000:
								{ COP1_MFC1_NS::Execute<void>(i,p); return true; }
							default:
								return false;
						}
					case 0x24000000:
						switch(i & 0x0000FFFF) {
							case 0x00000000:
								{ PROC_MOVE_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000) == 0x24000000)
									{ PROC_ADDIU_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x7C000000:
						switch(i & 0x03E007FF) {
							case 0x000000A0:
								{ PROC_WSBH_NS::Execute(i,p); return true; }
							case 0x00000620:
								{ PROC_SEH_NS::Execute(i,p); return true; }
							case 0x00000420:
								{ PROC_SEB_NS::Execute(i,p); return true; }
							case 0x00000020:
								{ PROC_BITSWAP_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC00003F) {
									case 0x7C00003B:
										{ PROC_RDHWR_NS::Execute(i,p); return true; }
									case 0x7C00001E:
										switch(i & 0x000007C0) {
											case 0x00000040:
												{ PROC_SCWPE_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007F) == 0x7C00001E)
													{ PROC_SCE_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C000026:
										switch(i & 0x000007C0) {
											case 0x00000040:
												{ PROC_SCWP_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007F) == 0x7C000026)
													{ PROC_SC_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C00002E:
										switch(i & 0x000007C0) {
											case 0x00000040:
												{ PROC_LLWPE_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007F) == 0x7C00002E)
													{ PROC_LLE_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C000036:
										switch(i & 0x000007C0) {
											case 0x00000040:
												{ PROC_LLWP_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007F) == 0x7C000036)
													{ PROC_LL_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C000020:
										{ PROC_ALIGN_NS::Execute(i,p); return true; }
									case 0x7C000025:
										{ PROC_CACHE_NS::Execute(i,p); return true; }
									case 0x7C00002C:
										{ PROC_LBE_NS::Execute(i,p); return true; }
									case 0x7C000028:
										{ PROC_LBUE_NS::Execute(i,p); return true; }
									case 0x7C00002D:
										{ PROC_LHE_NS::Execute(i,p); return true; }
									case 0x7C000029:
										{ PROC_LHUE_NS::Execute(i,p); return true; }
									case 0x7C00002F:
										{ PROC_LWE_NS::Execute(i,p); return true; }
									case 0x7C000035:
										{ PROC_PREF_NS::Execute(i,p); return true; }
									case 0x7C000023:
										{ PROC_PREFE_NS::Execute(i,p); return true; }
									case 0x7C00001C:
										{ PROC_SBE_NS::Execute(i,p); return true; }
									case 0x7C00001D:
										{ PROC_SHE_NS::Execute(i,p); return true; }
									case 0x7C00001F:
										{ PROC_SWE_NS::Execute(i,p); return true; }
									case 0x7C000000:
										{ PROC_EXT_NS::Execute(i,p); return true; }
									case 0x7C000004:
										{ PROC_INS_NS::Execute(i,p); return true; }
									default:
										return false;
								}
								return false;
						}
					case 0x04000000:
						switch(i & 0x03FF0000) {
							case 0x00170000:
								{ PROC_SIGRIE_NS::Execute(i,p); return true; }
							case 0x00100000:
								{ PROC_NAL_NS::Execute(i,p); return true; }
							case 0x00110000:
								{ PROC_BAL_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC1F0000) {
									case 0x041F0000:
										{ PROC_SYNCI_NS::Execute(i,p); return true; }
									case 0x04000000:
										{ PROC_BLTZ_NS::Execute(i,p); return true; }
									case 0x04010000:
										{ PROC_BGEZ_NS::Execute(i,p); return true; }
									default:
										return false;
								}
								return false;
						}
					case 0xEC000000:
						switch(i & 0x001F0000) {
							case 0x001E0000:
								{ PROC_AUIPC_NS::Execute(i,p); return true; }
							case 0x001F0000:
								{ PROC_ALUIPC_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC180000) {
									case 0xEC080000:
										{ PROC_LWPC_NS::Execute(i,p); return true; }
									case 0xEC000000:
										{ PROC_ADDIUPC_NS::Execute(i,p); return true; }
									default:
										return false;
								}
								return false;
						}
					case 0x18000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								{ PROC_BLEZALC_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC1F0000) {
									case 0x18000000:
										{ PROC_BLEZ_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC000000) == 0x18000000)
											{ PROC_POP06_NS::Execute(i,p); return true; }
										return false;
								}
								return false;
						}
					case 0x1C000000:
						switch(i & 0x03E00000) {
							case 0x00000000:
								{ PROC_BGTZALC_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC1F0000) {
									case 0x1C000000:
										{ PROC_BGTZ_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC000000) == 0x1C000000)
											{ PROC_POP07_NS::Execute(i,p); return true; }
										return false;
								}
								return false;
						}
					default:
						return false;
				}
				return false;
		}
		return false;
	}
}
