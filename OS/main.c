#include "proc.h"
#include "wait.h"
#include "io.h"
#include "int.h"

int int80h();


int procSize = sizeof( PROC );

#ifndef _MTXLIB_

int color = 0x0A;

#endif

int main() {
	extern PROC *readyQueue;
	extern PROC *running;
	extern PROC proc[NPROC];

	printf("\n\n");
	printf("MTX Starting in main\n\r");
	init();
	printf("Init complete\n\r");
	set_vec(80, int80h);
	
	kfork("/bin/u1");

	printf("P%d is now going to enter infinite loop!\n", running->pid);
	while(1) {
		printf("P%d is now waiting for something in the queue...\n", running->pid);
		while( ! readyQueue);
		printf("P%d is now going to switch!\n", running->pid);
		tswitch();
		printf("p%d is now running\n", running->pid);
	}
}
