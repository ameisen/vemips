#pragma once
/* WARNING: THIS IS AN AUTO-GENERATED FILE ('D:\Projects\vemips\src\tablegen\tablegen.cpp', Feb 15 2023 18:26:02) */
#include "mips/mips_common.hpp"

namespace mips {
	class processor;
	namespace instructions {
		struct InstructionInfo;
	}
}

#define _import /* __declspec(dllexport) */ extern

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
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ABS_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ABS_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ADD_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ADD_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_BC1EQZ_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_BC1NEZ_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_L_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_L_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_W_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CEIL_W_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CFC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CLASS_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CLASS_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CMP_condn_d_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CMP_condn_s_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CTC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_w;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_D_l;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_L_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_L_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_w;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_S_l;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_W_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_CVT_W_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_DIV_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_DIV_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_L_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_L_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_W_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_FLOOR_W_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_LDC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_LWC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MADDF_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MADDF_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MAX_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MAX_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MAXA_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MAXA_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MFC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MFHC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MIN_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MIN_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MINA_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MINA_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MOV_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MOV_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MSUBF_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MSUBF_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MTC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MTHC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MUL_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_MUL_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_NEG_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_NEG_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_RECIP_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_RECIP_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_RINT_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_RINT_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_L_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_L_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_W_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_ROUND_W_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_RSQRT_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_RSQRT_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SDC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SEL_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SEL_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SELEQZ_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SELEQZ_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SELNEZ_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SELNEZ_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SQRT_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SQRT_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SUB_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SUB_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_SWC1_v;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_L_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_L_d;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_W_f;
	_import const mips::instructions::InstructionInfo StaticProc_COP1_TRUNC_W_d;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ADD;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ADDIU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ADDIUPC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ADDU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ALIGN;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ALUIPC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_AND;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ANDI;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_AUI;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_AUIPC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BAL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BALC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BEQ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BEQZC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BGEZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BGTZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BGTZALC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BGTZC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BITSWAP;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BLEZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BLEZALC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BLEZC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BLTZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BNE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BNEZC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_BREAK;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_CACHE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_CLO;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_CLZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_DIV;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_DIVU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_EHB;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_EXT;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_INS;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_J;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_JAL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_JALR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_JIALC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_JIC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_JR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LB;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LBE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LBU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LBUE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LH;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LHE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LHU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LHUE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LLE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LLWP;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LLWPE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LSA;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LW;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LWE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_LWPC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MOD;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MODU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MOVE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MUH;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MUHU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MUL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_MULU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_NAL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_NOP;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_NOR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_OR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ORI;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_PAUSE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_POP06;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_POP07;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_POP10;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_POP26;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_POP27;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_POP30;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_PREF;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_PREFE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_RDHWR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ROTR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_ROTRV;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SB;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SBE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SCE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SCWP;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SCWPE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SDBBP;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SEB;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SEH;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SELEQZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SELNEZ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SH;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SHE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SIGRIE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SLL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SLLV;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SLT;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SLTI;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SLTIU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SLTU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SRA;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SRAV;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SRL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SRLV;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SSNOP;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SUB;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SUBU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SW;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SWE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SYNC;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SYNCI;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_SYSCALL;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_TEQ;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_TGE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_TGEU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_TLT;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_TLTU;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_TNE;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_WSBH;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_XOR;
	_import const mips::instructions::InstructionInfo StaticProc_PROC_XORI;
}

const mips::instructions::InstructionInfo * __restrict get_instruction (instruction_t i);

bool execute_instruction (instruction_t i, mips::processor & __restrict p);
