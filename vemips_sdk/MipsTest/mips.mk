MIPS_CPP ?= /t/LLVM/bin/clang++
MIPS_LD ?= /t/LLVM/bin/ld.lld
MIPS_SDK ?= /d/SDKs/MIPS
MIPS_OUT ?= ./MipsTest.bin

MIPS_CPP_FLAGS ?=

MIPS_TARGET ?= mipsel32

QUOTE = "
enquote = $(QUOTE)$(1)$(QUOTE)

# --target=$(shell ruby -e 'puts "mips#{($(call enquote,el) if ARGV[0].include?($(call enquote,el)) || ARGV[0].include?($(call enquote,le))) or $(call enquote,)}"' -- $(MIPS_TARGET) ) \

MIPS_CPP_FLAGS_BASE = \
	--target=$(shell ruby -e 'puts "mips#{($(call enquote,el) if ARGV[0].include?($(call enquote,el)) || ARGV[0].include?($(call enquote,le))) or $(call enquote,)}"' -- $(MIPS_TARGET) ) \
	-ffunction-sections \
	-fdata-sections \
	-mno-check-zero-division \
	-mcompact-branches=always \
	-fasynchronous-unwind-tables \
	-funwind-tables \
	-fexceptions \
	-fcxx-exceptions \
	-Wno-invalid-constexpr \
	-std=gnu++2b \
	-$(shell ruby -e 'puts "mips#{ARGV[0].include?($(call enquote,64)) ? 64 : 32}r#{/.*r(\d+).*/.match(ARGV[0])&.[](1) || 6}"' -- $(MIPS_TARGET) ) \
	"-I$(MIPS_SDK)/incxx" \
	"-I$(MIPS_SDK)/inc"

MIPS_CPP_FLAGS_POST =
	-mnan=2008

MIPS_CPP_FLAGS_ALL = $(MIPS_CPP_FLAGS_BASE) $(MIPS_CPP_FLAGS)

MIPS_LD_FLAGS ?=

MIPS_LD_FLAGS_BASE = \
	--discard-all \
	-znorelro \
	--gc-sections \
	--icf=all \
	--strip-all \
	--eh-frame-hdr \
	-lc \
	-lc++ \
	-lc++abi \
	-lllvmunwind

MIPS_LD_FLAGS := $(MIPS_LD_FLAGS_BASE) $(MIPS_LD_FLAGS)

ifeq (,$(INT_DIR))
mips_obj_files = $(addsuffix .mips.o,$(basename $(src_files)))
else
mips_obj_files = $(addprefix $(INT_DIR)/,$(addsuffix .mips.o,$(basename $(src_files))))
endif

mips : $(mips_obj_files)
	$(MIPS_LD) $(MIPS_LD_FLAGS_BASE) $(LD_FLAGS) "-L$(MIPS_SDK)/lib" $(mips_obj_files) -o $(MIPS_OUT)

clean-mips :
	rm -f $(mips_obj_files)
	rm -f $(MIPS_OUT)

$(INT_DIR)/%.mips.o : %.cpp $(INT_DIR)
	$(MIPS_CPP) $(MIPS_CPP_FLAGS_ALL) $(CPP_FLAGS) $(MIPS_CPP_FLAGS_POST) -c $< -o $@
