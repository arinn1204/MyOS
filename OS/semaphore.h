#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

typedef struct semaphore {
	int data;
	int lock;
	PROC *queue;
} SEMAPHORE;

int swait(PROC *queue);
int ssignal(PROC *queue);

int P(SEMAPHORE *s);
int V(SEMAPHORE *s);

int int_on(int flag);
int int_off();

#endif
