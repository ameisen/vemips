/* WARNING: THIS IS AN AUTO-GENERATED FILE ('..\..\src\tablegen\writers.cpp', Mar  4 2023 16:23:20) */
// ReSharper disable CppClangTidyClangDiagnosticUndefinedFuncTemplate CppClangTidyClangDiagnosticUnreachableCodeReturn IdentifierTypo CommentTypo CppUnreachableCode
#include "pch.hpp"

#define VEMIPS_ITABLE_EXPORT /* __declspec(dllexport) */ extern

#include "mips/instructions/instructions_table.hpp"
#include "mips/mips_common.hpp"
#include "mips/instructions/instructions_common.hpp"
#include "mips/processor/processor.hpp"

namespace mips::instructions {
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ABS_f ( "COP1_ABS", 1, &COP1_ABS_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ABS_d ( "COP1_ABS", 1, &COP1_ABS_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ADD_f ( "COP1_ADD", 1, &COP1_ADD_NS::Execute<float>, 0x00800077u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ADD_d ( "COP1_ADD", 1, &COP1_ADD_NS::Execute<double>, 0x00800077u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_BC1EQZ_v ( "COP1_BC1EQZ", 1, &COP1_BC1EQZ_NS::Execute<void>, 0x00972000u, { .control = true }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_BC1NEZ_v ( "COP1_BC1NEZ", 1, &COP1_BC1NEZ_NS::Execute<void>, 0x00972000u, { .control = true }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_L_f ( "COP1_CEIL_L", 1, &COP1_CEIL_L_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_L_d ( "COP1_CEIL_L", 1, &COP1_CEIL_L_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_W_f ( "COP1_CEIL_W", 1, &COP1_CEIL_W_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_W_d ( "COP1_CEIL_W", 1, &COP1_CEIL_W_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CFC1_v ( "COP1_CFC1", 1, &COP1_CFC1_NS::Execute<void>, 0x0081207Fu, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CLASS_f ( "COP1_CLASS", 1, &COP1_CLASS_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CLASS_d ( "COP1_CLASS", 1, &COP1_CLASS_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CMP_condn_d_v ( "COP1_CMP_condn_d", 1, &COP1_CMP_condn_d_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CMP_condn_s_v ( "COP1_CMP_condn_s", 1, &COP1_CMP_condn_s_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CTC1_v ( "COP1_CTC1", 1, &COP1_CTC1_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_f ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<float>, 0x00800047u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_d ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<double>, 0x00800047u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_w ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<int32>, 0x00800047u, { .control = false }, instruction_type::word_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_l ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<int64>, 0x00800047u, { .control = false }, instruction_type::long_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_L_f ( "COP1_CVT_L", 1, &COP1_CVT_L_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_L_d ( "COP1_CVT_L", 1, &COP1_CVT_L_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_f ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<float>, 0x00800047u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_d ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<double>, 0x00800047u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_w ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<int32>, 0x00800047u, { .control = false }, instruction_type::word_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_l ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<int64>, 0x00800047u, { .control = false }, instruction_type::long_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_W_f ( "COP1_CVT_W", 1, &COP1_CVT_W_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_W_d ( "COP1_CVT_W", 1, &COP1_CVT_W_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_DIV_f ( "COP1_DIV", 1, &COP1_DIV_NS::Execute<float>, 0x0080007Fu, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_DIV_d ( "COP1_DIV", 1, &COP1_DIV_NS::Execute<double>, 0x0080007Fu, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_L_f ( "COP1_FLOOR_L", 1, &COP1_FLOOR_L_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_L_d ( "COP1_FLOOR_L", 1, &COP1_FLOOR_L_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_W_f ( "COP1_FLOOR_W", 1, &COP1_FLOOR_W_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_W_d ( "COP1_FLOOR_W", 1, &COP1_FLOOR_W_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_LDC1_v ( "COP1_LDC1", 1, &COP1_LDC1_NS::Execute<void>, 0x00A12000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_LWC1_v ( "COP1_LWC1", 1, &COP1_LWC1_NS::Execute<void>, 0x00A12000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MADDF_f ( "COP1_MADDF", 1, &COP1_MADDF_NS::Execute<float>, 0x00800077u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MADDF_d ( "COP1_MADDF", 1, &COP1_MADDF_NS::Execute<double>, 0x00800077u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAX_f ( "COP1_MAX", 1, &COP1_MAX_NS::Execute<float>, 0x00812007u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAX_d ( "COP1_MAX", 1, &COP1_MAX_NS::Execute<double>, 0x00812007u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAXA_f ( "COP1_MAXA", 1, &COP1_MAXA_NS::Execute<float>, 0x00812007u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAXA_d ( "COP1_MAXA", 1, &COP1_MAXA_NS::Execute<double>, 0x00812007u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MFC1_v ( "COP1_MFC1", 1, &COP1_MFC1_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MFHC1_v ( "COP1_MFHC1", 1, &COP1_MFHC1_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MIN_f ( "COP1_MIN", 1, &COP1_MIN_NS::Execute<float>, 0x00812007u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MIN_d ( "COP1_MIN", 1, &COP1_MIN_NS::Execute<double>, 0x00812007u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MINA_f ( "COP1_MINA", 1, &COP1_MINA_NS::Execute<float>, 0x00812007u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MINA_d ( "COP1_MINA", 1, &COP1_MINA_NS::Execute<double>, 0x00812007u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MOV_f ( "COP1_MOV", 1, &COP1_MOV_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MOV_d ( "COP1_MOV", 1, &COP1_MOV_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MSUBF_f ( "COP1_MSUBF", 1, &COP1_MSUBF_NS::Execute<float>, 0x00800077u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MSUBF_d ( "COP1_MSUBF", 1, &COP1_MSUBF_NS::Execute<double>, 0x00800077u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MTC1_v ( "COP1_MTC1", 1, &COP1_MTC1_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MTHC1_v ( "COP1_MTHC1", 1, &COP1_MTHC1_NS::Execute<void>, 0x00812000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MUL_f ( "COP1_MUL", 1, &COP1_MUL_NS::Execute<float>, 0x00800077u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MUL_d ( "COP1_MUL", 1, &COP1_MUL_NS::Execute<double>, 0x00800077u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_NEG_f ( "COP1_NEG", 1, &COP1_NEG_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_NEG_d ( "COP1_NEG", 1, &COP1_NEG_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RECIP_f ( "COP1_RECIP", 1, &COP1_RECIP_NS::Execute<float>, 0x0080007Fu, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RECIP_d ( "COP1_RECIP", 1, &COP1_RECIP_NS::Execute<double>, 0x0080007Fu, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RINT_f ( "COP1_RINT", 1, &COP1_RINT_NS::Execute<float>, 0x00800077u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RINT_d ( "COP1_RINT", 1, &COP1_RINT_NS::Execute<double>, 0x00800077u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_L_f ( "COP1_ROUND_L", 1, &COP1_ROUND_L_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_L_d ( "COP1_ROUND_L", 1, &COP1_ROUND_L_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_W_f ( "COP1_ROUND_W", 1, &COP1_ROUND_W_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_W_d ( "COP1_ROUND_W", 1, &COP1_ROUND_W_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RSQRT_f ( "COP1_RSQRT", 1, &COP1_RSQRT_NS::Execute<float>, 0x0080007Fu, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RSQRT_d ( "COP1_RSQRT", 1, &COP1_RSQRT_NS::Execute<double>, 0x0080007Fu, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SDC1_v ( "COP1_SDC1", 1, &COP1_SDC1_NS::Execute<void>, 0x00C12000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SEL_f ( "COP1_SEL", 1, &COP1_SEL_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SEL_d ( "COP1_SEL", 1, &COP1_SEL_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELEQZ_f ( "COP1_SELEQZ", 1, &COP1_SELEQZ_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELEQZ_d ( "COP1_SELEQZ", 1, &COP1_SELEQZ_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELNEZ_f ( "COP1_SELNEZ", 1, &COP1_SELNEZ_NS::Execute<float>, 0x00812000u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELNEZ_d ( "COP1_SELNEZ", 1, &COP1_SELNEZ_NS::Execute<double>, 0x00812000u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SQRT_f ( "COP1_SQRT", 1, &COP1_SQRT_NS::Execute<float>, 0x00800047u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SQRT_d ( "COP1_SQRT", 1, &COP1_SQRT_NS::Execute<double>, 0x00800047u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SUB_f ( "COP1_SUB", 1, &COP1_SUB_NS::Execute<float>, 0x00800077u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SUB_d ( "COP1_SUB", 1, &COP1_SUB_NS::Execute<double>, 0x00800077u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SWC1_v ( "COP1_SWC1", 1, &COP1_SWC1_NS::Execute<void>, 0x00C12000u, { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_L_f ( "COP1_TRUNC_L", 1, &COP1_TRUNC_L_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_L_d ( "COP1_TRUNC_L", 1, &COP1_TRUNC_L_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_W_f ( "COP1_TRUNC_W", 1, &COP1_TRUNC_W_NS::Execute<float>, 0x008000C7u, { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_W_d ( "COP1_TRUNC_W", 1, &COP1_TRUNC_W_NS::Execute<double>, 0x008000C7u, { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADD ( "PROC_ADD", 1, &PROC_ADD_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDIU ( "PROC_ADDIU", 1, &PROC_ADDIU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDIUPC ( "PROC_ADDIUPC", 1, &PROC_ADDIUPC_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDU ( "PROC_ADDU", 1, &PROC_ADDU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ALIGN ( "PROC_ALIGN", 1, &PROC_ALIGN_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ALUIPC ( "PROC_ALUIPC", 1, &PROC_ALUIPC_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AND ( "PROC_AND", 1, &PROC_AND_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ANDI ( "PROC_ANDI", 1, &PROC_ANDI_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AUI ( "PROC_AUI", 1, &PROC_AUI_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AUIPC ( "PROC_AUIPC", 1, &PROC_AUIPC_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BAL ( "PROC_BAL", 1, &PROC_BAL_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BALC ( "PROC_BALC", 1, &PROC_BALC_NS::Execute, 0x000A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BC ( "PROC_BC", 1, &PROC_BC_NS::Execute, 0x000A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BEQ ( "PROC_BEQ", 1, &PROC_BEQ_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BEQZC ( "PROC_BEQZC", 1, &PROC_BEQZC_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGEZ ( "PROC_BGEZ", 1, &PROC_BGEZ_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZ ( "PROC_BGTZ", 1, &PROC_BGTZ_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZALC ( "PROC_BGTZALC", 1, &PROC_BGTZALC_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZC ( "PROC_BGTZC", 1, &PROC_BGTZC_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BITSWAP ( "PROC_BITSWAP", 1, &PROC_BITSWAP_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZ ( "PROC_BLEZ", 1, &PROC_BLEZ_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZALC ( "PROC_BLEZALC", 1, &PROC_BLEZALC_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZC ( "PROC_BLEZC", 1, &PROC_BLEZC_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLTZ ( "PROC_BLTZ", 1, &PROC_BLTZ_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BNE ( "PROC_BNE", 1, &PROC_BNE_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BNEZC ( "PROC_BNEZC", 1, &PROC_BNEZC_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BREAK ( "PROC_BREAK", 1, &PROC_BREAK_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CACHE ( "PROC_CACHE", 1, &PROC_CACHE_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CLO ( "PROC_CLO", 1, &PROC_CLO_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CLZ ( "PROC_CLZ", 1, &PROC_CLZ_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_DIV ( "PROC_DIV", 1, &PROC_DIV_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_DIVU ( "PROC_DIVU", 1, &PROC_DIVU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_EHB ( "PROC_EHB", 1, &PROC_EHB_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_EXT ( "PROC_EXT", 1, &PROC_EXT_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_INS ( "PROC_INS", 1, &PROC_INS_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_J ( "PROC_J", 1, &PROC_J_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JAL ( "PROC_JAL", 1, &PROC_JAL_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JALR ( "PROC_JALR", 1, &PROC_JALR_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JIALC ( "PROC_JIALC", 1, &PROC_JIALC_NS::Execute, 0x000A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JIC ( "PROC_JIC", 1, &PROC_JIC_NS::Execute, 0x000A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JR ( "PROC_JR", 1, &PROC_JR_NS::Execute, 0x00160000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LB ( "PROC_LB", 1, &PROC_LB_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBE ( "PROC_LBE", 1, &PROC_LBE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBU ( "PROC_LBU", 1, &PROC_LBU_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBUE ( "PROC_LBUE", 1, &PROC_LBUE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LH ( "PROC_LH", 1, &PROC_LH_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHE ( "PROC_LHE", 1, &PROC_LHE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHU ( "PROC_LHU", 1, &PROC_LHU_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHUE ( "PROC_LHUE", 1, &PROC_LHUE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LL ( "PROC_LL", 1, &PROC_LL_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLE ( "PROC_LLE", 1, &PROC_LLE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLWP ( "PROC_LLWP", 1, &PROC_LLWP_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLWPE ( "PROC_LLWPE", 1, &PROC_LLWPE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LSA ( "PROC_LSA", 1, &PROC_LSA_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LW ( "PROC_LW", 1, &PROC_LW_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LWE ( "PROC_LWE", 1, &PROC_LWE_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LWPC ( "PROC_LWPC", 1, &PROC_LWPC_NS::Execute, 0x00200000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MOD ( "PROC_MOD", 1, &PROC_MOD_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MODU ( "PROC_MODU", 1, &PROC_MODU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MOVE ( "PROC_MOVE", 1, &PROC_MOVE_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUH ( "PROC_MUH", 1, &PROC_MUH_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUHU ( "PROC_MUHU", 1, &PROC_MUHU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUL ( "PROC_MUL", 1, &PROC_MUL_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MULU ( "PROC_MULU", 1, &PROC_MULU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NAL ( "PROC_NAL", 1, &PROC_NAL_NS::Execute, 0x00120000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NOP ( "PROC_NOP", 1, &PROC_NOP_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NOR ( "PROC_NOR", 1, &PROC_NOR_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_OR ( "PROC_OR", 1, &PROC_OR_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ORI ( "PROC_ORI", 1, &PROC_ORI_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PAUSE ( "PROC_PAUSE", 1, &PROC_PAUSE_NS::Execute, 0x00020000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP06 ( "PROC_POP06", 1, &PROC_POP06_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP07 ( "PROC_POP07", 1, &PROC_POP07_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP10 ( "PROC_POP10", 1, &PROC_POP10_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP26 ( "PROC_POP26", 1, &PROC_POP26_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP27 ( "PROC_POP27", 1, &PROC_POP27_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP30 ( "PROC_POP30", 1, &PROC_POP30_NS::Execute, 0x001A0000u, { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PREF ( "PROC_PREF", 1, &PROC_PREF_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PREFE ( "PROC_PREFE", 1, &PROC_PREFE_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_RDHWR ( "PROC_RDHWR", 1, &PROC_RDHWR_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ROTR ( "PROC_ROTR", 1, &PROC_ROTR_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ROTRV ( "PROC_ROTRV", 1, &PROC_ROTRV_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SB ( "PROC_SB", 1, &PROC_SB_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SBE ( "PROC_SBE", 1, &PROC_SBE_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SC ( "PROC_SC", 1, &PROC_SC_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCE ( "PROC_SCE", 1, &PROC_SCE_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCWP ( "PROC_SCWP", 1, &PROC_SCWP_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCWPE ( "PROC_SCWPE", 1, &PROC_SCWPE_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SDBBP ( "PROC_SDBBP", 1, &PROC_SDBBP_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SEB ( "PROC_SEB", 1, &PROC_SEB_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SEH ( "PROC_SEH", 1, &PROC_SEH_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SELEQZ ( "PROC_SELEQZ", 1, &PROC_SELEQZ_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SELNEZ ( "PROC_SELNEZ", 1, &PROC_SELNEZ_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SH ( "PROC_SH", 1, &PROC_SH_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SHE ( "PROC_SHE", 1, &PROC_SHE_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SIGRIE ( "PROC_SIGRIE", 1, &PROC_SIGRIE_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLL ( "PROC_SLL", 1, &PROC_SLL_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLLV ( "PROC_SLLV", 1, &PROC_SLLV_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLT ( "PROC_SLT", 1, &PROC_SLT_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTI ( "PROC_SLTI", 1, &PROC_SLTI_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTIU ( "PROC_SLTIU", 1, &PROC_SLTIU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTU ( "PROC_SLTU", 1, &PROC_SLTU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRA ( "PROC_SRA", 1, &PROC_SRA_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRAV ( "PROC_SRAV", 1, &PROC_SRAV_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRL ( "PROC_SRL", 1, &PROC_SRL_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRLV ( "PROC_SRLV", 1, &PROC_SRLV_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SSNOP ( "PROC_SSNOP", 1, &PROC_SSNOP_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SUB ( "PROC_SUB", 1, &PROC_SUB_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SUBU ( "PROC_SUBU", 1, &PROC_SUBU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SW ( "PROC_SW", 1, &PROC_SW_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SWE ( "PROC_SWE", 1, &PROC_SWE_NS::Execute, 0x00400000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYNC ( "PROC_SYNC", 1, &PROC_SYNC_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYNCI ( "PROC_SYNCI", 1, &PROC_SYNCI_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYSCALL ( "PROC_SYSCALL", 1, &PROC_SYSCALL_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TEQ ( "PROC_TEQ", 1, &PROC_TEQ_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TGE ( "PROC_TGE", 1, &PROC_TGE_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TGEU ( "PROC_TGEU", 1, &PROC_TGEU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TLT ( "PROC_TLT", 1, &PROC_TLT_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TLTU ( "PROC_TLTU", 1, &PROC_TLTU_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TNE ( "PROC_TNE", 1, &PROC_TNE_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_WSBH ( "PROC_WSBH", 1, &PROC_WSBH_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_XOR ( "PROC_XOR", 1, &PROC_XOR_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_XORI ( "PROC_XORI", 1, &PROC_XORI_NS::Execute, 0x00000000u, { .control = false }, instruction_type::normal);
}

namespace mips::instructions {
	const InstructionInfo * get_instruction (instruction_t i) {
		switch(i) {
			case 0x00000040u:
				return &StaticProc_PROC_SSNOP;
			case 0x00000140u:
				return &StaticProc_PROC_PAUSE;
			case 0x00000000u:
				return &StaticProc_PROC_NOP;
			case 0x000000C0u:
				return &StaticProc_PROC_EHB;
			default:
				switch(i & 0xFC000000u) {
					case 0x38000000u:
						return &StaticProc_PROC_XORI;
					case 0xAC000000u:
						return &StaticProc_PROC_SW;
					case 0x2C000000u:
						return &StaticProc_PROC_SLTIU;
					case 0x28000000u:
						return &StaticProc_PROC_SLTI;
					case 0xA4000000u:
						return &StaticProc_PROC_SH;
					case 0xA0000000u:
						return &StaticProc_PROC_SB;
					case 0x34000000u:
						return &StaticProc_PROC_ORI;
					case 0x8C000000u:
						return &StaticProc_PROC_LW;
					case 0x94000000u:
						return &StaticProc_PROC_LHU;
					case 0x84000000u:
						return &StaticProc_PROC_LH;
					case 0x90000000u:
						return &StaticProc_PROC_LBU;
					case 0x80000000u:
						return &StaticProc_PROC_LB;
					case 0x0C000000u:
						return &StaticProc_PROC_JAL;
					case 0x08000000u:
						return &StaticProc_PROC_J;
					case 0x14000000u:
						return &StaticProc_PROC_BNE;
					case 0x60000000u:
						return &StaticProc_PROC_POP30;
					case 0x20000000u:
						return &StaticProc_PROC_POP10;
					case 0x10000000u:
						return &StaticProc_PROC_BEQ;
					case 0xC8000000u:
						return &StaticProc_PROC_BC;
					case 0xE8000000u:
						return &StaticProc_PROC_BALC;
					case 0x3C000000u:
						return &StaticProc_PROC_AUI;
					case 0x30000000u:
						return &StaticProc_PROC_ANDI;
					case 0xE4000000u:
						return &StaticProc_COP1_SWC1_v;
					case 0xF4000000u:
						return &StaticProc_COP1_SDC1_v;
					case 0xC4000000u:
						return &StaticProc_COP1_LWC1_v;
					case 0xD4000000u:
						return &StaticProc_COP1_LDC1_v;
					case 0xF8000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								return &StaticProc_PROC_JIALC;
							default:
								if ((i & 0xFC000000u) == 0xF8000000u)
									return &StaticProc_PROC_BNEZC;
								return nullptr;
						}
					case 0xD8000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								return &StaticProc_PROC_JIC;
							default:
								if ((i & 0xFC000000u) == 0xD8000000u)
									return &StaticProc_PROC_BEQZC;
								return nullptr;
						}
					case 0x5C000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								return &StaticProc_PROC_BGTZC;
							default:
								if ((i & 0xFC000000u) == 0x5C000000u)
									return &StaticProc_PROC_POP27;
								return nullptr;
						}
					case 0x58000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								return &StaticProc_PROC_BLEZC;
							default:
								if ((i & 0xFC000000u) == 0x58000000u)
									return &StaticProc_PROC_POP26;
								return nullptr;
						}
					case 0x00000000u:
						switch(i & 0x0000003Fu) {
							case 0x0000000Fu:
								return &StaticProc_PROC_SYNC;
							case 0x00000009u:
								switch(i & 0x001FF800u) {
									case 0x00000000u:
										return &StaticProc_PROC_JR;
									default:
										if ((i & 0xFC1F003Fu) == 0x00000009u)
											return &StaticProc_PROC_JALR;
										return nullptr;
								}
							case 0x00000021u:
								switch(i & 0x001F07C0u) {
									case 0x00000040u:
										return &StaticProc_PROC_CLO;
									default:
										if ((i & 0xFC0007FFu) == 0x00000021u)
											return &StaticProc_PROC_ADDU;
										return nullptr;
								}
							case 0x00000020u:
								return &StaticProc_PROC_ADD;
							case 0x00000010u:
								return &StaticProc_PROC_CLZ;
							case 0x00000024u:
								return &StaticProc_PROC_AND;
							case 0x0000001Au:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										return &StaticProc_PROC_MOD;
									case 0x00000080u:
										return &StaticProc_PROC_DIV;
									default:
										return nullptr;
								}
							case 0x0000001Bu:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										return &StaticProc_PROC_MODU;
									case 0x00000080u:
										return &StaticProc_PROC_DIVU;
									default:
										return nullptr;
								}
							case 0x00000018u:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										return &StaticProc_PROC_MUH;
									case 0x00000080u:
										return &StaticProc_PROC_MUL;
									default:
										return nullptr;
								}
							case 0x00000019u:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										return &StaticProc_PROC_MUHU;
									case 0x00000080u:
										return &StaticProc_PROC_MULU;
									default:
										return nullptr;
								}
							case 0x00000027u:
								return &StaticProc_PROC_NOR;
							case 0x00000025u:
								return &StaticProc_PROC_OR;
							case 0x00000002u:
								switch(i & 0x03E00000u) {
									case 0x00000000u:
										return &StaticProc_PROC_SRL;
									case 0x00200000u:
										return &StaticProc_PROC_ROTR;
									default:
										return nullptr;
								}
							case 0x00000006u:
								switch(i & 0x000007C0u) {
									case 0x00000000u:
										return &StaticProc_PROC_SRLV;
									case 0x00000040u:
										return &StaticProc_PROC_ROTRV;
									default:
										return nullptr;
								}
							case 0x00000035u:
								return &StaticProc_PROC_SELEQZ;
							case 0x00000037u:
								return &StaticProc_PROC_SELNEZ;
							case 0x00000000u:
								return &StaticProc_PROC_SLL;
							case 0x00000004u:
								return &StaticProc_PROC_SLLV;
							case 0x0000002Au:
								return &StaticProc_PROC_SLT;
							case 0x0000002Bu:
								return &StaticProc_PROC_SLTU;
							case 0x00000003u:
								return &StaticProc_PROC_SRA;
							case 0x00000007u:
								return &StaticProc_PROC_SRAV;
							case 0x00000022u:
								return &StaticProc_PROC_SUB;
							case 0x00000023u:
								return &StaticProc_PROC_SUBU;
							case 0x00000026u:
								return &StaticProc_PROC_XOR;
							case 0x00000005u:
								return &StaticProc_PROC_LSA;
							case 0x0000000Du:
								return &StaticProc_PROC_BREAK;
							case 0x0000000Eu:
								return &StaticProc_PROC_SDBBP;
							case 0x0000000Cu:
								return &StaticProc_PROC_SYSCALL;
							case 0x00000034u:
								return &StaticProc_PROC_TEQ;
							case 0x00000030u:
								return &StaticProc_PROC_TGE;
							case 0x00000031u:
								return &StaticProc_PROC_TGEU;
							case 0x00000032u:
								return &StaticProc_PROC_TLT;
							case 0x00000033u:
								return &StaticProc_PROC_TLTU;
							case 0x00000036u:
								return &StaticProc_PROC_TNE;
							default:
								return nullptr;
						}
					case 0x44000000u:
						switch(i & 0x03E00000u) {
							case 0x01A00000u:
								return &StaticProc_COP1_BC1NEZ_v;
							case 0x01200000u:
								return &StaticProc_COP1_BC1EQZ_v;
							case 0x00600000u:
								return &StaticProc_COP1_MFHC1_v;
							case 0x00E00000u:
								return &StaticProc_COP1_MTHC1_v;
							case 0x02000000u:
								switch(i & 0x001F003Fu) {
									case 0x00000025u:
										return &StaticProc_COP1_CVT_L_f;
									case 0x00000005u:
										return &StaticProc_COP1_ABS_f;
									case 0x0000001Bu:
										return &StaticProc_COP1_CLASS_f;
									case 0x00000021u:
										return &StaticProc_COP1_CVT_D_f;
									case 0x00000020u:
										return &StaticProc_COP1_CVT_S_f;
									case 0x00000024u:
										return &StaticProc_COP1_CVT_W_f;
									case 0x00000006u:
										return &StaticProc_COP1_MOV_f;
									case 0x00000007u:
										return &StaticProc_COP1_NEG_f;
									case 0x00000015u:
										return &StaticProc_COP1_RECIP_f;
									case 0x0000001Au:
										return &StaticProc_COP1_RINT_f;
									case 0x00000016u:
										return &StaticProc_COP1_RSQRT_f;
									case 0x00000004u:
										return &StaticProc_COP1_SQRT_f;
									case 0x00000008u:
										return &StaticProc_COP1_ROUND_L_f;
									case 0x0000000Cu:
										return &StaticProc_COP1_ROUND_W_f;
									case 0x0000000Au:
										return &StaticProc_COP1_CEIL_L_f;
									case 0x0000000Eu:
										return &StaticProc_COP1_CEIL_W_f;
									case 0x0000000Bu:
										return &StaticProc_COP1_FLOOR_L_f;
									case 0x0000000Fu:
										return &StaticProc_COP1_FLOOR_W_f;
									case 0x00000009u:
										return &StaticProc_COP1_TRUNC_L_f;
									case 0x0000000Du:
										return &StaticProc_COP1_TRUNC_W_f;
									default:
										switch(i & 0xFFE0003Fu) {
											case 0x46000018u:
												return &StaticProc_COP1_MADDF_f;
											case 0x46000000u:
												return &StaticProc_COP1_ADD_f;
											case 0x46000003u:
												return &StaticProc_COP1_DIV_f;
											case 0x46000019u:
												return &StaticProc_COP1_MSUBF_f;
											case 0x4600001Eu:
												return &StaticProc_COP1_MAX_f;
											case 0x4600001Cu:
												return &StaticProc_COP1_MIN_f;
											case 0x4600001Fu:
												return &StaticProc_COP1_MAXA_f;
											case 0x4600001Du:
												return &StaticProc_COP1_MINA_f;
											case 0x46000002u:
												return &StaticProc_COP1_MUL_f;
											case 0x46000010u:
												return &StaticProc_COP1_SEL_f;
											case 0x46000014u:
												return &StaticProc_COP1_SELEQZ_f;
											case 0x46000017u:
												return &StaticProc_COP1_SELNEZ_f;
											case 0x46000001u:
												return &StaticProc_COP1_SUB_f;
											default:
												return nullptr;
										}
										return nullptr;
								}
							case 0x02200000u:
								switch(i & 0x001F003Fu) {
									case 0x00000025u:
										return &StaticProc_COP1_CVT_L_d;
									case 0x00000005u:
										return &StaticProc_COP1_ABS_d;
									case 0x0000001Bu:
										return &StaticProc_COP1_CLASS_d;
									case 0x00000021u:
										return &StaticProc_COP1_CVT_D_d;
									case 0x00000020u:
										return &StaticProc_COP1_CVT_S_d;
									case 0x00000024u:
										return &StaticProc_COP1_CVT_W_d;
									case 0x00000006u:
										return &StaticProc_COP1_MOV_d;
									case 0x00000007u:
										return &StaticProc_COP1_NEG_d;
									case 0x00000015u:
										return &StaticProc_COP1_RECIP_d;
									case 0x0000001Au:
										return &StaticProc_COP1_RINT_d;
									case 0x00000016u:
										return &StaticProc_COP1_RSQRT_d;
									case 0x00000004u:
										return &StaticProc_COP1_SQRT_d;
									case 0x00000008u:
										return &StaticProc_COP1_ROUND_L_d;
									case 0x0000000Cu:
										return &StaticProc_COP1_ROUND_W_d;
									case 0x0000000Au:
										return &StaticProc_COP1_CEIL_L_d;
									case 0x0000000Eu:
										return &StaticProc_COP1_CEIL_W_d;
									case 0x0000000Bu:
										return &StaticProc_COP1_FLOOR_L_d;
									case 0x0000000Fu:
										return &StaticProc_COP1_FLOOR_W_d;
									case 0x00000009u:
										return &StaticProc_COP1_TRUNC_L_d;
									case 0x0000000Du:
										return &StaticProc_COP1_TRUNC_W_d;
									default:
										switch(i & 0xFFE0003Fu) {
											case 0x46200018u:
												return &StaticProc_COP1_MADDF_d;
											case 0x46200000u:
												return &StaticProc_COP1_ADD_d;
											case 0x46200003u:
												return &StaticProc_COP1_DIV_d;
											case 0x46200019u:
												return &StaticProc_COP1_MSUBF_d;
											case 0x4620001Eu:
												return &StaticProc_COP1_MAX_d;
											case 0x4620001Cu:
												return &StaticProc_COP1_MIN_d;
											case 0x4620001Fu:
												return &StaticProc_COP1_MAXA_d;
											case 0x4620001Du:
												return &StaticProc_COP1_MINA_d;
											case 0x46200002u:
												return &StaticProc_COP1_MUL_d;
											case 0x46200010u:
												return &StaticProc_COP1_SEL_d;
											case 0x46200014u:
												return &StaticProc_COP1_SELEQZ_d;
											case 0x46200017u:
												return &StaticProc_COP1_SELNEZ_d;
											case 0x46200001u:
												return &StaticProc_COP1_SUB_d;
											default:
												return nullptr;
										}
										return nullptr;
								}
							case 0x02800000u:
								switch(i & 0x001F003Fu) {
									case 0x00000021u:
										return &StaticProc_COP1_CVT_D_w;
									case 0x00000020u:
										return &StaticProc_COP1_CVT_S_w;
									default:
										if ((i & 0xFFE00020u) == 0x46800000u)
											return &StaticProc_COP1_CMP_condn_s_v;
										return nullptr;
								}
							case 0x02A00000u:
								switch(i & 0x001F003Fu) {
									case 0x00000021u:
										return &StaticProc_COP1_CVT_D_l;
									case 0x00000020u:
										return &StaticProc_COP1_CVT_S_l;
									default:
										if ((i & 0xFFE00020u) == 0x46A00000u)
											return &StaticProc_COP1_CMP_condn_d_v;
										return nullptr;
								}
							case 0x00C00000u:
								return &StaticProc_COP1_CTC1_v;
							case 0x00400000u:
								return &StaticProc_COP1_CFC1_v;
							case 0x00800000u:
								return &StaticProc_COP1_MTC1_v;
							case 0x00000000u:
								return &StaticProc_COP1_MFC1_v;
							default:
								return nullptr;
						}
					case 0x24000000u:
						switch(i & 0x0000FFFFu) {
							case 0x00000000u:
								return &StaticProc_PROC_MOVE;
							default:
								if ((i & 0xFC000000u) == 0x24000000u)
									return &StaticProc_PROC_ADDIU;
								return nullptr;
						}
					case 0x7C000000u:
						switch(i & 0x03E007FFu) {
							case 0x000000A0u:
								return &StaticProc_PROC_WSBH;
							case 0x00000620u:
								return &StaticProc_PROC_SEH;
							case 0x00000420u:
								return &StaticProc_PROC_SEB;
							case 0x00000020u:
								return &StaticProc_PROC_BITSWAP;
							default:
								switch(i & 0xFC00003Fu) {
									case 0x7C00003Bu:
										return &StaticProc_PROC_RDHWR;
									case 0x7C00001Eu:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												return &StaticProc_PROC_SCWPE;
											default:
												if ((i & 0xFC00007Fu) == 0x7C00001Eu)
													return &StaticProc_PROC_SCE;
												return nullptr;
										}
									case 0x7C000026u:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												return &StaticProc_PROC_SCWP;
											default:
												if ((i & 0xFC00007Fu) == 0x7C000026u)
													return &StaticProc_PROC_SC;
												return nullptr;
										}
									case 0x7C00002Eu:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												return &StaticProc_PROC_LLWPE;
											default:
												if ((i & 0xFC00007Fu) == 0x7C00002Eu)
													return &StaticProc_PROC_LLE;
												return nullptr;
										}
									case 0x7C000036u:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												return &StaticProc_PROC_LLWP;
											default:
												if ((i & 0xFC00007Fu) == 0x7C000036u)
													return &StaticProc_PROC_LL;
												return nullptr;
										}
									case 0x7C000020u:
										return &StaticProc_PROC_ALIGN;
									case 0x7C000025u:
										return &StaticProc_PROC_CACHE;
									case 0x7C00002Cu:
										return &StaticProc_PROC_LBE;
									case 0x7C000028u:
										return &StaticProc_PROC_LBUE;
									case 0x7C00002Du:
										return &StaticProc_PROC_LHE;
									case 0x7C000029u:
										return &StaticProc_PROC_LHUE;
									case 0x7C00002Fu:
										return &StaticProc_PROC_LWE;
									case 0x7C000035u:
										return &StaticProc_PROC_PREF;
									case 0x7C000023u:
										return &StaticProc_PROC_PREFE;
									case 0x7C00001Cu:
										return &StaticProc_PROC_SBE;
									case 0x7C00001Du:
										return &StaticProc_PROC_SHE;
									case 0x7C00001Fu:
										return &StaticProc_PROC_SWE;
									case 0x7C000000u:
										return &StaticProc_PROC_EXT;
									case 0x7C000004u:
										return &StaticProc_PROC_INS;
									default:
										return nullptr;
								}
								return nullptr;
						}
					case 0x04000000u:
						switch(i & 0x03FF0000u) {
							case 0x00170000u:
								return &StaticProc_PROC_SIGRIE;
							case 0x00100000u:
								return &StaticProc_PROC_NAL;
							case 0x00110000u:
								return &StaticProc_PROC_BAL;
							default:
								switch(i & 0xFC1F0000u) {
									case 0x041F0000u:
										return &StaticProc_PROC_SYNCI;
									case 0x04000000u:
										return &StaticProc_PROC_BLTZ;
									case 0x04010000u:
										return &StaticProc_PROC_BGEZ;
									default:
										return nullptr;
								}
								return nullptr;
						}
					case 0xEC000000u:
						switch(i & 0x001F0000u) {
							case 0x001E0000u:
								return &StaticProc_PROC_AUIPC;
							case 0x001F0000u:
								return &StaticProc_PROC_ALUIPC;
							default:
								switch(i & 0xFC180000u) {
									case 0xEC080000u:
										return &StaticProc_PROC_LWPC;
									case 0xEC000000u:
										return &StaticProc_PROC_ADDIUPC;
									default:
										return nullptr;
								}
								return nullptr;
						}
					case 0x18000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								return &StaticProc_PROC_BLEZALC;
							default:
								switch(i & 0xFC1F0000u) {
									case 0x18000000u:
										return &StaticProc_PROC_BLEZ;
									default:
										if ((i & 0xFC000000u) == 0x18000000u)
											return &StaticProc_PROC_POP06;
										return nullptr;
								}
								return nullptr;
						}
					case 0x1C000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								return &StaticProc_PROC_BGTZALC;
							default:
								switch(i & 0xFC1F0000u) {
									case 0x1C000000u:
										return &StaticProc_PROC_BGTZ;
									default:
										if ((i & 0xFC000000u) == 0x1C000000u)
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
			case 0x00000040u:
				{ PROC_SSNOP_NS::Execute(i,p); return true; }
			case 0x00000140u:
				{ PROC_PAUSE_NS::Execute(i,p); return true; }
			case 0x00000000u:
				{ PROC_NOP_NS::Execute(i,p); return true; }
			case 0x000000C0u:
				{ PROC_EHB_NS::Execute(i,p); return true; }
			default:
				switch(i & 0xFC000000u) {
					case 0x38000000u:
						{ PROC_XORI_NS::Execute(i,p); return true; }
					case 0xAC000000u:
						{ PROC_SW_NS::Execute(i,p); return true; }
					case 0x2C000000u:
						{ PROC_SLTIU_NS::Execute(i,p); return true; }
					case 0x28000000u:
						{ PROC_SLTI_NS::Execute(i,p); return true; }
					case 0xA4000000u:
						{ PROC_SH_NS::Execute(i,p); return true; }
					case 0xA0000000u:
						{ PROC_SB_NS::Execute(i,p); return true; }
					case 0x34000000u:
						{ PROC_ORI_NS::Execute(i,p); return true; }
					case 0x8C000000u:
						{ PROC_LW_NS::Execute(i,p); return true; }
					case 0x94000000u:
						{ PROC_LHU_NS::Execute(i,p); return true; }
					case 0x84000000u:
						{ PROC_LH_NS::Execute(i,p); return true; }
					case 0x90000000u:
						{ PROC_LBU_NS::Execute(i,p); return true; }
					case 0x80000000u:
						{ PROC_LB_NS::Execute(i,p); return true; }
					case 0x0C000000u:
						{ PROC_JAL_NS::Execute(i,p); return true; }
					case 0x08000000u:
						{ PROC_J_NS::Execute(i,p); return true; }
					case 0x14000000u:
						{ PROC_BNE_NS::Execute(i,p); return true; }
					case 0x60000000u:
						{ PROC_POP30_NS::Execute(i,p); return true; }
					case 0x20000000u:
						{ PROC_POP10_NS::Execute(i,p); return true; }
					case 0x10000000u:
						{ PROC_BEQ_NS::Execute(i,p); return true; }
					case 0xC8000000u:
						{ PROC_BC_NS::Execute(i,p); return true; }
					case 0xE8000000u:
						{ PROC_BALC_NS::Execute(i,p); return true; }
					case 0x3C000000u:
						{ PROC_AUI_NS::Execute(i,p); return true; }
					case 0x30000000u:
						{ PROC_ANDI_NS::Execute(i,p); return true; }
					case 0xE4000000u:
						{ COP1_SWC1_NS::Execute<void>(i,p); return true; }
					case 0xF4000000u:
						{ COP1_SDC1_NS::Execute<void>(i,p); return true; }
					case 0xC4000000u:
						{ COP1_LWC1_NS::Execute<void>(i,p); return true; }
					case 0xD4000000u:
						{ COP1_LDC1_NS::Execute<void>(i,p); return true; }
					case 0xF8000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								{ PROC_JIALC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000u) == 0xF8000000u)
									{ PROC_BNEZC_NS::Execute(i,p); return true; }
								return false;
						}
					case 0xD8000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								{ PROC_JIC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000u) == 0xD8000000u)
									{ PROC_BEQZC_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x5C000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								{ PROC_BGTZC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000u) == 0x5C000000u)
									{ PROC_POP27_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x58000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								{ PROC_BLEZC_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000u) == 0x58000000u)
									{ PROC_POP26_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x00000000u:
						switch(i & 0x0000003Fu) {
							case 0x0000000Fu:
								{ PROC_SYNC_NS::Execute(i,p); return true; }
							case 0x00000009u:
								switch(i & 0x001FF800u) {
									case 0x00000000u:
										{ PROC_JR_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC1F003Fu) == 0x00000009u)
											{ PROC_JALR_NS::Execute(i,p); return true; }
										return false;
								}
							case 0x00000021u:
								switch(i & 0x001F07C0u) {
									case 0x00000040u:
										{ PROC_CLO_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC0007FFu) == 0x00000021u)
											{ PROC_ADDU_NS::Execute(i,p); return true; }
										return false;
								}
							case 0x00000020u:
								{ PROC_ADD_NS::Execute(i,p); return true; }
							case 0x00000010u:
								{ PROC_CLZ_NS::Execute(i,p); return true; }
							case 0x00000024u:
								{ PROC_AND_NS::Execute(i,p); return true; }
							case 0x0000001Au:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										{ PROC_MOD_NS::Execute(i,p); return true; }
									case 0x00000080u:
										{ PROC_DIV_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x0000001Bu:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										{ PROC_MODU_NS::Execute(i,p); return true; }
									case 0x00000080u:
										{ PROC_DIVU_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000018u:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										{ PROC_MUH_NS::Execute(i,p); return true; }
									case 0x00000080u:
										{ PROC_MUL_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000019u:
								switch(i & 0x000007C0u) {
									case 0x000000C0u:
										{ PROC_MUHU_NS::Execute(i,p); return true; }
									case 0x00000080u:
										{ PROC_MULU_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000027u:
								{ PROC_NOR_NS::Execute(i,p); return true; }
							case 0x00000025u:
								{ PROC_OR_NS::Execute(i,p); return true; }
							case 0x00000002u:
								switch(i & 0x03E00000u) {
									case 0x00000000u:
										{ PROC_SRL_NS::Execute(i,p); return true; }
									case 0x00200000u:
										{ PROC_ROTR_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000006u:
								switch(i & 0x000007C0u) {
									case 0x00000000u:
										{ PROC_SRLV_NS::Execute(i,p); return true; }
									case 0x00000040u:
										{ PROC_ROTRV_NS::Execute(i,p); return true; }
									default:
										return false;
								}
							case 0x00000035u:
								{ PROC_SELEQZ_NS::Execute(i,p); return true; }
							case 0x00000037u:
								{ PROC_SELNEZ_NS::Execute(i,p); return true; }
							case 0x00000000u:
								{ PROC_SLL_NS::Execute(i,p); return true; }
							case 0x00000004u:
								{ PROC_SLLV_NS::Execute(i,p); return true; }
							case 0x0000002Au:
								{ PROC_SLT_NS::Execute(i,p); return true; }
							case 0x0000002Bu:
								{ PROC_SLTU_NS::Execute(i,p); return true; }
							case 0x00000003u:
								{ PROC_SRA_NS::Execute(i,p); return true; }
							case 0x00000007u:
								{ PROC_SRAV_NS::Execute(i,p); return true; }
							case 0x00000022u:
								{ PROC_SUB_NS::Execute(i,p); return true; }
							case 0x00000023u:
								{ PROC_SUBU_NS::Execute(i,p); return true; }
							case 0x00000026u:
								{ PROC_XOR_NS::Execute(i,p); return true; }
							case 0x00000005u:
								{ PROC_LSA_NS::Execute(i,p); return true; }
							case 0x0000000Du:
								{ PROC_BREAK_NS::Execute(i,p); return true; }
							case 0x0000000Eu:
								{ PROC_SDBBP_NS::Execute(i,p); return true; }
							case 0x0000000Cu:
								{ PROC_SYSCALL_NS::Execute(i,p); return true; }
							case 0x00000034u:
								{ PROC_TEQ_NS::Execute(i,p); return true; }
							case 0x00000030u:
								{ PROC_TGE_NS::Execute(i,p); return true; }
							case 0x00000031u:
								{ PROC_TGEU_NS::Execute(i,p); return true; }
							case 0x00000032u:
								{ PROC_TLT_NS::Execute(i,p); return true; }
							case 0x00000033u:
								{ PROC_TLTU_NS::Execute(i,p); return true; }
							case 0x00000036u:
								{ PROC_TNE_NS::Execute(i,p); return true; }
							default:
								return false;
						}
					case 0x44000000u:
						switch(i & 0x03E00000u) {
							case 0x01A00000u:
								{ COP1_BC1NEZ_NS::Execute<void>(i,p); return true; }
							case 0x01200000u:
								{ COP1_BC1EQZ_NS::Execute<void>(i,p); return true; }
							case 0x00600000u:
								{ COP1_MFHC1_NS::Execute<void>(i,p); return true; }
							case 0x00E00000u:
								{ COP1_MTHC1_NS::Execute<void>(i,p); return true; }
							case 0x02000000u:
								switch(i & 0x001F003Fu) {
									case 0x00000025u:
										{ COP1_CVT_L_NS::Execute<float>(i,p); return true; }
									case 0x00000005u:
										{ COP1_ABS_NS::Execute<float>(i,p); return true; }
									case 0x0000001Bu:
										{ COP1_CLASS_NS::Execute<float>(i,p); return true; }
									case 0x00000021u:
										{ COP1_CVT_D_NS::Execute<float>(i,p); return true; }
									case 0x00000020u:
										{ COP1_CVT_S_NS::Execute<float>(i,p); return true; }
									case 0x00000024u:
										{ COP1_CVT_W_NS::Execute<float>(i,p); return true; }
									case 0x00000006u:
										{ COP1_MOV_NS::Execute<float>(i,p); return true; }
									case 0x00000007u:
										{ COP1_NEG_NS::Execute<float>(i,p); return true; }
									case 0x00000015u:
										{ COP1_RECIP_NS::Execute<float>(i,p); return true; }
									case 0x0000001Au:
										{ COP1_RINT_NS::Execute<float>(i,p); return true; }
									case 0x00000016u:
										{ COP1_RSQRT_NS::Execute<float>(i,p); return true; }
									case 0x00000004u:
										{ COP1_SQRT_NS::Execute<float>(i,p); return true; }
									case 0x00000008u:
										{ COP1_ROUND_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000Cu:
										{ COP1_ROUND_W_NS::Execute<float>(i,p); return true; }
									case 0x0000000Au:
										{ COP1_CEIL_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000Eu:
										{ COP1_CEIL_W_NS::Execute<float>(i,p); return true; }
									case 0x0000000Bu:
										{ COP1_FLOOR_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000Fu:
										{ COP1_FLOOR_W_NS::Execute<float>(i,p); return true; }
									case 0x00000009u:
										{ COP1_TRUNC_L_NS::Execute<float>(i,p); return true; }
									case 0x0000000Du:
										{ COP1_TRUNC_W_NS::Execute<float>(i,p); return true; }
									default:
										switch(i & 0xFFE0003Fu) {
											case 0x46000018u:
												{ COP1_MADDF_NS::Execute<float>(i,p); return true; }
											case 0x46000000u:
												{ COP1_ADD_NS::Execute<float>(i,p); return true; }
											case 0x46000003u:
												{ COP1_DIV_NS::Execute<float>(i,p); return true; }
											case 0x46000019u:
												{ COP1_MSUBF_NS::Execute<float>(i,p); return true; }
											case 0x4600001Eu:
												{ COP1_MAX_NS::Execute<float>(i,p); return true; }
											case 0x4600001Cu:
												{ COP1_MIN_NS::Execute<float>(i,p); return true; }
											case 0x4600001Fu:
												{ COP1_MAXA_NS::Execute<float>(i,p); return true; }
											case 0x4600001Du:
												{ COP1_MINA_NS::Execute<float>(i,p); return true; }
											case 0x46000002u:
												{ COP1_MUL_NS::Execute<float>(i,p); return true; }
											case 0x46000010u:
												{ COP1_SEL_NS::Execute<float>(i,p); return true; }
											case 0x46000014u:
												{ COP1_SELEQZ_NS::Execute<float>(i,p); return true; }
											case 0x46000017u:
												{ COP1_SELNEZ_NS::Execute<float>(i,p); return true; }
											case 0x46000001u:
												{ COP1_SUB_NS::Execute<float>(i,p); return true; }
											default:
												return false;
										}
										return false;
								}
							case 0x02200000u:
								switch(i & 0x001F003Fu) {
									case 0x00000025u:
										{ COP1_CVT_L_NS::Execute<double>(i,p); return true; }
									case 0x00000005u:
										{ COP1_ABS_NS::Execute<double>(i,p); return true; }
									case 0x0000001Bu:
										{ COP1_CLASS_NS::Execute<double>(i,p); return true; }
									case 0x00000021u:
										{ COP1_CVT_D_NS::Execute<double>(i,p); return true; }
									case 0x00000020u:
										{ COP1_CVT_S_NS::Execute<double>(i,p); return true; }
									case 0x00000024u:
										{ COP1_CVT_W_NS::Execute<double>(i,p); return true; }
									case 0x00000006u:
										{ COP1_MOV_NS::Execute<double>(i,p); return true; }
									case 0x00000007u:
										{ COP1_NEG_NS::Execute<double>(i,p); return true; }
									case 0x00000015u:
										{ COP1_RECIP_NS::Execute<double>(i,p); return true; }
									case 0x0000001Au:
										{ COP1_RINT_NS::Execute<double>(i,p); return true; }
									case 0x00000016u:
										{ COP1_RSQRT_NS::Execute<double>(i,p); return true; }
									case 0x00000004u:
										{ COP1_SQRT_NS::Execute<double>(i,p); return true; }
									case 0x00000008u:
										{ COP1_ROUND_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000Cu:
										{ COP1_ROUND_W_NS::Execute<double>(i,p); return true; }
									case 0x0000000Au:
										{ COP1_CEIL_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000Eu:
										{ COP1_CEIL_W_NS::Execute<double>(i,p); return true; }
									case 0x0000000Bu:
										{ COP1_FLOOR_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000Fu:
										{ COP1_FLOOR_W_NS::Execute<double>(i,p); return true; }
									case 0x00000009u:
										{ COP1_TRUNC_L_NS::Execute<double>(i,p); return true; }
									case 0x0000000Du:
										{ COP1_TRUNC_W_NS::Execute<double>(i,p); return true; }
									default:
										switch(i & 0xFFE0003Fu) {
											case 0x46200018u:
												{ COP1_MADDF_NS::Execute<double>(i,p); return true; }
											case 0x46200000u:
												{ COP1_ADD_NS::Execute<double>(i,p); return true; }
											case 0x46200003u:
												{ COP1_DIV_NS::Execute<double>(i,p); return true; }
											case 0x46200019u:
												{ COP1_MSUBF_NS::Execute<double>(i,p); return true; }
											case 0x4620001Eu:
												{ COP1_MAX_NS::Execute<double>(i,p); return true; }
											case 0x4620001Cu:
												{ COP1_MIN_NS::Execute<double>(i,p); return true; }
											case 0x4620001Fu:
												{ COP1_MAXA_NS::Execute<double>(i,p); return true; }
											case 0x4620001Du:
												{ COP1_MINA_NS::Execute<double>(i,p); return true; }
											case 0x46200002u:
												{ COP1_MUL_NS::Execute<double>(i,p); return true; }
											case 0x46200010u:
												{ COP1_SEL_NS::Execute<double>(i,p); return true; }
											case 0x46200014u:
												{ COP1_SELEQZ_NS::Execute<double>(i,p); return true; }
											case 0x46200017u:
												{ COP1_SELNEZ_NS::Execute<double>(i,p); return true; }
											case 0x46200001u:
												{ COP1_SUB_NS::Execute<double>(i,p); return true; }
											default:
												return false;
										}
										return false;
								}
							case 0x02800000u:
								switch(i & 0x001F003Fu) {
									case 0x00000021u:
										{ COP1_CVT_D_NS::Execute<int32>(i,p); return true; }
									case 0x00000020u:
										{ COP1_CVT_S_NS::Execute<int32>(i,p); return true; }
									default:
										if ((i & 0xFFE00020u) == 0x46800000u)
											{ COP1_CMP_condn_s_NS::Execute<void>(i,p); return true; }
										return false;
								}
							case 0x02A00000u:
								switch(i & 0x001F003Fu) {
									case 0x00000021u:
										{ COP1_CVT_D_NS::Execute<int64>(i,p); return true; }
									case 0x00000020u:
										{ COP1_CVT_S_NS::Execute<int64>(i,p); return true; }
									default:
										if ((i & 0xFFE00020u) == 0x46A00000u)
											{ COP1_CMP_condn_d_NS::Execute<void>(i,p); return true; }
										return false;
								}
							case 0x00C00000u:
								{ COP1_CTC1_NS::Execute<void>(i,p); return true; }
							case 0x00400000u:
								{ COP1_CFC1_NS::Execute<void>(i,p); return true; }
							case 0x00800000u:
								{ COP1_MTC1_NS::Execute<void>(i,p); return true; }
							case 0x00000000u:
								{ COP1_MFC1_NS::Execute<void>(i,p); return true; }
							default:
								return false;
						}
					case 0x24000000u:
						switch(i & 0x0000FFFFu) {
							case 0x00000000u:
								{ PROC_MOVE_NS::Execute(i,p); return true; }
							default:
								if ((i & 0xFC000000u) == 0x24000000u)
									{ PROC_ADDIU_NS::Execute(i,p); return true; }
								return false;
						}
					case 0x7C000000u:
						switch(i & 0x03E007FFu) {
							case 0x000000A0u:
								{ PROC_WSBH_NS::Execute(i,p); return true; }
							case 0x00000620u:
								{ PROC_SEH_NS::Execute(i,p); return true; }
							case 0x00000420u:
								{ PROC_SEB_NS::Execute(i,p); return true; }
							case 0x00000020u:
								{ PROC_BITSWAP_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC00003Fu) {
									case 0x7C00003Bu:
										{ PROC_RDHWR_NS::Execute(i,p); return true; }
									case 0x7C00001Eu:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												{ PROC_SCWPE_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007Fu) == 0x7C00001Eu)
													{ PROC_SCE_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C000026u:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												{ PROC_SCWP_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007Fu) == 0x7C000026u)
													{ PROC_SC_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C00002Eu:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												{ PROC_LLWPE_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007Fu) == 0x7C00002Eu)
													{ PROC_LLE_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C000036u:
										switch(i & 0x000007C0u) {
											case 0x00000040u:
												{ PROC_LLWP_NS::Execute(i,p); return true; }
											default:
												if ((i & 0xFC00007Fu) == 0x7C000036u)
													{ PROC_LL_NS::Execute(i,p); return true; }
												return false;
										}
									case 0x7C000020u:
										{ PROC_ALIGN_NS::Execute(i,p); return true; }
									case 0x7C000025u:
										{ PROC_CACHE_NS::Execute(i,p); return true; }
									case 0x7C00002Cu:
										{ PROC_LBE_NS::Execute(i,p); return true; }
									case 0x7C000028u:
										{ PROC_LBUE_NS::Execute(i,p); return true; }
									case 0x7C00002Du:
										{ PROC_LHE_NS::Execute(i,p); return true; }
									case 0x7C000029u:
										{ PROC_LHUE_NS::Execute(i,p); return true; }
									case 0x7C00002Fu:
										{ PROC_LWE_NS::Execute(i,p); return true; }
									case 0x7C000035u:
										{ PROC_PREF_NS::Execute(i,p); return true; }
									case 0x7C000023u:
										{ PROC_PREFE_NS::Execute(i,p); return true; }
									case 0x7C00001Cu:
										{ PROC_SBE_NS::Execute(i,p); return true; }
									case 0x7C00001Du:
										{ PROC_SHE_NS::Execute(i,p); return true; }
									case 0x7C00001Fu:
										{ PROC_SWE_NS::Execute(i,p); return true; }
									case 0x7C000000u:
										{ PROC_EXT_NS::Execute(i,p); return true; }
									case 0x7C000004u:
										{ PROC_INS_NS::Execute(i,p); return true; }
									default:
										return false;
								}
								return false;
						}
					case 0x04000000u:
						switch(i & 0x03FF0000u) {
							case 0x00170000u:
								{ PROC_SIGRIE_NS::Execute(i,p); return true; }
							case 0x00100000u:
								{ PROC_NAL_NS::Execute(i,p); return true; }
							case 0x00110000u:
								{ PROC_BAL_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC1F0000u) {
									case 0x041F0000u:
										{ PROC_SYNCI_NS::Execute(i,p); return true; }
									case 0x04000000u:
										{ PROC_BLTZ_NS::Execute(i,p); return true; }
									case 0x04010000u:
										{ PROC_BGEZ_NS::Execute(i,p); return true; }
									default:
										return false;
								}
								return false;
						}
					case 0xEC000000u:
						switch(i & 0x001F0000u) {
							case 0x001E0000u:
								{ PROC_AUIPC_NS::Execute(i,p); return true; }
							case 0x001F0000u:
								{ PROC_ALUIPC_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC180000u) {
									case 0xEC080000u:
										{ PROC_LWPC_NS::Execute(i,p); return true; }
									case 0xEC000000u:
										{ PROC_ADDIUPC_NS::Execute(i,p); return true; }
									default:
										return false;
								}
								return false;
						}
					case 0x18000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								{ PROC_BLEZALC_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC1F0000u) {
									case 0x18000000u:
										{ PROC_BLEZ_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC000000u) == 0x18000000u)
											{ PROC_POP06_NS::Execute(i,p); return true; }
										return false;
								}
								return false;
						}
					case 0x1C000000u:
						switch(i & 0x03E00000u) {
							case 0x00000000u:
								{ PROC_BGTZALC_NS::Execute(i,p); return true; }
							default:
								switch(i & 0xFC1F0000u) {
									case 0x1C000000u:
										{ PROC_BGTZ_NS::Execute(i,p); return true; }
									default:
										if ((i & 0xFC000000u) == 0x1C000000u)
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
