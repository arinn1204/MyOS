
typedef unsigned short u16;

#include "proc.h"
#include "forkexec.h"
#include "queue.h"
#include "io.h"

#define REG_COUNT 8
#define ADDR_COUNT (REG_COUNT + 1)

#ifndef _LAB_3_

#define USER_REG 13

#else

#define USER_REG 9

#endif



int goUmode();

/**
*/
PROC *kfork(char *filename) {
	extern PROC *running;
	extern PROC *readyQueue;
	extern PROC *freeList;
	extern PROC proc[NPROC];
	int i,segment, segsize, offset, ret;
	PROC *p;
	u16 word;

	p = get_proc(&freeList);

	if (!p) {
		printf("No more PROC kfork() failed\n");
		return 0;
	}

	
	p->status = READY;
	p->priority = 1;
	p->ppid = running->pid;
	p->parent = &proc[running->pid];

	for(i = 1; i <= ADDR_COUNT; i++) {
		p->kstack[SSIZE - i] = 0;
	}

	p->kstack[SSIZE - 1] = (int)goUmode;
	p->ksp = &p->kstack[SSIZE - ADDR_COUNT];

	enqueue(&readyQueue, p);



	if(filename) {
		//printf("File: %s\n\r", filename);
		segsize = 0x1000;
		segment = (p->pid + 1) * segsize;
		ret = load(filename, segment);
		if (!ret) return p;

		for(i = 1; i < USER_REG; i++) {
			offset = (i * (-2)); 
			switch(i) {
				case 1:				word = 0x0200;	break; 	//uFlag
				case 2:										//uCS
				case USER_REG - 2:							//uES
				case USER_REG - 1:	word = segment; break;	//uDS
				default: 			word = 0; 		break;	//everything else
			}
			put_word(word, segment, offset);
		}		
		p->usp = offset;
		p->uss = segment;
		printf("Process set up with %s\n", filename);
	}

	return p;
}

int copyImage(u16 parentSegment, u16 childSegment, u16 size) {
	u16 i;
	for(i = 0; i < size; i++) put_word( get_word(parentSegment, 2*i), childSegment, 2*i );
}

int fork() {
	int pid; u16 segment;

	PROC *p = kfork(0);
	if( !p ) return -1;

	segment = (p->pid + 1)*0x1000;
	copyImage(running->uss, segment, 32*1024);

	p->uss = segment;
	p->usp = running->usp;


	put_word(segment, 	segment, p->usp);			//uDS = segment
	put_word(segment, 	segment, p->usp+2);			//uES = segment
	put_word(0, 		segment, p->usp+2*8);		//uAX = 0
	put_word(segment,	segment, p->usp+2*10);		//uCS = segment
	return p->pid;

}

int kexec(char *filename) {
	int i, length = 0, word;
	int offset;
	char temp[64], cmd[64],  *file = temp;
	u16 segment = running->uss;
	for(i=0;i<64;i++)cmd[i]=0;

	while( (cmd[length] = get_byte(running->uss, filename++)) && length++ < 64);

	cmd[ (length == 64) ? 63 : length ] = 0;
	strcpy(temp, cmd);
	file = strtok(temp, " ");
	if ( !load(file, segment) ) return -1;

	length = strlen(cmd);


	for(i = 1; i < 13; i++) {
		switch(i) {
			case 1: word = 0x0200; 		break;
			case 2: 
			case 11:
			case 12: word = segment; 	break;
			default: word = 0; 			break;
		}
		offset = -2*i - (2 + length);
		put_word(word, segment, offset);
	}
	running->usp = -2*12 - ( 2 + length );

	//since the pointer has to live in a location, this will be two positions over
	//hence the -2*-1 instead of -2*0
	i = -1;

	word = -2*i - (2 + length); 
	put_word(word, segment, -(2 + length));

	for(i = 0; i < length; i++) {
		put_byte(cmd[i], segment, -(length - i ) );
	}

	//put_byte(0, segment, 0);
	
}


