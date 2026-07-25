section .text
global pangram
; my solution
pangram:
	; rdi: source string
	mov edx, 0
	_loop:
	  movzx eax, byte [rdi] 
	  add rdi, 1
	  cmp eax, 0
	  je _loop_end
      cmp eax, 0x40
	  jl _loop
	  and eax, 0x1f
	  mov ecx, eax 
	  mov ebx, 1
	  shl ebx, cl 
	  or edx, ebx
	  jmp _loop
	_loop_end:
	  and edx, 0x07fffffe
	  cmp edx, 0x07fffffe
	  sete al
	  movzx eax, al
	ret

#define MASK 0x07fffffe
; instructor's solution
pangram:
	xor ecx, ecx; bs = 0 
.loop:
	movzx edx, byte [rdi]; c = *s
	cmp edx, 0
	je .end
	add rdi, 1
	cmp edx, '@' ; if c is in first 64 chars of ascii table
	jl .loop
	and edx, 0x1f
	bts ecx, edx 
	jmp .loop
.end: 
    xor eax, eax
	and ecx, MASK
    cmp ecx, MASK
	sete al
	ret


; bool ispangram(char *s) {
;   uint32_t bs = 0;
;   char c;
;   while ((c = *s++) != '\0') {
;     if (c < '@')
;       continue; // ignore first 64 chars in ascii table
;     bs |= 1 << (c & 0x1f);
;   }
;   return (bs & MASK) == MASK;
; }