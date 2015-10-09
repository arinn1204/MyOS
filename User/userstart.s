.globl _main0,_syscall,_exit
.globl _getcs
.globl auto_start

auto_start:
		call _main0
		push  #0
    	call  _exit        

_getcs:
    mov   ax, cs
    ret

_syscall:
    int    80
    ret
