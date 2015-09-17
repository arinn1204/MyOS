#include "proc.h"
#include "wait.h"
#include "print.h"

int procSize = sizeof(proc);
int color = 0x0A;

int main() {
	extern PROC *readyQueue;
	extern PROC *running;
	extern PROC proc[NPROC];

	printf("\n\n");
	printf("MTX Starting in main\n\r");
	init();
	
	//printf("Forking new process\n");
	do_kfork();

	//printf("Fork has completed\n");

	printf("P%d is now going to enter infinite loop!\n", running->pid);
	while(1) {
		getc();
		printf("P%d is now waiting for something in the queue...\n", running->pid);
		while( ! readyQueue);
		printf("P%d is now going to switch!\n", running->pid);
		do_tswitch();
		printf("p%d is now running\n", running->pid);
		getc();
	}
}
