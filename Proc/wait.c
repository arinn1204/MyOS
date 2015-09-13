///@file wait.c
///@brief

#include "proc.h"
#include "wait.h"
#include "queue.h"
#include "print.h"

/**
* @brief This function is responsible for putting the processes to sleep
* @param event This parameter is used and stored in the running processes event field before it gives up the CPU
*/
int ksleep(int event) {
	extern PROC *running;
	running->event = event;
	running->status = SLEEP;
	tswitch();
}

/**
* @brief This function is called when a process has been woken up
* @param event This parameter is used to update the sleeping processes event field
*/
int kwakeup(int event) {
	extern PROC *readyQueue;
	extern PROC proc[NPROC];
	
	int i; PROC *p;
	for (i = 1; i < NPROC; i++) {
		p = &proc[i];
		if (p->status == SLEEP && p->event == event) {
			p->event = 0;
			p->status = READY;
			enqueue(&readyQueue, p);
		}
	}
}

/**
* @brief This function is used to mark a process as ready
* @param p This process is the process to be marked as ready
*/
int ready(PROC *p) {
	extern PROC *readyQueue;
	
	p->status = READY;
	enqueue(&readyQueue, p);
}

/**
* @brief This is the exit function for the processes
* @param exitValue This is the exitcode that the process will report back, it is stored in the running exitCode field
*/
int kexit(int exitValue) {
	extern int nproc;
	extern PROC *running;
	extern PROC proc[NPROC];

	PROC *p;
	int i, wakeupP1 = 0;
	if (running->pid == 1 && nproc > 2) {
		printf("Other procs still exist, P1 can't die yet\n");
		return -1;
	}

	//Send children to P1's orphanage
	for (i = 1; i < NPROC; i++) {
		p = &proc[i];
		if (p->status != FREE && p->ppid == running->pid) {
			p->ppid = 1;
			p->parent = &proc[i];
			wakeupP1++;
		}
	}

	//record exit code and become ZOMBIE
	running->exitCode = exitValue;
	running->status = ZOMBIE;
	//wake up parent and P1 if needed
	kwakeup(running->parent);
	if (wakeupP1) {
		kwakeup(&proc[1]);
	}
	tswitch();	//give up the CPU
}

/**
*/
int kwait(int *status) {
	//external globals
	extern PROC *running;
	extern PROC proc[NPROC];
	extern PROC *freeList;

	PROC *p; int i, hasChild = 0;
	while (1) {
		for (i = 1; i < NPROC; i++) { //searching for a child
			p = &proc[i];		//makes typing easier
			if (p->status != FREE && p->ppid == running->pid) {
				hasChild = 1;	//mark that it has a child
				if (p->status == ZOMBIE) {	//bury the dead child
					*status = p->exitCode;	//collect its exit code
					p->status = FREE;		//set the status to free
					put_proc(&freeList, p);	//put it in the freelist
					nproc--;			//decrement the number of processes running
					return(p->pid);		//return its PID
				}
			}
		}
		if (!hasChild) {	//if no children then return -1
			return -1;
		}
		ksleep(running);	//still has kids alive, go to sleep
	}
}





