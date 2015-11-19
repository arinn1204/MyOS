
#include "file.h"
#include "pipe.h"
#include "proc.h"
#include "semaphore.h"


/**************** CONSTANTS ***********************/
#define INT_CTL 		0x20
#define ENABLE 			0x20

#define NULLCHAR 		0
#define BEEP 			7
#define BACKSPACE 		8
#define ESC 			27
#define SPACE 			32

#define BUFLEN 			64
#define LSIZE 			64

#define NR_STTY 		2    /* number of serial ports */

/* offset from serial ports base */
#define DATA 			0   /* Data reg for Rx, Tx   */
#define DIVL			0   /* When used as divisor  */
#define DIVH 			1   /* to generate baud rate */
#define IER 			1   /* Interrupt Enable reg  */
#define IIR				2   /* Interrupt ID rer      */
#define LCR				3   /* Line Control reg      */
#define MCR				4   /* Modem Control reg     */
#define LSR				5   /* Line Status reg       */
#define MSR				6   /* Modem Status reg      */

/**** The serial terminal data structure ****/


struct stty {
	//input
	char inbuf[BUFLEN];
	int inhead, intail;
	struct semaphore inchars;

	//output
	char outbuf[BUFLEN];
	int outhead, outtail;
	struct semaphore outchars;

	int tx_on;

	//control section
	char echo;
	char ison;
	char erase, kill, intr, quit, x_on, x_off, eof;


	//IO port base
	int port;
}; 
struct stty stty[NR_STTY];


int bputc(int port, int c) {
	while( ( in_byte(port + LSR) & 0x20 ) == 0 );
	out_byte(port + DATA, c);
}

int bgetc(int port) {
	while ( ( in_byte( port + LSR ) & 0x01 ) == 0 );
	return ( in_byte(port + DATA) & 0x7F );
}

typedef unsigned char u8;

int enable_irq(u8 irq_nr) {
	out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr) );
}

int sinit() {
	int i;
	struct stty *t;
	char *q;
	char *str = "\n\rSerial Port Ready\n\r\007";

	for(i = 0; i < NR_STTY; i++) {
		q = str;
		printf("sinit port=#%d\n", i);

		t = &stty[i];
		//initialize the structures pointers
		if ( ! i )  t->port = 0x3F8;
		else 		t->port = 0x2F8;

		t->inchars.queue = 0; 	t->inchars.data = 0;
		t->outchars.queue = 0; 	t->outchars.data = 0;

		t->inhead 	= t->intail  = 0;
		t->outhead 	= t->outtail = 0;

		t->tx_on = 0;

		//not used currently, control characters
		t->ison = t->echo = 1;
		t->erase = 	'\b';
		t->kill = 	'@';
		t->intr = 	(char)0177; // del
		t->quit = 	(char)034;  // CTRL+C
		t->x_on = 	(char)021;  // CTRL+Q
		t->x_off = 	(char)023;  // CTRL+S
		t->eof = 	(char)004;  // CTRL+D

		lock();

			out_byte(t->port + IER, 0x00);  //disable serial interruprs
			out_byte(t->port + LCR, 0x80);  //use 3f9,3f8 as divisor
			out_byte(t->port + DIVH, 0x00); 
			out_byte(t->port + DIVL, 0x0c); //divisor 12 == 9600 baud

			//term 9600 /dev/ttyS0 8 bits/char no parity
			out_byte(t->port + LCR, 0x03);

			out_byte(t->port + MCR, 0x08);
			out_byte(t->port + IER, 0x01);

		unlock();

		enable_irq(4-i);

		while(*q) {
			bputc(t->port, *q);
			q++;
		}
	}
}


int s0handler() {
	shandler(0);
}

int s1handler() {
	shandler(1);
}


int shandler(int port) {
	struct stty *t;
	int IntID, LineStatus, ModemStatus, intType, c;

	t = &stty[port];

	//read registers
	IntID = 		in_byte(t->port + IIR); //read interrupt reg
	LineStatus = 	in_byte(t->port + LSR); //linestatus
	ModemStatus = 	in_byte(t->port + MSR);

	intType = IntID & 7;
	switch(intType) {
		case 6: do_errors(t); 	break;
		case 4: do_rx(t);		break;
		case 2: do_tx(t);		break;
		case 0: do_modem(t);	break;
	}

	out_byte(0x20, 0x20);

}

int do_errors() {
	printf("No Error handling at this time\n");
}

int do_modem() {
	printf("No modem handling at this time\n");
}

int enable_tx(struct stty *t) {
	lock();
	out_byte(t->port + IER, 0x03);
	t->tx_on = 1;
	unlock();
}

int disable_tx(struct stty *t) {
	lock();

	out_byte(t->port + IER, 0x01);
	t->tx_on = 0;

	unlock();
}

int do_rx(struct stty *t) {
	int c;
	c = in_byte(t->port) & 0x7F;

	printf("port %x interrupt:c=%c ", t->port, c);

	t->intail = (t->intail + 1) % BUFLEN;

	t->inbuf[t->intail] = c;
	
	t->inchars.data++;

	V(&t->inchars);
}

int sgetc(struct stty *t) {
	int c;
	P(&t->inchars);

	lock();

	t->inhead = (t->inhead + 1) % BUFLEN;

	c = t->inbuf[t->inhead];
	t->inchars.data--;

	unlock();

	return c;
}

//since this function relies upon sgetc, no need to call P()
int sgetline(struct stty *t, char *line) {
	int i = 0;
	while(i < BUFLEN && t->inbuf[t->inhead] != '\n')
		line[i++] = sgetc(t);

	return strlen(line);
}

int sgets(int port, int strADDR) {
	int seg, ret, i;
	char buf[BUFLEN + 1];
	struct stty *t;
	extern PROC *running;


	seg = (running->pid + 1) * 0x1000;

	if (port < 0 || port > NR_STTY)	
		return -1;

	t = &stty[port];

	ret = sgetline(t, buf);
	buf[ret] = 0;

	for(i = 0; buf[i]; i++) {
		put_byte(buf[i], seg, strADDR + i);
	}

	return ret;

}


int do_tx(struct stty *t) {
	int c;
	//printf("TX interrupt\n");

	if(t->outchars.data == BUFLEN) {
		disable_tx(t);
		return 1; //1 for nothing done
	}
	
	t->outhead = ( t->outhead + 1) % BUFLEN;
	c = t->outbuf[t->outhead];
	t->outchars.data--;

	out_byte(t->port, c);
	V(&t->outchars);
	return 0; //0 for success

}

int sputc(struct stty *t, int c) {

	if(t->outchars.data == BUFLEN)
		P(&t->outchars);

	lock();
	
	t->outtail = (t->outtail + 1 ) % BUFLEN;
	t->outbuf[t->outtail] = c;
	
	t->outchars.data++;
	
	if(t->tx_on == 0)
		enable_tx(t);

	unlock();
}

int sputline(struct stty *t, char *line) {
	int ret = strlen(line);
	while(*line) {
		sputc(t, *line);
		line++;
	}

	return ret;

}


int sputs(int port, int strADDR) {
	char buf[BUFLEN];
	int i, seg, ret;

	struct stty *t;

	extern PROC *running;



	seg = (running->pid + 1)*0x1000;
	t = &stty[port];
	for(i = 0; get_word(seg, strADDR + i) != 0 && i < BUFLEN; i++) {
		buf[i] = get_word(seg, strADDR + i);
	}

	ret = sputline(t, buf);

	return ret;
}
