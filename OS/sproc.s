!----------------- proc.s file -----------------------------------------------
        .globl _tswitch,_putc,_getc,_readfd ! EXPORT variables
        .globl _main,_running,_scheduler,_proc,_procSize,_color   ! IMPORT these
        !----- USER MODE -------------
        .globl _int80h, _goUmode ! EXPORT functions
        .globl _kcinth ! IMPORT functions

        !----- REG GET/SET ----------
        .globl _setds,_getds				! DS
        .globl _setes,_inces				! ES


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

_readfd:                             
        push  bp
      	mov   bp,sp            ! bp = stack frame pointer

        movb  dl, #0x00        ! drive 0=FD0
        movb  dh, 6[bp]        ! head
        movb  cl, 8[bp]        ! sector
        incb  cl               ! BIOS count sector from 1
        movb  ch, 4[bp]        ! cyl
        mov   bx, 10[bp]       ! BX=buf ==> memory addr=(ES,BX)
        mov   ax, #0x0202      ! READ 2 sectors to (EX, BX)

        int  0x13              ! call BIOS to read the block 
        !jb   _error            ! to error if CarryBit is on [read failed] 

        pop  bp                
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


.globl _putc,_getc,_color

_putc:           
        push   bp
        mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        mov     bx,_color
        int    0x10            ! call BIOS to display the char

        pop    bp
        ret

_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret         
