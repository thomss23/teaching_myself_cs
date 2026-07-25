section .text
global binary_convert
; my solution
binary_convert:
mov eax, 0
.loop:
    cmp byte [rdi], 0
	je .loop_end
	movzx ecx, byte [rdi]  
	sub ecx, '0' 
	shl eax, 1
	or eax, ecx
	add rdi, 1
	jmp .loop
.loop_end:
	ret

