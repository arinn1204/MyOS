#include "pipe.h"
#include "file.h"
#include "proc.h"
/*
typedef struct pipe {
	char buf[PSIZE];
	int head, tail, data, room;
	int nreader, nwriter;
	int busy;
} PIPE;
*/


int show_pipe(PIPE *pipe) {	
	//print running procs opened file descriptors
	int k, flag = 0;
	int fd[2];
	fd[0] = -1;
	fd[1] = -1;

	printf("-------------PIPE-CONTENTS----------\n");
	for(k = 0; k < NFD; k++) {
		if (!flag && oft[k].pipe_ptr == pipe)	 		fd[0] = k, flag = 1;
		else if (flag && oft[k].pipe_ptr == pipe)		fd[1] = k, flag = 2;
		if (flag == 2) break;
	}
		printf("Pipe: FD = [%d, %d]\n", fd[0], fd[1]);
		printf("Data: %d Room %d\nWriters: %d Readers: %d\nBuffer: %s\n",
			pipe->data, pipe->room, pipe->nreader, pipe->nwriter, pipe->buffer.buffer);
	printf("------------------------------------\n");
}

char *MODE[] = {"READ_PIPE", "WRITE_PIPE", 0};

int pfd() {
	int i;
	extern PIPE pipe[NPIPE];

	while(i < NPIPE) {
		if(pipe[i].busy) show_pipe(&pipe[i]);
		i++;
	}

}


int readFromBuffer(CYCLIC *buf) {
	int ret = 0;
	int head = buf->head;
	int tail = buf->tail;

//	printf("Head: %d, Tail: %d, Buffer: %s\n", head, tail, buf->buffer);
	
	ret = buf->buffer[head];
	buf->buffer[head] = 0;
	buf->head = (head + 1) % PSIZE;



	return ret;
}

int read_pipe(int fd, char *buf, int n) {
	int r = 0, loc = 0;
	int byte;
	PIPE *pp;
	OFT *op;

	extern OFT oft[NOFT];


	if (n <= 0) return 0;
	if (fd < 0 || fd > NFD) return -1;

	op = &oft[fd];
	pp = op->pipe_ptr;

	while(n) {
		while(pp->data) {
			//but buf is only 10 bytes... so only 10 bytes can be stored at a time?
			//put_byte(pp->buf.buf[pos], running->uss, buf + r);
			byte = readFromBuffer(&pp->buffer);
			put_byte(byte, running->uss, buf + r);
			pp->data--; pp->room++;
			r++; n--;
			if ( n == 0 ) break;
		}
		// i read data, wake up whoever slept on pp->room if there is anyone
		if ( r ) {
			kwakeup(&pp->room);
			return r;
		}
		// now we know that the pipe has no data remaining
		if ( pp->nwriter ) {
			kwakeup(&pp->room);
			ksleep(&pp->data);
			continue;
		}
		//theres no writer and no data to read
		return 0;
	}
}

int addToBuffer(CYCLIC *buf, int data) {
	int tail = buf->tail;
	int head = buf->head;

	buf->buffer[tail] = data;
	buf->tail = (tail + 1) % PSIZE;

//	printf("Head: %d, Tail: %d, Buffer: %s\n", head, tail, buf->buffer);

	return 0;
}

int write_pipe(int fd, char *buf, int n) {
	int r = 0;
	int byte = 0;
	PIPE *pp;
	OFT *op;

	extern OFT oft[NOFT];
	
	if (n <= 0) return 0;
	if (fd < 0 || fd > NFD) return -1;

	op = &oft[fd];
	pp = op->pipe_ptr;

	while(n) {
		if ( !pp->nreader ) kexit(BROKEN_PIPE);
		
		while(pp->room) {
			//pp->buf.buf[pos] = get_byte(running->uss, buf + r);
			byte = get_byte(running->uss, buf + r);
			addToBuffer( &pp->buffer, byte);
			pp->data++; pp->room--;
			r++; n--;

			if ( n == 0 ) break;
		}
		kwakeup(&pp->data);
		if(n == 0) return r;
		ksleep(&pp->room);
	}

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
			p->data = p->buffer.head = p->buffer.tail = 0;
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
			if(fd[0] == -1) { //first descriptor ie read
				fd[0] = i; //read oft
				oft[i].mode = READ_PIPE;
				oft[i].refCount++;
				oft[i].pipe_ptr = p;
			}
			else if (fd[1] == -1) { // second descriptor ie write
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

