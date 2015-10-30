#include "file.h"
#include "proc.h"
#include "wait.h"
#include "io.h"
#include "video.h"
#include "int.h"
#include "keyboard.h"

int int80h(); int tinth(); int kbinth();


int procSize = sizeof( PROC );

void setInts() {
	//SYSCALL
	set_vec(80, int80h);
	
	lock();
	//TIMER
	set_vec(8, tinth);
	timer_init();
	//KEYBOARD
	set_vec(9, kbinth);
	kbd_init();

	unlock();
}

void MTXInit() {

	//initialize the screen
	vid_init();
	printf("\n");
	printf("MTX Starting in main\n\r");
	init();
	setInts();

}


int main() {
	extern PROC *readyQueue;
	extern PROC *running;
	extern PROC proc[NPROC];
	int i;

	MTXInit();
	#ifdef _SLEEPER_
		for(i = 0; i < 5; i++) kfork("/bin/u1");
	#else
		kfork("/bin/u1");
	#endif

	printf("P%d is now going to enter infinite loop!\n", running->pid);
	while(1) {
		//printf("P%d is now waiting for something in the queue...\n", running->pid);
		while( readyQueue) tswitch();
		//printf("P%d is now going to switch!\n", running->pid);
		
		//printf("P%d is now running\n", running->pid);
	}
}
