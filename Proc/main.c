#include "proc.h"
#include "wait.h"
#include "print.h"

int procSize = sizeof(proc);

int main() {
	extern PROC *readyQueue;
	extern PROC *running;

	printf("\n\n");
	printf("MTX Starting in main\n");
	getc();
	init();
	//printf("Forking new process\n");
	getc();
	do_kfork();

	//printf("Fork has completed\n");

	printf("P%d is now going to enter infinite loop!\n", running->pid);
	while(1) {
		while( ! readyQueue);
		do_tswitch();
	}
}
