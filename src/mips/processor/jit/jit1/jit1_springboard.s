include abi/jit1_abi_win64.s

.data

.code

jit1_unwind_stack PROC PRIVATE
	; handle the reserved space from the second springboard as well, and handle parity to fix alignment.
	mov r12, qword ptr [rsp + 20h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov r13, qword ptr [rsp + 28h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov r14, qword ptr [rsp + 30h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov r15, qword ptr [rsp + 38h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov rdi, qword ptr [rsp + 40h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov rsi, qword ptr [rsp + 48h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov rbp, qword ptr [rsp + 50h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	mov rbx, qword ptr [rsp + 58h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY]
	; XMM6 through XMM15
	add rsp, 70h + CALLEE_STACK_RESERVE + CALLEE_STACK_RESERVE + CALLER_STACK_PARITY
	ret
jit1_unwind_stack endp

jit1_springboard PROC FRAME
	sub rsp, 70h + CALLEE_STACK_RESERVE
	.allocstack 70h + CALLEE_STACK_RESERVE
	mov qword ptr [rsp + 20h + CALLEE_STACK_RESERVE], r12
	mov qword ptr [rsp + 28h + CALLEE_STACK_RESERVE], r13
	mov qword ptr [rsp + 30h + CALLEE_STACK_RESERVE], r14
	mov qword ptr [rsp + 38h + CALLEE_STACK_RESERVE], r15
	mov qword ptr [rsp + 40h + CALLEE_STACK_RESERVE], rdi
	mov qword ptr [rsp + 48h + CALLEE_STACK_RESERVE], rsi
	mov qword ptr [rsp + 50h + CALLEE_STACK_RESERVE], rbp
	mov qword ptr [rsp + 58h + CALLEE_STACK_RESERVE], rbx
	; XMM6 through XMM15

	.setframe rsp, 0h
	.endprolog

	; mov qword ptr [signal_sp], rsp

	;xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
	;xor edi, edi
	xor esi, esi
	xor ebx, ebx
	;xor ebp, ebp
	lea rbp, qword ptr [rdx + 128]

	; r9 = parameter pack struct.
	mov r12, qword ptr [r9 + 0]
	mov ebx, dword ptr [r9 + 8]
	mov esi, dword ptr [r9 + 12]
	mov r14, qword ptr [r9 + 16]
	mov r15d, dword ptr [r9 + 24]

	sub r12, -128
	mov rdi, r8
	call jit1_second_springboard
	jmp jit1_unwind_stack
jit1_springboard endp

jit1_second_springboard PROC PRIVATE
	sub rsp, CALLEE_STACK_RESERVE
	jmp rcx
jit1_second_springboard endp
end