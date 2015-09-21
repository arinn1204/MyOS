/*@file queue.h
*@brief
*/
#ifndef _QUEUE_H
#define _QUEUE_H

PROC *get_proc(PROC **list);
PROC *dequeue(PROC **queue);

int put_proc(PROC **list, PROC *p);
int enqueue(PROC **queue, PROC *p);

#endif