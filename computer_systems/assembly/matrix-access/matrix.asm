section .text
global index
index:
	; rdi: matrix
	; esi: rows
	; edx: cols
	; ecx: rindex
	; r8d: cindex
	imul ecx, edx
	add ecx, r8d
	mov eax, [rdi + rcx * 4]
	ret

; [[1,2,3],[4,5,6]]

; matrix[1][2] = 6

; offset = (rindex * cols + cindex) * 4
