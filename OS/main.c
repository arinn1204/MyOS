#include "file.h"
#include "proc.h"
#include "wait.h"
#include "io.h"
#include "video.h"
#include "int.h"
#include "keyboard.h"
#include "serial.h"


int int80h(); int tinth(); int kbinth(); int s0inth(); int s1inth();

#define TIMERINT 	8
#define KBINT 		9
#define S0INT 		12
#define S1INT		11

#define SYSCALL 	80


int procSize = sizeof( PROC );

void setInts() {
	//SYSCALL
	set_vec(SYSCALL, int80h);
	
	lock();
	
	//TIMER
	set_vec(TIMERINT, tinth);
	timer_init();

	//KEYBOARD
	set_vec(KBINT, kbinth);
	kbd_init();

	set_vec(S0INT, s0inth);
	set_vec(S1INT, s1inth);
	sinit();

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
	
	MTXInit();
	kfork("/bin/u1");

	while(1) {
		//printf("P%d is now waiting for something in the queue...\n", running->pid);
		if( readyQueue ) tswitch();
		//printf("P%d is now going to switch!\n", running->pid);
		
		//printf("P%d is now running\n", running->pid);
	}
}
