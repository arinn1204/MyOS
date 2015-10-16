#include "pipe.h"
#include "file.h"
#include "proc.h"


#define READ_PIPE 4
#define WRITE_PIPE 5


int show_pipe(PIPE *pipe) {
	int i, j;
	printf("-------------PIPE-CONTENTS----------\n");
	//actually display the shit, idk what that means though?
	printf("------------------------------------\n");
}

char *MODE[] = {"READ_PIPE", "WRITE_PIPE", 0};

int pfd() {
	//print running procs opened file descriptors
}

int read_pipe(int fd, char *buf, int n) {

}

int write_pipe(int fd, char *buf, int n) {

}

//pd is located in user mode
int kpipe(int pd[]) {
	
}


int close_pipe(int fd) {
	OFT *op;
	PIPE *pp;
	
	extern PROC *running;

	printf("Proc %d close pipe: %d\n", running->pid, fd);

	op = running->fd[fd];

	running->fd[fd] = 0;

	if(op->mode == READ_PIPE) {
		pp = op->pipe_ptr;
		pp->nreader--;

		if(--op->refCount == 0) {	//last reader
			if (pp->nwriter <= 0) {	//no writers left
				pp->busy = 0;		//free the pipe
				return;
			}
		}

		kwakeup(&pp->room);
		return;
	}

	else {

	}

}

