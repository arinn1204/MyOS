///@file proc.h
///@brief This contains the process structures and the function prototypes for the functions related to the processes

#ifndef PROC_H
#define PROC_H

#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD 0
#define READY 1
   
struct proc {
            struct proc *next;   
            int  ksp;               /* saved sp; offset = 2 */
            int  pid;
            int  status;            /* READY|DEAD, etc */
            int  kstack[SSIZE];     // kmode stack of task
};
typedef struct proc PROC;
PROC proc[NPROC], *running;
int procSize = sizeof(proc);


int body(int pid);
int scheduler();
int initialize();

#endif


