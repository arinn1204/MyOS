

.globl _procSize, _running
.globl _ireturn, _lock, _unlock, _in_byte, _out_byte

.globl _int80h, _tinth ! #EXPORT the interrupt handlers
.globl _kcinth, _thandler ! #IMPORT C interrupt handlers



USS = 4
USP = 6
INK = 8

! as86 macro: parameters are ?1 ?2, etc 
! as86 -m -l listing src (generates listing with macro expansion)

MACRO INTH
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
	pop  ds

	mov bx,_running		! ready to access proc
	inc INK[bx]
	cmp INK[bx],#1
	jg   ?1

	! was in Umode: save interrupted (SS,SP) into proc
	mov USS[bx],ss 		! save SS  in proc.USS
	mov USP[bx],sp 		! save SP  in proc.USP

	! change DS,ES,SS to Kernel segment
	mov  di,ds 			! stupid !!        
	mov  es,di 			! CS=DS=SS=ES in Kmode
	mov  ss,di

	mov  sp,_running 	! sp -> runnings kstack[] high end
	add  sp,_procSize

	?1:	call  _?1		! call handler in C
	br    _ireturn		! return to interrupted point

MEND


_int80h: INTH kcinth
_tinth:  INTH thandler



_lock:
	cli
	ret

_unlock:
	sti
	ret


_in_byte:
	push bp
	mov bp,sp
	mov dx,4[bp]
	in ax,dx
	subb ah,ah
	pop bp
	ret

_out_byte:
	push bp
	mov bp,sp
	mov dx,4[bp]
	mov ax,6[bp]
	outb dx,al
	pop bp
	ret


