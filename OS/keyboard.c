
#define KEYBD 	0x60
#define PORT_B	0x61
#define KBIT	0x80

#define NSCAN	64
#define KBLEN	64


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

typedef struct kbd {
	char buf[KBLEN];
	int head, tail;
	int data;
} KBD;


KBD kbd;

#include "io.h"
#include "wait.h"


int kbd_init() {
	printf("KB Init....\n");
	kbd.data = 0;
	kbd.head = kbd.tail = 0;
}


int kbhandler() {
	int scode, value, c;

	scode = in_byte(KEYBD);
	value = in_byte(PORT_B);
	out_byte(PORT_B, value | KBIT);
	out_byte(PORT_B, value);

	//printf("scode = %x", scode);
	if (scode & KBIT) {
		goto out;
	}

	c = unshift[scode];
	if(kbd.data >= KBLEN) {
		printf("%c\n", 0x07);
		goto out;
	}

	kbd.buf[kbd.tail++] = c;
	kbd.tail %= KBLEN;

	kbd.data++;
	kwakeup(&kbd.data);


	out:
		out_byte(0x20, 0x20);
}

int getc() {
	int c;

	lock();

	while(kbd.data <= 0) {
		unlock();
		ksleep(&kbd.data);
		lock();
	}

	c = kbd.buf[kbd.head++] & 0x7F;
	kbd.head %= KBLEN;
	kbd.data--;

	unlock();
	return c;
}

