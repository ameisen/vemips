/*===------- llvm/Config/llvm-config.h - llvm configuration -------*- C -*-===*/
/*                                                                            */
/* Part of the LLVM Project, under the Apache License v2.0 with LLVM          */
/* Exceptions.                                                                */
/* See https://llvm.org/LICENSE.txt for license information.                  */
/* SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception                    */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

/* This file enumerates variables from the LLVM configuration so that they
   can be in exported headers and won't override package specific directives.
   This is a C header that can be included in the llvm-c headers. */

#ifndef LLVM_CONFIG_H
#define LLVM_CONFIG_H

/* Define if LLVM_ENABLE_DUMP is enabled */
/* #undef LLVM_ENABLE_DUMP */

/* Target triple LLVM will generate code for by default */
/* Doesn't use `cmakedefine` because it is allowed to be empty. */
#define LLVM_DEFAULT_TARGET_TRIPLE "mipsisa32r6el-vemips-generic-musl"

/* Define if threads enabled */
#define LLVM_ENABLE_THREADS 1

/* Has gcc/MSVC atomic intrinsics */
#define LLVM_HAS_ATOMICS 1

/* Host triple LLVM will be executed on */
#define LLVM_HOST_TRIPLE "x86_64"

/* LLVM architecture name for the native architecture, if available */
#define LLVM_NATIVE_ARCH Mips

/* LLVM name for the native AsmParser init function, if available */
#define LLVM_NATIVE_ASMPARSER LLVMInitializeMipsAsmParser

/* LLVM name for the native AsmPrinter init function, if available */
#define LLVM_NATIVE_ASMPRINTER LLVMInitializeMipsAsmPrinter

/* LLVM name for the native Disassembler init function, if available */
#define LLVM_NATIVE_DISASSEMBLER LLVMInitializeMipsDisassembler

/* LLVM name for the native Target init function, if available */
#define LLVM_NATIVE_TARGET LLVMInitializeMipsTarget

/* LLVM name for the native TargetInfo init function, if available */
#define LLVM_NATIVE_TARGETINFO LLVMInitializeMipsTargetInfo

/* LLVM name for the native target MC init function, if available */
#define LLVM_NATIVE_TARGETMC LLVMInitializeMipsTargetMC

/* LLVM name for the native target MCA init function, if available */
/* #undef LLVM_NATIVE_TARGETMCA */

/* Define if the AArch64 target is built in */
#define LLVM_HAS_AARCH64_TARGET 0

/* Define if the AMDGPU target is built in */
#define LLVM_HAS_AMDGPU_TARGET 0

/* Define if the ARC target is built in */
#define LLVM_HAS_ARC_TARGET 0

/* Define if the ARM target is built in */
#define LLVM_HAS_ARM_TARGET 0

/* Define if the AVR target is built in */
#define LLVM_HAS_AVR_TARGET 0

/* Define if the BPF target is built in */
#define LLVM_HAS_BPF_TARGET 0

/* Define if the CSKY target is built in */
#define LLVM_HAS_CSKY_TARGET 0

/* Define if the DirectX target is built in */
#define LLVM_HAS_DIRECTX_TARGET 0

/* Define if the Hexagon target is built in */
#define LLVM_HAS_HEXAGON_TARGET 0

/* Define if the Lanai target is built in */
#define LLVM_HAS_LANAI_TARGET 0

/* Define if the LoongArch target is built in */
#define LLVM_HAS_LOONGARCH_TARGET 0

/* Define if the M68k target is built in */
#define LLVM_HAS_M68K_TARGET 0

/* Define if the Mips target is built in */
#define LLVM_HAS_MIPS_TARGET 1

/* Define if the MSP430 target is built in */
#define LLVM_HAS_MSP430_TARGET 0

/* Define if the NVPTX target is built in */
#define LLVM_HAS_NVPTX_TARGET 0

/* Define if the PowerPC target is built in */
#define LLVM_HAS_POWERPC_TARGET 0

/* Define if the RISCV target is built in */
#define LLVM_HAS_RISCV_TARGET 0

/* Define if the Sparc target is built in */
#define LLVM_HAS_SPARC_TARGET 0

/* Define if the SPIRV target is built in */
#define LLVM_HAS_SPIRV_TARGET 0

/* Define if the SystemZ target is built in */
#define LLVM_HAS_SYSTEMZ_TARGET 0

/* Define if the VE target is built in */
#define LLVM_HAS_VE_TARGET 0

/* Define if the WebAssembly target is built in */
#define LLVM_HAS_WEBASSEMBLY_TARGET 0

/* Define if the X86 target is built in */
#define LLVM_HAS_X86_TARGET 0

/* Define if the XCore target is built in */
#define LLVM_HAS_XCORE_TARGET 0

/* Define if the Xtensa target is built in */
#define LLVM_HAS_XTENSA_TARGET 0

/* Define if this is Unixish platform */
/* #undef LLVM_ON_UNIX */

/* Define if we have the Intel JIT API runtime support library */
#define LLVM_USE_INTEL_JITEVENTS 0

/* Define if we have the oprofile JIT-support library */
#define LLVM_USE_OPROFILE 0

/* Define if we have the perf JIT-support library */
#define LLVM_USE_PERF 0

/* Major version of the LLVM API */
#define LLVM_VERSION_MAJOR 20

/* Minor version of the LLVM API */
#define LLVM_VERSION_MINOR 1

/* Patch version of the LLVM API */
#define LLVM_VERSION_PATCH 6

/* LLVM version string */
#define LLVM_VERSION_STRING "20.1.6"

/* Whether LLVM records statistics for use with GetStatistics(),
 * PrintStatistics() or PrintStatisticsJSON()
 */
#define LLVM_FORCE_ENABLE_STATS 0

/* Define if we have z3 and want to build it */
/* #undef LLVM_WITH_Z3 */

/* Define if we have curl and want to use it */
/* #undef LLVM_ENABLE_CURL */

/* Define if we have cpp-httplib and want to use it */
/* #undef LLVM_ENABLE_HTTPLIB */

/* Define if zlib compression is available */
#define LLVM_ENABLE_ZLIB 1

/* Define if zstd compression is available */
#define LLVM_ENABLE_ZSTD 1

/* Define if LLVM is using tflite */
/* #undef LLVM_HAVE_TFLITE */

/* Define to 1 if you have the <sysexits.h> header file. */
/* #undef HAVE_SYSEXITS_H */

/* Define if building libLLVM shared library */
/* #undef LLVM_BUILD_LLVM_DYLIB */

/* Define if building LLVM with BUILD_SHARED_LIBS */
/* #undef LLVM_BUILD_SHARED_LIBS */

/* Define if building LLVM with LLVM_FORCE_USE_OLD_TOOLCHAIN_LIBS */
/* #undef LLVM_FORCE_USE_OLD_TOOLCHAIN */

/* Define if llvm_unreachable should be optimized with undefined behavior
 * in non assert builds */
#define LLVM_UNREACHABLE_OPTIMIZE 1

/* Define to 1 if you have the DIA SDK installed, and to 0 if you don't. */
#define LLVM_ENABLE_DIA_SDK 1

/* Define if plugins enabled */
/* #undef LLVM_ENABLE_PLUGINS */

/* Define if logf128 is available */
/* #undef LLVM_HAS_LOGF128 */

#endif
