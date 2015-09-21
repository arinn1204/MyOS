!----------------- proc.s file -----------------------------------------------
        .globl _tswitch,_kputc,_kgetc ! EXPORT variables
        .globl _main,_running,_scheduler,_proc,_procSize,_color   ! IMPORT these
        !----- USER MODE -------------
        .globl _int80h, _goUmode ! EXPORT functions
        .globl _kcinth ! IMPORT functions


        MTXSEG = 0x1000
        USS = 4
        USP = 6
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

	
_tswitch:
SAVE:	

	push ax
    push bx
	push cx
	push dx
	push bp
	push si
	push di
    pushf

	mov   bx, _running
	mov   2[bx], sp

FIND:	call _scheduler
	
RESUME:	
	mov   bx, _running
	mov   sp, 2[bx]

	popf
	pop  di
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax


	ret

_kputc:           
        push   bp
        mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        mov     bx,_color
        int    0x10            ! call BIOS to display the char

        pop    bp
        ret

_kgetc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret         



! these are for KUMODE


_int80h:
	push bp
	push si
	push di
	push es
	push ds
	push cs


	pop ds

	mov si, _running
	mov USS[si], ss
	mov USP[si], sp

	mov di, ds
	mov es, di
	mov ss, di


	mov sp,_running
	add sp,_procSize

	call _kcinth
	jmp _goUmode



_goUmode:
	cli
	mov bx,_running
	mov cx,USS[bx]
	mov ss,cx
	mov sp,USP[bx]

	pop ds
	pop es
	pop di
	pop si
	pop bp

	iret


