/**@file kernel.c
*@brief something
*/
#include "file.h"
#include "proc.h"
#include "wait.h"
#include "queue.h"
#include "io.h"

int getpid() {
	extern PROC *running;
	#ifdef _LAB_3_
		printf("%d",running->pid);
	#else
		return running->pid;
	#endif
}

int maxName() {
	extern PROC proc[NPROC];
	int i, max = -1;

	for(i = 0; i < NPROC; i++) {
		if ( strlen(proc[i].name) > max)
			max = strlen(proc[i].name);
	}
	return max;
}

int printName(char *name) {
	int i, max = maxName();
	printf("%s:", name);
	for(i = strlen(name) + 1; i <= max; i++) putc(' ');
}


/*typedef enum STATUS {FREE,
      READY,
      RUNNING,
      STOPPED,
      SLEEP,
      ZOMBIE
} STATUS;
*/
char *statuses[] = {"FREE", "READY", "RUNNING", "STOPPED", "SLEEP", "ZOMBIE", 0};

int printStatus(int status) {
	int i, max = -1;

	for(i = 0; i < 6; i++) if( strlen( statuses[i] ) > max ) max = strlen( statuses[i] );

	printf("%s", statuses[status]);
	for (i = strlen( statuses[status] ); i < max; i++) putc(' ');
}



/**
*/
int do_ps() {
	extern PROC proc[NPROC];

	int i;

	printName("Name"), printf("       PID  PPID  STATUS  PRIORITY\n\n");
	for(i = 0; i < NPROC; i++) {
		printName(proc[i].name);
		puts("    ");
		(proc[i].pid < 0) 	? 	printf("   %d", proc[i].pid) 	: printf("    %d", proc[i].pid);
		(proc[i].ppid < 0) 	? 	printf("    %d", proc[i].ppid) 	: printf("     %d", proc[i].ppid);
		printf("   "), printStatus(proc[i].status);
		printf("    %d\n\r", proc[i].priority);
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
	char *file = 0;
	PROC *p = kfork(file);
	if (p == 0) {
		printf("fork() failed!\n");
		return -1;
	}
	printf("PROC %d fork a child %d\n", running->pid, p->pid);
	return p->pid;
}

/**
*/
int do_exit(int exit_code) {
	kexit(exit_code);
}

/**
*/
int do_wait(int *status) {
	int pid;
	pid = kwait(status);
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
	printf("Enter a pid between 0 and 9 to resume\n"); c=getc();
	pid = c - '0';
	if (pid >= NPROC || pid < 0) {
		printf("%d is an invalid number\n", pid);
		return -1;
	}

	if (proc[pid].status != STOPPED) {
		printf("Process %d is not stopped\n", pid);
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
	printf("Which proc do you want to wakeup? ");
	pid = getI();
	printf("\n\r");
	printf("Waking P%d\n", pid);

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
		printf("chpriority failed\n");
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
	printf("input pid: "); 			pid = getI();
	printf("\n\r");
	printf("input new priority: "); 	pri = getI();
	printf("\n\r");

	if (pri  < 1) pri = 1;
	return chpriority(pid, pri);
}
