/**
* @file
* @brief
*/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#include "fs.h"
#include "kernel.h"
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
		putc(name[i++]);
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
		case 0: 	r = getpid();				break;
		case 1: 	r = do_ps();				break;
		case 2:		r = do_chname(b);			break;
		case 3:		r = kfork("/bin/u1")->pid;	break;
		case 4:		r = do_tswitch();			break;
		case 5:		r = kwait(b);				break;
		case 6:		r = kexit(b);				break;
		case 7:		color = 0xA + (running->pid % 7);
					r = putc(b);				break;
		case 8:		r = getc();					break;
		case 9:		r = fork();					break;
		case 10:	r = kexec(b);				break;
		default: printf("%d is not supported currently.\n\r"); break;
	}
#ifndef _LAB_3_
	put_word(r, segment, offset + 2*AX);
#endif

	return 0;
}

