!----------------- proc.s file -----------------------------------------------
        .globl _main,_proc,_procSize,_running   ! IMPORT these
        !----- USER MODE -------------
        .globl _int80h, _goUmode ! EXPORT functions
        .globl _kcinth ! IMPORT functions


        ! ------- TESTING ---------
        .globl _putc

	MTXSEG = 0x1000
	jmpi start, MTXSEG


start:
	mov     ax,cs                   ! establish segments 
	mov     ds,ax                   ! Let DS,SS,ES = CS=0x1000.
	mov     ss,ax                   
	mov     es,ax
	
	mov     sp,#_proc               ! sp -> proc[0]
	add     sp,_procSize            ! sp -> proc[0]s HIGH END

	call _main                      ! call main() in C

dead:	jmp dead                        ! loop if main() ever returns

	

! these are for KUMODE


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
   
!------------------------------------------------------------------------
