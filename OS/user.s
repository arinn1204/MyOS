
.globl _running,_procSize

!----- USER MODE -------------
.globl _int80h, _goUmode ! EXPORT functions
.globl _kcinth ! IMPORT functions



_int80h:
	push ax
	push bx
	push cx
	push dx
	push bp
	push si
	push di
	push es
	push ds
	push cs
	pop ds

    USS = 4
    USP = 6
	mov bx, _running
	mov USS[bx], ss
	mov USP[bx], sp

	mov ax, ds
	mov es, ax
	mov ss, ax
	mov sp,bx
	add sp,_procSize
	call _kcinth
	jmp _goUmode

_goUmode:
	cli
	mov bx,_running
	mov ax,USS[bx]
	mov ss,ax
	mov sp,USP[bx]
	
	pop ds
	pop es
	pop di
	pop si
	pop bp
	pop dx
	pop cx
	pop bx
	pop ax
	iret
   


