#pragma once
/* WARNING: THIS IS AN AUTO-GENERATED FILE ('D:\Projects\vemips\src\tablegen\tablegen.cpp', Feb 20 2023 15:45:45) */
// ReSharper disable CppClangTidyClangDiagnosticUnreachableCodeReturn IdentifierTypo CommentTypo
#include "mips/mips_common.hpp"

namespace mips {
	class processor;
	namespace instructions {
		struct InstructionInfo;
	}
}

#ifdef VEMIPS_ITABLE_EXPORT
#	define VEMIPS_ITABLE_IMPORT VEMIPS_ITABLE_EXPORT
#else
#	define VEMIPS_ITABLE_IMPORT /* __declspec(dllimport) */ extern
#endif

namespace mips::instructions {
	namespace COP1_ABS_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_ADD_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_BC1EQZ_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_BC1NEZ_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CEIL_L_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CEIL_W_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CFC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CLASS_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CMP_condn_d_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CMP_condn_s_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CTC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_D_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_L_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_S_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_CVT_W_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_DIV_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_FLOOR_L_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_FLOOR_W_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_LDC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_LWC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MADDF_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MAX_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MAXA_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MFC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MFHC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MIN_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MINA_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MOV_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MSUBF_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MTC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MTHC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_MUL_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_NEG_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_RECIP_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_RINT_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_ROUND_L_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_ROUND_W_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_RSQRT_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SDC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SEL_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SELEQZ_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SELNEZ_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SQRT_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SUB_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_SWC1_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_TRUNC_L_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace COP1_TRUNC_W_NS { template <typename format_t> VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADD_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADDIU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADDIUPC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ADDU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ALIGN_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ALUIPC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_AND_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ANDI_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_AUI_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_AUIPC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BAL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BALC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BEQ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BEQZC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGEZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGTZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGTZALC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BGTZC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BITSWAP_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLEZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLEZALC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLEZC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BLTZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BNE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BNEZC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_BREAK_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_CACHE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_CLO_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_CLZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_DIV_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_DIVU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_EHB_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_EXT_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_INS_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_J_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JAL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JALR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JIALC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JIC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_JR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LB_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LBE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LBU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LBUE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LH_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LHE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LHU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LHUE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LLE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LLWP_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LLWPE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LSA_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LW_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LWE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_LWPC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MOD_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MODU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MOVE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MUH_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MUHU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MUL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_MULU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_NAL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_NOP_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_NOR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_OR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ORI_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_PAUSE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP06_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP07_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP10_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP26_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP27_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_POP30_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_PREF_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_PREFE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_RDHWR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ROTR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_ROTRV_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SB_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SBE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SCE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SCWP_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SCWPE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SDBBP_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SEB_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SEH_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SELEQZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SELNEZ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SH_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SHE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SIGRIE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLLV_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLT_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLTI_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLTIU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SLTU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRA_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRAV_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SRLV_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SSNOP_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SUB_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SUBU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SW_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SWE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SYNC_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SYNCI_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_SYSCALL_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TEQ_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TGE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TGEU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TLT_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TLTU_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_TNE_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_WSBH_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_XOR_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
	namespace PROC_XORI_NS { VEMIPS_ITABLE_IMPORT uint64 Execute(instruction_t, mips::processor & __restrict); }
}

namespace mips::instructions {
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ABS_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ABS_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ADD_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ADD_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_BC1EQZ_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_BC1NEZ_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_L_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_L_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_W_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_W_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CFC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CLASS_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CLASS_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CMP_condn_d_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CMP_condn_s_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CTC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_w;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_l;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_L_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_L_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_w;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_l;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_W_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_W_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_DIV_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_DIV_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_L_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_L_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_W_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_W_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_LDC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_LWC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MADDF_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MADDF_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAX_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAX_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAXA_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAXA_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MFC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MFHC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MIN_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MIN_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MINA_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MINA_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MOV_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MOV_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MSUBF_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MSUBF_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MTC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MTHC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MUL_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MUL_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_NEG_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_NEG_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RECIP_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RECIP_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RINT_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RINT_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_L_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_L_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_W_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_W_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RSQRT_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RSQRT_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SDC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SEL_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SEL_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELEQZ_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELEQZ_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELNEZ_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELNEZ_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SQRT_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SQRT_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SUB_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SUB_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SWC1_v;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_L_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_L_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_W_f;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_W_d;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADD;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADDIU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADDIUPC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADDU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ALIGN;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ALUIPC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_AND;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ANDI;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_AUI;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_AUIPC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BAL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BALC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BEQ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BEQZC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGEZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGTZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGTZALC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGTZC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BITSWAP;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLEZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLEZALC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLEZC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLTZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BNE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BNEZC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BREAK;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_CACHE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_CLO;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_CLZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_DIV;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_DIVU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_EHB;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_EXT;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_INS;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_J;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JAL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JALR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JIALC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JIC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LB;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LBE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LBU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LBUE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LH;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LHE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LHU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LHUE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LLE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LLWP;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LLWPE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LSA;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LW;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LWE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LWPC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MOD;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MODU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MOVE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MUH;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MUHU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MUL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MULU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_NAL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_NOP;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_NOR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_OR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ORI;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_PAUSE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP06;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP07;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP10;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP26;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP27;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP30;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_PREF;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_PREFE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_RDHWR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ROTR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ROTRV;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SB;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SBE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SCE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SCWP;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SCWPE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SDBBP;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SEB;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SEH;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SELEQZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SELNEZ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SH;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SHE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SIGRIE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLLV;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLT;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLTI;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLTIU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLTU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRA;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRAV;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRLV;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SSNOP;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SUB;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SUBU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SW;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SWE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SYNC;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SYNCI;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SYSCALL;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TEQ;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TGE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TGEU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TLT;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TLTU;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TNE;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_WSBH;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_XOR;
	VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_XORI;
}

namespace mips::instructions {
	const InstructionInfo * get_instruction (instruction_t i);

	bool execute_instruction (instruction_t i, mips::processor & __restrict p);
}

#	undef VEMIPS_ITABLE_IMPORT
