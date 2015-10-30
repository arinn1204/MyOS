#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

int kbd_init();
int kbhandler();
int getc();

typedef struct semaphore {
	int data;
	PROC *queue;
} SEMAPHORE;

#endif