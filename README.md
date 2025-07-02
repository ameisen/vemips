# vemips
MIPS32r6 Emulator

VeMIPS is an embeddable MIPS32r6 emulator/virtual machine. It was designed to be able to be embedded into simulations or games, and also allow you to run one or thousands of VM instances with a low footprint.

## Support
Presently only runs on Windows 64, though other platform support will be re-added once MMU and debugger logic is isolated. Has run on Linux and in a browser via Emscripten in the past.

## Features

* Can be embedded as a library (`mips`) or run as an emulator (`vemips`) with an integrated kernel (`VeMIX`) 
* Dynamic recompilation (`JIT1`) on Windows on x86-64
* Multiple memory manager models
* * `host`-mediated (Windows only)
* * `emulated`
* * `none`
* Multiple stack arrangement models
* Full exception handler
* Completely encapsulated instructions in all execution modes - the VM can enter or exit at _any_ instruction, and the full, correct state will be present
* The ability to execute a specified number of instructions and then return
* Embedded GDB server (and VC++ line-by-line debugging support but only in VS15, it needs to be rewritten for later versions)
* A very limited subset of Linux-compatible system calls ("VeMIX") in prewritten emulator
* * `debug` (custom)
* * `brk` and `sbrk`
* * `writev`
* * `exit_group`
* * `exit`
* * `set_thread_area`
* The ability to hook system calls so the VM can interact with your program
* LLVM toolchain (20.1.6)
* * Clang/Clang++
* * lld
* * lldb
* * libc++ / libc++abi / libunwind
* * Polly
* Musl libc (1.2.5) providing very basic functionality, with some extensions
