section .text
global sum_to_n
sum_to_n:
	mov rax, rdi
	mov rcx, 0
	loop_start:
		cmp rax, 0
		jle loop_done
		add rcx, rax
		sub rax, 1
		jmp loop_start
	loop_done:
		mov rax, rcx
		ret
