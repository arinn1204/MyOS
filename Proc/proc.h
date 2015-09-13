///@file proc.h
///@brief This contains the process structures and the function prototypes for the functions related to the processes

#ifndef PROC_H
#define PROC_H

#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define FREE 0
#define READY 1
#define RUNNING 2
#define STOPPED 3
#define SLEEP 4
#define ZOMBIE 5

struct proc {
            struct proc *next;   
            int  ksp;               /* saved sp; offset = 2 */
            int  pid;
            int  status;            /* READY|DEAD, etc */
            int  ppid;
            struct proc *parent;
            int priority;
            int event;
            int exitCode;
            int  kstack[SSIZE];     // kmode stack of task
};

typedef struct proc PROC;
PROC proc[NPROC], *running, *readyQueue, *freeList, *sleepList;
int nproc, rflag;
int procSize = sizeof(proc);

PROC *kfork();
int body(int pid);
int scheduler();
int init();

#endif


