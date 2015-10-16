/**
* @file
* @brief
*/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#include "fs.h"
#include "kernel.h"
#include "file.h"
#include "proc.h"
#include "io.h"
#include "forkexec.h"

#define AX 8

#ifndef _LAB_3_

	#define PA 13

#else

	#define PA 9

#endif



int do_chname(char *str) {
	extern PROC *running;
	int i = 0;
	char name[32];
	u16 segment = ( running->pid + 1 )*0x1000;

	while( get_byte(segment, str + i ) != 0 && i < 32) {
		name[i] = get_byte(segment, str + i);
	}

	name[ i ] = 0;
	return chname(&name[0]);
}

/**
* @brief
*/
int kcinth() {
	extern int color;
	extern PROC *running;
	int a,b,c,d,r;
	u16 offset = running->usp;
	u16 segment = ( running->pid + 1 ) * 0x1000;

	//getting cmd params off of stack from ax, bx, cx, dx
	a = get_word(segment, offset + 2*PA);
	b = get_word(segment, offset + 2*(PA + 1));
	c = get_word(segment, offset + 2*(PA + 2));
	d = get_word(segment, offset + 2*(PA + 3));

	switch(a) {
		//BASIC FUNCTIONS
		case 0: 	r = getpid();				break;
		case 1: 	r = do_ps();				break;
		case 2:		r = do_chname(b);			break;
		case 3:		r = do_tswitch();			break;
		case 4:		r = do_wait(&c);
					put_word(c, segment, b);	break;

		case 5:		r = do_exit(b);				break;
		case 6:		r = fork();					break;
		case 7:		r = exec(b);				break;

		//		IO FUNCTIONS
		case 10:	color = 0xA + (running->pid % NPROC);
					r = putc(b);				break;
					
		case 11:	r = getc();					break;
		//		END IO
		
		//		PIPE FUNCTIONS
		case 30:	r = kpipe(b);				break;
		case 31:	r = read_pipe(b, c, d);		break;
		case 32:	r = write_pipe(b, c, d);	break;
		case 33:	r = close_pipe(b);			break;
		case 34:	r = pfd();					break;
		// 		END PIPE



		default: printf("%d is not supported currently.\n\r"); break;
	}
#ifndef _LAB_3_
	put_word(r, segment, offset + 2*AX);
#endif

	return 0;
}

