
#include "print.h"
#include "usercode.h"

int pipe(int pd[]) {
	printf("Pipe system call\n");
	syscall(30, pd, 0, 0);
	printf("Proc %d returned with fd = %d %d\n", getpid(), pd[0], pd[1]);
}


//for now, this will only read from pipes
int read(int fd, char line[], int linelength) {
	int n;

	n = syscall(31, fd, line, linelength);
	if (n < 0) {
		printf("PIPE Failed reading.\n");
	}
	return n;
}

//for now, this will only write to pipes
int write(int fd, char line[], int linelength) {
	int n;

	n = syscall(32, fd, line, linelength);
	if (n < 0) {
		printf("PIPE Failed writing.\n");
	}
	return n;
}


int close(int fd) {
	return syscall(33, fd, 0, 0);

}

int pfd() {
	return syscall(34, 0, 0, 0);
}

