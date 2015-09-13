///@file proc.c
///@brief This file contains the necessary files to run the basic processes
// this file will also contain a sample main file that can be ran in order to show that
// these functions work. In order for this to happen, the make file must be ran with the "make PROC" command

#include "proc.h"
#include "queue.h"
#include "kernel.h"
#include "print.h"

/**
*/
PROC *kfork() {
	extern PROC *running;
	extern PROC *readyQueue;
	extern PROC *freeList;
	extern int nproc;

	int i;
	PROC *p = get_proc(&freeList);

	if (!p) {
		printf("No m ore PROC kfork() failed\n");
		return 0;
	}


	p->status = READY;
	p->priority = 1;
	p->ppid = running->pid;

	for(i = 1; i < NPROC; i++) {
		p->kstack[SSIZE - i] = 0;
	}

	p->kstack[SSIZE - 1] = (int)body;
	p->ksp = &p->kstack[SSIZE - 9];
	enqueue(&readyQueue, p);
	return p;
}
/**
*/
int body(int pid) {
	extern int rflag;
	extern PROC *running;
	extern PROC *freeList;
	extern PROC *readyQueue;

	char c;	
	while(1) {
		if (rflag) {
			printf("Proc %d: reschedule\n", running->pid);
			rflag = 0;
			tswitch();
		}

		printList("freeList    ", freeList);
		printQueue("readyQueue      ", readyQueue);
		printf("Proc %d running: priority=%d parent=%d enter a char:\n"
			"[s|f|t|c|z|a|p|w|q]\n",
			running->pid, running->priority, running->parent->pid);
		c = getc(); printf("%c\n", c);
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
}
