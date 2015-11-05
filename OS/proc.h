/**@file proc.h
*@brief This contains the process structures and the function prototypes for the functions related to the processes
*/
#ifndef _PROC_H
#define _PROC_H

#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define FREE      0
#define READY     1
#define RUNNING   2
#define STOPPED   3
#define SLEEP     4
#define ZOMBIE    5
#define BLOCK     6
/*typedef enum STATUS {FREE,
      READY,
      RUNNING,
      STOPPED,
      SLEEP,
      ZOMBIE
} STATUS;
*/
struct proc {
            struct proc *next;   
            int *ksp;                /* saved sp; offset = 2 */
            int uss;                 /* usermode stack segment, offset = 4 */
            int usp;                 /* usermode stack pointer, offset = 6 */
            int inkmode;             /* Kernel mode flag, offset = 8 */
            
            int  timer;
            int  pid;
            int  status;            /* READY|DEAD, etc */
            int  ppid;
            struct proc *parent;
            int priority;
            int event;
            int exitCode;
            char name[32];

            OFT *fd[NFD];


            int  kstack[SSIZE];     // kmode stack of task
};

typedef struct proc PROC;
PROC proc[NPROC], *running, *readyQueue, *freeList, *sleepList;
int nproc, rflag;


PROC *kfork(char *filename);
int body();
int scheduler();
int init();

#endif
