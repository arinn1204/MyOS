///@file queue.c
///@brief

#include "proc.h"
#include "queue.h"

#ifndef _MTXLIB_
/**
*/
int enqueue(PROC **queue, PROC *p)
{
     PROC *q = *queue;
     if (q == 0 || p->priority > q->priority){
        *queue = p;
         p->next = q;
     }
     else{
         while (q->next && p->priority <= q->next->priority)
                q = q->next;
         p->next = q->next;
         q->next = p;
     }
}

// remove and return first PROC in queue 
/**
*/
PROC *dequeue(PROC **queue)
{
     PROC *p = *queue;
     if (p)
        *queue = (*queue)->next;
     return p;
}

/**
*/
PROC *get_proc(PROC **list) {
    extern int nproc;

    PROC *p = *list;
    if (p)
        *list = (*list)->next;
    nproc++;
    return p;
}

/**
*/
int put_proc(PROC **list, PROC *p) {
    PROC *t = *list;
    if ( ! t ) {
        *list = p;
    }
    else {
        while ( t->next ) { // go to the end
            t = t->next;
        }
        //now throw p in there
            t->next = p;
    }

}

#endif

