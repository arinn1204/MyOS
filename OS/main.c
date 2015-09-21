#include "proc.h"
#include "wait.h"
#include "io.h"


int procSize = sizeof( PROC );
int color = 0x0A;

int main() {
	extern PROC *readyQueue;
	extern PROC *running;
	extern PROC proc[NPROC];

	kprintf("\n\n");
	kprintf("MTX Starting in main\n\r");
	init();
	
	//printf("Forking new process\n");
	do_kfork();

	do_ps();

	//printf("Fork has completed\n");

	kprintf("P%d is now going to enter infinite loop!\n", running->pid);
	while(1) {
		kgetc();
		kprintf("P%d is now waiting for something in the queue...\n", running->pid);
		while( ! readyQueue);
		kprintf("P%d is now going to switch!\n", running->pid);
		do_tswitch();
		kprintf("p%d is now running\n", running->pid);
		kgetc();
	}
}
