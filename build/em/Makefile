build:
	mkdir ../../out/em; \
	cd ../../out/em; \
	emcc --memory-init-file 0 -O3 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0 -s PRECISE_F32=1 -std=c++20 -I../../src -I../../src/elf -I../../src/mips ../../src/elf/elf.cpp ../../src/mips/coprocessor/coprocessor1/coprocessor1.cpp ../../src/mips/processor/processor.cpp ../../src/mips/instructions/coprocessor1_inst.cpp ../../src/mips/instructions/instructions.cpp ../../src/mips/instructions/instructions_test.cpp ../../src/mips/instructions/processor_inst.cpp ../../src/emulator/main.cpp ../../src/emulator/base64.cpp -o emulator.html

clean:
	rm -rf ../../out/em