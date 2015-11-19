#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

typedef struct semaphore {
	int data;
	int lock;
	PROC *queue;
} SEMAPHORE;

int swait(PROC *queue);
int ssignal(PROC *queue);

int P(SEMAPHORE *s); //wait until you get your thing
int V(SEMAPHORE *s); //notify first in line that something is available

int int_on(int flag);
int int_off();

#endif
