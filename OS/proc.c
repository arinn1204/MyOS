///@file proc.c
///@brief This file contains the necessary files to run the basic processes
// this file will also contain a sample main file that can be ran in order to show that
// these functions work. In order for this to happen, the make file must be ran with the "make PROC" command
#include "file.h"
#include "pipe.h"
#include "proc.h"
#include "queue.h"
#include "kernel.h"
#include "io.h"



int goUmode();

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
char *names[] = {"Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Neptune", "Uranus", 0};
/**
*/
int init() {
	extern PROC proc[NPROC];
	extern PROC *freeList;
	extern PROC *readyQueue;
	extern PROC *sleepList;

	extern int nproc;


	extern OFT oft[NOFT];
	extern PIPE pipe[NPIPE];



	PROC *p; int i, j;


	for (i = 0; i < NPROC; i++) {
		p = &proc[i];
		p->pid = i;
		p->inkmode = 1;
		p->ppid = -1;
		p->status = FREE;
		p->priority = 0;
		p->next = &proc[i + 1];
		strcpy(p->name, names[i]);

		for(j = 0; j < NFD; j++) p->fd[j] = 0;
	}

	freeList = &proc[0]; proc[NPROC - 1].next = 0;

	readyQueue = sleepList = 0;
	//make P0 running
	p = get_proc(&freeList);
	p->status = READY;
	running = p;
	nproc = 1;



	for(i = 0; i < NOFT; i++) oft[i].refCount = 0;
	for(i = 0; i < NPIPE; i++) pipe[i].busy = 0;

	printf("Init has now finished.\n");
}
