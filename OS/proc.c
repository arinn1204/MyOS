///@file proc.c
///@brief This file contains the necessary files to run the basic processes
// this file will also contain a sample main file that can be ran in order to show that
// these functions work. In order for this to happen, the make file must be ran with the "make PROC" command

#include "proc.h"
#include "queue.h"
#include "kernel.h"
#include "io.h"

#define REG_COUNT 10
#define ADDR_COUNT (REG_COUNT + 3)

/**
*/
PROC *kfork() {
	extern PROC *running;
	extern PROC *readyQueue;
	extern PROC *freeList;
	extern PROC proc[NPROC];

	int i;
	PROC *p;
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

	p->kstack[SSIZE - ADDR_COUNT] = 0x1000;
	p->kstack[SSIZE - (ADDR_COUNT - 1)] = 0x1000;
	p->kstack[SSIZE - 3] = (int)body;
	p->kstack[SSIZE - 1] = p->pid;
	p->ksp = &p->kstack[SSIZE - ADDR_COUNT];

	enqueue(&readyQueue, p);
	return p;
}
/**
*/
int body(int pid) {
	extern int rflag;
	extern PROC *freeList;
	extern PROC *readyQueue;
	extern PROC proc[NPROC];
	extern int color;


	char c;	
	while(1) {
		color = (pid % 6) + 0x0A;
		if (rflag) {
			printf("Proc %d: reschedule\n", pid);
			rflag = 0;
			tswitch();
		}
		printList("freeList\t", freeList);
		printQueue("readyQueue\t", readyQueue);
		printf("The color should be... %x\n", color);
		//printList("SleepList\t", sleepList);

		printf("Proc %d running: priority=%d parent=%d enter a char:\n"
			"[s|f|t|c|z|a|p|w|q]\n",
			pid, proc[pid].priority, proc[pid].ppid);
		c = getc(); putc(c); puts("\n\r");
		switch(c) {
			case 's':	do_tswitch(); 		break;
			case 'f':	do_kfork(); 		break;
			case 'q':	do_exit(); 		break;
			case 't':	do_stop(); 		break;
			case 'c':	do_continue(); 		break;
			case 'z':	do_sleep(); 		break;
			case 'a':	do_wakeup(); 		break;
			case 'p':	do_chpriority(); 	break;
			case 'w':	do_wait(); 		break;
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
		p->status = FREE;
		p->priority = 0;
		p->next = &proc[i + 1];
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
