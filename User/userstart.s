.globl _main,_syscall,_exit
.globl _getcs
.globl _putc,_getc

start:	
		call _main
		push  #0
    	call  _exit        

_getcs:
    mov   ax, cs
    ret

_syscall:
    int    80
    ret
