/**@file kernel.c
*@brief something
*/
#include "proc.h"
#include "wait.h"
#include "queue.h"
#include "io.h"



/**
*/
int do_ps() {
	extern PROC proc[NPROC];

	int i;

	kprintf("Name:          PID  PPID  STATUS\n\r");
	for(i = 0; i < NPROC; i++) {
		kprintf("%s:           %d   %d   %d   %d\n\r",
			proc[i].name, proc[i].pid, proc[i].ppid, proc[i].status, proc[i].priority);
	}
}
/**
*/
int chname(char *newname) { 
	extern PROC *running;
	strcpy(running->name, newname);

}

/**
*/
int set_vec(unsigned short vector, unsigned short handler) {
	put_word(handler, 0, vector << 2);
	put_word(0x1000, 0, (vector << 2) + 2);
}


/**
*/
int do_tswitch() {
	tswitch();
}

/**
*/
int do_kfork() {
	extern PROC *running;

	PROC *p = kfork(0);
	if (p == 0) {
		kprintf("fork() failed!\n");
		return -1;
	}
	kprintf("PROC %d fork a child %d\n", running->pid, p->pid);
	return p->pid;
}

/**
*/
int do_exit() {
	kexit(0);
}

/**
*/
int do_wait() {
	int status, pid;
	pid = kwait(&status);
	return pid;
}

/**
*/
int do_stop() {
	extern PROC *running;
	running->status = STOPPED;
	tswitch();
}

/**
*/
int do_continue() {
	extern PROC proc[NPROC];
	extern PROC *readyQueue;

	int pid;
	char c;
	kprintf("Enter a pid between 0 and 9 to resume\n"); c=kgetc();
	pid = c - '0';
	if (pid >= NPROC || pid < 0) {
		kprintf("%d is an invalid number\n", pid);
		return -1;
	}

	if (proc[pid].status != STOPPED) {
		kprintf("Process %d is not stopped\n", pid);
		return -1;
	}

	proc[pid].status = READY;
	enqueue(&readyQueue, &proc[pid]);
	return 0;
}

/**
*/
int do_sleep() {
	extern PROC *running;
	ksleep(running);
}

/**
*/
int do_wakeup() {
	extern PROC proc[NPROC];
	int pid;
	kprintf("Which proc do you want to wakeup? ");
	pid = getI();
	kputs("\n\r");
	kprintf("Waking P%d\n", pid);

	kwakeup(&proc[pid]);
}

/**
*/
int reschedule() {
	extern PROC *readyQueue;
	extern int rflag;
	extern PROC *running;

	PROC *p, *tempQ = 0;
	while ( (p=dequeue(&readyQueue) ) ) {
		enqueue(&tempQ, p);
	}

	readyQueue = tempQ;
	rflag = 0;	//reschedule flag

	if(running->priority < readyQueue->priority) {
		rflag = 1;
	}
}

/**
*/
int chpriority(int pid, int pri) {
	extern PROC *running;
	extern PROC *readyQueue;
	extern PROC proc[NPROC];

	PROC *p; int i, ok=0, reQ = 0;

	if (pid == running->pid) {
		running->priority = pri;
		if (pri < readyQueue->priority) {
			rflag = 1;
		}
		return 1;
	}

	for (i = 1; i < NPROC; i++) {
		p = &proc[i];
		if(p->pid == pid && p->status != FREE) {
			p->priority = pri;
			ok = 1;
			if (p->status == READY) {
				reQ = 1;
			}
		}
	} //end for

	if ( ! ok ) {
		kprintf("chpriority failed\n");
		return -1;
	}

	if ( reQ ) {
		reschedule(p);
	}
}

/**
*/
int do_chpriority() {
	int pid, pri;
	kprintf("input pid: "); 			pid = getI();
	kputs("\n\r");
	kprintf("input new priority: "); 	pri = getI();
	kputs("\n\r");

	if (pri  < 1) pri = 1;
	return chpriority(pid, pri);
}
