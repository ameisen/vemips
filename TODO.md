# recompiler
## program counter
If the system or something else changes the program counter, this won't be reflected in the recompiled code which doesn't necessarily check the PC.

If it does, it won't necessarily execute the current instruction's epilog. Need to add checks and trigger a compact branch epilogue in such cases to force a branch to the new PC.