INT_DIR ?= obj

CPP_FLAGS ?=

LD_FLAG_PREFIX = -Wl,

ldflag = $(addprefix $(LD_FLAG_PREFIX),$(1))

LD_FLAGS ?=

src_files = interpreter.cpp

include mips.mk

ifeq ($(OS),Windows_NT)
include windows.mk
else
$(error unknown native platform)
endif


all : native mips

clean : clean-mips clean-native

$(INT_DIR) :
	mkdir -p $@
