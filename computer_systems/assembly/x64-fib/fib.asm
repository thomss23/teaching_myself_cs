section .text
global fib
fib:

.main:
	cmp edi, 1
	jle .end_base
	push rdi
	dec edi
	call fib
	pop rdi
	push rax
	dec edi
	dec edi
	call fib
	pop rcx
	add eax, ecx
	jmp .end_fib
.end_base:	
	mov eax, edi
	ret

.end_fib:
	ret
