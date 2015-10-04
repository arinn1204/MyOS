.globl _getds,_setds,_setes,_inces


_getds:
	xor ax,ax ! clear out ax
	mov ax,ds ! move ds into ax
	ret 	  ! return hopefully with getds

_setds:
	push  bp
    mov   bp,sp

    mov   ax,4[bp]        
    mov   ds,ax

    pop   bp
    ret

_setes:
	push bp
	mov bp,sp

	mov ax,4[bp]
	mov es,ax

	pop bp
	ret

_inces:

	mov ax,es
	add ax,#0x40
	mov es,ax

	ret

