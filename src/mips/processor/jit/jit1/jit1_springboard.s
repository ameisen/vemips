.data

signal_sp:
	dq 0

.code

unwind_stack:
	mov r12, qword ptr [rsp + 20h]
	mov r13, qword ptr [rsp + 28h]
	mov r14, qword ptr [rsp + 30h]
	mov r15, qword ptr [rsp + 38h]
	mov rdi, qword ptr [rsp + 40h]
	mov rsi, qword ptr [rsp + 48h]
	mov rbx, qword ptr [rsp + 50h]
	mov rbp, qword ptr [rsp + 58h]
	add rsp, 104
	ret

jit1_drop_signal PROC
	lea rsp, signal_sp
	mov rsp, qword ptr rsp
	jmp unwind_stack
jit1_drop_signal endp

jit1_springboard PROC
	sub rsp, 104
	mov qword ptr [rsp + 20h], r12
	mov qword ptr [rsp + 28h], r13
	mov qword ptr [rsp + 30h], r14
	mov qword ptr [rsp + 38h], r15
	mov qword ptr [rsp + 40h], rdi
	mov qword ptr [rsp + 48h], rsi
	mov qword ptr [rsp + 50h], rbx
	mov qword ptr [rsp + 58h], rbp
	
	mov qword ptr [signal_sp], rsp

	;xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
	;xor edi, edi
	xor esi, esi
	xor ebx, ebx
	;xor ebp, ebp
	mov rbp, rdx
	sub rbp, -128 ; 136

	; r9 = parameter pack struct.
	mov r12, qword ptr [r9 + 0]
	mov ebx, dword ptr [r9 + 8]
	mov esi, dword ptr [r9 + 12]
	mov r14, qword ptr [r9 + 16]
	mov r15d, dword ptr [r9 + 24]

	sub r12, -128
	mov rdi, r8
	;mov qword ptr [rsp], offset after_second_springboard
	call second_springboard
	jmp unwind_stack;
	
	second_springboard:

	sub rsp, 40
	jmp rcx
jit1_springboard endp
end