!----------------- proc.s file -----------------------------------------------
        .globl _tswitch,_putc,_getc ! EXPORT variables
        .globl _main,_running,_scheduler,_proc,_procSize   ! IMPORT these

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
SAVE:	push ax
        	push bx
	push cx
	push dx
	push bp
	push si
	push di
	push ds
	push ss
        	pushf
	mov   bx, _running
	mov   2[bx], sp

FIND:	call _scheduler
	
RESUME:	
	mov   bx, _running
	mov   sp, 2[bx]
	popf
	pop ds
	pop ss
	pop  di
	pop  si
	pop  bp
	pop  dx
	pop  cx
	pop  bx
	pop  ax
	ret

_putc:           
        push   bp
        mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        movb   bl,#0x0D        ! bL = cyan color 
        int    0x10            ! call BIOS to display the char

        pop    bp
        ret

_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret         

