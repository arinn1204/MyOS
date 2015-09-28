#include "proc.h"
#include "wait.h"
#include "io.h"
#include "int.h"

int int80h();


int procSize = sizeof( PROC );
int color = 0x0A;

int main() {
	extern PROC *readyQueue;
	extern PROC *running;
	extern PROC proc[NPROC];

	printf("\n\n");
	printf("MTX Starting in main\n\r");
	init();
	printf("Init complete\n\r");
	set_vec(80, int80h);
	
	getc();
	
	kfork("/bin/u1");

	printf("P%d is now going to enter infinite loop!\n", running->pid);
	while(1) {
		printf("P%d is now waiting for something in the queue...\n", running->pid);
		while( ! readyQueue);
		printf("P%d is now going to switch!\n", running->pid);
		do_tswitch();
		printf("p%d is now running\n", running->pid);
	}
}
