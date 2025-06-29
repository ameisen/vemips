rm -rf obj/interpreter.cpp.o
rm -rf ./MipsTest.bin
#if [ -z "${MIPS_SDK}" ]; then
#	MIPS_SDK=/d/SDKs/MIPS
#fi

if [ -z "${VEMIPS_CC}" ]; then
	VEMIPS_CC=/t/LLVM/bin/clang
fi
"$VEMIPS_CXX" -std=gnu++23 "$@" \
	-Wl,--discard-all \
	-Wl,-znorelro \
	-Wl,--gc-sections \
	-Wl,--icf=safe \
	-Wl,--strip-all \
	-Wl,--eh-frame-hdr \
	-ffunction-sections \
	-fdata-sections \
	-mno-check-zero-division \
	-mcompact-branches=always \
	-fasynchronous-unwind-tables \
	-funwind-tables \
	-fexceptions \
	-fcxx-exceptions \
	-Wno-assume \
	-flto=thin \
	-O3 \
	./interpreter.cpp -o MipsTest.bin \
	&
"$VEMIPS_CXX" -std=gnu++23 "$@" \
	-ffunction-sections \
	-fdata-sections \
	-mno-check-zero-division \
	-mcompact-branches=always \
	-fasynchronous-unwind-tables \
	-funwind-tables \
	-fexceptions \
	-fcxx-exceptions \
	-Wno-assume \
	-g3 \
	-Og \
	./interpreter.cpp -o MipsTestD.bin \
	&
	
wait