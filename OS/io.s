.globl _getc,_readfd



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

_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret      
