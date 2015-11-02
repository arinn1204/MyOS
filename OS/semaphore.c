
#define READY 1
#define BLOCK 6

#include "file.h"
#include "pipe.h"
#include "proc.h"
#include "io.h"
#include "semaphore.h"
#include "queue.h"

int swait(PROC *queue) {
	extern PROC *running;
	printf("Running address: %x\n", running);
	
	running->status = BLOCK;
	enqueue(&queue, running);
	tswitch();
}

int ssignal(PROC *queue) {
	PROC *p;
	extern PROC *readyQueue;

	p = dequeue(&queue);
	printf("New Proc: %x\n", p);
	p->status = READY;
	enqueue(&readyQueue, p);
	do_ps();
}


int P(SEMAPHORE *s) {
	int sr = int_off();
	s->data--;
	printf("Process Blocked, Data: %d\n", s->data);
	if(s->data  <= 0) swait(s->queue);
	int_on(sr);
}

int V(SEMAPHORE *s) {
	int sr = int_off();
	s->data++;
	printf("Process Dequeued, Data: %d\n", s->data);
	if(s->data <= 0) ssignal(s->queue);
	int_on(sr);
}

