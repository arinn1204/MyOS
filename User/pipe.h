#ifndef _PIPE_H_
#define _PIPE_H_

int pipe(int pd[]);
int uclose();
int read(int fd, char line[], int linelength);
int uread();
int write(int fd, char line[], int linelength);
int uwrite();
int close(int fd);
int pfd();


#endif
