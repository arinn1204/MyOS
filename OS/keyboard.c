
#define KEYBD 	0x60
#define PORT_B	0x61
#define KBIT	0x80

#define NSCAN	64
#define KBLEN	64
#define READY   1
#define BLOCK   6


//break macro
#define BREAK(x) (x+0x80)

//special key codes
#define LSHIFT 	0x2A
#define LSHBRK 	BREAK(LSHIFT)

#define RSHIFT 	0x36
#define RSHBRK 	BREAK(RSHIFT)

#define CAPS 	0x3A
#define CAPSBRK BREAK(CAPS)

//F keys
#define F1		0x3B
#define F1BRK	BREAK(F1)

#define F2		0x3C
#define F2BRK	BREAK(F2)

#define F3		0x3D
#define F3BRK	BREAK(F3)

#define F4		0x3E
#define F4BRK	BREAK(F4)

#define F5		0x3F
#define F5BRK	BREAK(F5)

#define F6		0x40
#define F6BRK	BREAK(F6)

#define F7		0x41
#define F7BRK	BREAK(F7)

#define F8		0x42
#define F8BRK	BREAK(F8)

#define F9		0x43
#define F9BRK	BREAK(F9)

#define F10		0x44
#define F10BRK	BREAK(F10)


char shift[NSCAN] = {
	0,033,'!','@','#','$','%','^',		'&','*','(',')','_','+','\b','\t',
	'Q','W','E','R','T','Y','U','I',	'O','P','{','}', '\r', 0,'A','S',
	'D','F','G','H','J','K','L',':', 	 0,'~', 0,'|','Z','X','C','V',
	'B','N','M','<','>','?',0,'*', 	 	 0, ' '  
};

char unshift[NSCAN] = { 
	0,033,'1','2','3','4','5','6',		'7','8','9','0','-','=','\b','\t',
	'q','w','e','r','t','y','u','i',	'o','p','[',']', '\r', 0,'a','s',
	'd','f','g','h','j','k','l',';',	 0, 0, 0, 0,'z','x','c','v',
	'b','n','m',',','.','/', 0,'*',		 0, ' '       
};

#include "file.h"
#include "pipe.h"
#include "io.h"
#include "wait.h"
#include "proc.h"
#include "keyboard.h"
#include "kernel.h"
#include "queue.h"
#include "semaphore.h"

typedef struct kbd {
	char buf[KBLEN];
	int head, tail;
	SEMAPHORE data;
} KBD;


KBD kbd;


int kbd_init() {
	printf("KB Init....\n");
	kbd.data.data = 0;
	kbd.data.queue = 0;
	kbd.head = kbd.tail = 0;
}


int kbhandler() {
	int scode, value, c = 0;
	int status;

	static int shiftKey = 0;
	static int shifted = 0;

	static int caps = 0;
	static int capcount = 0;

	static int fpush = 0;

	extern PROC *running;
	extern int uptime;

	scode = in_byte(KEYBD);
	value = in_byte(PORT_B);
	out_byte(PORT_B, value | KBIT);
	out_byte(PORT_B, value);

	//printf("scode = %x\n", scode);

	if (scode >= F1 && scode <= F10) goto F_Key;
	if (scode >= F1BRK && scode <= F10BRK) fpush = 0;


	if (scode == CAPS) {
		if(capcount == 0) {
			caps = 1;
			capcount++;
		}
		else {
			caps = 0;
			capcount--;
		}
		goto out;
	}
	else if (scode == CAPSBRK) {
		goto out;
	}


	if (scode == LSHIFT || scode == RSHIFT) {
		shiftKey++;
		shifted = 1;
		goto out;
	}
	else if (scode == LSHBRK || scode == RSHBRK) {//left and right shift break keys
		if (--shiftKey == 0) shifted = 0;
		goto out;	
	}

	if (scode & KBIT) goto out;
	value = shifted + caps;

	//range for all the characters
	if 	( 	(scode >= 0x10 && scode <= 0x19) ||
			(scode >= 0x1E && scode <= 0x26) ||
			(scode >= 0x2C && scode <= 0x32)
		) {

		switch(value) {
			case 0: c = unshift[scode]; break;
			case 1: c = shift[scode]; 	break;
			case 2: c = unshift[scode]; break;
		}
	}
	//everything else on keyboard
	//caps lock only effects letters, so now it is based on if shifted or not
	else {
		c = (shifted) ? shift[scode] : unshift[scode];
	}

	if(kbd.data.data >= KBLEN) {
		printf("%c\n", 0x07);
		goto out;
	}

	kbd.buf[kbd.tail++] = c;
	kbd.tail %= KBLEN;

	V(&kbd.data);
	goto out;


	F_Key:
		printf("\n");
		switch(scode) {
			case F1: if(fpush == 0) do_ps();
					 fpush = 1; 
					 goto out;
			//don't change priority of P0
			case F2: if(fpush == 0 && running->pid != 0) chpriority(running->pid, 2);
					 fpush = 1;
					 goto out;

			case F3: if(fpush == 0 && running->pid != 0) ksleep(uptime + 10);
					 fpush = 1;
					 goto out;
					 //can't exit if P0
			case F4: if(fpush == 0 && running->pid != 0) {
						out_byte(0x20, 0x20);	
					 	fpush = 1;
					 	do_exit(0);
					 }

			default: goto out;
		}


	out:
		out_byte(0x20, 0x20);
}

int getc() {
	int c;

	P(&kbd.data);

	lock();
	
	c = kbd.buf[kbd.head++] & 0x7F;
	kbd.head %= KBLEN;

	unlock();
	return c;
}

