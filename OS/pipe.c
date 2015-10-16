#include "pipe.h"
#include "file.h"
#include "proc.h"

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
	int i, fd[2];
	int j = 0;

	PIPE *p = 0;
	extern PIPE pipe[NPIPE];

	extern OFT oft[NOFT];

	extern PROC *running;

	fd[0] = -1;	//read
	fd[1] = -1;	//write


	for(i = 0; i < NPIPE; i++) {
		if( ! pipe[i].busy ) { //found an available pipe
			p = &pipe[i];
			p->data = p->head = p->tail = 0;
			p->room = PSIZE;
			p->busy = p->nreader = p->nwriter = 1;
			break;
		}
	}

	if( ! p ) {
		printf("No available pipes!\n");
		return 1;
	}

	for(i = 0; i < NOFT; i++) {
		if (fd[0] != -1 && fd[1] != -1) break;
		if (oft[i].refCount == 0 ) {
			if(fd[0] == -1) { //first descriptor
				fd[0] = i; //read oft
				oft[i].mode = READ_PIPE;
				oft[i].refCount++;
				oft[i].pipe_ptr = p;
			}
			else if (fd[1] == -1) { // second descriptor
				fd[1] = i;
				oft[i].mode = WRITE_PIPE;
				oft[i].refCount++;
				oft[i].pipe_ptr = p;
			} // end of else if
		} //end of if refcount = 0
	} // end of for loop

	for(i = 0; i < NFD; i++) {
		if (j > 1) break;
		if(running->fd[i]->refCount == 0) {
			running->fd[i] = &oft[ fd[ j++ ] ]; //since fd contains the two indicies
		}
	}
	put_word(fd[0], running->uss, pd);
	put_word(fd[1], running->uss, pd + 1);

}


int close_pipe(int fd) {
	OFT *op;
	PIPE *pp;
	
	extern PROC *running;

	printf("Proc %d close pipe: %d\n", running->pid, fd);

	if(fd >= NFD || fd < 0) {
		printf("Not a valid FD: %d\n", fd);
		return 1;
	}

	op = running->fd[fd];

	running->fd[fd] = 0;

	if(op->mode == READ_PIPE) {
		pp = op->pipe_ptr;
		pp->nreader--;

		if(--op->refCount == 0) {	//last reader
			if (pp->nwriter <= 0) {	//no writers left
				pp->busy = 0;		//free the pipe
				return 0;
			}
		}

		kwakeup(&pp->room);
		return 0;
	}

	else { //write pipe
		pp = op->pipe_ptr;
		pp->nwriter--;
		if(--op->refCount == 0) { //last writer
			if(pp->nreader <= 0) { //no readers left
				pp->busy = 0;
				return 0;
			}
		}

		kwakeup(&pp->data);
		return 0;
	}

}

