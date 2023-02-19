/* WARNING: THIS IS AN AUTO-GENERATED FILE ('D:\Projects\vemips\src\tablegen\tablegen.cpp', Feb 15 2023 18:26:02) */
#include "pch.hpp"
#include "mips/instructions/instructions_table.hpp"
#include "mips/mips_common.hpp"
#include "mips/instructions/instructions_common.hpp"
#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor.hpp"

#define _export /* __declspec(dllexport) */ extern

namespace mips::instructions {
	namespace COP1_ABS_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_ADD_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_BC1EQZ_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_BC1NEZ_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CEIL_L_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CEIL_W_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CFC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CLASS_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CMP_condn_d_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CMP_condn_s_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CTC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_D_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_L_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_S_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_W_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_DIV_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_FLOOR_L_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_FLOOR_W_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_LDC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_LWC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MADDF_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MAX_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MAXA_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MFC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MFHC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MIN_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MINA_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MOV_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MSUBF_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MTC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MTHC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MUL_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_NEG_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_RECIP_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_RINT_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_ROUND_L_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_ROUND_W_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_RSQRT_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SDC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SEL_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SELEQZ_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SELNEZ_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SQRT_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SUB_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SWC1_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_TRUNC_L_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_TRUNC_W_NS { template <typename format_t> uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADD_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADDIU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADDIUPC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADDU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ALIGN_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ALUIPC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_AND_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ANDI_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_AUI_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_AUIPC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BAL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BALC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BEQ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BEQZC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGEZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGTZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGTZALC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGTZC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BITSWAP_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLEZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLEZALC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLEZC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLTZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BNE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BNEZC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BREAK_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_CACHE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_CLO_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_CLZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_DIV_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_DIVU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_EHB_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_EXT_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_INS_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_J_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JAL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JALR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JIALC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JIC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LB_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LBE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LBU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LBUE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LH_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LHE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LHU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LHUE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LLE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LLWP_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LLWPE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LSA_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LW_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LWE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LWPC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MOD_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MODU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MOVE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MUH_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MUHU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MUL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MULU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_NAL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_NOP_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_NOR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_OR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ORI_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_PAUSE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP06_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP07_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP10_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP26_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP27_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP30_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_PREF_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_PREFE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_RDHWR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ROTR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ROTRV_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SB_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SBE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SCE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SCWP_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SCWPE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SDBBP_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SEB_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SEH_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SELEQZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SELNEZ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SH_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SHE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SIGRIE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLLV_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLT_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLTI_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLTIU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLTU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRA_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRAV_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRLV_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SSNOP_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SUB_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SUBU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SW_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SWE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SYNC_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SYNCI_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SYSCALL_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TEQ_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TGE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TGEU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TLT_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TLTU_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TNE_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_WSBH_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_XOR_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_XORI_NS { extern uint64 Execute(instruction_t, mips::processor & __restrict); }
}

namespace mips::instructions {
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ABS_f ( "COP1_ABS", false, 1, &mips::instructions::COP1_ABS_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ABS_d ( "COP1_ABS", false, 1, &mips::instructions::COP1_ABS_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ADD_f ( "COP1_ADD", false, 1, &mips::instructions::COP1_ADD_NS::Execute<float>, 0x00800077u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ADD_d ( "COP1_ADD", false, 1, &mips::instructions::COP1_ADD_NS::Execute<double>, 0x00800077u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_BC1EQZ_v ( "COP1_BC1EQZ", true, 1, &mips::instructions::COP1_BC1EQZ_NS::Execute<void>, 0x00972000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_BC1NEZ_v ( "COP1_BC1NEZ", true, 1, &mips::instructions::COP1_BC1NEZ_NS::Execute<void>, 0x00972000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_L_f ( "COP1_CEIL_L", false, 1, &mips::instructions::COP1_CEIL_L_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_L_d ( "COP1_CEIL_L", false, 1, &mips::instructions::COP1_CEIL_L_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_W_f ( "COP1_CEIL_W", false, 1, &mips::instructions::COP1_CEIL_W_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_W_d ( "COP1_CEIL_W", false, 1, &mips::instructions::COP1_CEIL_W_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CFC1_v ( "COP1_CFC1", false, 1, &mips::instructions::COP1_CFC1_NS::Execute<void>, 0x0081207Fu, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CLASS_f ( "COP1_CLASS", false, 1, &mips::instructions::COP1_CLASS_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CLASS_d ( "COP1_CLASS", false, 1, &mips::instructions::COP1_CLASS_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CMP_condn_d_v ( "COP1_CMP_condn_d", false, 1, &mips::instructions::COP1_CMP_condn_d_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CMP_condn_s_v ( "COP1_CMP_condn_s", false, 1, &mips::instructions::COP1_CMP_condn_s_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CTC1_v ( "COP1_CTC1", false, 1, &mips::instructions::COP1_CTC1_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_f ( "COP1_CVT_D", false, 1, &mips::instructions::COP1_CVT_D_NS::Execute<float>, 0x00800047u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_d ( "COP1_CVT_D", false, 1, &mips::instructions::COP1_CVT_D_NS::Execute<double>, 0x00800047u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_w ( "COP1_CVT_D", false, 1, &mips::instructions::COP1_CVT_D_NS::Execute<int32>, 0x00800047u, 'w' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_l ( "COP1_CVT_D", false, 1, &mips::instructions::COP1_CVT_D_NS::Execute<int64>, 0x00800047u, 'l' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_L_f ( "COP1_CVT_L", false, 1, &mips::instructions::COP1_CVT_L_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_L_d ( "COP1_CVT_L", false, 1, &mips::instructions::COP1_CVT_L_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_f ( "COP1_CVT_S", false, 1, &mips::instructions::COP1_CVT_S_NS::Execute<float>, 0x00800047u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_d ( "COP1_CVT_S", false, 1, &mips::instructions::COP1_CVT_S_NS::Execute<double>, 0x00800047u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_w ( "COP1_CVT_S", false, 1, &mips::instructions::COP1_CVT_S_NS::Execute<int32>, 0x00800047u, 'w' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_l ( "COP1_CVT_S", false, 1, &mips::instructions::COP1_CVT_S_NS::Execute<int64>, 0x00800047u, 'l' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_W_f ( "COP1_CVT_W", false, 1, &mips::instructions::COP1_CVT_W_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_CVT_W_d ( "COP1_CVT_W", false, 1, &mips::instructions::COP1_CVT_W_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_DIV_f ( "COP1_DIV", false, 1, &mips::instructions::COP1_DIV_NS::Execute<float>, 0x0080007Fu, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_DIV_d ( "COP1_DIV", false, 1, &mips::instructions::COP1_DIV_NS::Execute<double>, 0x0080007Fu, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_L_f ( "COP1_FLOOR_L", false, 1, &mips::instructions::COP1_FLOOR_L_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_L_d ( "COP1_FLOOR_L", false, 1, &mips::instructions::COP1_FLOOR_L_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_W_f ( "COP1_FLOOR_W", false, 1, &mips::instructions::COP1_FLOOR_W_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_W_d ( "COP1_FLOOR_W", false, 1, &mips::instructions::COP1_FLOOR_W_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_LDC1_v ( "COP1_LDC1", false, 1, &mips::instructions::COP1_LDC1_NS::Execute<void>, 0x00A12000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_LWC1_v ( "COP1_LWC1", false, 1, &mips::instructions::COP1_LWC1_NS::Execute<void>, 0x00A12000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MADDF_f ( "COP1_MADDF", false, 1, &mips::instructions::COP1_MADDF_NS::Execute<float>, 0x00800077u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MADDF_d ( "COP1_MADDF", false, 1, &mips::instructions::COP1_MADDF_NS::Execute<double>, 0x00800077u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MAX_f ( "COP1_MAX", false, 1, &mips::instructions::COP1_MAX_NS::Execute<float>, 0x00812007u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MAX_d ( "COP1_MAX", false, 1, &mips::instructions::COP1_MAX_NS::Execute<double>, 0x00812007u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MAXA_f ( "COP1_MAXA", false, 1, &mips::instructions::COP1_MAXA_NS::Execute<float>, 0x00812007u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MAXA_d ( "COP1_MAXA", false, 1, &mips::instructions::COP1_MAXA_NS::Execute<double>, 0x00812007u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MFC1_v ( "COP1_MFC1", false, 1, &mips::instructions::COP1_MFC1_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MFHC1_v ( "COP1_MFHC1", false, 1, &mips::instructions::COP1_MFHC1_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MIN_f ( "COP1_MIN", false, 1, &mips::instructions::COP1_MIN_NS::Execute<float>, 0x00812007u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MIN_d ( "COP1_MIN", false, 1, &mips::instructions::COP1_MIN_NS::Execute<double>, 0x00812007u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MINA_f ( "COP1_MINA", false, 1, &mips::instructions::COP1_MINA_NS::Execute<float>, 0x00812007u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MINA_d ( "COP1_MINA", false, 1, &mips::instructions::COP1_MINA_NS::Execute<double>, 0x00812007u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MOV_f ( "COP1_MOV", false, 1, &mips::instructions::COP1_MOV_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MOV_d ( "COP1_MOV", false, 1, &mips::instructions::COP1_MOV_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MSUBF_f ( "COP1_MSUBF", false, 1, &mips::instructions::COP1_MSUBF_NS::Execute<float>, 0x00800077u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MSUBF_d ( "COP1_MSUBF", false, 1, &mips::instructions::COP1_MSUBF_NS::Execute<double>, 0x00800077u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MTC1_v ( "COP1_MTC1", false, 1, &mips::instructions::COP1_MTC1_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MTHC1_v ( "COP1_MTHC1", false, 1, &mips::instructions::COP1_MTHC1_NS::Execute<void>, 0x00812000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MUL_f ( "COP1_MUL", false, 1, &mips::instructions::COP1_MUL_NS::Execute<float>, 0x00800077u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_MUL_d ( "COP1_MUL", false, 1, &mips::instructions::COP1_MUL_NS::Execute<double>, 0x00800077u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_NEG_f ( "COP1_NEG", false, 1, &mips::instructions::COP1_NEG_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_NEG_d ( "COP1_NEG", false, 1, &mips::instructions::COP1_NEG_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_RECIP_f ( "COP1_RECIP", false, 1, &mips::instructions::COP1_RECIP_NS::Execute<float>, 0x0080007Fu, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_RECIP_d ( "COP1_RECIP", false, 1, &mips::instructions::COP1_RECIP_NS::Execute<double>, 0x0080007Fu, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_RINT_f ( "COP1_RINT", false, 1, &mips::instructions::COP1_RINT_NS::Execute<float>, 0x00800077u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_RINT_d ( "COP1_RINT", false, 1, &mips::instructions::COP1_RINT_NS::Execute<double>, 0x00800077u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_L_f ( "COP1_ROUND_L", false, 1, &mips::instructions::COP1_ROUND_L_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_L_d ( "COP1_ROUND_L", false, 1, &mips::instructions::COP1_ROUND_L_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_W_f ( "COP1_ROUND_W", false, 1, &mips::instructions::COP1_ROUND_W_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_W_d ( "COP1_ROUND_W", false, 1, &mips::instructions::COP1_ROUND_W_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_RSQRT_f ( "COP1_RSQRT", false, 1, &mips::instructions::COP1_RSQRT_NS::Execute<float>, 0x0080007Fu, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_RSQRT_d ( "COP1_RSQRT", false, 1, &mips::instructions::COP1_RSQRT_NS::Execute<double>, 0x0080007Fu, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SDC1_v ( "COP1_SDC1", false, 1, &mips::instructions::COP1_SDC1_NS::Execute<void>, 0x00C12000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SEL_f ( "COP1_SEL", false, 1, &mips::instructions::COP1_SEL_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SEL_d ( "COP1_SEL", false, 1, &mips::instructions::COP1_SEL_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SELEQZ_f ( "COP1_SELEQZ", false, 1, &mips::instructions::COP1_SELEQZ_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SELEQZ_d ( "COP1_SELEQZ", false, 1, &mips::instructions::COP1_SELEQZ_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SELNEZ_f ( "COP1_SELNEZ", false, 1, &mips::instructions::COP1_SELNEZ_NS::Execute<float>, 0x00812000u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SELNEZ_d ( "COP1_SELNEZ", false, 1, &mips::instructions::COP1_SELNEZ_NS::Execute<double>, 0x00812000u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SQRT_f ( "COP1_SQRT", false, 1, &mips::instructions::COP1_SQRT_NS::Execute<float>, 0x00800047u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SQRT_d ( "COP1_SQRT", false, 1, &mips::instructions::COP1_SQRT_NS::Execute<double>, 0x00800047u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SUB_f ( "COP1_SUB", false, 1, &mips::instructions::COP1_SUB_NS::Execute<float>, 0x00800077u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SUB_d ( "COP1_SUB", false, 1, &mips::instructions::COP1_SUB_NS::Execute<double>, 0x00800077u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_SWC1_v ( "COP1_SWC1", false, 1, &mips::instructions::COP1_SWC1_NS::Execute<void>, 0x00C12000u, 'v' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_L_f ( "COP1_TRUNC_L", false, 1, &mips::instructions::COP1_TRUNC_L_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_L_d ( "COP1_TRUNC_L", false, 1, &mips::instructions::COP1_TRUNC_L_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_W_f ( "COP1_TRUNC_W", false, 1, &mips::instructions::COP1_TRUNC_W_NS::Execute<float>, 0x008000C7u, 'f' );
	_export const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_W_d ( "COP1_TRUNC_W", false, 1, &mips::instructions::COP1_TRUNC_W_NS::Execute<double>, 0x008000C7u, 'd' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ADD ( "PROC_ADD", false, 1, &mips::instructions::PROC_ADD_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ADDIU ( "PROC_ADDIU", false, 1, &mips::instructions::PROC_ADDIU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ADDIUPC ( "PROC_ADDIUPC", false, 1, &mips::instructions::PROC_ADDIUPC_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ADDU ( "PROC_ADDU", false, 1, &mips::instructions::PROC_ADDU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ALIGN ( "PROC_ALIGN", false, 1, &mips::instructions::PROC_ALIGN_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ALUIPC ( "PROC_ALUIPC", false, 1, &mips::instructions::PROC_ALUIPC_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_AND ( "PROC_AND", false, 1, &mips::instructions::PROC_AND_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ANDI ( "PROC_ANDI", false, 1, &mips::instructions::PROC_ANDI_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_AUI ( "PROC_AUI", false, 1, &mips::instructions::PROC_AUI_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_AUIPC ( "PROC_AUIPC", false, 1, &mips::instructions::PROC_AUIPC_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BAL ( "PROC_BAL", true, 1, &mips::instructions::PROC_BAL_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BALC ( "PROC_BALC", true, 1, &mips::instructions::PROC_BALC_NS::Execute, 0x000A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BC ( "PROC_BC", true, 1, &mips::instructions::PROC_BC_NS::Execute, 0x000A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BEQ ( "PROC_BEQ", true, 1, &mips::instructions::PROC_BEQ_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BEQZC ( "PROC_BEQZC", true, 1, &mips::instructions::PROC_BEQZC_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BGEZ ( "PROC_BGEZ", true, 1, &mips::instructions::PROC_BGEZ_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BGTZ ( "PROC_BGTZ", true, 1, &mips::instructions::PROC_BGTZ_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BGTZALC ( "PROC_BGTZALC", true, 1, &mips::instructions::PROC_BGTZALC_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BGTZC ( "PROC_BGTZC", true, 1, &mips::instructions::PROC_BGTZC_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BITSWAP ( "PROC_BITSWAP", false, 1, &mips::instructions::PROC_BITSWAP_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BLEZ ( "PROC_BLEZ", true, 1, &mips::instructions::PROC_BLEZ_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BLEZALC ( "PROC_BLEZALC", true, 1, &mips::instructions::PROC_BLEZALC_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BLEZC ( "PROC_BLEZC", true, 1, &mips::instructions::PROC_BLEZC_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BLTZ ( "PROC_BLTZ", true, 1, &mips::instructions::PROC_BLTZ_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BNE ( "PROC_BNE", true, 1, &mips::instructions::PROC_BNE_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BNEZC ( "PROC_BNEZC", true, 1, &mips::instructions::PROC_BNEZC_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_BREAK ( "PROC_BREAK", false, 1, &mips::instructions::PROC_BREAK_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_CACHE ( "PROC_CACHE", false, 1, &mips::instructions::PROC_CACHE_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_CLO ( "PROC_CLO", false, 1, &mips::instructions::PROC_CLO_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_CLZ ( "PROC_CLZ", false, 1, &mips::instructions::PROC_CLZ_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_DIV ( "PROC_DIV", false, 1, &mips::instructions::PROC_DIV_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_DIVU ( "PROC_DIVU", false, 1, &mips::instructions::PROC_DIVU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_EHB ( "PROC_EHB", false, 1, &mips::instructions::PROC_EHB_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_EXT ( "PROC_EXT", false, 1, &mips::instructions::PROC_EXT_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_INS ( "PROC_INS", false, 1, &mips::instructions::PROC_INS_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_J ( "PROC_J", true, 1, &mips::instructions::PROC_J_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_JAL ( "PROC_JAL", true, 1, &mips::instructions::PROC_JAL_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_JALR ( "PROC_JALR", true, 1, &mips::instructions::PROC_JALR_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_JIALC ( "PROC_JIALC", true, 1, &mips::instructions::PROC_JIALC_NS::Execute, 0x000A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_JIC ( "PROC_JIC", true, 1, &mips::instructions::PROC_JIC_NS::Execute, 0x000A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_JR ( "PROC_JR", true, 1, &mips::instructions::PROC_JR_NS::Execute, 0x00160000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LB ( "PROC_LB", false, 1, &mips::instructions::PROC_LB_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LBE ( "PROC_LBE", false, 1, &mips::instructions::PROC_LBE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LBU ( "PROC_LBU", false, 1, &mips::instructions::PROC_LBU_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LBUE ( "PROC_LBUE", false, 1, &mips::instructions::PROC_LBUE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LH ( "PROC_LH", false, 1, &mips::instructions::PROC_LH_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LHE ( "PROC_LHE", false, 1, &mips::instructions::PROC_LHE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LHU ( "PROC_LHU", false, 1, &mips::instructions::PROC_LHU_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LHUE ( "PROC_LHUE", false, 1, &mips::instructions::PROC_LHUE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LL ( "PROC_LL", false, 1, &mips::instructions::PROC_LL_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LLE ( "PROC_LLE", false, 1, &mips::instructions::PROC_LLE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LLWP ( "PROC_LLWP", false, 1, &mips::instructions::PROC_LLWP_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LLWPE ( "PROC_LLWPE", false, 1, &mips::instructions::PROC_LLWPE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LSA ( "PROC_LSA", false, 1, &mips::instructions::PROC_LSA_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LW ( "PROC_LW", false, 1, &mips::instructions::PROC_LW_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LWE ( "PROC_LWE", false, 1, &mips::instructions::PROC_LWE_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_LWPC ( "PROC_LWPC", false, 1, &mips::instructions::PROC_LWPC_NS::Execute, 0x00200000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MOD ( "PROC_MOD", false, 1, &mips::instructions::PROC_MOD_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MODU ( "PROC_MODU", false, 1, &mips::instructions::PROC_MODU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MOVE ( "PROC_MOVE", false, 1, &mips::instructions::PROC_MOVE_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MUH ( "PROC_MUH", false, 1, &mips::instructions::PROC_MUH_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MUHU ( "PROC_MUHU", false, 1, &mips::instructions::PROC_MUHU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MUL ( "PROC_MUL", false, 1, &mips::instructions::PROC_MUL_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_MULU ( "PROC_MULU", false, 1, &mips::instructions::PROC_MULU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_NAL ( "PROC_NAL", true, 1, &mips::instructions::PROC_NAL_NS::Execute, 0x00120000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_NOP ( "PROC_NOP", false, 1, &mips::instructions::PROC_NOP_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_NOR ( "PROC_NOR", false, 1, &mips::instructions::PROC_NOR_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_OR ( "PROC_OR", false, 1, &mips::instructions::PROC_OR_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ORI ( "PROC_ORI", false, 1, &mips::instructions::PROC_ORI_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_PAUSE ( "PROC_PAUSE", true, 1, &mips::instructions::PROC_PAUSE_NS::Execute, 0x00020000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_POP06 ( "PROC_POP06", true, 1, &mips::instructions::PROC_POP06_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_POP07 ( "PROC_POP07", true, 1, &mips::instructions::PROC_POP07_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_POP10 ( "PROC_POP10", true, 1, &mips::instructions::PROC_POP10_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_POP26 ( "PROC_POP26", true, 1, &mips::instructions::PROC_POP26_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_POP27 ( "PROC_POP27", true, 1, &mips::instructions::PROC_POP27_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_POP30 ( "PROC_POP30", true, 1, &mips::instructions::PROC_POP30_NS::Execute, 0x001A0000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_PREF ( "PROC_PREF", false, 1, &mips::instructions::PROC_PREF_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_PREFE ( "PROC_PREFE", false, 1, &mips::instructions::PROC_PREFE_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_RDHWR ( "PROC_RDHWR", false, 1, &mips::instructions::PROC_RDHWR_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ROTR ( "PROC_ROTR", false, 1, &mips::instructions::PROC_ROTR_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_ROTRV ( "PROC_ROTRV", false, 1, &mips::instructions::PROC_ROTRV_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SB ( "PROC_SB", false, 1, &mips::instructions::PROC_SB_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SBE ( "PROC_SBE", false, 1, &mips::instructions::PROC_SBE_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SC ( "PROC_SC", false, 1, &mips::instructions::PROC_SC_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SCE ( "PROC_SCE", false, 1, &mips::instructions::PROC_SCE_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SCWP ( "PROC_SCWP", false, 1, &mips::instructions::PROC_SCWP_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SCWPE ( "PROC_SCWPE", false, 1, &mips::instructions::PROC_SCWPE_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SDBBP ( "PROC_SDBBP", false, 1, &mips::instructions::PROC_SDBBP_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SEB ( "PROC_SEB", false, 1, &mips::instructions::PROC_SEB_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SEH ( "PROC_SEH", false, 1, &mips::instructions::PROC_SEH_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SELEQZ ( "PROC_SELEQZ", false, 1, &mips::instructions::PROC_SELEQZ_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SELNEZ ( "PROC_SELNEZ", false, 1, &mips::instructions::PROC_SELNEZ_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SH ( "PROC_SH", false, 1, &mips::instructions::PROC_SH_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SHE ( "PROC_SHE", false, 1, &mips::instructions::PROC_SHE_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SIGRIE ( "PROC_SIGRIE", false, 1, &mips::instructions::PROC_SIGRIE_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SLL ( "PROC_SLL", false, 1, &mips::instructions::PROC_SLL_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SLLV ( "PROC_SLLV", false, 1, &mips::instructions::PROC_SLLV_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SLT ( "PROC_SLT", false, 1, &mips::instructions::PROC_SLT_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SLTI ( "PROC_SLTI", false, 1, &mips::instructions::PROC_SLTI_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SLTIU ( "PROC_SLTIU", false, 1, &mips::instructions::PROC_SLTIU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SLTU ( "PROC_SLTU", false, 1, &mips::instructions::PROC_SLTU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SRA ( "PROC_SRA", false, 1, &mips::instructions::PROC_SRA_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SRAV ( "PROC_SRAV", false, 1, &mips::instructions::PROC_SRAV_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SRL ( "PROC_SRL", false, 1, &mips::instructions::PROC_SRL_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SRLV ( "PROC_SRLV", false, 1, &mips::instructions::PROC_SRLV_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SSNOP ( "PROC_SSNOP", false, 1, &mips::instructions::PROC_SSNOP_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SUB ( "PROC_SUB", false, 1, &mips::instructions::PROC_SUB_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SUBU ( "PROC_SUBU", false, 1, &mips::instructions::PROC_SUBU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SW ( "PROC_SW", false, 1, &mips::instructions::PROC_SW_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SWE ( "PROC_SWE", false, 1, &mips::instructions::PROC_SWE_NS::Execute, 0x00400000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SYNC ( "PROC_SYNC", false, 1, &mips::instructions::PROC_SYNC_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SYNCI ( "PROC_SYNCI", false, 1, &mips::instructions::PROC_SYNCI_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_SYSCALL ( "PROC_SYSCALL", false, 1, &mips::instructions::PROC_SYSCALL_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_TEQ ( "PROC_TEQ", false, 1, &mips::instructions::PROC_TEQ_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_TGE ( "PROC_TGE", false, 1, &mips::instructions::PROC_TGE_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_TGEU ( "PROC_TGEU", false, 1, &mips::instructions::PROC_TGEU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_TLT ( "PROC_TLT", false, 1, &mips::instructions::PROC_TLT_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_TLTU ( "PROC_TLTU", false, 1, &mips::instructions::PROC_TLTU_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_TNE ( "PROC_TNE", false, 1, &mips::instructions::PROC_TNE_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_WSBH ( "PROC_WSBH", false, 1, &mips::instructions::PROC_WSBH_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_XOR ( "PROC_XOR", false, 1, &mips::instructions::PROC_XOR_NS::Execute, 0x00000000u, 'n' );
	_export const mips::instructions::InstructionInfo StaticProc_PROC_XORI ( "PROC_XORI", false, 1, &mips::instructions::PROC_XORI_NS::Execute, 0x00000000u, 'n' );
}

const mips::instructions::InstructionInfo * __restrict get_instruction (instruction_t i) {
	switch(i) {
		case 0x00000040u:
			return &mips::instructions::StaticProc_PROC_SSNOP;
		case 0x00000140u:
			return &mips::instructions::StaticProc_PROC_PAUSE;
		case 0x00000000u:
			return &mips::instructions::StaticProc_PROC_NOP;
		case 0x000000C0u:
			return &mips::instructions::StaticProc_PROC_EHB;
		default:
			switch(i & 0xFC000000u) {
				case 0x38000000u:
					return &mips::instructions::StaticProc_PROC_XORI;
				case 0xAC000000u:
					return &mips::instructions::StaticProc_PROC_SW;
				case 0x2C000000u:
					return &mips::instructions::StaticProc_PROC_SLTIU;
				case 0x28000000u:
					return &mips::instructions::StaticProc_PROC_SLTI;
				case 0xA4000000u:
					return &mips::instructions::StaticProc_PROC_SH;
				case 0xA0000000u:
					return &mips::instructions::StaticProc_PROC_SB;
				case 0x34000000u:
					return &mips::instructions::StaticProc_PROC_ORI;
				case 0x8C000000u:
					return &mips::instructions::StaticProc_PROC_LW;
				case 0x94000000u:
					return &mips::instructions::StaticProc_PROC_LHU;
				case 0x84000000u:
					return &mips::instructions::StaticProc_PROC_LH;
				case 0x90000000u:
					return &mips::instructions::StaticProc_PROC_LBU;
				case 0x80000000u:
					return &mips::instructions::StaticProc_PROC_LB;
				case 0x0C000000u:
					return &mips::instructions::StaticProc_PROC_JAL;
				case 0x08000000u:
					return &mips::instructions::StaticProc_PROC_J;
				case 0x14000000u:
					return &mips::instructions::StaticProc_PROC_BNE;
				case 0x60000000u:
					return &mips::instructions::StaticProc_PROC_POP30;
				case 0x20000000u:
					return &mips::instructions::StaticProc_PROC_POP10;
				case 0x10000000u:
					return &mips::instructions::StaticProc_PROC_BEQ;
				case 0xC8000000u:
					return &mips::instructions::StaticProc_PROC_BC;
				case 0xE8000000u:
					return &mips::instructions::StaticProc_PROC_BALC;
				case 0x3C000000u:
					return &mips::instructions::StaticProc_PROC_AUI;
				case 0x30000000u:
					return &mips::instructions::StaticProc_PROC_ANDI;
				case 0xE4000000u:
					return &mips::instructions::StaticProc_COP1_SWC1_v;
				case 0xF4000000u:
					return &mips::instructions::StaticProc_COP1_SDC1_v;
				case 0xC4000000u:
					return &mips::instructions::StaticProc_COP1_LWC1_v;
				case 0xD4000000u:
					return &mips::instructions::StaticProc_COP1_LDC1_v;
				case 0xF8000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_JIALC;
						default:
							if ((i & 0xFC000000u) == 0xF8000000u)
								return &mips::instructions::StaticProc_PROC_BNEZC;
							return nullptr;
					}
				case 0xD8000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_JIC;
						default:
							if ((i & 0xFC000000u) == 0xD8000000u)
								return &mips::instructions::StaticProc_PROC_BEQZC;
							return nullptr;
					}
				case 0x5C000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_BGTZC;
						default:
							if ((i & 0xFC000000u) == 0x5C000000u)
								return &mips::instructions::StaticProc_PROC_POP27;
							return nullptr;
					}
				case 0x58000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_BLEZC;
						default:
							if ((i & 0xFC000000u) == 0x58000000u)
								return &mips::instructions::StaticProc_PROC_POP26;
							return nullptr;
					}
				case 0x00000000u:
					switch(i & 0x0000003Fu) {
						case 0x0000000Fu:
							return &mips::instructions::StaticProc_PROC_SYNC;
						case 0x00000009u:
							switch(i & 0x001FF800u) {
								case 0x00000000u:
									return &mips::instructions::StaticProc_PROC_JR;
								default:
									if ((i & 0xFC1F003Fu) == 0x00000009u)
										return &mips::instructions::StaticProc_PROC_JALR;
									return nullptr;
							}
						case 0x00000021u:
							switch(i & 0x001F07C0u) {
								case 0x00000040u:
									return &mips::instructions::StaticProc_PROC_CLO;
								default:
									if ((i & 0xFC0007FFu) == 0x00000021u)
										return &mips::instructions::StaticProc_PROC_ADDU;
									return nullptr;
							}
						case 0x00000020u:
							return &mips::instructions::StaticProc_PROC_ADD;
						case 0x00000010u:
							return &mips::instructions::StaticProc_PROC_CLZ;
						case 0x00000024u:
							return &mips::instructions::StaticProc_PROC_AND;
						case 0x0000001Au:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									return &mips::instructions::StaticProc_PROC_MOD;
								case 0x00000080u:
									return &mips::instructions::StaticProc_PROC_DIV;
								default:
									return nullptr;
							}
						case 0x0000001Bu:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									return &mips::instructions::StaticProc_PROC_MODU;
								case 0x00000080u:
									return &mips::instructions::StaticProc_PROC_DIVU;
								default:
									return nullptr;
							}
						case 0x00000018u:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									return &mips::instructions::StaticProc_PROC_MUH;
								case 0x00000080u:
									return &mips::instructions::StaticProc_PROC_MUL;
								default:
									return nullptr;
							}
						case 0x00000019u:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									return &mips::instructions::StaticProc_PROC_MUHU;
								case 0x00000080u:
									return &mips::instructions::StaticProc_PROC_MULU;
								default:
									return nullptr;
							}
						case 0x00000027u:
							return &mips::instructions::StaticProc_PROC_NOR;
						case 0x00000025u:
							return &mips::instructions::StaticProc_PROC_OR;
						case 0x00000002u:
							switch(i & 0x03E00000u) {
								case 0x00000000u:
									return &mips::instructions::StaticProc_PROC_SRL;
								case 0x00200000u:
									return &mips::instructions::StaticProc_PROC_ROTR;
								default:
									return nullptr;
							}
						case 0x00000006u:
							switch(i & 0x000007C0u) {
								case 0x00000000u:
									return &mips::instructions::StaticProc_PROC_SRLV;
								case 0x00000040u:
									return &mips::instructions::StaticProc_PROC_ROTRV;
								default:
									return nullptr;
							}
						case 0x00000035u:
							return &mips::instructions::StaticProc_PROC_SELEQZ;
						case 0x00000037u:
							return &mips::instructions::StaticProc_PROC_SELNEZ;
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_SLL;
						case 0x00000004u:
							return &mips::instructions::StaticProc_PROC_SLLV;
						case 0x0000002Au:
							return &mips::instructions::StaticProc_PROC_SLT;
						case 0x0000002Bu:
							return &mips::instructions::StaticProc_PROC_SLTU;
						case 0x00000003u:
							return &mips::instructions::StaticProc_PROC_SRA;
						case 0x00000007u:
							return &mips::instructions::StaticProc_PROC_SRAV;
						case 0x00000022u:
							return &mips::instructions::StaticProc_PROC_SUB;
						case 0x00000023u:
							return &mips::instructions::StaticProc_PROC_SUBU;
						case 0x00000026u:
							return &mips::instructions::StaticProc_PROC_XOR;
						case 0x00000005u:
							return &mips::instructions::StaticProc_PROC_LSA;
						case 0x0000000Du:
							return &mips::instructions::StaticProc_PROC_BREAK;
						case 0x0000000Eu:
							return &mips::instructions::StaticProc_PROC_SDBBP;
						case 0x0000000Cu:
							return &mips::instructions::StaticProc_PROC_SYSCALL;
						case 0x00000034u:
							return &mips::instructions::StaticProc_PROC_TEQ;
						case 0x00000030u:
							return &mips::instructions::StaticProc_PROC_TGE;
						case 0x00000031u:
							return &mips::instructions::StaticProc_PROC_TGEU;
						case 0x00000032u:
							return &mips::instructions::StaticProc_PROC_TLT;
						case 0x00000033u:
							return &mips::instructions::StaticProc_PROC_TLTU;
						case 0x00000036u:
							return &mips::instructions::StaticProc_PROC_TNE;
						default:
							return nullptr;
					}
				case 0x44000000u:
					switch(i & 0x03E00000u) {
						case 0x01A00000u:
							return &mips::instructions::StaticProc_COP1_BC1NEZ_v;
						case 0x01200000u:
							return &mips::instructions::StaticProc_COP1_BC1EQZ_v;
						case 0x00600000u:
							return &mips::instructions::StaticProc_COP1_MFHC1_v;
						case 0x00E00000u:
							return &mips::instructions::StaticProc_COP1_MTHC1_v;
						case 0x02000000u:
							switch(i & 0x001F003Fu) {
								case 0x00000025u:
									return &mips::instructions::StaticProc_COP1_CVT_L_f;
								case 0x00000005u:
									return &mips::instructions::StaticProc_COP1_ABS_f;
								case 0x0000001Bu:
									return &mips::instructions::StaticProc_COP1_CLASS_f;
								case 0x00000021u:
									return &mips::instructions::StaticProc_COP1_CVT_D_f;
								case 0x00000020u:
									return &mips::instructions::StaticProc_COP1_CVT_S_f;
								case 0x00000024u:
									return &mips::instructions::StaticProc_COP1_CVT_W_f;
								case 0x00000006u:
									return &mips::instructions::StaticProc_COP1_MOV_f;
								case 0x00000007u:
									return &mips::instructions::StaticProc_COP1_NEG_f;
								case 0x00000015u:
									return &mips::instructions::StaticProc_COP1_RECIP_f;
								case 0x0000001Au:
									return &mips::instructions::StaticProc_COP1_RINT_f;
								case 0x00000016u:
									return &mips::instructions::StaticProc_COP1_RSQRT_f;
								case 0x00000004u:
									return &mips::instructions::StaticProc_COP1_SQRT_f;
								case 0x00000008u:
									return &mips::instructions::StaticProc_COP1_ROUND_L_f;
								case 0x0000000Cu:
									return &mips::instructions::StaticProc_COP1_ROUND_W_f;
								case 0x0000000Au:
									return &mips::instructions::StaticProc_COP1_CEIL_L_f;
								case 0x0000000Eu:
									return &mips::instructions::StaticProc_COP1_CEIL_W_f;
								case 0x0000000Bu:
									return &mips::instructions::StaticProc_COP1_FLOOR_L_f;
								case 0x0000000Fu:
									return &mips::instructions::StaticProc_COP1_FLOOR_W_f;
								case 0x00000009u:
									return &mips::instructions::StaticProc_COP1_TRUNC_L_f;
								case 0x0000000Du:
									return &mips::instructions::StaticProc_COP1_TRUNC_W_f;
								default:
									switch(i & 0xFFE0003Fu) {
										case 0x46000018u:
											return &mips::instructions::StaticProc_COP1_MADDF_f;
										case 0x46000000u:
											return &mips::instructions::StaticProc_COP1_ADD_f;
										case 0x46000003u:
											return &mips::instructions::StaticProc_COP1_DIV_f;
										case 0x46000019u:
											return &mips::instructions::StaticProc_COP1_MSUBF_f;
										case 0x4600001Eu:
											return &mips::instructions::StaticProc_COP1_MAX_f;
										case 0x4600001Cu:
											return &mips::instructions::StaticProc_COP1_MIN_f;
										case 0x4600001Fu:
											return &mips::instructions::StaticProc_COP1_MAXA_f;
										case 0x4600001Du:
											return &mips::instructions::StaticProc_COP1_MINA_f;
										case 0x46000002u:
											return &mips::instructions::StaticProc_COP1_MUL_f;
										case 0x46000010u:
											return &mips::instructions::StaticProc_COP1_SEL_f;
										case 0x46000014u:
											return &mips::instructions::StaticProc_COP1_SELEQZ_f;
										case 0x46000017u:
											return &mips::instructions::StaticProc_COP1_SELNEZ_f;
										case 0x46000001u:
											return &mips::instructions::StaticProc_COP1_SUB_f;
										default:
											return nullptr;
									}
									return nullptr;
							}
						case 0x02200000u:
							switch(i & 0x001F003Fu) {
								case 0x00000025u:
									return &mips::instructions::StaticProc_COP1_CVT_L_d;
								case 0x00000005u:
									return &mips::instructions::StaticProc_COP1_ABS_d;
								case 0x0000001Bu:
									return &mips::instructions::StaticProc_COP1_CLASS_d;
								case 0x00000021u:
									return &mips::instructions::StaticProc_COP1_CVT_D_d;
								case 0x00000020u:
									return &mips::instructions::StaticProc_COP1_CVT_S_d;
								case 0x00000024u:
									return &mips::instructions::StaticProc_COP1_CVT_W_d;
								case 0x00000006u:
									return &mips::instructions::StaticProc_COP1_MOV_d;
								case 0x00000007u:
									return &mips::instructions::StaticProc_COP1_NEG_d;
								case 0x00000015u:
									return &mips::instructions::StaticProc_COP1_RECIP_d;
								case 0x0000001Au:
									return &mips::instructions::StaticProc_COP1_RINT_d;
								case 0x00000016u:
									return &mips::instructions::StaticProc_COP1_RSQRT_d;
								case 0x00000004u:
									return &mips::instructions::StaticProc_COP1_SQRT_d;
								case 0x00000008u:
									return &mips::instructions::StaticProc_COP1_ROUND_L_d;
								case 0x0000000Cu:
									return &mips::instructions::StaticProc_COP1_ROUND_W_d;
								case 0x0000000Au:
									return &mips::instructions::StaticProc_COP1_CEIL_L_d;
								case 0x0000000Eu:
									return &mips::instructions::StaticProc_COP1_CEIL_W_d;
								case 0x0000000Bu:
									return &mips::instructions::StaticProc_COP1_FLOOR_L_d;
								case 0x0000000Fu:
									return &mips::instructions::StaticProc_COP1_FLOOR_W_d;
								case 0x00000009u:
									return &mips::instructions::StaticProc_COP1_TRUNC_L_d;
								case 0x0000000Du:
									return &mips::instructions::StaticProc_COP1_TRUNC_W_d;
								default:
									switch(i & 0xFFE0003Fu) {
										case 0x46200018u:
											return &mips::instructions::StaticProc_COP1_MADDF_d;
										case 0x46200000u:
											return &mips::instructions::StaticProc_COP1_ADD_d;
										case 0x46200003u:
											return &mips::instructions::StaticProc_COP1_DIV_d;
										case 0x46200019u:
											return &mips::instructions::StaticProc_COP1_MSUBF_d;
										case 0x4620001Eu:
											return &mips::instructions::StaticProc_COP1_MAX_d;
										case 0x4620001Cu:
											return &mips::instructions::StaticProc_COP1_MIN_d;
										case 0x4620001Fu:
											return &mips::instructions::StaticProc_COP1_MAXA_d;
										case 0x4620001Du:
											return &mips::instructions::StaticProc_COP1_MINA_d;
										case 0x46200002u:
											return &mips::instructions::StaticProc_COP1_MUL_d;
										case 0x46200010u:
											return &mips::instructions::StaticProc_COP1_SEL_d;
										case 0x46200014u:
											return &mips::instructions::StaticProc_COP1_SELEQZ_d;
										case 0x46200017u:
											return &mips::instructions::StaticProc_COP1_SELNEZ_d;
										case 0x46200001u:
											return &mips::instructions::StaticProc_COP1_SUB_d;
										default:
											return nullptr;
									}
									return nullptr;
							}
						case 0x02800000u:
							switch(i & 0x001F003Fu) {
								case 0x00000021u:
									return &mips::instructions::StaticProc_COP1_CVT_D_w;
								case 0x00000020u:
									return &mips::instructions::StaticProc_COP1_CVT_S_w;
								default:
									if ((i & 0xFFE00020u) == 0x46800000u)
										return &mips::instructions::StaticProc_COP1_CMP_condn_s_v;
									return nullptr;
							}
						case 0x02A00000u:
							switch(i & 0x001F003Fu) {
								case 0x00000021u:
									return &mips::instructions::StaticProc_COP1_CVT_D_l;
								case 0x00000020u:
									return &mips::instructions::StaticProc_COP1_CVT_S_l;
								default:
									if ((i & 0xFFE00020u) == 0x46A00000u)
										return &mips::instructions::StaticProc_COP1_CMP_condn_d_v;
									return nullptr;
							}
						case 0x00C00000u:
							return &mips::instructions::StaticProc_COP1_CTC1_v;
						case 0x00400000u:
							return &mips::instructions::StaticProc_COP1_CFC1_v;
						case 0x00800000u:
							return &mips::instructions::StaticProc_COP1_MTC1_v;
						case 0x00000000u:
							return &mips::instructions::StaticProc_COP1_MFC1_v;
						default:
							return nullptr;
					}
				case 0x24000000u:
					switch(i & 0x0000FFFFu) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_MOVE;
						default:
							if ((i & 0xFC000000u) == 0x24000000u)
								return &mips::instructions::StaticProc_PROC_ADDIU;
							return nullptr;
					}
				case 0x7C000000u:
					switch(i & 0x03E007FFu) {
						case 0x000000A0u:
							return &mips::instructions::StaticProc_PROC_WSBH;
						case 0x00000620u:
							return &mips::instructions::StaticProc_PROC_SEH;
						case 0x00000420u:
							return &mips::instructions::StaticProc_PROC_SEB;
						case 0x00000020u:
							return &mips::instructions::StaticProc_PROC_BITSWAP;
						default:
							switch(i & 0xFC00003Fu) {
								case 0x7C00003Bu:
									return &mips::instructions::StaticProc_PROC_RDHWR;
								case 0x7C00001Eu:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											return &mips::instructions::StaticProc_PROC_SCWPE;
										default:
											if ((i & 0xFC00007Fu) == 0x7C00001Eu)
												return &mips::instructions::StaticProc_PROC_SCE;
											return nullptr;
									}
								case 0x7C000026u:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											return &mips::instructions::StaticProc_PROC_SCWP;
										default:
											if ((i & 0xFC00007Fu) == 0x7C000026u)
												return &mips::instructions::StaticProc_PROC_SC;
											return nullptr;
									}
								case 0x7C00002Eu:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											return &mips::instructions::StaticProc_PROC_LLWPE;
										default:
											if ((i & 0xFC00007Fu) == 0x7C00002Eu)
												return &mips::instructions::StaticProc_PROC_LLE;
											return nullptr;
									}
								case 0x7C000036u:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											return &mips::instructions::StaticProc_PROC_LLWP;
										default:
											if ((i & 0xFC00007Fu) == 0x7C000036u)
												return &mips::instructions::StaticProc_PROC_LL;
											return nullptr;
									}
								case 0x7C000020u:
									return &mips::instructions::StaticProc_PROC_ALIGN;
								case 0x7C000025u:
									return &mips::instructions::StaticProc_PROC_CACHE;
								case 0x7C00002Cu:
									return &mips::instructions::StaticProc_PROC_LBE;
								case 0x7C000028u:
									return &mips::instructions::StaticProc_PROC_LBUE;
								case 0x7C00002Du:
									return &mips::instructions::StaticProc_PROC_LHE;
								case 0x7C000029u:
									return &mips::instructions::StaticProc_PROC_LHUE;
								case 0x7C00002Fu:
									return &mips::instructions::StaticProc_PROC_LWE;
								case 0x7C000035u:
									return &mips::instructions::StaticProc_PROC_PREF;
								case 0x7C000023u:
									return &mips::instructions::StaticProc_PROC_PREFE;
								case 0x7C00001Cu:
									return &mips::instructions::StaticProc_PROC_SBE;
								case 0x7C00001Du:
									return &mips::instructions::StaticProc_PROC_SHE;
								case 0x7C00001Fu:
									return &mips::instructions::StaticProc_PROC_SWE;
								case 0x7C000000u:
									return &mips::instructions::StaticProc_PROC_EXT;
								case 0x7C000004u:
									return &mips::instructions::StaticProc_PROC_INS;
								default:
									return nullptr;
							}
							return nullptr;
					}
				case 0x04000000u:
					switch(i & 0x03FF0000u) {
						case 0x00170000u:
							return &mips::instructions::StaticProc_PROC_SIGRIE;
						case 0x00100000u:
							return &mips::instructions::StaticProc_PROC_NAL;
						case 0x00110000u:
							return &mips::instructions::StaticProc_PROC_BAL;
						default:
							switch(i & 0xFC1F0000u) {
								case 0x041F0000u:
									return &mips::instructions::StaticProc_PROC_SYNCI;
								case 0x04000000u:
									return &mips::instructions::StaticProc_PROC_BLTZ;
								case 0x04010000u:
									return &mips::instructions::StaticProc_PROC_BGEZ;
								default:
									return nullptr;
							}
							return nullptr;
					}
				case 0xEC000000u:
					switch(i & 0x001F0000u) {
						case 0x001E0000u:
							return &mips::instructions::StaticProc_PROC_AUIPC;
						case 0x001F0000u:
							return &mips::instructions::StaticProc_PROC_ALUIPC;
						default:
							switch(i & 0xFC180000u) {
								case 0xEC080000u:
									return &mips::instructions::StaticProc_PROC_LWPC;
								case 0xEC000000u:
									return &mips::instructions::StaticProc_PROC_ADDIUPC;
								default:
									return nullptr;
							}
							return nullptr;
					}
				case 0x18000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_BLEZALC;
						default:
							switch(i & 0xFC1F0000u) {
								case 0x18000000u:
									return &mips::instructions::StaticProc_PROC_BLEZ;
								default:
									if ((i & 0xFC000000u) == 0x18000000u)
										return &mips::instructions::StaticProc_PROC_POP06;
									return nullptr;
							}
							return nullptr;
					}
				case 0x1C000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							return &mips::instructions::StaticProc_PROC_BGTZALC;
						default:
							switch(i & 0xFC1F0000u) {
								case 0x1C000000u:
									return &mips::instructions::StaticProc_PROC_BGTZ;
								default:
									if ((i & 0xFC000000u) == 0x1C000000u)
										return &mips::instructions::StaticProc_PROC_POP07;
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
			{ mips::instructions::PROC_SSNOP_NS::Execute(i,p); return true; }
		case 0x00000140u:
			{ mips::instructions::PROC_PAUSE_NS::Execute(i,p); return true; }
		case 0x00000000u:
			{ mips::instructions::PROC_NOP_NS::Execute(i,p); return true; }
		case 0x000000C0u:
			{ mips::instructions::PROC_EHB_NS::Execute(i,p); return true; }
		default:
			switch(i & 0xFC000000u) {
				case 0x38000000u:
					{ mips::instructions::PROC_XORI_NS::Execute(i,p); return true; }
				case 0xAC000000u:
					{ mips::instructions::PROC_SW_NS::Execute(i,p); return true; }
				case 0x2C000000u:
					{ mips::instructions::PROC_SLTIU_NS::Execute(i,p); return true; }
				case 0x28000000u:
					{ mips::instructions::PROC_SLTI_NS::Execute(i,p); return true; }
				case 0xA4000000u:
					{ mips::instructions::PROC_SH_NS::Execute(i,p); return true; }
				case 0xA0000000u:
					{ mips::instructions::PROC_SB_NS::Execute(i,p); return true; }
				case 0x34000000u:
					{ mips::instructions::PROC_ORI_NS::Execute(i,p); return true; }
				case 0x8C000000u:
					{ mips::instructions::PROC_LW_NS::Execute(i,p); return true; }
				case 0x94000000u:
					{ mips::instructions::PROC_LHU_NS::Execute(i,p); return true; }
				case 0x84000000u:
					{ mips::instructions::PROC_LH_NS::Execute(i,p); return true; }
				case 0x90000000u:
					{ mips::instructions::PROC_LBU_NS::Execute(i,p); return true; }
				case 0x80000000u:
					{ mips::instructions::PROC_LB_NS::Execute(i,p); return true; }
				case 0x0C000000u:
					{ mips::instructions::PROC_JAL_NS::Execute(i,p); return true; }
				case 0x08000000u:
					{ mips::instructions::PROC_J_NS::Execute(i,p); return true; }
				case 0x14000000u:
					{ mips::instructions::PROC_BNE_NS::Execute(i,p); return true; }
				case 0x60000000u:
					{ mips::instructions::PROC_POP30_NS::Execute(i,p); return true; }
				case 0x20000000u:
					{ mips::instructions::PROC_POP10_NS::Execute(i,p); return true; }
				case 0x10000000u:
					{ mips::instructions::PROC_BEQ_NS::Execute(i,p); return true; }
				case 0xC8000000u:
					{ mips::instructions::PROC_BC_NS::Execute(i,p); return true; }
				case 0xE8000000u:
					{ mips::instructions::PROC_BALC_NS::Execute(i,p); return true; }
				case 0x3C000000u:
					{ mips::instructions::PROC_AUI_NS::Execute(i,p); return true; }
				case 0x30000000u:
					{ mips::instructions::PROC_ANDI_NS::Execute(i,p); return true; }
				case 0xE4000000u:
					{ mips::instructions::COP1_SWC1_NS::Execute<void>(i,p); return true; }
				case 0xF4000000u:
					{ mips::instructions::COP1_SDC1_NS::Execute<void>(i,p); return true; }
				case 0xC4000000u:
					{ mips::instructions::COP1_LWC1_NS::Execute<void>(i,p); return true; }
				case 0xD4000000u:
					{ mips::instructions::COP1_LDC1_NS::Execute<void>(i,p); return true; }
				case 0xF8000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							{ mips::instructions::PROC_JIALC_NS::Execute(i,p); return true; }
						default:
							if ((i & 0xFC000000u) == 0xF8000000u)
								{ mips::instructions::PROC_BNEZC_NS::Execute(i,p); return true; }
							return false;
					}
				case 0xD8000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							{ mips::instructions::PROC_JIC_NS::Execute(i,p); return true; }
						default:
							if ((i & 0xFC000000u) == 0xD8000000u)
								{ mips::instructions::PROC_BEQZC_NS::Execute(i,p); return true; }
							return false;
					}
				case 0x5C000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							{ mips::instructions::PROC_BGTZC_NS::Execute(i,p); return true; }
						default:
							if ((i & 0xFC000000u) == 0x5C000000u)
								{ mips::instructions::PROC_POP27_NS::Execute(i,p); return true; }
							return false;
					}
				case 0x58000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							{ mips::instructions::PROC_BLEZC_NS::Execute(i,p); return true; }
						default:
							if ((i & 0xFC000000u) == 0x58000000u)
								{ mips::instructions::PROC_POP26_NS::Execute(i,p); return true; }
							return false;
					}
				case 0x00000000u:
					switch(i & 0x0000003Fu) {
						case 0x0000000Fu:
							{ mips::instructions::PROC_SYNC_NS::Execute(i,p); return true; }
						case 0x00000009u:
							switch(i & 0x001FF800u) {
								case 0x00000000u:
									{ mips::instructions::PROC_JR_NS::Execute(i,p); return true; }
								default:
									if ((i & 0xFC1F003Fu) == 0x00000009u)
										{ mips::instructions::PROC_JALR_NS::Execute(i,p); return true; }
									return false;
							}
						case 0x00000021u:
							switch(i & 0x001F07C0u) {
								case 0x00000040u:
									{ mips::instructions::PROC_CLO_NS::Execute(i,p); return true; }
								default:
									if ((i & 0xFC0007FFu) == 0x00000021u)
										{ mips::instructions::PROC_ADDU_NS::Execute(i,p); return true; }
									return false;
							}
						case 0x00000020u:
							{ mips::instructions::PROC_ADD_NS::Execute(i,p); return true; }
						case 0x00000010u:
							{ mips::instructions::PROC_CLZ_NS::Execute(i,p); return true; }
						case 0x00000024u:
							{ mips::instructions::PROC_AND_NS::Execute(i,p); return true; }
						case 0x0000001Au:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									{ mips::instructions::PROC_MOD_NS::Execute(i,p); return true; }
								case 0x00000080u:
									{ mips::instructions::PROC_DIV_NS::Execute(i,p); return true; }
								default:
									return false;
							}
						case 0x0000001Bu:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									{ mips::instructions::PROC_MODU_NS::Execute(i,p); return true; }
								case 0x00000080u:
									{ mips::instructions::PROC_DIVU_NS::Execute(i,p); return true; }
								default:
									return false;
							}
						case 0x00000018u:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									{ mips::instructions::PROC_MUH_NS::Execute(i,p); return true; }
								case 0x00000080u:
									{ mips::instructions::PROC_MUL_NS::Execute(i,p); return true; }
								default:
									return false;
							}
						case 0x00000019u:
							switch(i & 0x000007C0u) {
								case 0x000000C0u:
									{ mips::instructions::PROC_MUHU_NS::Execute(i,p); return true; }
								case 0x00000080u:
									{ mips::instructions::PROC_MULU_NS::Execute(i,p); return true; }
								default:
									return false;
							}
						case 0x00000027u:
							{ mips::instructions::PROC_NOR_NS::Execute(i,p); return true; }
						case 0x00000025u:
							{ mips::instructions::PROC_OR_NS::Execute(i,p); return true; }
						case 0x00000002u:
							switch(i & 0x03E00000u) {
								case 0x00000000u:
									{ mips::instructions::PROC_SRL_NS::Execute(i,p); return true; }
								case 0x00200000u:
									{ mips::instructions::PROC_ROTR_NS::Execute(i,p); return true; }
								default:
									return false;
							}
						case 0x00000006u:
							switch(i & 0x000007C0u) {
								case 0x00000000u:
									{ mips::instructions::PROC_SRLV_NS::Execute(i,p); return true; }
								case 0x00000040u:
									{ mips::instructions::PROC_ROTRV_NS::Execute(i,p); return true; }
								default:
									return false;
							}
						case 0x00000035u:
							{ mips::instructions::PROC_SELEQZ_NS::Execute(i,p); return true; }
						case 0x00000037u:
							{ mips::instructions::PROC_SELNEZ_NS::Execute(i,p); return true; }
						case 0x00000000u:
							{ mips::instructions::PROC_SLL_NS::Execute(i,p); return true; }
						case 0x00000004u:
							{ mips::instructions::PROC_SLLV_NS::Execute(i,p); return true; }
						case 0x0000002Au:
							{ mips::instructions::PROC_SLT_NS::Execute(i,p); return true; }
						case 0x0000002Bu:
							{ mips::instructions::PROC_SLTU_NS::Execute(i,p); return true; }
						case 0x00000003u:
							{ mips::instructions::PROC_SRA_NS::Execute(i,p); return true; }
						case 0x00000007u:
							{ mips::instructions::PROC_SRAV_NS::Execute(i,p); return true; }
						case 0x00000022u:
							{ mips::instructions::PROC_SUB_NS::Execute(i,p); return true; }
						case 0x00000023u:
							{ mips::instructions::PROC_SUBU_NS::Execute(i,p); return true; }
						case 0x00000026u:
							{ mips::instructions::PROC_XOR_NS::Execute(i,p); return true; }
						case 0x00000005u:
							{ mips::instructions::PROC_LSA_NS::Execute(i,p); return true; }
						case 0x0000000Du:
							{ mips::instructions::PROC_BREAK_NS::Execute(i,p); return true; }
						case 0x0000000Eu:
							{ mips::instructions::PROC_SDBBP_NS::Execute(i,p); return true; }
						case 0x0000000Cu:
							{ mips::instructions::PROC_SYSCALL_NS::Execute(i,p); return true; }
						case 0x00000034u:
							{ mips::instructions::PROC_TEQ_NS::Execute(i,p); return true; }
						case 0x00000030u:
							{ mips::instructions::PROC_TGE_NS::Execute(i,p); return true; }
						case 0x00000031u:
							{ mips::instructions::PROC_TGEU_NS::Execute(i,p); return true; }
						case 0x00000032u:
							{ mips::instructions::PROC_TLT_NS::Execute(i,p); return true; }
						case 0x00000033u:
							{ mips::instructions::PROC_TLTU_NS::Execute(i,p); return true; }
						case 0x00000036u:
							{ mips::instructions::PROC_TNE_NS::Execute(i,p); return true; }
						default:
							return false;
					}
				case 0x44000000u:
					switch(i & 0x03E00000u) {
						case 0x01A00000u:
							{ mips::instructions::COP1_BC1NEZ_NS::Execute<void>(i,p); return true; }
						case 0x01200000u:
							{ mips::instructions::COP1_BC1EQZ_NS::Execute<void>(i,p); return true; }
						case 0x00600000u:
							{ mips::instructions::COP1_MFHC1_NS::Execute<void>(i,p); return true; }
						case 0x00E00000u:
							{ mips::instructions::COP1_MTHC1_NS::Execute<void>(i,p); return true; }
						case 0x02000000u:
							switch(i & 0x001F003Fu) {
								case 0x00000025u:
									{ mips::instructions::COP1_CVT_L_NS::Execute<float>(i,p); return true; }
								case 0x00000005u:
									{ mips::instructions::COP1_ABS_NS::Execute<float>(i,p); return true; }
								case 0x0000001Bu:
									{ mips::instructions::COP1_CLASS_NS::Execute<float>(i,p); return true; }
								case 0x00000021u:
									{ mips::instructions::COP1_CVT_D_NS::Execute<float>(i,p); return true; }
								case 0x00000020u:
									{ mips::instructions::COP1_CVT_S_NS::Execute<float>(i,p); return true; }
								case 0x00000024u:
									{ mips::instructions::COP1_CVT_W_NS::Execute<float>(i,p); return true; }
								case 0x00000006u:
									{ mips::instructions::COP1_MOV_NS::Execute<float>(i,p); return true; }
								case 0x00000007u:
									{ mips::instructions::COP1_NEG_NS::Execute<float>(i,p); return true; }
								case 0x00000015u:
									{ mips::instructions::COP1_RECIP_NS::Execute<float>(i,p); return true; }
								case 0x0000001Au:
									{ mips::instructions::COP1_RINT_NS::Execute<float>(i,p); return true; }
								case 0x00000016u:
									{ mips::instructions::COP1_RSQRT_NS::Execute<float>(i,p); return true; }
								case 0x00000004u:
									{ mips::instructions::COP1_SQRT_NS::Execute<float>(i,p); return true; }
								case 0x00000008u:
									{ mips::instructions::COP1_ROUND_L_NS::Execute<float>(i,p); return true; }
								case 0x0000000Cu:
									{ mips::instructions::COP1_ROUND_W_NS::Execute<float>(i,p); return true; }
								case 0x0000000Au:
									{ mips::instructions::COP1_CEIL_L_NS::Execute<float>(i,p); return true; }
								case 0x0000000Eu:
									{ mips::instructions::COP1_CEIL_W_NS::Execute<float>(i,p); return true; }
								case 0x0000000Bu:
									{ mips::instructions::COP1_FLOOR_L_NS::Execute<float>(i,p); return true; }
								case 0x0000000Fu:
									{ mips::instructions::COP1_FLOOR_W_NS::Execute<float>(i,p); return true; }
								case 0x00000009u:
									{ mips::instructions::COP1_TRUNC_L_NS::Execute<float>(i,p); return true; }
								case 0x0000000Du:
									{ mips::instructions::COP1_TRUNC_W_NS::Execute<float>(i,p); return true; }
								default:
									switch(i & 0xFFE0003Fu) {
										case 0x46000018u:
											{ mips::instructions::COP1_MADDF_NS::Execute<float>(i,p); return true; }
										case 0x46000000u:
											{ mips::instructions::COP1_ADD_NS::Execute<float>(i,p); return true; }
										case 0x46000003u:
											{ mips::instructions::COP1_DIV_NS::Execute<float>(i,p); return true; }
										case 0x46000019u:
											{ mips::instructions::COP1_MSUBF_NS::Execute<float>(i,p); return true; }
										case 0x4600001Eu:
											{ mips::instructions::COP1_MAX_NS::Execute<float>(i,p); return true; }
										case 0x4600001Cu:
											{ mips::instructions::COP1_MIN_NS::Execute<float>(i,p); return true; }
										case 0x4600001Fu:
											{ mips::instructions::COP1_MAXA_NS::Execute<float>(i,p); return true; }
										case 0x4600001Du:
											{ mips::instructions::COP1_MINA_NS::Execute<float>(i,p); return true; }
										case 0x46000002u:
											{ mips::instructions::COP1_MUL_NS::Execute<float>(i,p); return true; }
										case 0x46000010u:
											{ mips::instructions::COP1_SEL_NS::Execute<float>(i,p); return true; }
										case 0x46000014u:
											{ mips::instructions::COP1_SELEQZ_NS::Execute<float>(i,p); return true; }
										case 0x46000017u:
											{ mips::instructions::COP1_SELNEZ_NS::Execute<float>(i,p); return true; }
										case 0x46000001u:
											{ mips::instructions::COP1_SUB_NS::Execute<float>(i,p); return true; }
										default:
											return false;
									}
									return false;
							}
						case 0x02200000u:
							switch(i & 0x001F003Fu) {
								case 0x00000025u:
									{ mips::instructions::COP1_CVT_L_NS::Execute<double>(i,p); return true; }
								case 0x00000005u:
									{ mips::instructions::COP1_ABS_NS::Execute<double>(i,p); return true; }
								case 0x0000001Bu:
									{ mips::instructions::COP1_CLASS_NS::Execute<double>(i,p); return true; }
								case 0x00000021u:
									{ mips::instructions::COP1_CVT_D_NS::Execute<double>(i,p); return true; }
								case 0x00000020u:
									{ mips::instructions::COP1_CVT_S_NS::Execute<double>(i,p); return true; }
								case 0x00000024u:
									{ mips::instructions::COP1_CVT_W_NS::Execute<double>(i,p); return true; }
								case 0x00000006u:
									{ mips::instructions::COP1_MOV_NS::Execute<double>(i,p); return true; }
								case 0x00000007u:
									{ mips::instructions::COP1_NEG_NS::Execute<double>(i,p); return true; }
								case 0x00000015u:
									{ mips::instructions::COP1_RECIP_NS::Execute<double>(i,p); return true; }
								case 0x0000001Au:
									{ mips::instructions::COP1_RINT_NS::Execute<double>(i,p); return true; }
								case 0x00000016u:
									{ mips::instructions::COP1_RSQRT_NS::Execute<double>(i,p); return true; }
								case 0x00000004u:
									{ mips::instructions::COP1_SQRT_NS::Execute<double>(i,p); return true; }
								case 0x00000008u:
									{ mips::instructions::COP1_ROUND_L_NS::Execute<double>(i,p); return true; }
								case 0x0000000Cu:
									{ mips::instructions::COP1_ROUND_W_NS::Execute<double>(i,p); return true; }
								case 0x0000000Au:
									{ mips::instructions::COP1_CEIL_L_NS::Execute<double>(i,p); return true; }
								case 0x0000000Eu:
									{ mips::instructions::COP1_CEIL_W_NS::Execute<double>(i,p); return true; }
								case 0x0000000Bu:
									{ mips::instructions::COP1_FLOOR_L_NS::Execute<double>(i,p); return true; }
								case 0x0000000Fu:
									{ mips::instructions::COP1_FLOOR_W_NS::Execute<double>(i,p); return true; }
								case 0x00000009u:
									{ mips::instructions::COP1_TRUNC_L_NS::Execute<double>(i,p); return true; }
								case 0x0000000Du:
									{ mips::instructions::COP1_TRUNC_W_NS::Execute<double>(i,p); return true; }
								default:
									switch(i & 0xFFE0003Fu) {
										case 0x46200018u:
											{ mips::instructions::COP1_MADDF_NS::Execute<double>(i,p); return true; }
										case 0x46200000u:
											{ mips::instructions::COP1_ADD_NS::Execute<double>(i,p); return true; }
										case 0x46200003u:
											{ mips::instructions::COP1_DIV_NS::Execute<double>(i,p); return true; }
										case 0x46200019u:
											{ mips::instructions::COP1_MSUBF_NS::Execute<double>(i,p); return true; }
										case 0x4620001Eu:
											{ mips::instructions::COP1_MAX_NS::Execute<double>(i,p); return true; }
										case 0x4620001Cu:
											{ mips::instructions::COP1_MIN_NS::Execute<double>(i,p); return true; }
										case 0x4620001Fu:
											{ mips::instructions::COP1_MAXA_NS::Execute<double>(i,p); return true; }
										case 0x4620001Du:
											{ mips::instructions::COP1_MINA_NS::Execute<double>(i,p); return true; }
										case 0x46200002u:
											{ mips::instructions::COP1_MUL_NS::Execute<double>(i,p); return true; }
										case 0x46200010u:
											{ mips::instructions::COP1_SEL_NS::Execute<double>(i,p); return true; }
										case 0x46200014u:
											{ mips::instructions::COP1_SELEQZ_NS::Execute<double>(i,p); return true; }
										case 0x46200017u:
											{ mips::instructions::COP1_SELNEZ_NS::Execute<double>(i,p); return true; }
										case 0x46200001u:
											{ mips::instructions::COP1_SUB_NS::Execute<double>(i,p); return true; }
										default:
											return false;
									}
									return false;
							}
						case 0x02800000u:
							switch(i & 0x001F003Fu) {
								case 0x00000021u:
									{ mips::instructions::COP1_CVT_D_NS::Execute<int32>(i,p); return true; }
								case 0x00000020u:
									{ mips::instructions::COP1_CVT_S_NS::Execute<int32>(i,p); return true; }
								default:
									if ((i & 0xFFE00020u) == 0x46800000u)
										{ mips::instructions::COP1_CMP_condn_s_NS::Execute<void>(i,p); return true; }
									return false;
							}
						case 0x02A00000u:
							switch(i & 0x001F003Fu) {
								case 0x00000021u:
									{ mips::instructions::COP1_CVT_D_NS::Execute<int64>(i,p); return true; }
								case 0x00000020u:
									{ mips::instructions::COP1_CVT_S_NS::Execute<int64>(i,p); return true; }
								default:
									if ((i & 0xFFE00020u) == 0x46A00000u)
										{ mips::instructions::COP1_CMP_condn_d_NS::Execute<void>(i,p); return true; }
									return false;
							}
						case 0x00C00000u:
							{ mips::instructions::COP1_CTC1_NS::Execute<void>(i,p); return true; }
						case 0x00400000u:
							{ mips::instructions::COP1_CFC1_NS::Execute<void>(i,p); return true; }
						case 0x00800000u:
							{ mips::instructions::COP1_MTC1_NS::Execute<void>(i,p); return true; }
						case 0x00000000u:
							{ mips::instructions::COP1_MFC1_NS::Execute<void>(i,p); return true; }
						default:
							return false;
					}
				case 0x24000000u:
					switch(i & 0x0000FFFFu) {
						case 0x00000000u:
							{ mips::instructions::PROC_MOVE_NS::Execute(i,p); return true; }
						default:
							if ((i & 0xFC000000u) == 0x24000000u)
								{ mips::instructions::PROC_ADDIU_NS::Execute(i,p); return true; }
							return false;
					}
				case 0x7C000000u:
					switch(i & 0x03E007FFu) {
						case 0x000000A0u:
							{ mips::instructions::PROC_WSBH_NS::Execute(i,p); return true; }
						case 0x00000620u:
							{ mips::instructions::PROC_SEH_NS::Execute(i,p); return true; }
						case 0x00000420u:
							{ mips::instructions::PROC_SEB_NS::Execute(i,p); return true; }
						case 0x00000020u:
							{ mips::instructions::PROC_BITSWAP_NS::Execute(i,p); return true; }
						default:
							switch(i & 0xFC00003Fu) {
								case 0x7C00003Bu:
									{ mips::instructions::PROC_RDHWR_NS::Execute(i,p); return true; }
								case 0x7C00001Eu:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											{ mips::instructions::PROC_SCWPE_NS::Execute(i,p); return true; }
										default:
											if ((i & 0xFC00007Fu) == 0x7C00001Eu)
												{ mips::instructions::PROC_SCE_NS::Execute(i,p); return true; }
											return false;
									}
								case 0x7C000026u:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											{ mips::instructions::PROC_SCWP_NS::Execute(i,p); return true; }
										default:
											if ((i & 0xFC00007Fu) == 0x7C000026u)
												{ mips::instructions::PROC_SC_NS::Execute(i,p); return true; }
											return false;
									}
								case 0x7C00002Eu:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											{ mips::instructions::PROC_LLWPE_NS::Execute(i,p); return true; }
										default:
											if ((i & 0xFC00007Fu) == 0x7C00002Eu)
												{ mips::instructions::PROC_LLE_NS::Execute(i,p); return true; }
											return false;
									}
								case 0x7C000036u:
									switch(i & 0x000007C0u) {
										case 0x00000040u:
											{ mips::instructions::PROC_LLWP_NS::Execute(i,p); return true; }
										default:
											if ((i & 0xFC00007Fu) == 0x7C000036u)
												{ mips::instructions::PROC_LL_NS::Execute(i,p); return true; }
											return false;
									}
								case 0x7C000020u:
									{ mips::instructions::PROC_ALIGN_NS::Execute(i,p); return true; }
								case 0x7C000025u:
									{ mips::instructions::PROC_CACHE_NS::Execute(i,p); return true; }
								case 0x7C00002Cu:
									{ mips::instructions::PROC_LBE_NS::Execute(i,p); return true; }
								case 0x7C000028u:
									{ mips::instructions::PROC_LBUE_NS::Execute(i,p); return true; }
								case 0x7C00002Du:
									{ mips::instructions::PROC_LHE_NS::Execute(i,p); return true; }
								case 0x7C000029u:
									{ mips::instructions::PROC_LHUE_NS::Execute(i,p); return true; }
								case 0x7C00002Fu:
									{ mips::instructions::PROC_LWE_NS::Execute(i,p); return true; }
								case 0x7C000035u:
									{ mips::instructions::PROC_PREF_NS::Execute(i,p); return true; }
								case 0x7C000023u:
									{ mips::instructions::PROC_PREFE_NS::Execute(i,p); return true; }
								case 0x7C00001Cu:
									{ mips::instructions::PROC_SBE_NS::Execute(i,p); return true; }
								case 0x7C00001Du:
									{ mips::instructions::PROC_SHE_NS::Execute(i,p); return true; }
								case 0x7C00001Fu:
									{ mips::instructions::PROC_SWE_NS::Execute(i,p); return true; }
								case 0x7C000000u:
									{ mips::instructions::PROC_EXT_NS::Execute(i,p); return true; }
								case 0x7C000004u:
									{ mips::instructions::PROC_INS_NS::Execute(i,p); return true; }
								default:
									return false;
							}
							return false;
					}
				case 0x04000000u:
					switch(i & 0x03FF0000u) {
						case 0x00170000u:
							{ mips::instructions::PROC_SIGRIE_NS::Execute(i,p); return true; }
						case 0x00100000u:
							{ mips::instructions::PROC_NAL_NS::Execute(i,p); return true; }
						case 0x00110000u:
							{ mips::instructions::PROC_BAL_NS::Execute(i,p); return true; }
						default:
							switch(i & 0xFC1F0000u) {
								case 0x041F0000u:
									{ mips::instructions::PROC_SYNCI_NS::Execute(i,p); return true; }
								case 0x04000000u:
									{ mips::instructions::PROC_BLTZ_NS::Execute(i,p); return true; }
								case 0x04010000u:
									{ mips::instructions::PROC_BGEZ_NS::Execute(i,p); return true; }
								default:
									return false;
							}
							return false;
					}
				case 0xEC000000u:
					switch(i & 0x001F0000u) {
						case 0x001E0000u:
							{ mips::instructions::PROC_AUIPC_NS::Execute(i,p); return true; }
						case 0x001F0000u:
							{ mips::instructions::PROC_ALUIPC_NS::Execute(i,p); return true; }
						default:
							switch(i & 0xFC180000u) {
								case 0xEC080000u:
									{ mips::instructions::PROC_LWPC_NS::Execute(i,p); return true; }
								case 0xEC000000u:
									{ mips::instructions::PROC_ADDIUPC_NS::Execute(i,p); return true; }
								default:
									return false;
							}
							return false;
					}
				case 0x18000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							{ mips::instructions::PROC_BLEZALC_NS::Execute(i,p); return true; }
						default:
							switch(i & 0xFC1F0000u) {
								case 0x18000000u:
									{ mips::instructions::PROC_BLEZ_NS::Execute(i,p); return true; }
								default:
									if ((i & 0xFC000000u) == 0x18000000u)
										{ mips::instructions::PROC_POP06_NS::Execute(i,p); return true; }
									return false;
							}
							return false;
					}
				case 0x1C000000u:
					switch(i & 0x03E00000u) {
						case 0x00000000u:
							{ mips::instructions::PROC_BGTZALC_NS::Execute(i,p); return true; }
						default:
							switch(i & 0xFC1F0000u) {
								case 0x1C000000u:
									{ mips::instructions::PROC_BGTZ_NS::Execute(i,p); return true; }
								default:
									if ((i & 0xFC000000u) == 0x1C000000u)
										{ mips::instructions::PROC_POP07_NS::Execute(i,p); return true; }
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
