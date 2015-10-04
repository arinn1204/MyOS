.globl _tswitch
.globl _scheduler,_running
	
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

