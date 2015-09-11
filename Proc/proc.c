///@file proc.c
///@brief This file contains the necessary files to run the basic processes
// this file will also contain a sample main file that can be ran in order to show that
// these functions work. In order for this to happen, the make file must be ran with the "make PROC" command

#include "proc.h"
#include "io.h"

int body(int pid) {
	return 0;
}

int scheduler() {
	return 0;
}

int initialize() {
	extern PROC proc[NPROC];
	int i, j;
	PROC *p;

	for (i = 0; i < NPROC - 1; i++) {
		p = &proc[i];
		p->next = &proc[i + 1];
		p->pid = i;
		p->status = READY;

		if(i) {
			for(j = 1; j < 10; j++)
				p->kstack[SSIZE - j] = 0;
			p->kstack[SSIZE - 1] = (int)body;
			p->ksp = &(p->kstack[SSIZE - 9]);
		}
	}

	running = &proc[0];
	proc[NPROC - 1].next = &proc[0];
	printf("Initialization complete\n");
}


#ifdef _PROC_

	int main(void) {
		initialize();
		printf("Hello!\n");
		return 0;
	}


#endif

