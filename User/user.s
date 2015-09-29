.globl _main,_syscall,_exit
.globl _getcs
.globl _putc,_getc,_color

	call _main

	!if ever return, exit(0)
	push  #0
    call  _exit        

_getcs:
    mov   ax, cs
    ret

_syscall:
    int    80
    ret


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


