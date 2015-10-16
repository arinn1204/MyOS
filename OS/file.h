#ifndef _FILE_H_
#define _FILE_H_


#define NFD 10
#define NOFT 20

typedef struct OpenFileTable {
	int mode;
	int refCount;
	struct pipe *pipe_ptr;
} OFT;

OFT oft[NOFT];

#endif
