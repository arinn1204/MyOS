#include "proc.h"
#include "wait.h"
#include "print.h"

int main() {
	extern PROC *readyQueue;

	printf("MTX Starting in main\n");
	init();
	kfork();
	while(1) {
		while(!readyQueue);
		tswitch();
	}
}
