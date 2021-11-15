# vemips
MIPS32r6 Emulator

VeMIPS is an embeddable MIPS32r6 emulator/virtual machine. It was designed to be able to be embedded into simulations or games, and also allow you to run one or thousands of VM instances.

It has the following features:

* Dynamic recompilation (semi-AOT/JIT) on Windows on x86-64
* Multiple memory manager models
* Multiple stack arrangement models
* Completely encapsulated instructions in all execution modes - the VM can enter or exit at _any_ instruction, and the full, correct state will be present
* The ability to execute a specified number of instructions and then return
* Embedded GDB server (and VC++ line-by-line debugging support but only in VS15, it needs to be rewritten for later versions)
* A very limited subset of Linux-compatible system calls
* The ability to hook system calls so the VM can interact with your program
