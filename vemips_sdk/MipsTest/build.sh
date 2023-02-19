rm -rf obj/interpreter.cpp.o
rm -rf ./MipsTest.bin
MIPS_SDK=/d/SDKs/MIPS
COMPILER=/t/LLVM/bin/clang
#COMPILER=mipsc
"$COMPILER" --target=mipsel -fno-function-sections -fno-data-sections -mno-check-zero-division -mcompact-branches=always -fasynchronous-unwind-tables -funwind-tables -fexceptions -fcxx-exceptions -mips32r6 "$@" "-I${MIPS_SDK}/incxx" "-I${MIPS_SDK}/inc" -c ./interpreter.cpp -o obj/interpreter.cpp.o
/c/Tools/LLVM/bin/ld.lld --discard-all -znorelro --gc-sections --icf=all --strip-all --eh-frame-hdr -lc -lc++ -lc++abi -lllvmunwind "-L${MIPS_SDK}/lib" obj/interpreter.cpp.o -o ./MipsTest.bin
