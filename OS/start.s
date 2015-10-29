!----------------- proc.s file -----------------------------------------------
        .globl _main,_proc,_procSize   ! IMPORT these
        .globl _goUmode, _running, _ireturn
      
	MTXSEG = 0x1000
	jmpi start, MTXSEG


start:
	mov     ax,cs                   ! establish segments 
	mov     ds,ax                   ! Let DS,SS,ES = CS=0x1000.
	mov     ss,ax                   
	mov     es,ax
	
	mov     sp,#_proc               ! sp -> proc[0]
	add     sp,_procSize            ! sp -> proc[0]s HIGH END
	mov     ax,#0x0003
    int     #0x10
    
	call _main                      ! call main() in C

dead:	jmp dead                        ! loop if main() ever returns



USS = 4
USP = 6
INK = 8

_ireturn:
_goUmode:
	cli
	mov bx,_running

	dec INK[bx]
	cmp INK[bx], #0
	jg xkmode


	mov ax,USS[bx]
	mov ss,ax
	mov sp,USP[bx]

xkmode:	
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
