#pragma once
/* WARNING: THIS IS AN AUTO-GENERATED FILE ('D:\Projects\vemips\src\tablegen\writers.cpp', Feb 22 2023 14:38:06) */
// ReSharper disable CppClangTidyClangDiagnosticUnreachableCodeReturn IdentifierTypo CommentTypo
#include "mips/mips_common.hpp"

_define_segment(vemips_itable, read)

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
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ABS_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ABS_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ADD_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ADD_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_BC1EQZ_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_BC1NEZ_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_L_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_L_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_W_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CEIL_W_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CFC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CLASS_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CLASS_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CMP_condn_d_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CMP_condn_s_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CTC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_w; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_D_l; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_L_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_L_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_w; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_S_l; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_W_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_CVT_W_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_DIV_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_DIV_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_L_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_L_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_W_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_FLOOR_W_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_LDC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_LWC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MADDF_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MADDF_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAX_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAX_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAXA_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MAXA_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MFC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MFHC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MIN_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MIN_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MINA_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MINA_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MOV_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MOV_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MSUBF_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MSUBF_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MTC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MTHC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MUL_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_MUL_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_NEG_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_NEG_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RECIP_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RECIP_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RINT_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RINT_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_L_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_L_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_W_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_ROUND_W_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RSQRT_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_RSQRT_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SDC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SEL_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SEL_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELEQZ_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELEQZ_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELNEZ_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SELNEZ_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SQRT_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SQRT_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SUB_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SUB_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_SWC1_v; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_L_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_L_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_W_f; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_COP1_TRUNC_W_d; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADD; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADDIU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADDIUPC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ADDU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ALIGN; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ALUIPC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_AND; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ANDI; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_AUI; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_AUIPC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BAL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BALC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BEQ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BEQZC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGEZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGTZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGTZALC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BGTZC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BITSWAP; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLEZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLEZALC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLEZC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BLTZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BNE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BNEZC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_BREAK; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_CACHE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_CLO; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_CLZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_DIV; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_DIVU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_EHB; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_EXT; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_INS; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_J; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JAL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JALR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JIALC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JIC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_JR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LB; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LBE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LBU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LBUE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LH; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LHE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LHU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LHUE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LLE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LLWP; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LLWPE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LSA; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LW; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LWE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_LWPC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MOD; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MODU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MOVE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MUH; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MUHU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MUL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_MULU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_NAL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_NOP; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_NOR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_OR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ORI; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_PAUSE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP06; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP07; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP10; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP26; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP27; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_POP30; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_PREF; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_PREFE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_RDHWR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ROTR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_ROTRV; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SB; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SBE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SCE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SCWP; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SCWPE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SDBBP; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SEB; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SEH; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SELEQZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SELNEZ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SH; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SHE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SIGRIE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLLV; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLT; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLTI; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLTIU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SLTU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRA; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRAV; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SRLV; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SSNOP; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SUB; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SUBU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SW; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SWE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SYNC; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SYNCI; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_SYSCALL; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TEQ; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TGE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TGEU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TLT; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TLTU; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_TNE; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_WSBH; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_XOR; 
	_segment(vemips_itable) VEMIPS_ITABLE_IMPORT const InstructionInfo StaticProc_PROC_XORI; 
}

namespace mips::instructions {
	const InstructionInfo * get_instruction (instruction_t i);

	bool execute_instruction (instruction_t i, mips::processor & __restrict p);
}

#	undef VEMIPS_ITABLE_IMPORT
