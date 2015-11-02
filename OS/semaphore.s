.globl _int_on, _int_off


_int_on:
	push bp
	mov bp,sp
	mov ax,4[bp]
	push ax
	popf
	pop bp
	ret

_int_off:
	pushf
	cli
	pop ax
	ret



