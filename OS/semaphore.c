
#define READY 1
#define BLOCK 6

#include "file.h"
#include "pipe.h"
#include "proc.h"
#include "io.h"
#include "semaphore.h"

int wait(PROC *queue) {
	extern PROC *running;
	running->status = BLOCK;
	enqueue(&queue, running);
	tswitch();
}

int signal(PROC *queue) {
	PROC *p;
	extern PROC *readyQueue;
	p = dequeue(&queue);
	p->status = READY;
	enqueue(&readyQueue, p);
	do_ps();
}


int P(SEMAPHORE *s) {
	int sr = int_off();
	s->data--;
	printf("Process Blocked, Data: %d\n", s->data);
	if(s->data  <= 0) wait(s->queue);
	int_on(sr);
}

int V(SEMAPHORE *s) {
	int sr = int_off();
	s->data++;
	printf("Process Dequeued, Data: %d\n", s->data);
	if(s->data <= 0) signal(s->queue);
	int_on(sr);
}

