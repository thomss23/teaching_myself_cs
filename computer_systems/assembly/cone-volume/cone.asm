default rel

section .data
pi: dd 3.14159265    
three: dd 3.0     

section .text
global volume
volume:
	; xmm0 radius
	; xmm1 height
	divss xmm1, [rel three]
	mulss xmm0, xmm0
	mulss xmm0, [rel pi]
	mulss xmm0, xmm1
 	ret
