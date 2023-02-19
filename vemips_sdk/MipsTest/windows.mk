NATIVE_CPP ?= /t/LLVM/bin/clang++
NATIVE_LD ?= /t/LLVM/bin/ld.lld

WINDOWS_OUT ?= ./MipsTest.exe

WINDOWS_CPP_FLAGS ?=

WINDOWS_CPP_FLAGS_BASE = \
	-fuse-ld=lld \
	-fno-function-sections \
	-fno-data-sections \
	-fasynchronous-unwind-tables \
	-funwind-tables \
	-fexceptions \
	-fcxx-exceptions

WINDOWS_CPP_FLAGS_ALL := $(WINDOWS_CPP_FLAGS_BASE) $(WINDOWS_CPP_FLAGS)

WINDOWS_LD_FLAGS ?=

WINDOWS_LD_FLAGS_BASE = \
	-largeaddressaware \
	-machine:X64 \
	-opt:REF \
	-opt:ICF \
	-subsystem:CONSOLE \
	-LTCG

WINDOWS_LD_FLAGS := $(WINDOWS_LD_FLAGS_BASE) $(WINDOWS_LD_FLAGS)

native : windows

windows : $(src_files)
	$(NATIVE_CPP) $(call ldflag,$(WINDOWS_LD_FLAGS)) $(call ldflag,$(LD_FLAGS)) $(WINDOWS_CPP_FLAGS_ALL) $(CPP_FLAGS) $(src_files) -o $(WINDOWS_OUT)

clean-native : clean-windows

clean-windows : $(NATIVE_OUT)
	rm -f $(NATIVE_OUT)
