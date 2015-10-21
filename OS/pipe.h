#ifndef _PIPE_H_
#define _PIPE_H_

#define PSIZE 10
#define NPIPE 10


#define READ_PIPE 4
#define WRITE_PIPE 5
#define BROKEN_PIPE 10



typedef struct CyclicBuffer {
	char buffer[PSIZE];
	int head, tail;
} CYCLIC;


typedef struct pipe {
	struct CyclicBuffer buffer;
	int data, room;
	int nreader, nwriter;
	int busy;
} PIPE;



PIPE pipe[NPIPE];

//buffer stuff
int readFromBuffer(CYCLIC *buf);
int addToBuffer(CYCLIC *buf, int data);

int pfd();
int read_pipe(int , char *, int );
int write_pipe(int, char *, int );
int kpipe(int pd[]);
int close_pipe(int );


#endif