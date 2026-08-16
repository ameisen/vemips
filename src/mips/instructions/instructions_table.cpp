// ReSharper disable CppClangTidyClangDiagnosticUndefinedFuncTemplate CppClangTidyClangDiagnosticUnreachableCodeReturn
// ReSharper disable CppDefaultCaseNotHandledInSwitchStatement CppEnforceIfStatementBraces CppRedundantElseKeyword
// ReSharper disable CommentTypo CppUnreachableCode IdentifierTypo StringLiteralTypo

/* WARNING: THIS IS AN AUTO-GENERATED FILE ('D:\Projects\vemips\src\tablegen\writers.cpp', Aug 16 2026 16:22:11) */
#include "pch.hpp"
#include <array>

#include <type_traits>
#include "mips/instructions/instructions_table.hpp"
#include "mips/mips_common.hpp"
#include "mips/instructions/instructions_common.hpp"
#include "mips/processor/processor.hpp"

#define VEMIPS_ITABLE_EXPORT 

namespace mips::instructions {
	// ReSharper disable CppCStyleCast
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ABS_f ( "COP1_ABS", 1, (const instructionexec_t)&COP1_ABS_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ABS_d ( "COP1_ABS", 1, (const instructionexec_t)&COP1_ABS_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ADD_f ( "COP1_ADD", 1, &COP1_ADD_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ADD_d ( "COP1_ADD", 1, &COP1_ADD_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_BC1EQZ_v ( "COP1_BC1EQZ", 1, &COP1_BC1EQZ_NS::Execute<void>, static_cast<OpFlags>(0x02172000), { .control = true }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_BC1NEZ_v ( "COP1_BC1NEZ", 1, &COP1_BC1NEZ_NS::Execute<void>, static_cast<OpFlags>(0x02172000), { .control = true }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_L_f ( "COP1_CEIL_L", 1, &COP1_CEIL_L_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_L_d ( "COP1_CEIL_L", 1, &COP1_CEIL_L_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_W_f ( "COP1_CEIL_W", 1, &COP1_CEIL_W_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CEIL_W_d ( "COP1_CEIL_W", 1, &COP1_CEIL_W_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CFC1_v ( "COP1_CFC1", 1, &COP1_CFC1_NS::Execute<void>, static_cast<OpFlags>(0x03012003), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CLASS_f ( "COP1_CLASS", 1, (const instructionexec_t)&COP1_CLASS_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CLASS_d ( "COP1_CLASS", 1, (const instructionexec_t)&COP1_CLASS_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CMP_condn_d_v ( "COP1_CMP_condn_d", 1, &COP1_CMP_condn_d_NS::Execute<void>, static_cast<OpFlags>(0x42012002), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CMP_condn_s_v ( "COP1_CMP_condn_s", 1, &COP1_CMP_condn_s_NS::Execute<void>, static_cast<OpFlags>(0x42012002), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CTC1_v ( "COP1_CTC1", 1, &COP1_CTC1_NS::Execute<void>, static_cast<OpFlags>(0x0281207E), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_w ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<int32>, static_cast<OpFlags>(0x02000047), { .control = false }, instruction_type::word_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_l ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<int64>, static_cast<OpFlags>(0x02000047), { .control = false }, instruction_type::long_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_f ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<float>, static_cast<OpFlags>(0x02000047), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_D_d ( "COP1_CVT_D", 1, &COP1_CVT_D_NS::Execute<double>, static_cast<OpFlags>(0x02000047), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_L_f ( "COP1_CVT_L", 1, &COP1_CVT_L_NS::Execute<float>, static_cast<OpFlags>(0x02000083), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_L_d ( "COP1_CVT_L", 1, &COP1_CVT_L_NS::Execute<double>, static_cast<OpFlags>(0x02000083), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_w ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<int32>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::word_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_l ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<int64>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::long_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_f ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_S_d ( "COP1_CVT_S", 1, &COP1_CVT_S_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_W_f ( "COP1_CVT_W", 1, &COP1_CVT_W_NS::Execute<float>, static_cast<OpFlags>(0x02000083), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_CVT_W_d ( "COP1_CVT_W", 1, &COP1_CVT_W_NS::Execute<double>, static_cast<OpFlags>(0x02000083), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_DIV_f ( "COP1_DIV", 1, &COP1_DIV_NS::Execute<float>, static_cast<OpFlags>(0x0200007F), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_DIV_d ( "COP1_DIV", 1, &COP1_DIV_NS::Execute<double>, static_cast<OpFlags>(0x0200007F), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_L_f ( "COP1_FLOOR_L", 1, &COP1_FLOOR_L_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_L_d ( "COP1_FLOOR_L", 1, &COP1_FLOOR_L_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_W_f ( "COP1_FLOOR_W", 1, &COP1_FLOOR_W_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_FLOOR_W_d ( "COP1_FLOOR_W", 1, &COP1_FLOOR_W_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_LDC1_v ( "COP1_LDC1", 1, &COP1_LDC1_NS::Execute<void>, static_cast<OpFlags>(0x12A12000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_LWC1_v ( "COP1_LWC1", 1, &COP1_LWC1_NS::Execute<void>, static_cast<OpFlags>(0x12A12000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MADDF_f ( "COP1_MADDF", 1, &COP1_MADDF_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MADDF_d ( "COP1_MADDF", 1, &COP1_MADDF_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAX_f ( "COP1_MAX", 1, &COP1_MAX_NS::Execute<float>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAX_d ( "COP1_MAX", 1, &COP1_MAX_NS::Execute<double>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAXA_f ( "COP1_MAXA", 1, &COP1_MAXA_NS::Execute<float>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MAXA_d ( "COP1_MAXA", 1, &COP1_MAXA_NS::Execute<double>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MFC1_v ( "COP1_MFC1", 1, (const instructionexec_t)&COP1_MFC1_NS::Execute<void>, static_cast<OpFlags>(0x03012000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MFHC1_v ( "COP1_MFHC1", 1, (const instructionexec_t)&COP1_MFHC1_NS::Execute<void>, static_cast<OpFlags>(0x03012000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MIN_f ( "COP1_MIN", 1, &COP1_MIN_NS::Execute<float>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MIN_d ( "COP1_MIN", 1, &COP1_MIN_NS::Execute<double>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MINA_f ( "COP1_MINA", 1, &COP1_MINA_NS::Execute<float>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MINA_d ( "COP1_MINA", 1, &COP1_MINA_NS::Execute<double>, static_cast<OpFlags>(0x02012007), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MOV_f ( "COP1_MOV", 1, (const instructionexec_t)&COP1_MOV_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MOV_d ( "COP1_MOV", 1, (const instructionexec_t)&COP1_MOV_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MSUBF_f ( "COP1_MSUBF", 1, &COP1_MSUBF_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MSUBF_d ( "COP1_MSUBF", 1, &COP1_MSUBF_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MTC1_v ( "COP1_MTC1", 1, (const instructionexec_t)&COP1_MTC1_NS::Execute<void>, static_cast<OpFlags>(0x02812000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MTHC1_v ( "COP1_MTHC1", 1, (const instructionexec_t)&COP1_MTHC1_NS::Execute<void>, static_cast<OpFlags>(0x02812000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MUL_f ( "COP1_MUL", 1, &COP1_MUL_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_MUL_d ( "COP1_MUL", 1, &COP1_MUL_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_NEG_f ( "COP1_NEG", 1, (const instructionexec_t)&COP1_NEG_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_NEG_d ( "COP1_NEG", 1, (const instructionexec_t)&COP1_NEG_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RECIP_f ( "COP1_RECIP", 1, &COP1_RECIP_NS::Execute<float>, static_cast<OpFlags>(0x0200007F), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RECIP_d ( "COP1_RECIP", 1, &COP1_RECIP_NS::Execute<double>, static_cast<OpFlags>(0x0200007F), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RINT_f ( "COP1_RINT", 1, &COP1_RINT_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RINT_d ( "COP1_RINT", 1, &COP1_RINT_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_L_f ( "COP1_ROUND_L", 1, &COP1_ROUND_L_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_L_d ( "COP1_ROUND_L", 1, &COP1_ROUND_L_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_W_f ( "COP1_ROUND_W", 1, &COP1_ROUND_W_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_ROUND_W_d ( "COP1_ROUND_W", 1, &COP1_ROUND_W_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RSQRT_f ( "COP1_RSQRT", 1, &COP1_RSQRT_NS::Execute<float>, static_cast<OpFlags>(0x0200007F), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_RSQRT_d ( "COP1_RSQRT", 1, &COP1_RSQRT_NS::Execute<double>, static_cast<OpFlags>(0x0200007F), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SDC1_v ( "COP1_SDC1", 1, &COP1_SDC1_NS::Execute<void>, static_cast<OpFlags>(0x12C12000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SEL_f ( "COP1_SEL", 1, (const instructionexec_t)&COP1_SEL_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SEL_d ( "COP1_SEL", 1, (const instructionexec_t)&COP1_SEL_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELEQZ_f ( "COP1_SELEQZ", 1, (const instructionexec_t)&COP1_SELEQZ_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELEQZ_d ( "COP1_SELEQZ", 1, (const instructionexec_t)&COP1_SELEQZ_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELNEZ_f ( "COP1_SELNEZ", 1, (const instructionexec_t)&COP1_SELNEZ_NS::Execute<float>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SELNEZ_d ( "COP1_SELNEZ", 1, (const instructionexec_t)&COP1_SELNEZ_NS::Execute<double>, static_cast<OpFlags>(0x02012000), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SQRT_f ( "COP1_SQRT", 1, &COP1_SQRT_NS::Execute<float>, static_cast<OpFlags>(0x02000047), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SQRT_d ( "COP1_SQRT", 1, &COP1_SQRT_NS::Execute<double>, static_cast<OpFlags>(0x02000047), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SUB_f ( "COP1_SUB", 1, &COP1_SUB_NS::Execute<float>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SUB_d ( "COP1_SUB", 1, &COP1_SUB_NS::Execute<double>, static_cast<OpFlags>(0x02000077), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_SWC1_v ( "COP1_SWC1", 1, &COP1_SWC1_NS::Execute<void>, static_cast<OpFlags>(0x12C12000), { .control = false }, instruction_type::void_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_L_f ( "COP1_TRUNC_L", 1, &COP1_TRUNC_L_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_L_d ( "COP1_TRUNC_L", 1, &COP1_TRUNC_L_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_W_f ( "COP1_TRUNC_W", 1, &COP1_TRUNC_W_NS::Execute<float>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::single_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_COP1_TRUNC_W_d ( "COP1_TRUNC_W", 1, &COP1_TRUNC_W_NS::Execute<double>, static_cast<OpFlags>(0x020000C7), { .control = false }, instruction_type::double_fp);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADD ( "PROC_ADD", 1, &PROC_ADD_NS::Execute, static_cast<OpFlags>(0x01800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDIU ( "PROC_ADDIU", 1, &PROC_ADDIU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDIUPC ( "PROC_ADDIUPC", 1, &PROC_ADDIUPC_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ADDU ( "PROC_ADDU", 1, &PROC_ADDU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ALIGN ( "PROC_ALIGN", 1, &PROC_ALIGN_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ALUIPC ( "PROC_ALUIPC", 1, &PROC_ALUIPC_NS::Execute, static_cast<OpFlags>(0x01000000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AND ( "PROC_AND", 1, &PROC_AND_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ANDI ( "PROC_ANDI", 1, &PROC_ANDI_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AUI ( "PROC_AUI", 1, &PROC_AUI_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_AUIPC ( "PROC_AUIPC", 1, &PROC_AUIPC_NS::Execute, static_cast<OpFlags>(0x01000000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BAL ( "PROC_BAL", 1, &PROC_BAL_NS::Execute, static_cast<OpFlags>(0x00160000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BALC ( "PROC_BALC", 1, &PROC_BALC_NS::Execute, static_cast<OpFlags>(0x000A0000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BC ( "PROC_BC", 1, &PROC_BC_NS::Execute, static_cast<OpFlags>(0x000A0000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BEQ ( "PROC_BEQ", 1, &PROC_BEQ_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BEQZC ( "PROC_BEQZC", 1, &PROC_BEQZC_NS::Execute, static_cast<OpFlags>(0x009A0000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGEZ ( "PROC_BGEZ", 1, &PROC_BGEZ_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZ ( "PROC_BGTZ", 1, &PROC_BGTZ_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZALC ( "PROC_BGTZALC", 1, &PROC_BGTZALC_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BGTZC ( "PROC_BGTZC", 1, &PROC_BGTZC_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BITSWAP ( "PROC_BITSWAP", 1, &PROC_BITSWAP_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZ ( "PROC_BLEZ", 1, &PROC_BLEZ_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZALC ( "PROC_BLEZALC", 1, &PROC_BLEZALC_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLEZC ( "PROC_BLEZC", 1, &PROC_BLEZC_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BLTZ ( "PROC_BLTZ", 1, &PROC_BLTZ_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BNE ( "PROC_BNE", 1, &PROC_BNE_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BNEZC ( "PROC_BNEZC", 1, &PROC_BNEZC_NS::Execute, static_cast<OpFlags>(0x009A0000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_BREAK ( "PROC_BREAK", 1, &PROC_BREAK_NS::Execute, static_cast<OpFlags>(0x80000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CACHE ( "PROC_CACHE", 1, &PROC_CACHE_NS::Execute, static_cast<OpFlags>(0x00800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CLO ( "PROC_CLO", 1, &PROC_CLO_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_CLZ ( "PROC_CLZ", 1, &PROC_CLZ_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_DIV ( "PROC_DIV", 1, &PROC_DIV_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_DIVU ( "PROC_DIVU", 1, &PROC_DIVU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_EHB ( "PROC_EHB", 1, &PROC_EHB_NS::Execute, static_cast<OpFlags>(0x04000000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ERET ( "PROC_ERET", 1, &PROC_ERET_NS::Execute, static_cast<OpFlags>(0xAC000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ERETNC ( "PROC_ERETNC", 1, &PROC_ERETNC_NS::Execute, static_cast<OpFlags>(0xAC000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_EXT ( "PROC_EXT", 1, &PROC_EXT_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_GINVI ( "PROC_GINVI", 1, &PROC_GINVI_NS::Execute, static_cast<OpFlags>(0xA0800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_INS ( "PROC_INS", 1, &PROC_INS_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_J ( "PROC_J", 1, &PROC_J_NS::Execute, static_cast<OpFlags>(0x00160000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JAL ( "PROC_JAL", 1, &PROC_JAL_NS::Execute, static_cast<OpFlags>(0x00160000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JALR ( "PROC_JALR", 1, &PROC_JALR_NS::Execute, static_cast<OpFlags>(0x01960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JALR_HB ( "PROC_JALR_HB", 1, &PROC_JALR_HB_NS::Execute, static_cast<OpFlags>(0x0D960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JIALC ( "PROC_JIALC", 1, &PROC_JIALC_NS::Execute, static_cast<OpFlags>(0x008A0000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JIC ( "PROC_JIC", 1, &PROC_JIC_NS::Execute, static_cast<OpFlags>(0x008A0000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JR ( "PROC_JR", 1, &PROC_JR_NS::Execute, static_cast<OpFlags>(0x00960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_JR_HB ( "PROC_JR_HB", 1, &PROC_JR_HB_NS::Execute, static_cast<OpFlags>(0x0C960000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LB ( "PROC_LB", 1, &PROC_LB_NS::Execute, static_cast<OpFlags>(0x01A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBE ( "PROC_LBE", 1, &PROC_LBE_NS::Execute, static_cast<OpFlags>(0xA1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBU ( "PROC_LBU", 1, &PROC_LBU_NS::Execute, static_cast<OpFlags>(0x01A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LBUE ( "PROC_LBUE", 1, &PROC_LBUE_NS::Execute, static_cast<OpFlags>(0xA1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LH ( "PROC_LH", 1, &PROC_LH_NS::Execute, static_cast<OpFlags>(0x11A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHE ( "PROC_LHE", 1, &PROC_LHE_NS::Execute, static_cast<OpFlags>(0xB1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHU ( "PROC_LHU", 1, &PROC_LHU_NS::Execute, static_cast<OpFlags>(0x11A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LHUE ( "PROC_LHUE", 1, &PROC_LHUE_NS::Execute, static_cast<OpFlags>(0xB1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LL ( "PROC_LL", 1, &PROC_LL_NS::Execute, static_cast<OpFlags>(0x01A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLE ( "PROC_LLE", 1, &PROC_LLE_NS::Execute, static_cast<OpFlags>(0xA1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLWP ( "PROC_LLWP", 1, &PROC_LLWP_NS::Execute, static_cast<OpFlags>(0x01A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LLWPE ( "PROC_LLWPE", 1, &PROC_LLWPE_NS::Execute, static_cast<OpFlags>(0xA1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LSA ( "PROC_LSA", 1, &PROC_LSA_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LW ( "PROC_LW", 1, &PROC_LW_NS::Execute, static_cast<OpFlags>(0x11A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LWE ( "PROC_LWE", 1, &PROC_LWE_NS::Execute, static_cast<OpFlags>(0xB1A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_LWPC ( "PROC_LWPC", 1, &PROC_LWPC_NS::Execute, static_cast<OpFlags>(0x01A00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MFC0 ( "PROC_MFC0", 1, &PROC_MFC0_NS::Execute, static_cast<OpFlags>(0xA1000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MFC2 ( "PROC_MFC2", 1, &PROC_MFC2_NS::Execute, static_cast<OpFlags>(0x81000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MFHC0 ( "PROC_MFHC0", 1, &PROC_MFHC0_NS::Execute, static_cast<OpFlags>(0xA1000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MFHC2 ( "PROC_MFHC2", 1, &PROC_MFHC2_NS::Execute, static_cast<OpFlags>(0x81000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MOD ( "PROC_MOD", 1, &PROC_MOD_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MODU ( "PROC_MODU", 1, &PROC_MODU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MOVE ( "PROC_MOVE", 1, &PROC_MOVE_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MTC0 ( "PROC_MTC0", 1, &PROC_MTC0_NS::Execute, static_cast<OpFlags>(0xA0800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MTC2 ( "PROC_MTC2", 1, &PROC_MTC2_NS::Execute, static_cast<OpFlags>(0x80800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MTHC0 ( "PROC_MTHC0", 1, &PROC_MTHC0_NS::Execute, static_cast<OpFlags>(0xA0800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MTHC2 ( "PROC_MTHC2", 1, &PROC_MTHC2_NS::Execute, static_cast<OpFlags>(0x80800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUH ( "PROC_MUH", 1, &PROC_MUH_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUHU ( "PROC_MUHU", 1, &PROC_MUHU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MUL ( "PROC_MUL", 1, &PROC_MUL_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_MULU ( "PROC_MULU", 1, &PROC_MULU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NAL ( "PROC_NAL", 1, &PROC_NAL_NS::Execute, static_cast<OpFlags>(0x00120000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NOP ( "PROC_NOP", 1, &PROC_NOP_NS::Execute, static_cast<OpFlags>(0x00000000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_NOR ( "PROC_NOR", 1, &PROC_NOR_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_OR ( "PROC_OR", 1, &PROC_OR_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ORI ( "PROC_ORI", 1, &PROC_ORI_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PAUSE ( "PROC_PAUSE", 1, &PROC_PAUSE_NS::Execute, static_cast<OpFlags>(0x00020000), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP06 ( "PROC_POP06", 1, &PROC_POP06_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP07 ( "PROC_POP07", 1, &PROC_POP07_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP10 ( "PROC_POP10", 1, &PROC_POP10_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP26 ( "PROC_POP26", 1, &PROC_POP26_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP27 ( "PROC_POP27", 1, &PROC_POP27_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_POP30 ( "PROC_POP30", 1, &PROC_POP30_NS::Execute, static_cast<OpFlags>(0x009A0002), { .control = true }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PREF ( "PROC_PREF", 1, &PROC_PREF_NS::Execute, static_cast<OpFlags>(0x00800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_PREFE ( "PROC_PREFE", 1, &PROC_PREFE_NS::Execute, static_cast<OpFlags>(0xA0800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_RDHWR ( "PROC_RDHWR", 1, &PROC_RDHWR_NS::Execute, static_cast<OpFlags>(0x01800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ROTR ( "PROC_ROTR", 1, &PROC_ROTR_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_ROTRV ( "PROC_ROTRV", 1, &PROC_ROTRV_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SB ( "PROC_SB", 1, &PROC_SB_NS::Execute, static_cast<OpFlags>(0x00C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SBE ( "PROC_SBE", 1, &PROC_SBE_NS::Execute, static_cast<OpFlags>(0xA0C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SC ( "PROC_SC", 1, &PROC_SC_NS::Execute, static_cast<OpFlags>(0x11C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCE ( "PROC_SCE", 1, &PROC_SCE_NS::Execute, static_cast<OpFlags>(0xB1C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCWP ( "PROC_SCWP", 1, &PROC_SCWP_NS::Execute, static_cast<OpFlags>(0x01C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SCWPE ( "PROC_SCWPE", 1, &PROC_SCWPE_NS::Execute, static_cast<OpFlags>(0xA1C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SDBBP ( "PROC_SDBBP", 1, &PROC_SDBBP_NS::Execute, static_cast<OpFlags>(0x80000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SEB ( "PROC_SEB", 1, &PROC_SEB_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SEH ( "PROC_SEH", 1, &PROC_SEH_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SELEQZ ( "PROC_SELEQZ", 1, &PROC_SELEQZ_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SELNEZ ( "PROC_SELNEZ", 1, &PROC_SELNEZ_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SH ( "PROC_SH", 1, &PROC_SH_NS::Execute, static_cast<OpFlags>(0x10C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SHE ( "PROC_SHE", 1, &PROC_SHE_NS::Execute, static_cast<OpFlags>(0xB0C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SIGRIE ( "PROC_SIGRIE", 1, &PROC_SIGRIE_NS::Execute, static_cast<OpFlags>(0x00000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLL ( "PROC_SLL", 1, &PROC_SLL_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLLV ( "PROC_SLLV", 1, &PROC_SLLV_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLT ( "PROC_SLT", 1, &PROC_SLT_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTI ( "PROC_SLTI", 1, &PROC_SLTI_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTIU ( "PROC_SLTIU", 1, &PROC_SLTIU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SLTU ( "PROC_SLTU", 1, &PROC_SLTU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRA ( "PROC_SRA", 1, &PROC_SRA_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRAV ( "PROC_SRAV", 1, &PROC_SRAV_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRL ( "PROC_SRL", 1, &PROC_SRL_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SRLV ( "PROC_SRLV", 1, &PROC_SRLV_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SSNOP ( "PROC_SSNOP", 1, &PROC_SSNOP_NS::Execute, static_cast<OpFlags>(0x00000000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SUB ( "PROC_SUB", 1, &PROC_SUB_NS::Execute, static_cast<OpFlags>(0x01800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SUBU ( "PROC_SUBU", 1, &PROC_SUBU_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SW ( "PROC_SW", 1, &PROC_SW_NS::Execute, static_cast<OpFlags>(0x10C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SWE ( "PROC_SWE", 1, &PROC_SWE_NS::Execute, static_cast<OpFlags>(0xB0C00002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYNC ( "PROC_SYNC", 1, &PROC_SYNC_NS::Execute, static_cast<OpFlags>(0x04000000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYNCI ( "PROC_SYNCI", 1, &PROC_SYNCI_NS::Execute, static_cast<OpFlags>(0x00800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_SYSCALL ( "PROC_SYSCALL", 1, &PROC_SYSCALL_NS::Execute, static_cast<OpFlags>(0x01800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TEQ ( "PROC_TEQ", 1, &PROC_TEQ_NS::Execute, static_cast<OpFlags>(0x00800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TGE ( "PROC_TGE", 1, &PROC_TGE_NS::Execute, static_cast<OpFlags>(0x00800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TGEU ( "PROC_TGEU", 1, &PROC_TGEU_NS::Execute, static_cast<OpFlags>(0x00800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TLT ( "PROC_TLT", 1, &PROC_TLT_NS::Execute, static_cast<OpFlags>(0x00800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TLTU ( "PROC_TLTU", 1, &PROC_TLTU_NS::Execute, static_cast<OpFlags>(0x00800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_TNE ( "PROC_TNE", 1, &PROC_TNE_NS::Execute, static_cast<OpFlags>(0x00800002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_WAIT ( "PROC_WAIT", 1, &PROC_WAIT_NS::Execute, static_cast<OpFlags>(0xAC000002), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_WSBH ( "PROC_WSBH", 1, &PROC_WSBH_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_XOR ( "PROC_XOR", 1, &PROC_XOR_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	_segment(vemips_itable) VEMIPS_ITABLE_EXPORT const InstructionInfo StaticProc_PROC_XORI ( "PROC_XORI", 1, &PROC_XORI_NS::Execute, static_cast<OpFlags>(0x01800000), { .control = false }, instruction_type::normal);
	// ReSharper restore CppCStyleCast

	const std::array<const std::reference_wrapper<const InstructionInfo>, 220> AllInstructions = {
		std::ref(StaticProc_COP1_ABS_f),
		std::ref(StaticProc_COP1_ABS_d),
		std::ref(StaticProc_COP1_ADD_f),
		std::ref(StaticProc_COP1_ADD_d),
		std::ref(StaticProc_COP1_BC1EQZ_v),
		std::ref(StaticProc_COP1_BC1NEZ_v),
		std::ref(StaticProc_COP1_CEIL_L_f),
		std::ref(StaticProc_COP1_CEIL_L_d),
		std::ref(StaticProc_COP1_CEIL_W_f),
		std::ref(StaticProc_COP1_CEIL_W_d),
		std::ref(StaticProc_COP1_CFC1_v),
		std::ref(StaticProc_COP1_CLASS_f),
		std::ref(StaticProc_COP1_CLASS_d),
		std::ref(StaticProc_COP1_CMP_condn_d_v),
		std::ref(StaticProc_COP1_CMP_condn_s_v),
		std::ref(StaticProc_COP1_CTC1_v),
		std::ref(StaticProc_COP1_CVT_D_w),
		std::ref(StaticProc_COP1_CVT_D_l),
		std::ref(StaticProc_COP1_CVT_D_f),
		std::ref(StaticProc_COP1_CVT_D_d),
		std::ref(StaticProc_COP1_CVT_L_f),
		std::ref(StaticProc_COP1_CVT_L_d),
		std::ref(StaticProc_COP1_CVT_S_w),
		std::ref(StaticProc_COP1_CVT_S_l),
		std::ref(StaticProc_COP1_CVT_S_f),
		std::ref(StaticProc_COP1_CVT_S_d),
		std::ref(StaticProc_COP1_CVT_W_f),
		std::ref(StaticProc_COP1_CVT_W_d),
		std::ref(StaticProc_COP1_DIV_f),
		std::ref(StaticProc_COP1_DIV_d),
		std::ref(StaticProc_COP1_FLOOR_L_f),
		std::ref(StaticProc_COP1_FLOOR_L_d),
		std::ref(StaticProc_COP1_FLOOR_W_f),
		std::ref(StaticProc_COP1_FLOOR_W_d),
		std::ref(StaticProc_COP1_LDC1_v),
		std::ref(StaticProc_COP1_LWC1_v),
		std::ref(StaticProc_COP1_MADDF_f),
		std::ref(StaticProc_COP1_MADDF_d),
		std::ref(StaticProc_COP1_MAX_f),
		std::ref(StaticProc_COP1_MAX_d),
		std::ref(StaticProc_COP1_MAXA_f),
		std::ref(StaticProc_COP1_MAXA_d),
		std::ref(StaticProc_COP1_MFC1_v),
		std::ref(StaticProc_COP1_MFHC1_v),
		std::ref(StaticProc_COP1_MIN_f),
		std::ref(StaticProc_COP1_MIN_d),
		std::ref(StaticProc_COP1_MINA_f),
		std::ref(StaticProc_COP1_MINA_d),
		std::ref(StaticProc_COP1_MOV_f),
		std::ref(StaticProc_COP1_MOV_d),
		std::ref(StaticProc_COP1_MSUBF_f),
		std::ref(StaticProc_COP1_MSUBF_d),
		std::ref(StaticProc_COP1_MTC1_v),
		std::ref(StaticProc_COP1_MTHC1_v),
		std::ref(StaticProc_COP1_MUL_f),
		std::ref(StaticProc_COP1_MUL_d),
		std::ref(StaticProc_COP1_NEG_f),
		std::ref(StaticProc_COP1_NEG_d),
		std::ref(StaticProc_COP1_RECIP_f),
		std::ref(StaticProc_COP1_RECIP_d),
		std::ref(StaticProc_COP1_RINT_f),
		std::ref(StaticProc_COP1_RINT_d),
		std::ref(StaticProc_COP1_ROUND_L_f),
		std::ref(StaticProc_COP1_ROUND_L_d),
		std::ref(StaticProc_COP1_ROUND_W_f),
		std::ref(StaticProc_COP1_ROUND_W_d),
		std::ref(StaticProc_COP1_RSQRT_f),
		std::ref(StaticProc_COP1_RSQRT_d),
		std::ref(StaticProc_COP1_SDC1_v),
		std::ref(StaticProc_COP1_SEL_f),
		std::ref(StaticProc_COP1_SEL_d),
		std::ref(StaticProc_COP1_SELEQZ_f),
		std::ref(StaticProc_COP1_SELEQZ_d),
		std::ref(StaticProc_COP1_SELNEZ_f),
		std::ref(StaticProc_COP1_SELNEZ_d),
		std::ref(StaticProc_COP1_SQRT_f),
		std::ref(StaticProc_COP1_SQRT_d),
		std::ref(StaticProc_COP1_SUB_f),
		std::ref(StaticProc_COP1_SUB_d),
		std::ref(StaticProc_COP1_SWC1_v),
		std::ref(StaticProc_COP1_TRUNC_L_f),
		std::ref(StaticProc_COP1_TRUNC_L_d),
		std::ref(StaticProc_COP1_TRUNC_W_f),
		std::ref(StaticProc_COP1_TRUNC_W_d),
		std::ref(StaticProc_PROC_ADD),
		std::ref(StaticProc_PROC_ADDIU),
		std::ref(StaticProc_PROC_ADDIUPC),
		std::ref(StaticProc_PROC_ADDU),
		std::ref(StaticProc_PROC_ALIGN),
		std::ref(StaticProc_PROC_ALUIPC),
		std::ref(StaticProc_PROC_AND),
		std::ref(StaticProc_PROC_ANDI),
		std::ref(StaticProc_PROC_AUI),
		std::ref(StaticProc_PROC_AUIPC),
		std::ref(StaticProc_PROC_BAL),
		std::ref(StaticProc_PROC_BALC),
		std::ref(StaticProc_PROC_BC),
		std::ref(StaticProc_PROC_BEQ),
		std::ref(StaticProc_PROC_BEQZC),
		std::ref(StaticProc_PROC_BGEZ),
		std::ref(StaticProc_PROC_BGTZ),
		std::ref(StaticProc_PROC_BGTZALC),
		std::ref(StaticProc_PROC_BGTZC),
		std::ref(StaticProc_PROC_BITSWAP),
		std::ref(StaticProc_PROC_BLEZ),
		std::ref(StaticProc_PROC_BLEZALC),
		std::ref(StaticProc_PROC_BLEZC),
		std::ref(StaticProc_PROC_BLTZ),
		std::ref(StaticProc_PROC_BNE),
		std::ref(StaticProc_PROC_BNEZC),
		std::ref(StaticProc_PROC_BREAK),
		std::ref(StaticProc_PROC_CACHE),
		std::ref(StaticProc_PROC_CLO),
		std::ref(StaticProc_PROC_CLZ),
		std::ref(StaticProc_PROC_DIV),
		std::ref(StaticProc_PROC_DIVU),
		std::ref(StaticProc_PROC_EHB),
		std::ref(StaticProc_PROC_ERET),
		std::ref(StaticProc_PROC_ERETNC),
		std::ref(StaticProc_PROC_EXT),
		std::ref(StaticProc_PROC_GINVI),
		std::ref(StaticProc_PROC_INS),
		std::ref(StaticProc_PROC_J),
		std::ref(StaticProc_PROC_JAL),
		std::ref(StaticProc_PROC_JALR),
		std::ref(StaticProc_PROC_JALR_HB),
		std::ref(StaticProc_PROC_JIALC),
		std::ref(StaticProc_PROC_JIC),
		std::ref(StaticProc_PROC_JR),
		std::ref(StaticProc_PROC_JR_HB),
		std::ref(StaticProc_PROC_LB),
		std::ref(StaticProc_PROC_LBE),
		std::ref(StaticProc_PROC_LBU),
		std::ref(StaticProc_PROC_LBUE),
		std::ref(StaticProc_PROC_LH),
		std::ref(StaticProc_PROC_LHE),
		std::ref(StaticProc_PROC_LHU),
		std::ref(StaticProc_PROC_LHUE),
		std::ref(StaticProc_PROC_LL),
		std::ref(StaticProc_PROC_LLE),
		std::ref(StaticProc_PROC_LLWP),
		std::ref(StaticProc_PROC_LLWPE),
		std::ref(StaticProc_PROC_LSA),
		std::ref(StaticProc_PROC_LW),
		std::ref(StaticProc_PROC_LWE),
		std::ref(StaticProc_PROC_LWPC),
		std::ref(StaticProc_PROC_MFC0),
		std::ref(StaticProc_PROC_MFC2),
		std::ref(StaticProc_PROC_MFHC0),
		std::ref(StaticProc_PROC_MFHC2),
		std::ref(StaticProc_PROC_MOD),
		std::ref(StaticProc_PROC_MODU),
		std::ref(StaticProc_PROC_MOVE),
		std::ref(StaticProc_PROC_MTC0),
		std::ref(StaticProc_PROC_MTC2),
		std::ref(StaticProc_PROC_MTHC0),
		std::ref(StaticProc_PROC_MTHC2),
		std::ref(StaticProc_PROC_MUH),
		std::ref(StaticProc_PROC_MUHU),
		std::ref(StaticProc_PROC_MUL),
		std::ref(StaticProc_PROC_MULU),
		std::ref(StaticProc_PROC_NAL),
		std::ref(StaticProc_PROC_NOP),
		std::ref(StaticProc_PROC_NOR),
		std::ref(StaticProc_PROC_OR),
		std::ref(StaticProc_PROC_ORI),
		std::ref(StaticProc_PROC_PAUSE),
		std::ref(StaticProc_PROC_POP06),
		std::ref(StaticProc_PROC_POP07),
		std::ref(StaticProc_PROC_POP10),
		std::ref(StaticProc_PROC_POP26),
		std::ref(StaticProc_PROC_POP27),
		std::ref(StaticProc_PROC_POP30),
		std::ref(StaticProc_PROC_PREF),
		std::ref(StaticProc_PROC_PREFE),
		std::ref(StaticProc_PROC_RDHWR),
		std::ref(StaticProc_PROC_ROTR),
		std::ref(StaticProc_PROC_ROTRV),
		std::ref(StaticProc_PROC_SB),
		std::ref(StaticProc_PROC_SBE),
		std::ref(StaticProc_PROC_SC),
		std::ref(StaticProc_PROC_SCE),
		std::ref(StaticProc_PROC_SCWP),
		std::ref(StaticProc_PROC_SCWPE),
		std::ref(StaticProc_PROC_SDBBP),
		std::ref(StaticProc_PROC_SEB),
		std::ref(StaticProc_PROC_SEH),
		std::ref(StaticProc_PROC_SELEQZ),
		std::ref(StaticProc_PROC_SELNEZ),
		std::ref(StaticProc_PROC_SH),
		std::ref(StaticProc_PROC_SHE),
		std::ref(StaticProc_PROC_SIGRIE),
		std::ref(StaticProc_PROC_SLL),
		std::ref(StaticProc_PROC_SLLV),
		std::ref(StaticProc_PROC_SLT),
		std::ref(StaticProc_PROC_SLTI),
		std::ref(StaticProc_PROC_SLTIU),
		std::ref(StaticProc_PROC_SLTU),
		std::ref(StaticProc_PROC_SRA),
		std::ref(StaticProc_PROC_SRAV),
		std::ref(StaticProc_PROC_SRL),
		std::ref(StaticProc_PROC_SRLV),
		std::ref(StaticProc_PROC_SSNOP),
		std::ref(StaticProc_PROC_SUB),
		std::ref(StaticProc_PROC_SUBU),
		std::ref(StaticProc_PROC_SW),
		std::ref(StaticProc_PROC_SWE),
		std::ref(StaticProc_PROC_SYNC),
		std::ref(StaticProc_PROC_SYNCI),
		std::ref(StaticProc_PROC_SYSCALL),
		std::ref(StaticProc_PROC_TEQ),
		std::ref(StaticProc_PROC_TGE),
		std::ref(StaticProc_PROC_TGEU),
		std::ref(StaticProc_PROC_TLT),
		std::ref(StaticProc_PROC_TLTU),
		std::ref(StaticProc_PROC_TNE),
		std::ref(StaticProc_PROC_WAIT),
		std::ref(StaticProc_PROC_WSBH),
		std::ref(StaticProc_PROC_XOR),
		std::ref(StaticProc_PROC_XORI),
	};
	// ReSharper disable CppCStyleCast
	const std::array<const instructionexec_t, 220> AllInstructionsProcs = {
		(const instructionexec_t)&COP1_ABS_NS::Execute<float>,
		(const instructionexec_t)&COP1_ABS_NS::Execute<double>,
		(const instructionexec_t)&COP1_ADD_NS::Execute<float>,
		(const instructionexec_t)&COP1_ADD_NS::Execute<double>,
		(const instructionexec_t)&COP1_BC1EQZ_NS::Execute<void>,
		(const instructionexec_t)&COP1_BC1NEZ_NS::Execute<void>,
		(const instructionexec_t)&COP1_CEIL_L_NS::Execute<float>,
		(const instructionexec_t)&COP1_CEIL_L_NS::Execute<double>,
		(const instructionexec_t)&COP1_CEIL_W_NS::Execute<float>,
		(const instructionexec_t)&COP1_CEIL_W_NS::Execute<double>,
		(const instructionexec_t)&COP1_CFC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_CLASS_NS::Execute<float>,
		(const instructionexec_t)&COP1_CLASS_NS::Execute<double>,
		(const instructionexec_t)&COP1_CMP_condn_d_NS::Execute<void>,
		(const instructionexec_t)&COP1_CMP_condn_s_NS::Execute<void>,
		(const instructionexec_t)&COP1_CTC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_CVT_D_NS::Execute<int32>,
		(const instructionexec_t)&COP1_CVT_D_NS::Execute<int64>,
		(const instructionexec_t)&COP1_CVT_D_NS::Execute<float>,
		(const instructionexec_t)&COP1_CVT_D_NS::Execute<double>,
		(const instructionexec_t)&COP1_CVT_L_NS::Execute<float>,
		(const instructionexec_t)&COP1_CVT_L_NS::Execute<double>,
		(const instructionexec_t)&COP1_CVT_S_NS::Execute<int32>,
		(const instructionexec_t)&COP1_CVT_S_NS::Execute<int64>,
		(const instructionexec_t)&COP1_CVT_S_NS::Execute<float>,
		(const instructionexec_t)&COP1_CVT_S_NS::Execute<double>,
		(const instructionexec_t)&COP1_CVT_W_NS::Execute<float>,
		(const instructionexec_t)&COP1_CVT_W_NS::Execute<double>,
		(const instructionexec_t)&COP1_DIV_NS::Execute<float>,
		(const instructionexec_t)&COP1_DIV_NS::Execute<double>,
		(const instructionexec_t)&COP1_FLOOR_L_NS::Execute<float>,
		(const instructionexec_t)&COP1_FLOOR_L_NS::Execute<double>,
		(const instructionexec_t)&COP1_FLOOR_W_NS::Execute<float>,
		(const instructionexec_t)&COP1_FLOOR_W_NS::Execute<double>,
		(const instructionexec_t)&COP1_LDC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_LWC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_MADDF_NS::Execute<float>,
		(const instructionexec_t)&COP1_MADDF_NS::Execute<double>,
		(const instructionexec_t)&COP1_MAX_NS::Execute<float>,
		(const instructionexec_t)&COP1_MAX_NS::Execute<double>,
		(const instructionexec_t)&COP1_MAXA_NS::Execute<float>,
		(const instructionexec_t)&COP1_MAXA_NS::Execute<double>,
		(const instructionexec_t)&COP1_MFC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_MFHC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_MIN_NS::Execute<float>,
		(const instructionexec_t)&COP1_MIN_NS::Execute<double>,
		(const instructionexec_t)&COP1_MINA_NS::Execute<float>,
		(const instructionexec_t)&COP1_MINA_NS::Execute<double>,
		(const instructionexec_t)&COP1_MOV_NS::Execute<float>,
		(const instructionexec_t)&COP1_MOV_NS::Execute<double>,
		(const instructionexec_t)&COP1_MSUBF_NS::Execute<float>,
		(const instructionexec_t)&COP1_MSUBF_NS::Execute<double>,
		(const instructionexec_t)&COP1_MTC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_MTHC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_MUL_NS::Execute<float>,
		(const instructionexec_t)&COP1_MUL_NS::Execute<double>,
		(const instructionexec_t)&COP1_NEG_NS::Execute<float>,
		(const instructionexec_t)&COP1_NEG_NS::Execute<double>,
		(const instructionexec_t)&COP1_RECIP_NS::Execute<float>,
		(const instructionexec_t)&COP1_RECIP_NS::Execute<double>,
		(const instructionexec_t)&COP1_RINT_NS::Execute<float>,
		(const instructionexec_t)&COP1_RINT_NS::Execute<double>,
		(const instructionexec_t)&COP1_ROUND_L_NS::Execute<float>,
		(const instructionexec_t)&COP1_ROUND_L_NS::Execute<double>,
		(const instructionexec_t)&COP1_ROUND_W_NS::Execute<float>,
		(const instructionexec_t)&COP1_ROUND_W_NS::Execute<double>,
		(const instructionexec_t)&COP1_RSQRT_NS::Execute<float>,
		(const instructionexec_t)&COP1_RSQRT_NS::Execute<double>,
		(const instructionexec_t)&COP1_SDC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_SEL_NS::Execute<float>,
		(const instructionexec_t)&COP1_SEL_NS::Execute<double>,
		(const instructionexec_t)&COP1_SELEQZ_NS::Execute<float>,
		(const instructionexec_t)&COP1_SELEQZ_NS::Execute<double>,
		(const instructionexec_t)&COP1_SELNEZ_NS::Execute<float>,
		(const instructionexec_t)&COP1_SELNEZ_NS::Execute<double>,
		(const instructionexec_t)&COP1_SQRT_NS::Execute<float>,
		(const instructionexec_t)&COP1_SQRT_NS::Execute<double>,
		(const instructionexec_t)&COP1_SUB_NS::Execute<float>,
		(const instructionexec_t)&COP1_SUB_NS::Execute<double>,
		(const instructionexec_t)&COP1_SWC1_NS::Execute<void>,
		(const instructionexec_t)&COP1_TRUNC_L_NS::Execute<float>,
		(const instructionexec_t)&COP1_TRUNC_L_NS::Execute<double>,
		(const instructionexec_t)&COP1_TRUNC_W_NS::Execute<float>,
		(const instructionexec_t)&COP1_TRUNC_W_NS::Execute<double>,
		(const instructionexec_t)&PROC_ADD_NS::Execute,
		(const instructionexec_t)&PROC_ADDIU_NS::Execute,
		(const instructionexec_t)&PROC_ADDIUPC_NS::Execute,
		(const instructionexec_t)&PROC_ADDU_NS::Execute,
		(const instructionexec_t)&PROC_ALIGN_NS::Execute,
		(const instructionexec_t)&PROC_ALUIPC_NS::Execute,
		(const instructionexec_t)&PROC_AND_NS::Execute,
		(const instructionexec_t)&PROC_ANDI_NS::Execute,
		(const instructionexec_t)&PROC_AUI_NS::Execute,
		(const instructionexec_t)&PROC_AUIPC_NS::Execute,
		(const instructionexec_t)&PROC_BAL_NS::Execute,
		(const instructionexec_t)&PROC_BALC_NS::Execute,
		(const instructionexec_t)&PROC_BC_NS::Execute,
		(const instructionexec_t)&PROC_BEQ_NS::Execute,
		(const instructionexec_t)&PROC_BEQZC_NS::Execute,
		(const instructionexec_t)&PROC_BGEZ_NS::Execute,
		(const instructionexec_t)&PROC_BGTZ_NS::Execute,
		(const instructionexec_t)&PROC_BGTZALC_NS::Execute,
		(const instructionexec_t)&PROC_BGTZC_NS::Execute,
		(const instructionexec_t)&PROC_BITSWAP_NS::Execute,
		(const instructionexec_t)&PROC_BLEZ_NS::Execute,
		(const instructionexec_t)&PROC_BLEZALC_NS::Execute,
		(const instructionexec_t)&PROC_BLEZC_NS::Execute,
		(const instructionexec_t)&PROC_BLTZ_NS::Execute,
		(const instructionexec_t)&PROC_BNE_NS::Execute,
		(const instructionexec_t)&PROC_BNEZC_NS::Execute,
		(const instructionexec_t)&PROC_BREAK_NS::Execute,
		(const instructionexec_t)&PROC_CACHE_NS::Execute,
		(const instructionexec_t)&PROC_CLO_NS::Execute,
		(const instructionexec_t)&PROC_CLZ_NS::Execute,
		(const instructionexec_t)&PROC_DIV_NS::Execute,
		(const instructionexec_t)&PROC_DIVU_NS::Execute,
		(const instructionexec_t)&PROC_EHB_NS::Execute,
		(const instructionexec_t)&PROC_ERET_NS::Execute,
		(const instructionexec_t)&PROC_ERETNC_NS::Execute,
		(const instructionexec_t)&PROC_EXT_NS::Execute,
		(const instructionexec_t)&PROC_GINVI_NS::Execute,
		(const instructionexec_t)&PROC_INS_NS::Execute,
		(const instructionexec_t)&PROC_J_NS::Execute,
		(const instructionexec_t)&PROC_JAL_NS::Execute,
		(const instructionexec_t)&PROC_JALR_NS::Execute,
		(const instructionexec_t)&PROC_JALR_HB_NS::Execute,
		(const instructionexec_t)&PROC_JIALC_NS::Execute,
		(const instructionexec_t)&PROC_JIC_NS::Execute,
		(const instructionexec_t)&PROC_JR_NS::Execute,
		(const instructionexec_t)&PROC_JR_HB_NS::Execute,
		(const instructionexec_t)&PROC_LB_NS::Execute,
		(const instructionexec_t)&PROC_LBE_NS::Execute,
		(const instructionexec_t)&PROC_LBU_NS::Execute,
		(const instructionexec_t)&PROC_LBUE_NS::Execute,
		(const instructionexec_t)&PROC_LH_NS::Execute,
		(const instructionexec_t)&PROC_LHE_NS::Execute,
		(const instructionexec_t)&PROC_LHU_NS::Execute,
		(const instructionexec_t)&PROC_LHUE_NS::Execute,
		(const instructionexec_t)&PROC_LL_NS::Execute,
		(const instructionexec_t)&PROC_LLE_NS::Execute,
		(const instructionexec_t)&PROC_LLWP_NS::Execute,
		(const instructionexec_t)&PROC_LLWPE_NS::Execute,
		(const instructionexec_t)&PROC_LSA_NS::Execute,
		(const instructionexec_t)&PROC_LW_NS::Execute,
		(const instructionexec_t)&PROC_LWE_NS::Execute,
		(const instructionexec_t)&PROC_LWPC_NS::Execute,
		(const instructionexec_t)&PROC_MFC0_NS::Execute,
		(const instructionexec_t)&PROC_MFC2_NS::Execute,
		(const instructionexec_t)&PROC_MFHC0_NS::Execute,
		(const instructionexec_t)&PROC_MFHC2_NS::Execute,
		(const instructionexec_t)&PROC_MOD_NS::Execute,
		(const instructionexec_t)&PROC_MODU_NS::Execute,
		(const instructionexec_t)&PROC_MOVE_NS::Execute,
		(const instructionexec_t)&PROC_MTC0_NS::Execute,
		(const instructionexec_t)&PROC_MTC2_NS::Execute,
		(const instructionexec_t)&PROC_MTHC0_NS::Execute,
		(const instructionexec_t)&PROC_MTHC2_NS::Execute,
		(const instructionexec_t)&PROC_MUH_NS::Execute,
		(const instructionexec_t)&PROC_MUHU_NS::Execute,
		(const instructionexec_t)&PROC_MUL_NS::Execute,
		(const instructionexec_t)&PROC_MULU_NS::Execute,
		(const instructionexec_t)&PROC_NAL_NS::Execute,
		(const instructionexec_t)&PROC_NOP_NS::Execute,
		(const instructionexec_t)&PROC_NOR_NS::Execute,
		(const instructionexec_t)&PROC_OR_NS::Execute,
		(const instructionexec_t)&PROC_ORI_NS::Execute,
		(const instructionexec_t)&PROC_PAUSE_NS::Execute,
		(const instructionexec_t)&PROC_POP06_NS::Execute,
		(const instructionexec_t)&PROC_POP07_NS::Execute,
		(const instructionexec_t)&PROC_POP10_NS::Execute,
		(const instructionexec_t)&PROC_POP26_NS::Execute,
		(const instructionexec_t)&PROC_POP27_NS::Execute,
		(const instructionexec_t)&PROC_POP30_NS::Execute,
		(const instructionexec_t)&PROC_PREF_NS::Execute,
		(const instructionexec_t)&PROC_PREFE_NS::Execute,
		(const instructionexec_t)&PROC_RDHWR_NS::Execute,
		(const instructionexec_t)&PROC_ROTR_NS::Execute,
		(const instructionexec_t)&PROC_ROTRV_NS::Execute,
		(const instructionexec_t)&PROC_SB_NS::Execute,
		(const instructionexec_t)&PROC_SBE_NS::Execute,
		(const instructionexec_t)&PROC_SC_NS::Execute,
		(const instructionexec_t)&PROC_SCE_NS::Execute,
		(const instructionexec_t)&PROC_SCWP_NS::Execute,
		(const instructionexec_t)&PROC_SCWPE_NS::Execute,
		(const instructionexec_t)&PROC_SDBBP_NS::Execute,
		(const instructionexec_t)&PROC_SEB_NS::Execute,
		(const instructionexec_t)&PROC_SEH_NS::Execute,
		(const instructionexec_t)&PROC_SELEQZ_NS::Execute,
		(const instructionexec_t)&PROC_SELNEZ_NS::Execute,
		(const instructionexec_t)&PROC_SH_NS::Execute,
		(const instructionexec_t)&PROC_SHE_NS::Execute,
		(const instructionexec_t)&PROC_SIGRIE_NS::Execute,
		(const instructionexec_t)&PROC_SLL_NS::Execute,
		(const instructionexec_t)&PROC_SLLV_NS::Execute,
		(const instructionexec_t)&PROC_SLT_NS::Execute,
		(const instructionexec_t)&PROC_SLTI_NS::Execute,
		(const instructionexec_t)&PROC_SLTIU_NS::Execute,
		(const instructionexec_t)&PROC_SLTU_NS::Execute,
		(const instructionexec_t)&PROC_SRA_NS::Execute,
		(const instructionexec_t)&PROC_SRAV_NS::Execute,
		(const instructionexec_t)&PROC_SRL_NS::Execute,
		(const instructionexec_t)&PROC_SRLV_NS::Execute,
		(const instructionexec_t)&PROC_SSNOP_NS::Execute,
		(const instructionexec_t)&PROC_SUB_NS::Execute,
		(const instructionexec_t)&PROC_SUBU_NS::Execute,
		(const instructionexec_t)&PROC_SW_NS::Execute,
		(const instructionexec_t)&PROC_SWE_NS::Execute,
		(const instructionexec_t)&PROC_SYNC_NS::Execute,
		(const instructionexec_t)&PROC_SYNCI_NS::Execute,
		(const instructionexec_t)&PROC_SYSCALL_NS::Execute,
		(const instructionexec_t)&PROC_TEQ_NS::Execute,
		(const instructionexec_t)&PROC_TGE_NS::Execute,
		(const instructionexec_t)&PROC_TGEU_NS::Execute,
		(const instructionexec_t)&PROC_TLT_NS::Execute,
		(const instructionexec_t)&PROC_TLTU_NS::Execute,
		(const instructionexec_t)&PROC_TNE_NS::Execute,
		(const instructionexec_t)&PROC_WAIT_NS::Execute,
		(const instructionexec_t)&PROC_WSBH_NS::Execute,
		(const instructionexec_t)&PROC_XOR_NS::Execute,
		(const instructionexec_t)&PROC_XORI_NS::Execute,
	};
	// ReSharper restore CppCStyleCast
}

namespace mips::instructions {
	_nothrow const InstructionInfo* get_instruction(const instruction_t instruction) noexcept {
		switch(instruction) {
			case 0x00000040:
				return &StaticProc_PROC_SSNOP;
			case 0x00000140:
				return &StaticProc_PROC_PAUSE;
			case 0x00000000:
				return &StaticProc_PROC_NOP;
			case 0x000000C0:
				return &StaticProc_PROC_EHB;
			default:
				switch(instruction & 0xFC000000) {
					case 0xD4000000:
						return &StaticProc_COP1_LDC1_v;
					case 0xC4000000:
						return &StaticProc_COP1_LWC1_v;
					case 0xF4000000:
						return &StaticProc_COP1_SDC1_v;
					case 0xE4000000:
						return &StaticProc_COP1_SWC1_v;
					case 0x30000000:
						return &StaticProc_PROC_ANDI;
					case 0x3C000000:
						return &StaticProc_PROC_AUI;
					case 0xE8000000:
						return &StaticProc_PROC_BALC;
					case 0xC8000000:
						return &StaticProc_PROC_BC;
					case 0x10000000:
						return &StaticProc_PROC_BEQ;
					case 0x20000000:
						return &StaticProc_PROC_POP10;
					case 0x60000000:
						return &StaticProc_PROC_POP30;
					case 0x14000000:
						return &StaticProc_PROC_BNE;
					case 0x08000000:
						return &StaticProc_PROC_J;
					case 0x0C000000:
						return &StaticProc_PROC_JAL;
					case 0x80000000:
						return &StaticProc_PROC_LB;
					case 0x90000000:
						return &StaticProc_PROC_LBU;
					case 0x84000000:
						return &StaticProc_PROC_LH;
					case 0x94000000:
						return &StaticProc_PROC_LHU;
					case 0x8C000000:
						return &StaticProc_PROC_LW;
					case 0x34000000:
						return &StaticProc_PROC_ORI;
					case 0xA0000000:
						return &StaticProc_PROC_SB;
					case 0xA4000000:
						return &StaticProc_PROC_SH;
					case 0x28000000:
						return &StaticProc_PROC_SLTI;
					case 0x2C000000:
						return &StaticProc_PROC_SLTIU;
					case 0xAC000000:
						return &StaticProc_PROC_SW;
					case 0x38000000:
						return &StaticProc_PROC_XORI;
					case 0xEC000000:
						switch(instruction & 0x001F0000) {
							case 0x001F0000:
								return &StaticProc_PROC_ALUIPC;
							case 0x001E0000:
								return &StaticProc_PROC_AUIPC;
							default:
								switch(instruction & 0xFC180000) {
									case 0xEC080000:
										return &StaticProc_PROC_LWPC;
									case 0xEC000000:
										return &StaticProc_PROC_ADDIUPC;
									default:
										return nullptr;
								}
						}
					case 0x58000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return &StaticProc_PROC_BLEZC;
						else if ((instruction & 0xFC000000) == 0x58000000)
							return &StaticProc_PROC_POP26;
						else
							return nullptr;
					case 0x5C000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return &StaticProc_PROC_BGTZC;
						else if ((instruction & 0xFC000000) == 0x5C000000)
							return &StaticProc_PROC_POP27;
						else
							return nullptr;
					case 0xD8000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return &StaticProc_PROC_JIC;
						else if ((instruction & 0xFC000000) == 0xD8000000)
							return &StaticProc_PROC_BEQZC;
						else
							return nullptr;
					case 0xF8000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return &StaticProc_PROC_JIALC;
						else if ((instruction & 0xFC000000) == 0xF8000000)
							return &StaticProc_PROC_BNEZC;
						else
							return nullptr;
					case 0x1C000000:
						switch(instruction & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_BGTZALC;
							default:
								if      ((instruction & 0xFC1F0000) == 0x1C000000)
									return &StaticProc_PROC_BGTZ;
								else if ((instruction & 0xFC000000) == 0x1C000000)
									return &StaticProc_PROC_POP07;
								else
									return nullptr;
						}
					case 0x40000000:
						switch(instruction & 0x02000058) {
							case 0x00000000:
								switch(instruction & 0x01E007A0) {
									case 0x00800000:
										return &StaticProc_PROC_MTC0;
									case 0x00400000:
										return &StaticProc_PROC_MFHC0;
									case 0x00000000:
										return &StaticProc_PROC_MFC0;
									case 0x00C00000:
										return &StaticProc_PROC_MTHC0;
									default:
										return nullptr;
								}
							case 0x02000040:
								if      ((instruction & 0x01FFFF80) == 0x00000000)
									return &StaticProc_PROC_ERETNC;
								else if ((instruction & 0xFE00007F) == 0x42000060)
									return &StaticProc_PROC_WAIT;
								else
									return nullptr;
							case 0x02000000:
								return &StaticProc_PROC_ERET;
							default:
								return nullptr;
						}
					case 0x00000000:
						switch(instruction & 0x0000003F) {
							case 0x0000000F:
								return &StaticProc_PROC_SYNC;
							case 0x00000021:
								if      ((instruction & 0x001F07C0) == 0x00000040)
									return &StaticProc_PROC_CLO;
								else if ((instruction & 0xFC0007FF) == 0x00000021)
									return &StaticProc_PROC_ADDU;
								else
									return nullptr;
							case 0x00000009:
								switch(instruction & 0x001FFC00) {
									case 0x00000000:
										return &StaticProc_PROC_JR;
									case 0x00000400:
										return &StaticProc_PROC_JR_HB;
									default:
										switch(instruction & 0xFC1F043F) {
											case 0x00000409:
												return &StaticProc_PROC_JALR_HB;
											case 0x00000009:
												return &StaticProc_PROC_JALR;
											default:
												return nullptr;
										}
								}
							case 0x00000010:
								return &StaticProc_PROC_CLZ;
							case 0x00000006:
								switch(instruction & 0x000007C0) {
									case 0x00000040:
										return &StaticProc_PROC_ROTRV;
									case 0x00000000:
										return &StaticProc_PROC_SRLV;
									default:
										return nullptr;
								}
							case 0x00000026:
								return &StaticProc_PROC_XOR;
							case 0x00000023:
								return &StaticProc_PROC_SUBU;
							case 0x00000022:
								return &StaticProc_PROC_SUB;
							case 0x00000002:
								switch(instruction & 0x03E00000) {
									case 0x00200000:
										return &StaticProc_PROC_ROTR;
									case 0x00000000:
										return &StaticProc_PROC_SRL;
									default:
										return nullptr;
								}
							case 0x00000007:
								return &StaticProc_PROC_SRAV;
							case 0x00000003:
								return &StaticProc_PROC_SRA;
							case 0x0000002B:
								return &StaticProc_PROC_SLTU;
							case 0x0000002A:
								return &StaticProc_PROC_SLT;
							case 0x00000004:
								return &StaticProc_PROC_SLLV;
							case 0x00000000:
								return &StaticProc_PROC_SLL;
							case 0x00000037:
								return &StaticProc_PROC_SELNEZ;
							case 0x00000035:
								return &StaticProc_PROC_SELEQZ;
							case 0x00000025:
								return &StaticProc_PROC_OR;
							case 0x00000027:
								return &StaticProc_PROC_NOR;
							case 0x00000019:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return &StaticProc_PROC_MULU;
									case 0x000000C0:
										return &StaticProc_PROC_MUHU;
									default:
										return nullptr;
								}
							case 0x00000018:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return &StaticProc_PROC_MUL;
									case 0x000000C0:
										return &StaticProc_PROC_MUH;
									default:
										return nullptr;
								}
							case 0x0000001B:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return &StaticProc_PROC_DIVU;
									case 0x000000C0:
										return &StaticProc_PROC_MODU;
									default:
										return nullptr;
								}
							case 0x0000001A:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return &StaticProc_PROC_DIV;
									case 0x000000C0:
										return &StaticProc_PROC_MOD;
									default:
										return nullptr;
								}
							case 0x00000024:
								return &StaticProc_PROC_AND;
							case 0x00000020:
								return &StaticProc_PROC_ADD;
							case 0x00000005:
								return &StaticProc_PROC_LSA;
							case 0x00000036:
								return &StaticProc_PROC_TNE;
							case 0x00000033:
								return &StaticProc_PROC_TLTU;
							case 0x00000032:
								return &StaticProc_PROC_TLT;
							case 0x00000031:
								return &StaticProc_PROC_TGEU;
							case 0x00000030:
								return &StaticProc_PROC_TGE;
							case 0x00000034:
								return &StaticProc_PROC_TEQ;
							case 0x0000000C:
								return &StaticProc_PROC_SYSCALL;
							case 0x0000000E:
								return &StaticProc_PROC_SDBBP;
							case 0x0000000D:
								return &StaticProc_PROC_BREAK;
							default:
								return nullptr;
						}
					case 0x7C000000:
						switch(instruction & 0x0000003F) {
							case 0x0000003D:
								return &StaticProc_PROC_GINVI;
							case 0x00000020:
								switch(instruction & 0x03E007C0) {
									case 0x00000600:
										return &StaticProc_PROC_SEH;
									case 0x00000400:
										return &StaticProc_PROC_SEB;
									case 0x00000000:
										return &StaticProc_PROC_BITSWAP;
									case 0x00000080:
										return &StaticProc_PROC_WSBH;
									default:
										if      ((instruction & 0xFC00073F) == 0x7C000220)
											return &StaticProc_PROC_ALIGN;
										else
											return nullptr;
								}
							case 0x0000003B:
								return &StaticProc_PROC_RDHWR;
							case 0x00000036:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return &StaticProc_PROC_LLWP;
								else if ((instruction & 0xFC00007F) == 0x7C000036)
									return &StaticProc_PROC_LL;
								else
									return nullptr;
							case 0x0000002E:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return &StaticProc_PROC_LLWPE;
								else if ((instruction & 0xFC00007F) == 0x7C00002E)
									return &StaticProc_PROC_LLE;
								else
									return nullptr;
							case 0x00000026:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return &StaticProc_PROC_SCWP;
								else if ((instruction & 0xFC00007F) == 0x7C000026)
									return &StaticProc_PROC_SC;
								else
									return nullptr;
							case 0x0000001E:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return &StaticProc_PROC_SCWPE;
								else if ((instruction & 0xFC00007F) == 0x7C00001E)
									return &StaticProc_PROC_SCE;
								else
									return nullptr;
							case 0x0000001F:
								return &StaticProc_PROC_SWE;
							case 0x0000001D:
								return &StaticProc_PROC_SHE;
							case 0x0000001C:
								return &StaticProc_PROC_SBE;
							case 0x00000023:
								return &StaticProc_PROC_PREFE;
							case 0x00000035:
								return &StaticProc_PROC_PREF;
							case 0x0000002F:
								return &StaticProc_PROC_LWE;
							case 0x00000029:
								return &StaticProc_PROC_LHUE;
							case 0x0000002D:
								return &StaticProc_PROC_LHE;
							case 0x00000028:
								return &StaticProc_PROC_LBUE;
							case 0x0000002C:
								return &StaticProc_PROC_LBE;
							case 0x00000025:
								return &StaticProc_PROC_CACHE;
							case 0x00000004:
								return &StaticProc_PROC_INS;
							case 0x00000000:
								return &StaticProc_PROC_EXT;
							default:
								return nullptr;
						}
					case 0x24000000:
						if      ((instruction & 0x0000FFFF) == 0x00000000)
							return &StaticProc_PROC_MOVE;
						else if ((instruction & 0xFC000000) == 0x24000000)
							return &StaticProc_PROC_ADDIU;
						else
							return nullptr;
					case 0x44000000:
						switch(instruction & 0x03E00000) {
							case 0x01200000:
								return &StaticProc_COP1_BC1EQZ_v;
							case 0x01A00000:
								return &StaticProc_COP1_BC1NEZ_v;
							case 0x02800000:
								switch(instruction & 0x001F003F) {
									case 0x00000020:
										return &StaticProc_COP1_CVT_S_w;
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_w;
									default:
										if      ((instruction & 0xFFE00020) == 0x46800000)
											return &StaticProc_COP1_CMP_condn_s_v;
										else
											return nullptr;
								}
							case 0x02A00000:
								switch(instruction & 0x001F003F) {
									case 0x00000020:
										return &StaticProc_COP1_CVT_S_l;
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_l;
									default:
										if      ((instruction & 0xFFE00020) == 0x46A00000)
											return &StaticProc_COP1_CMP_condn_d_v;
										else
											return nullptr;
								}
							case 0x02000000:
								switch(instruction & 0x001F003F) {
									case 0x00000025:
										return &StaticProc_COP1_CVT_L_f;
									case 0x00000005:
										return &StaticProc_COP1_ABS_f;
									case 0x0000000D:
										return &StaticProc_COP1_TRUNC_W_f;
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
									default:
										switch(instruction & 0xFFE0003F) {
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
								}
							case 0x02200000:
								switch(instruction & 0x001F003F) {
									case 0x00000021:
										return &StaticProc_COP1_CVT_D_d;
									case 0x00000009:
										return &StaticProc_COP1_TRUNC_L_d;
									case 0x00000025:
										return &StaticProc_COP1_CVT_L_d;
									case 0x00000005:
										return &StaticProc_COP1_ABS_d;
									case 0x0000000D:
										return &StaticProc_COP1_TRUNC_W_d;
									case 0x0000001B:
										return &StaticProc_COP1_CLASS_d;
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
									default:
										switch(instruction & 0xFFE0003F) {
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
								}
							case 0x00600000:
								return &StaticProc_COP1_MFHC1_v;
							case 0x00E00000:
								return &StaticProc_COP1_MTHC1_v;
							case 0x00000000:
								return &StaticProc_COP1_MFC1_v;
							case 0x00800000:
								return &StaticProc_COP1_MTC1_v;
							case 0x00400000:
								return &StaticProc_COP1_CFC1_v;
							case 0x00C00000:
								return &StaticProc_COP1_CTC1_v;
							default:
								return nullptr;
						}
					case 0x48000000:
						switch(instruction & 0x03E007F8) {
							case 0x00800000:
								return &StaticProc_PROC_MTC2;
							case 0x00600000:
								return &StaticProc_PROC_MFHC2;
							case 0x00000000:
								return &StaticProc_PROC_MFC2;
							case 0x00E00000:
								return &StaticProc_PROC_MTHC2;
							default:
								return nullptr;
						}
					case 0x04000000:
						switch(instruction & 0x03FF0000) {
							case 0x00100000:
								return &StaticProc_PROC_NAL;
							case 0x00110000:
								return &StaticProc_PROC_BAL;
							case 0x00170000:
								return &StaticProc_PROC_SIGRIE;
							default:
								switch(instruction & 0xFC1F0000) {
									case 0x041F0000:
										return &StaticProc_PROC_SYNCI;
									case 0x04000000:
										return &StaticProc_PROC_BLTZ;
									case 0x04010000:
										return &StaticProc_PROC_BGEZ;
									default:
										return nullptr;
								}
						}
					case 0x18000000:
						switch(instruction & 0x03E00000) {
							case 0x00000000:
								return &StaticProc_PROC_BLEZALC;
							default:
								if      ((instruction & 0xFC1F0000) == 0x18000000)
									return &StaticProc_PROC_BLEZ;
								else if ((instruction & 0xFC000000) == 0x18000000)
									return &StaticProc_PROC_POP06;
								else
									return nullptr;
						}
					default:
						return nullptr;
				}
		}
		return nullptr;
	}

	bool execute_instruction(const instruction_t instruction, processor& __restrict processor) {
		switch(instruction) {
			case 0x00000040:
				{ PROC_SSNOP_NS::Execute(instruction, processor); return true; }
			case 0x00000140:
				{ PROC_PAUSE_NS::Execute(instruction, processor); return true; }
			case 0x00000000:
				{ PROC_NOP_NS::Execute(instruction, processor); return true; }
			case 0x000000C0:
				{ PROC_EHB_NS::Execute(instruction, processor); return true; }
			default:
				switch(instruction & 0xFC000000) {
					case 0xD4000000:
						{ COP1_LDC1_NS::Execute<void>(instruction, processor); return true; }
					case 0xC4000000:
						{ COP1_LWC1_NS::Execute<void>(instruction, processor); return true; }
					case 0xF4000000:
						{ COP1_SDC1_NS::Execute<void>(instruction, processor); return true; }
					case 0xE4000000:
						{ COP1_SWC1_NS::Execute<void>(instruction, processor); return true; }
					case 0x30000000:
						{ PROC_ANDI_NS::Execute(instruction, processor); return true; }
					case 0x3C000000:
						{ PROC_AUI_NS::Execute(instruction, processor); return true; }
					case 0xE8000000:
						{ PROC_BALC_NS::Execute(instruction, processor); return true; }
					case 0xC8000000:
						{ PROC_BC_NS::Execute(instruction, processor); return true; }
					case 0x10000000:
						{ PROC_BEQ_NS::Execute(instruction, processor); return true; }
					case 0x20000000:
						{ PROC_POP10_NS::Execute(instruction, processor); return true; }
					case 0x60000000:
						{ PROC_POP30_NS::Execute(instruction, processor); return true; }
					case 0x14000000:
						{ PROC_BNE_NS::Execute(instruction, processor); return true; }
					case 0x08000000:
						{ PROC_J_NS::Execute(instruction, processor); return true; }
					case 0x0C000000:
						{ PROC_JAL_NS::Execute(instruction, processor); return true; }
					case 0x80000000:
						{ PROC_LB_NS::Execute(instruction, processor); return true; }
					case 0x90000000:
						{ PROC_LBU_NS::Execute(instruction, processor); return true; }
					case 0x84000000:
						{ PROC_LH_NS::Execute(instruction, processor); return true; }
					case 0x94000000:
						{ PROC_LHU_NS::Execute(instruction, processor); return true; }
					case 0x8C000000:
						{ PROC_LW_NS::Execute(instruction, processor); return true; }
					case 0x34000000:
						{ PROC_ORI_NS::Execute(instruction, processor); return true; }
					case 0xA0000000:
						{ PROC_SB_NS::Execute(instruction, processor); return true; }
					case 0xA4000000:
						{ PROC_SH_NS::Execute(instruction, processor); return true; }
					case 0x28000000:
						{ PROC_SLTI_NS::Execute(instruction, processor); return true; }
					case 0x2C000000:
						{ PROC_SLTIU_NS::Execute(instruction, processor); return true; }
					case 0xAC000000:
						{ PROC_SW_NS::Execute(instruction, processor); return true; }
					case 0x38000000:
						{ PROC_XORI_NS::Execute(instruction, processor); return true; }
					case 0xEC000000:
						switch(instruction & 0x001F0000) {
							case 0x001F0000:
								{ PROC_ALUIPC_NS::Execute(instruction, processor); return true; }
							case 0x001E0000:
								{ PROC_AUIPC_NS::Execute(instruction, processor); return true; }
							default:
								switch(instruction & 0xFC180000) {
									case 0xEC080000:
										{ PROC_LWPC_NS::Execute(instruction, processor); return true; }
									case 0xEC000000:
										{ PROC_ADDIUPC_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
						}
					case 0x58000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							{ PROC_BLEZC_NS::Execute(instruction, processor); return true; }
						else if ((instruction & 0xFC000000) == 0x58000000)
							{ PROC_POP26_NS::Execute(instruction, processor); return true; }
						else
							return false;
					case 0x5C000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							{ PROC_BGTZC_NS::Execute(instruction, processor); return true; }
						else if ((instruction & 0xFC000000) == 0x5C000000)
							{ PROC_POP27_NS::Execute(instruction, processor); return true; }
						else
							return false;
					case 0xD8000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							{ PROC_JIC_NS::Execute(instruction, processor); return true; }
						else if ((instruction & 0xFC000000) == 0xD8000000)
							{ PROC_BEQZC_NS::Execute(instruction, processor); return true; }
						else
							return false;
					case 0xF8000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							{ PROC_JIALC_NS::Execute(instruction, processor); return true; }
						else if ((instruction & 0xFC000000) == 0xF8000000)
							{ PROC_BNEZC_NS::Execute(instruction, processor); return true; }
						else
							return false;
					case 0x1C000000:
						switch(instruction & 0x03E00000) {
							case 0x00000000:
								{ PROC_BGTZALC_NS::Execute(instruction, processor); return true; }
							default:
								if      ((instruction & 0xFC1F0000) == 0x1C000000)
									{ PROC_BGTZ_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC000000) == 0x1C000000)
									{ PROC_POP07_NS::Execute(instruction, processor); return true; }
								else
									return false;
						}
					case 0x40000000:
						switch(instruction & 0x02000058) {
							case 0x00000000:
								switch(instruction & 0x01E007A0) {
									case 0x00800000:
										{ PROC_MTC0_NS::Execute(instruction, processor); return true; }
									case 0x00400000:
										{ PROC_MFHC0_NS::Execute(instruction, processor); return true; }
									case 0x00000000:
										{ PROC_MFC0_NS::Execute(instruction, processor); return true; }
									case 0x00C00000:
										{ PROC_MTHC0_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x02000040:
								if      ((instruction & 0x01FFFF80) == 0x00000000)
									{ PROC_ERETNC_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFE00007F) == 0x42000060)
									{ PROC_WAIT_NS::Execute(instruction, processor); return true; }
								else
									return false;
							case 0x02000000:
								{ PROC_ERET_NS::Execute(instruction, processor); return true; }
							default:
								return false;
						}
					case 0x00000000:
						switch(instruction & 0x0000003F) {
							case 0x0000000F:
								{ PROC_SYNC_NS::Execute(instruction, processor); return true; }
							case 0x00000021:
								if      ((instruction & 0x001F07C0) == 0x00000040)
									{ PROC_CLO_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC0007FF) == 0x00000021)
									{ PROC_ADDU_NS::Execute(instruction, processor); return true; }
								else
									return false;
							case 0x00000009:
								switch(instruction & 0x001FFC00) {
									case 0x00000000:
										{ PROC_JR_NS::Execute(instruction, processor); return true; }
									case 0x00000400:
										{ PROC_JR_HB_NS::Execute(instruction, processor); return true; }
									default:
										switch(instruction & 0xFC1F043F) {
											case 0x00000409:
												{ PROC_JALR_HB_NS::Execute(instruction, processor); return true; }
											case 0x00000009:
												{ PROC_JALR_NS::Execute(instruction, processor); return true; }
											default:
												return false;
										}
								}
							case 0x00000010:
								{ PROC_CLZ_NS::Execute(instruction, processor); return true; }
							case 0x00000006:
								switch(instruction & 0x000007C0) {
									case 0x00000040:
										{ PROC_ROTRV_NS::Execute(instruction, processor); return true; }
									case 0x00000000:
										{ PROC_SRLV_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x00000026:
								{ PROC_XOR_NS::Execute(instruction, processor); return true; }
							case 0x00000023:
								{ PROC_SUBU_NS::Execute(instruction, processor); return true; }
							case 0x00000022:
								{ PROC_SUB_NS::Execute(instruction, processor); return true; }
							case 0x00000002:
								switch(instruction & 0x03E00000) {
									case 0x00200000:
										{ PROC_ROTR_NS::Execute(instruction, processor); return true; }
									case 0x00000000:
										{ PROC_SRL_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x00000007:
								{ PROC_SRAV_NS::Execute(instruction, processor); return true; }
							case 0x00000003:
								{ PROC_SRA_NS::Execute(instruction, processor); return true; }
							case 0x0000002B:
								{ PROC_SLTU_NS::Execute(instruction, processor); return true; }
							case 0x0000002A:
								{ PROC_SLT_NS::Execute(instruction, processor); return true; }
							case 0x00000004:
								{ PROC_SLLV_NS::Execute(instruction, processor); return true; }
							case 0x00000000:
								{ PROC_SLL_NS::Execute(instruction, processor); return true; }
							case 0x00000037:
								{ PROC_SELNEZ_NS::Execute(instruction, processor); return true; }
							case 0x00000035:
								{ PROC_SELEQZ_NS::Execute(instruction, processor); return true; }
							case 0x00000025:
								{ PROC_OR_NS::Execute(instruction, processor); return true; }
							case 0x00000027:
								{ PROC_NOR_NS::Execute(instruction, processor); return true; }
							case 0x00000019:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										{ PROC_MULU_NS::Execute(instruction, processor); return true; }
									case 0x000000C0:
										{ PROC_MUHU_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x00000018:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										{ PROC_MUL_NS::Execute(instruction, processor); return true; }
									case 0x000000C0:
										{ PROC_MUH_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x0000001B:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										{ PROC_DIVU_NS::Execute(instruction, processor); return true; }
									case 0x000000C0:
										{ PROC_MODU_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x0000001A:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										{ PROC_DIV_NS::Execute(instruction, processor); return true; }
									case 0x000000C0:
										{ PROC_MOD_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
							case 0x00000024:
								{ PROC_AND_NS::Execute(instruction, processor); return true; }
							case 0x00000020:
								{ PROC_ADD_NS::Execute(instruction, processor); return true; }
							case 0x00000005:
								{ PROC_LSA_NS::Execute(instruction, processor); return true; }
							case 0x00000036:
								{ PROC_TNE_NS::Execute(instruction, processor); return true; }
							case 0x00000033:
								{ PROC_TLTU_NS::Execute(instruction, processor); return true; }
							case 0x00000032:
								{ PROC_TLT_NS::Execute(instruction, processor); return true; }
							case 0x00000031:
								{ PROC_TGEU_NS::Execute(instruction, processor); return true; }
							case 0x00000030:
								{ PROC_TGE_NS::Execute(instruction, processor); return true; }
							case 0x00000034:
								{ PROC_TEQ_NS::Execute(instruction, processor); return true; }
							case 0x0000000C:
								{ PROC_SYSCALL_NS::Execute(instruction, processor); return true; }
							case 0x0000000E:
								{ PROC_SDBBP_NS::Execute(instruction, processor); return true; }
							case 0x0000000D:
								{ PROC_BREAK_NS::Execute(instruction, processor); return true; }
							default:
								return false;
						}
					case 0x7C000000:
						switch(instruction & 0x0000003F) {
							case 0x0000003D:
								{ PROC_GINVI_NS::Execute(instruction, processor); return true; }
							case 0x00000020:
								switch(instruction & 0x03E007C0) {
									case 0x00000600:
										{ PROC_SEH_NS::Execute(instruction, processor); return true; }
									case 0x00000400:
										{ PROC_SEB_NS::Execute(instruction, processor); return true; }
									case 0x00000000:
										{ PROC_BITSWAP_NS::Execute(instruction, processor); return true; }
									case 0x00000080:
										{ PROC_WSBH_NS::Execute(instruction, processor); return true; }
									default:
										if      ((instruction & 0xFC00073F) == 0x7C000220)
											{ PROC_ALIGN_NS::Execute(instruction, processor); return true; }
										else
											return false;
								}
							case 0x0000003B:
								{ PROC_RDHWR_NS::Execute(instruction, processor); return true; }
							case 0x00000036:
								if      ((instruction & 0x000007C0) == 0x00000040)
									{ PROC_LLWP_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC00007F) == 0x7C000036)
									{ PROC_LL_NS::Execute(instruction, processor); return true; }
								else
									return false;
							case 0x0000002E:
								if      ((instruction & 0x000007C0) == 0x00000040)
									{ PROC_LLWPE_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC00007F) == 0x7C00002E)
									{ PROC_LLE_NS::Execute(instruction, processor); return true; }
								else
									return false;
							case 0x00000026:
								if      ((instruction & 0x000007C0) == 0x00000040)
									{ PROC_SCWP_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC00007F) == 0x7C000026)
									{ PROC_SC_NS::Execute(instruction, processor); return true; }
								else
									return false;
							case 0x0000001E:
								if      ((instruction & 0x000007C0) == 0x00000040)
									{ PROC_SCWPE_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC00007F) == 0x7C00001E)
									{ PROC_SCE_NS::Execute(instruction, processor); return true; }
								else
									return false;
							case 0x0000001F:
								{ PROC_SWE_NS::Execute(instruction, processor); return true; }
							case 0x0000001D:
								{ PROC_SHE_NS::Execute(instruction, processor); return true; }
							case 0x0000001C:
								{ PROC_SBE_NS::Execute(instruction, processor); return true; }
							case 0x00000023:
								{ PROC_PREFE_NS::Execute(instruction, processor); return true; }
							case 0x00000035:
								{ PROC_PREF_NS::Execute(instruction, processor); return true; }
							case 0x0000002F:
								{ PROC_LWE_NS::Execute(instruction, processor); return true; }
							case 0x00000029:
								{ PROC_LHUE_NS::Execute(instruction, processor); return true; }
							case 0x0000002D:
								{ PROC_LHE_NS::Execute(instruction, processor); return true; }
							case 0x00000028:
								{ PROC_LBUE_NS::Execute(instruction, processor); return true; }
							case 0x0000002C:
								{ PROC_LBE_NS::Execute(instruction, processor); return true; }
							case 0x00000025:
								{ PROC_CACHE_NS::Execute(instruction, processor); return true; }
							case 0x00000004:
								{ PROC_INS_NS::Execute(instruction, processor); return true; }
							case 0x00000000:
								{ PROC_EXT_NS::Execute(instruction, processor); return true; }
							default:
								return false;
						}
					case 0x24000000:
						if      ((instruction & 0x0000FFFF) == 0x00000000)
							{ PROC_MOVE_NS::Execute(instruction, processor); return true; }
						else if ((instruction & 0xFC000000) == 0x24000000)
							{ PROC_ADDIU_NS::Execute(instruction, processor); return true; }
						else
							return false;
					case 0x44000000:
						switch(instruction & 0x03E00000) {
							case 0x01200000:
								{ COP1_BC1EQZ_NS::Execute<void>(instruction, processor); return true; }
							case 0x01A00000:
								{ COP1_BC1NEZ_NS::Execute<void>(instruction, processor); return true; }
							case 0x02800000:
								switch(instruction & 0x001F003F) {
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<int32>(instruction, processor); return true; }
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<int32>(instruction, processor); return true; }
									default:
										if      ((instruction & 0xFFE00020) == 0x46800000)
											{ COP1_CMP_condn_s_NS::Execute<void>(instruction, processor); return true; }
										else
											return false;
								}
							case 0x02A00000:
								switch(instruction & 0x001F003F) {
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<int64>(instruction, processor); return true; }
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<int64>(instruction, processor); return true; }
									default:
										if      ((instruction & 0xFFE00020) == 0x46A00000)
											{ COP1_CMP_condn_d_NS::Execute<void>(instruction, processor); return true; }
										else
											return false;
								}
							case 0x02000000:
								switch(instruction & 0x001F003F) {
									case 0x00000025:
										{ COP1_CVT_L_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000005:
										{ COP1_ABS_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000000D:
										{ COP1_TRUNC_W_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000001B:
										{ COP1_CLASS_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000024:
										{ COP1_CVT_W_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000006:
										{ COP1_MOV_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000007:
										{ COP1_NEG_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000015:
										{ COP1_RECIP_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000001A:
										{ COP1_RINT_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000016:
										{ COP1_RSQRT_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000004:
										{ COP1_SQRT_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000008:
										{ COP1_ROUND_L_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000000C:
										{ COP1_ROUND_W_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000000A:
										{ COP1_CEIL_L_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000000E:
										{ COP1_CEIL_W_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000000B:
										{ COP1_FLOOR_L_NS::Execute<float>(instruction, processor); return true; }
									case 0x0000000F:
										{ COP1_FLOOR_W_NS::Execute<float>(instruction, processor); return true; }
									case 0x00000009:
										{ COP1_TRUNC_L_NS::Execute<float>(instruction, processor); return true; }
									default:
										switch(instruction & 0xFFE0003F) {
											case 0x46000018:
												{ COP1_MADDF_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000000:
												{ COP1_ADD_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000003:
												{ COP1_DIV_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000019:
												{ COP1_MSUBF_NS::Execute<float>(instruction, processor); return true; }
											case 0x4600001E:
												{ COP1_MAX_NS::Execute<float>(instruction, processor); return true; }
											case 0x4600001C:
												{ COP1_MIN_NS::Execute<float>(instruction, processor); return true; }
											case 0x4600001F:
												{ COP1_MAXA_NS::Execute<float>(instruction, processor); return true; }
											case 0x4600001D:
												{ COP1_MINA_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000002:
												{ COP1_MUL_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000010:
												{ COP1_SEL_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000014:
												{ COP1_SELEQZ_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000017:
												{ COP1_SELNEZ_NS::Execute<float>(instruction, processor); return true; }
											case 0x46000001:
												{ COP1_SUB_NS::Execute<float>(instruction, processor); return true; }
											default:
												return false;
										}
								}
							case 0x02200000:
								switch(instruction & 0x001F003F) {
									case 0x00000021:
										{ COP1_CVT_D_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000009:
										{ COP1_TRUNC_L_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000025:
										{ COP1_CVT_L_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000005:
										{ COP1_ABS_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000000D:
										{ COP1_TRUNC_W_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000001B:
										{ COP1_CLASS_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000020:
										{ COP1_CVT_S_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000024:
										{ COP1_CVT_W_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000006:
										{ COP1_MOV_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000007:
										{ COP1_NEG_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000015:
										{ COP1_RECIP_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000001A:
										{ COP1_RINT_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000016:
										{ COP1_RSQRT_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000004:
										{ COP1_SQRT_NS::Execute<double>(instruction, processor); return true; }
									case 0x00000008:
										{ COP1_ROUND_L_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000000C:
										{ COP1_ROUND_W_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000000A:
										{ COP1_CEIL_L_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000000E:
										{ COP1_CEIL_W_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000000B:
										{ COP1_FLOOR_L_NS::Execute<double>(instruction, processor); return true; }
									case 0x0000000F:
										{ COP1_FLOOR_W_NS::Execute<double>(instruction, processor); return true; }
									default:
										switch(instruction & 0xFFE0003F) {
											case 0x46200018:
												{ COP1_MADDF_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200000:
												{ COP1_ADD_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200003:
												{ COP1_DIV_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200019:
												{ COP1_MSUBF_NS::Execute<double>(instruction, processor); return true; }
											case 0x4620001E:
												{ COP1_MAX_NS::Execute<double>(instruction, processor); return true; }
											case 0x4620001C:
												{ COP1_MIN_NS::Execute<double>(instruction, processor); return true; }
											case 0x4620001F:
												{ COP1_MAXA_NS::Execute<double>(instruction, processor); return true; }
											case 0x4620001D:
												{ COP1_MINA_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200002:
												{ COP1_MUL_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200010:
												{ COP1_SEL_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200014:
												{ COP1_SELEQZ_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200017:
												{ COP1_SELNEZ_NS::Execute<double>(instruction, processor); return true; }
											case 0x46200001:
												{ COP1_SUB_NS::Execute<double>(instruction, processor); return true; }
											default:
												return false;
										}
								}
							case 0x00600000:
								{ COP1_MFHC1_NS::Execute<void>(instruction, processor); return true; }
							case 0x00E00000:
								{ COP1_MTHC1_NS::Execute<void>(instruction, processor); return true; }
							case 0x00000000:
								{ COP1_MFC1_NS::Execute<void>(instruction, processor); return true; }
							case 0x00800000:
								{ COP1_MTC1_NS::Execute<void>(instruction, processor); return true; }
							case 0x00400000:
								{ COP1_CFC1_NS::Execute<void>(instruction, processor); return true; }
							case 0x00C00000:
								{ COP1_CTC1_NS::Execute<void>(instruction, processor); return true; }
							default:
								return false;
						}
					case 0x48000000:
						switch(instruction & 0x03E007F8) {
							case 0x00800000:
								{ PROC_MTC2_NS::Execute(instruction, processor); return true; }
							case 0x00600000:
								{ PROC_MFHC2_NS::Execute(instruction, processor); return true; }
							case 0x00000000:
								{ PROC_MFC2_NS::Execute(instruction, processor); return true; }
							case 0x00E00000:
								{ PROC_MTHC2_NS::Execute(instruction, processor); return true; }
							default:
								return false;
						}
					case 0x04000000:
						switch(instruction & 0x03FF0000) {
							case 0x00100000:
								{ PROC_NAL_NS::Execute(instruction, processor); return true; }
							case 0x00110000:
								{ PROC_BAL_NS::Execute(instruction, processor); return true; }
							case 0x00170000:
								{ PROC_SIGRIE_NS::Execute(instruction, processor); return true; }
							default:
								switch(instruction & 0xFC1F0000) {
									case 0x041F0000:
										{ PROC_SYNCI_NS::Execute(instruction, processor); return true; }
									case 0x04000000:
										{ PROC_BLTZ_NS::Execute(instruction, processor); return true; }
									case 0x04010000:
										{ PROC_BGEZ_NS::Execute(instruction, processor); return true; }
									default:
										return false;
								}
						}
					case 0x18000000:
						switch(instruction & 0x03E00000) {
							case 0x00000000:
								{ PROC_BLEZALC_NS::Execute(instruction, processor); return true; }
							default:
								if      ((instruction & 0xFC1F0000) == 0x18000000)
									{ PROC_BLEZ_NS::Execute(instruction, processor); return true; }
								else if ((instruction & 0xFC000000) == 0x18000000)
									{ PROC_POP06_NS::Execute(instruction, processor); return true; }
								else
									return false;
						}
					default:
						return false;
				}
		}
		return false;
	}

	// ReSharper disable CppUnreachableCode
	template <bool AssumeSafe, bool AlternateImplementation>
	std::conditional_t<AssumeSafe, void, bool> execute_instruction(
		const uint32 instruction_index,
		const instruction_t instruction,
		processor& __restrict processor
	) {
		if constexpr (!AlternateImplementation) {
			if constexpr (AssumeSafe) {
				AllInstructionsProcs[instruction_index](instruction, processor);
				return;
			}
			else {
				if _likely(instruction_index < std::size(AllInstructionsProcs)) [[likely]] {
					AllInstructionsProcs[instruction_index](instruction, processor);
					return true;
				}
				return false;
			}
		}
		else {
			switch (instruction_index) {
			case 0:
				COP1_ABS_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 1:
				COP1_ABS_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 2:
				COP1_ADD_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 3:
				COP1_ADD_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 4:
				COP1_BC1EQZ_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 5:
				COP1_BC1NEZ_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 6:
				COP1_CEIL_L_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 7:
				COP1_CEIL_L_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 8:
				COP1_CEIL_W_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 9:
				COP1_CEIL_W_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 10:
				COP1_CFC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 11:
				COP1_CLASS_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 12:
				COP1_CLASS_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 13:
				COP1_CMP_condn_d_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 14:
				COP1_CMP_condn_s_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 15:
				COP1_CTC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 16:
				COP1_CVT_D_NS::Execute<int32>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 17:
				COP1_CVT_D_NS::Execute<int64>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 18:
				COP1_CVT_D_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 19:
				COP1_CVT_D_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 20:
				COP1_CVT_L_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 21:
				COP1_CVT_L_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 22:
				COP1_CVT_S_NS::Execute<int32>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 23:
				COP1_CVT_S_NS::Execute<int64>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 24:
				COP1_CVT_S_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 25:
				COP1_CVT_S_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 26:
				COP1_CVT_W_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 27:
				COP1_CVT_W_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 28:
				COP1_DIV_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 29:
				COP1_DIV_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 30:
				COP1_FLOOR_L_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 31:
				COP1_FLOOR_L_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 32:
				COP1_FLOOR_W_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 33:
				COP1_FLOOR_W_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 34:
				COP1_LDC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 35:
				COP1_LWC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 36:
				COP1_MADDF_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 37:
				COP1_MADDF_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 38:
				COP1_MAX_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 39:
				COP1_MAX_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 40:
				COP1_MAXA_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 41:
				COP1_MAXA_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 42:
				COP1_MFC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 43:
				COP1_MFHC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 44:
				COP1_MIN_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 45:
				COP1_MIN_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 46:
				COP1_MINA_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 47:
				COP1_MINA_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 48:
				COP1_MOV_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 49:
				COP1_MOV_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 50:
				COP1_MSUBF_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 51:
				COP1_MSUBF_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 52:
				COP1_MTC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 53:
				COP1_MTHC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 54:
				COP1_MUL_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 55:
				COP1_MUL_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 56:
				COP1_NEG_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 57:
				COP1_NEG_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 58:
				COP1_RECIP_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 59:
				COP1_RECIP_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 60:
				COP1_RINT_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 61:
				COP1_RINT_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 62:
				COP1_ROUND_L_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 63:
				COP1_ROUND_L_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 64:
				COP1_ROUND_W_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 65:
				COP1_ROUND_W_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 66:
				COP1_RSQRT_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 67:
				COP1_RSQRT_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 68:
				COP1_SDC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 69:
				COP1_SEL_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 70:
				COP1_SEL_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 71:
				COP1_SELEQZ_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 72:
				COP1_SELEQZ_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 73:
				COP1_SELNEZ_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 74:
				COP1_SELNEZ_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 75:
				COP1_SQRT_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 76:
				COP1_SQRT_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 77:
				COP1_SUB_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 78:
				COP1_SUB_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 79:
				COP1_SWC1_NS::Execute<void>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 80:
				COP1_TRUNC_L_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 81:
				COP1_TRUNC_L_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 82:
				COP1_TRUNC_W_NS::Execute<float>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 83:
				COP1_TRUNC_W_NS::Execute<double>(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 84:
				PROC_ADD_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 85:
				PROC_ADDIU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 86:
				PROC_ADDIUPC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 87:
				PROC_ADDU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 88:
				PROC_ALIGN_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 89:
				PROC_ALUIPC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 90:
				PROC_AND_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 91:
				PROC_ANDI_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 92:
				PROC_AUI_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 93:
				PROC_AUIPC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 94:
				PROC_BAL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 95:
				PROC_BALC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 96:
				PROC_BC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 97:
				PROC_BEQ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 98:
				PROC_BEQZC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 99:
				PROC_BGEZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 100:
				PROC_BGTZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 101:
				PROC_BGTZALC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 102:
				PROC_BGTZC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 103:
				PROC_BITSWAP_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 104:
				PROC_BLEZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 105:
				PROC_BLEZALC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 106:
				PROC_BLEZC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 107:
				PROC_BLTZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 108:
				PROC_BNE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 109:
				PROC_BNEZC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 110:
				PROC_BREAK_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 111:
				PROC_CACHE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 112:
				PROC_CLO_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 113:
				PROC_CLZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 114:
				PROC_DIV_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 115:
				PROC_DIVU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 116:
				PROC_EHB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 117:
				PROC_ERET_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 118:
				PROC_ERETNC_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 119:
				PROC_EXT_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 120:
				PROC_GINVI_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 121:
				PROC_INS_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 122:
				PROC_J_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 123:
				PROC_JAL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 124:
				PROC_JALR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 125:
				PROC_JALR_HB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 126:
				PROC_JIALC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 127:
				PROC_JIC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 128:
				PROC_JR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 129:
				PROC_JR_HB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 130:
				PROC_LB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 131:
				PROC_LBE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 132:
				PROC_LBU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 133:
				PROC_LBUE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 134:
				PROC_LH_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 135:
				PROC_LHE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 136:
				PROC_LHU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 137:
				PROC_LHUE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 138:
				PROC_LL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 139:
				PROC_LLE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 140:
				PROC_LLWP_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 141:
				PROC_LLWPE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 142:
				PROC_LSA_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 143:
				PROC_LW_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 144:
				PROC_LWE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 145:
				PROC_LWPC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 146:
				PROC_MFC0_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 147:
				PROC_MFC2_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 148:
				PROC_MFHC0_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 149:
				PROC_MFHC2_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 150:
				PROC_MOD_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 151:
				PROC_MODU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 152:
				PROC_MOVE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 153:
				PROC_MTC0_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 154:
				PROC_MTC2_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 155:
				PROC_MTHC0_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 156:
				PROC_MTHC2_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 157:
				PROC_MUH_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 158:
				PROC_MUHU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 159:
				PROC_MUL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 160:
				PROC_MULU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 161:
				PROC_NAL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 162:
				PROC_NOP_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 163:
				PROC_NOR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 164:
				PROC_OR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 165:
				PROC_ORI_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 166:
				PROC_PAUSE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 167:
				PROC_POP06_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 168:
				PROC_POP07_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 169:
				PROC_POP10_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 170:
				PROC_POP26_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 171:
				PROC_POP27_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 172:
				PROC_POP30_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 173:
				PROC_PREF_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 174:
				PROC_PREFE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 175:
				PROC_RDHWR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 176:
				PROC_ROTR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 177:
				PROC_ROTRV_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 178:
				PROC_SB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 179:
				PROC_SBE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 180:
				PROC_SC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 181:
				PROC_SCE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 182:
				PROC_SCWP_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 183:
				PROC_SCWPE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 184:
				PROC_SDBBP_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 185:
				PROC_SEB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 186:
				PROC_SEH_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 187:
				PROC_SELEQZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 188:
				PROC_SELNEZ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 189:
				PROC_SH_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 190:
				PROC_SHE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 191:
				PROC_SIGRIE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 192:
				PROC_SLL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 193:
				PROC_SLLV_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 194:
				PROC_SLT_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 195:
				PROC_SLTI_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 196:
				PROC_SLTIU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 197:
				PROC_SLTU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 198:
				PROC_SRA_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 199:
				PROC_SRAV_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 200:
				PROC_SRL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 201:
				PROC_SRLV_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 202:
				PROC_SSNOP_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 203:
				PROC_SUB_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 204:
				PROC_SUBU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 205:
				PROC_SW_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 206:
				PROC_SWE_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 207:
				PROC_SYNC_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 208:
				PROC_SYNCI_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 209:
				PROC_SYSCALL_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 210:
				PROC_TEQ_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 211:
				PROC_TGE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 212:
				PROC_TGEU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 213:
				PROC_TLT_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 214:
				PROC_TLTU_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 215:
				PROC_TNE_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 216:
				PROC_WAIT_NS::Execute(instruction, processor);
				xunreachable("Instruction is always supposed to throw.");
			case 217:
				PROC_WSBH_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 218:
				PROC_XOR_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			case 219:
				PROC_XORI_NS::Execute(instruction, processor);
				if constexpr (AssumeSafe) {
					return;
				}
				else {
					return true;
				}
			default:
				if constexpr (AssumeSafe) {
					xunreachable("Instruction was not recognized but AssumeSafe is true.");
				}
				else {
					return false;
				}
			}
		}
	}
	// ReSharper restore CppUnreachableCode

	template
	bool execute_instruction<false, false>(
		uint32 instruction_index,
		instruction_t instruction,
		processor& __restrict processor
	);
	template
	bool execute_instruction<false, true>(
		uint32 instruction_index,
		instruction_t instruction,
		processor& __restrict processor
	);
	template
	void execute_instruction<true, false>(
		uint32 instruction_index,
		instruction_t instruction,
		processor& __restrict processor
	);
	template
	void execute_instruction<true, true>(
		uint32 instruction_index,
		instruction_t instruction,
		processor& __restrict processor
	);

	_nothrow uint32 get_instruction_index(const instruction_t instruction) noexcept {
		switch(instruction) {
			case 0x00000040:
				return 202;
			case 0x00000140:
				return 166;
			case 0x00000000:
				return 162;
			case 0x000000C0:
				return 116;
			default:
				switch(instruction & 0xFC000000) {
					case 0xD4000000:
						return 34;
					case 0xC4000000:
						return 35;
					case 0xF4000000:
						return 68;
					case 0xE4000000:
						return 79;
					case 0x30000000:
						return 91;
					case 0x3C000000:
						return 92;
					case 0xE8000000:
						return 95;
					case 0xC8000000:
						return 96;
					case 0x10000000:
						return 97;
					case 0x20000000:
						return 169;
					case 0x60000000:
						return 172;
					case 0x14000000:
						return 108;
					case 0x08000000:
						return 122;
					case 0x0C000000:
						return 123;
					case 0x80000000:
						return 130;
					case 0x90000000:
						return 132;
					case 0x84000000:
						return 134;
					case 0x94000000:
						return 136;
					case 0x8C000000:
						return 143;
					case 0x34000000:
						return 165;
					case 0xA0000000:
						return 178;
					case 0xA4000000:
						return 189;
					case 0x28000000:
						return 195;
					case 0x2C000000:
						return 196;
					case 0xAC000000:
						return 205;
					case 0x38000000:
						return 219;
					case 0xEC000000:
						switch(instruction & 0x001F0000) {
							case 0x001F0000:
								return 89;
							case 0x001E0000:
								return 93;
							default:
								switch(instruction & 0xFC180000) {
									case 0xEC080000:
										return 145;
									case 0xEC000000:
										return 86;
									default:
										return std::numeric_limits<uint32>::max();
								}
						}
					case 0x58000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return 106;
						else if ((instruction & 0xFC000000) == 0x58000000)
							return 170;
						else
							return std::numeric_limits<uint32>::max();
					case 0x5C000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return 102;
						else if ((instruction & 0xFC000000) == 0x5C000000)
							return 171;
						else
							return std::numeric_limits<uint32>::max();
					case 0xD8000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return 127;
						else if ((instruction & 0xFC000000) == 0xD8000000)
							return 98;
						else
							return std::numeric_limits<uint32>::max();
					case 0xF8000000:
						if      ((instruction & 0x03E00000) == 0x00000000)
							return 126;
						else if ((instruction & 0xFC000000) == 0xF8000000)
							return 109;
						else
							return std::numeric_limits<uint32>::max();
					case 0x1C000000:
						switch(instruction & 0x03E00000) {
							case 0x00000000:
								return 101;
							default:
								if      ((instruction & 0xFC1F0000) == 0x1C000000)
									return 100;
								else if ((instruction & 0xFC000000) == 0x1C000000)
									return 168;
								else
									return std::numeric_limits<uint32>::max();
						}
					case 0x40000000:
						switch(instruction & 0x02000058) {
							case 0x00000000:
								switch(instruction & 0x01E007A0) {
									case 0x00800000:
										return 153;
									case 0x00400000:
										return 148;
									case 0x00000000:
										return 146;
									case 0x00C00000:
										return 155;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x02000040:
								if      ((instruction & 0x01FFFF80) == 0x00000000)
									return 118;
								else if ((instruction & 0xFE00007F) == 0x42000060)
									return 216;
								else
									return std::numeric_limits<uint32>::max();
							case 0x02000000:
								return 117;
							default:
								return std::numeric_limits<uint32>::max();
						}
					case 0x00000000:
						switch(instruction & 0x0000003F) {
							case 0x0000000F:
								return 207;
							case 0x00000021:
								if      ((instruction & 0x001F07C0) == 0x00000040)
									return 112;
								else if ((instruction & 0xFC0007FF) == 0x00000021)
									return 87;
								else
									return std::numeric_limits<uint32>::max();
							case 0x00000009:
								switch(instruction & 0x001FFC00) {
									case 0x00000000:
										return 128;
									case 0x00000400:
										return 129;
									default:
										switch(instruction & 0xFC1F043F) {
											case 0x00000409:
												return 125;
											case 0x00000009:
												return 124;
											default:
												return std::numeric_limits<uint32>::max();
										}
								}
							case 0x00000010:
								return 113;
							case 0x00000006:
								switch(instruction & 0x000007C0) {
									case 0x00000040:
										return 177;
									case 0x00000000:
										return 201;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x00000026:
								return 218;
							case 0x00000023:
								return 204;
							case 0x00000022:
								return 203;
							case 0x00000002:
								switch(instruction & 0x03E00000) {
									case 0x00200000:
										return 176;
									case 0x00000000:
										return 200;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x00000007:
								return 199;
							case 0x00000003:
								return 198;
							case 0x0000002B:
								return 197;
							case 0x0000002A:
								return 194;
							case 0x00000004:
								return 193;
							case 0x00000000:
								return 192;
							case 0x00000037:
								return 188;
							case 0x00000035:
								return 187;
							case 0x00000025:
								return 164;
							case 0x00000027:
								return 163;
							case 0x00000019:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return 160;
									case 0x000000C0:
										return 158;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x00000018:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return 159;
									case 0x000000C0:
										return 157;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x0000001B:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return 115;
									case 0x000000C0:
										return 151;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x0000001A:
								switch(instruction & 0x000007C0) {
									case 0x00000080:
										return 114;
									case 0x000000C0:
										return 150;
									default:
										return std::numeric_limits<uint32>::max();
								}
							case 0x00000024:
								return 90;
							case 0x00000020:
								return 84;
							case 0x00000005:
								return 142;
							case 0x00000036:
								return 215;
							case 0x00000033:
								return 214;
							case 0x00000032:
								return 213;
							case 0x00000031:
								return 212;
							case 0x00000030:
								return 211;
							case 0x00000034:
								return 210;
							case 0x0000000C:
								return 209;
							case 0x0000000E:
								return 184;
							case 0x0000000D:
								return 110;
							default:
								return std::numeric_limits<uint32>::max();
						}
					case 0x7C000000:
						switch(instruction & 0x0000003F) {
							case 0x0000003D:
								return 120;
							case 0x00000020:
								switch(instruction & 0x03E007C0) {
									case 0x00000600:
										return 186;
									case 0x00000400:
										return 185;
									case 0x00000000:
										return 103;
									case 0x00000080:
										return 217;
									default:
										if      ((instruction & 0xFC00073F) == 0x7C000220)
											return 88;
										else
											return std::numeric_limits<uint32>::max();
								}
							case 0x0000003B:
								return 175;
							case 0x00000036:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return 140;
								else if ((instruction & 0xFC00007F) == 0x7C000036)
									return 138;
								else
									return std::numeric_limits<uint32>::max();
							case 0x0000002E:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return 141;
								else if ((instruction & 0xFC00007F) == 0x7C00002E)
									return 139;
								else
									return std::numeric_limits<uint32>::max();
							case 0x00000026:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return 182;
								else if ((instruction & 0xFC00007F) == 0x7C000026)
									return 180;
								else
									return std::numeric_limits<uint32>::max();
							case 0x0000001E:
								if      ((instruction & 0x000007C0) == 0x00000040)
									return 183;
								else if ((instruction & 0xFC00007F) == 0x7C00001E)
									return 181;
								else
									return std::numeric_limits<uint32>::max();
							case 0x0000001F:
								return 206;
							case 0x0000001D:
								return 190;
							case 0x0000001C:
								return 179;
							case 0x00000023:
								return 174;
							case 0x00000035:
								return 173;
							case 0x0000002F:
								return 144;
							case 0x00000029:
								return 137;
							case 0x0000002D:
								return 135;
							case 0x00000028:
								return 133;
							case 0x0000002C:
								return 131;
							case 0x00000025:
								return 111;
							case 0x00000004:
								return 121;
							case 0x00000000:
								return 119;
							default:
								return std::numeric_limits<uint32>::max();
						}
					case 0x24000000:
						if      ((instruction & 0x0000FFFF) == 0x00000000)
							return 152;
						else if ((instruction & 0xFC000000) == 0x24000000)
							return 85;
						else
							return std::numeric_limits<uint32>::max();
					case 0x44000000:
						switch(instruction & 0x03E00000) {
							case 0x01200000:
								return 4;
							case 0x01A00000:
								return 5;
							case 0x02800000:
								switch(instruction & 0x001F003F) {
									case 0x00000020:
										return 22;
									case 0x00000021:
										return 16;
									default:
										if      ((instruction & 0xFFE00020) == 0x46800000)
											return 14;
										else
											return std::numeric_limits<uint32>::max();
								}
							case 0x02A00000:
								switch(instruction & 0x001F003F) {
									case 0x00000020:
										return 23;
									case 0x00000021:
										return 17;
									default:
										if      ((instruction & 0xFFE00020) == 0x46A00000)
											return 13;
										else
											return std::numeric_limits<uint32>::max();
								}
							case 0x02000000:
								switch(instruction & 0x001F003F) {
									case 0x00000025:
										return 20;
									case 0x00000005:
										return 0;
									case 0x0000000D:
										return 82;
									case 0x0000001B:
										return 11;
									case 0x00000021:
										return 18;
									case 0x00000020:
										return 24;
									case 0x00000024:
										return 26;
									case 0x00000006:
										return 48;
									case 0x00000007:
										return 56;
									case 0x00000015:
										return 58;
									case 0x0000001A:
										return 60;
									case 0x00000016:
										return 66;
									case 0x00000004:
										return 75;
									case 0x00000008:
										return 62;
									case 0x0000000C:
										return 64;
									case 0x0000000A:
										return 6;
									case 0x0000000E:
										return 8;
									case 0x0000000B:
										return 30;
									case 0x0000000F:
										return 32;
									case 0x00000009:
										return 80;
									default:
										switch(instruction & 0xFFE0003F) {
											case 0x46000018:
												return 36;
											case 0x46000000:
												return 2;
											case 0x46000003:
												return 28;
											case 0x46000019:
												return 50;
											case 0x4600001E:
												return 38;
											case 0x4600001C:
												return 44;
											case 0x4600001F:
												return 40;
											case 0x4600001D:
												return 46;
											case 0x46000002:
												return 54;
											case 0x46000010:
												return 69;
											case 0x46000014:
												return 71;
											case 0x46000017:
												return 73;
											case 0x46000001:
												return 77;
											default:
												return std::numeric_limits<uint32>::max();
										}
								}
							case 0x02200000:
								switch(instruction & 0x001F003F) {
									case 0x00000021:
										return 19;
									case 0x00000009:
										return 81;
									case 0x00000025:
										return 21;
									case 0x00000005:
										return 1;
									case 0x0000000D:
										return 83;
									case 0x0000001B:
										return 12;
									case 0x00000020:
										return 25;
									case 0x00000024:
										return 27;
									case 0x00000006:
										return 49;
									case 0x00000007:
										return 57;
									case 0x00000015:
										return 59;
									case 0x0000001A:
										return 61;
									case 0x00000016:
										return 67;
									case 0x00000004:
										return 76;
									case 0x00000008:
										return 63;
									case 0x0000000C:
										return 65;
									case 0x0000000A:
										return 7;
									case 0x0000000E:
										return 9;
									case 0x0000000B:
										return 31;
									case 0x0000000F:
										return 33;
									default:
										switch(instruction & 0xFFE0003F) {
											case 0x46200018:
												return 37;
											case 0x46200000:
												return 3;
											case 0x46200003:
												return 29;
											case 0x46200019:
												return 51;
											case 0x4620001E:
												return 39;
											case 0x4620001C:
												return 45;
											case 0x4620001F:
												return 41;
											case 0x4620001D:
												return 47;
											case 0x46200002:
												return 55;
											case 0x46200010:
												return 70;
											case 0x46200014:
												return 72;
											case 0x46200017:
												return 74;
											case 0x46200001:
												return 78;
											default:
												return std::numeric_limits<uint32>::max();
										}
								}
							case 0x00600000:
								return 43;
							case 0x00E00000:
								return 53;
							case 0x00000000:
								return 42;
							case 0x00800000:
								return 52;
							case 0x00400000:
								return 10;
							case 0x00C00000:
								return 15;
							default:
								return std::numeric_limits<uint32>::max();
						}
					case 0x48000000:
						switch(instruction & 0x03E007F8) {
							case 0x00800000:
								return 154;
							case 0x00600000:
								return 149;
							case 0x00000000:
								return 147;
							case 0x00E00000:
								return 156;
							default:
								return std::numeric_limits<uint32>::max();
						}
					case 0x04000000:
						switch(instruction & 0x03FF0000) {
							case 0x00100000:
								return 161;
							case 0x00110000:
								return 94;
							case 0x00170000:
								return 191;
							default:
								switch(instruction & 0xFC1F0000) {
									case 0x041F0000:
										return 208;
									case 0x04000000:
										return 107;
									case 0x04010000:
										return 99;
									default:
										return std::numeric_limits<uint32>::max();
								}
						}
					case 0x18000000:
						switch(instruction & 0x03E00000) {
							case 0x00000000:
								return 105;
							default:
								if      ((instruction & 0xFC1F0000) == 0x18000000)
									return 104;
								else if ((instruction & 0xFC000000) == 0x18000000)
									return 167;
								else
									return std::numeric_limits<uint32>::max();
						}
					default:
						return std::numeric_limits<uint32>::max();
				}
		}
		return std::numeric_limits<uint32>::max();
	}
}
