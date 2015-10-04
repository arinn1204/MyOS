///@file proc.c
///@brief This file contains the necessary files to run the basic processes
// this file will also contain a sample main file that can be ran in order to show that
// these functions work. In order for this to happen, the make file must be ran with the "make PROC" command

#include "proc.h"
#include "queue.h"
#include "kernel.h"
#include "io.h"

#define REG_COUNT 8
#define ADDR_COUNT (REG_COUNT + 1)

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
	unsigned short word;

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

	p->kstack[SSIZE - 1] = (int)body;
	p->ksp = &p->kstack[SSIZE - ADDR_COUNT];

	enqueue(&readyQueue, p);



	if(filename) {
		//printf("File: %s\n\r", filename);
		segsize = 0x1000;
		segment = (p->pid + 1) * segsize;
		ret = load(filename, segment);
		if (!ret) return p;

		for(i = 1; i < 9; i++) {
			offset = (i * (-2)); 
			switch(i) {
				case 1:		word = 0x0200;	break; 	//uFlag
				case 2:								//uCS
				case 7:								//uES
				case 8:		word = segment; break;	//uDS
				default: 	word = 0; 		break;	//everything else
			}
			put_word(word, segment, offset);
		}		
		p->usp = offset;
		p->uss = segment;
		printf("Process set up with %s\n", filename);
	}

	return p;
}
/**
*/
int body() {
	extern int rflag;
	extern PROC *freeList;
	extern PROC *readyQueue;
	extern PROC proc[NPROC];
	extern int color;
	extern PROC *running;

	int pid = running->pid;
	char c;	
	while(1) {
		color = (pid % 6) + 0x0A;
		if (rflag) {
			printf("Proc %d: reschedule\n", pid);
			rflag = 0;
			tswitch();
		}
		printf("|********************************************|\n\r");
		printList("freeList ", freeList);
		printQueue("readyQueue ", readyQueue);
		printf("UMode Segment %x\n\r", (pid + 1)*0x1000);
		printf("|********************************************|\n\r");
		//printList("SleepList\t", sleepList);
		if (pid < 10) 	printf("P0%d", pid);
		else			printf("P%d", pid);
		printf(" running: priority=%d parent=%d enter a char:\n"
			"[s|f|t|c|z|a|p|w|q|u]\n",
			pid, proc[pid].priority, proc[pid].ppid);
		c = getc(); putc(c); printf("\n\r");
		switch(c) {
			case 's':	do_tswitch(); 		break;
			case 'f':	kfork("/bin/u1"); 	break;
			case 'q':	do_exit(); 			break;
			case 't':	do_stop(); 			break;
			case 'c':	do_continue(); 		break;
			case 'z':	do_sleep(); 		break;
			case 'a':	do_wakeup(); 		break;
			case 'p':	do_chpriority(); 	break;
			case 'w':	do_wait(); 			break;
			case 'u':	goUmode();			break;
			default: printf("Not a supported command\n"); break;
		}
	}
}

/**
*/
int scheduler() {
	extern PROC *running;
	extern PROC *readyQueue;
	extern int rflag;

	if (running->status == READY) {
		enqueue(&readyQueue, running);
	}
	running = dequeue(&readyQueue);
	rflag = 0;

}
char *names[] = {"sun", "mercury", "venus", "earth", "mars", "jupiter", "saturn", "neptune", "uranus"};
/**
*/
int init() {
	extern PROC proc[NPROC];
	extern PROC *freeList;
	extern PROC *readyQueue;
	extern PROC *sleepList;

	extern int nproc;

	PROC *p; int i;


	for (i = 0; i < NPROC; i++) {
		p = &proc[i];
		p->pid = i;
		p->ppid = -1;
		p->status = FREE;
		p->priority = 0;
		p->next = &proc[i + 1];
		strcpy(p->name, names[i]);
	}

	freeList = &proc[0]; proc[NPROC - 1].next = 0;

	readyQueue = sleepList = 0;
	//make P0 running
	p = get_proc(&freeList);
	p->status = READY;
	running = p;
	nproc = 1;

	printf("Init has now finished.\n");
}
