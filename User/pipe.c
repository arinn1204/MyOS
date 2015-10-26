
#include "print.h"
#include "usercode.h"

int pipe(int pd[]) {
	printf("Pipe system call\n");
	syscall(30, pd, 0, 0);
	printf("Proc %d returned with fd = %d %d\n", getpid(), pd[0], pd[1]);
}

int uclose() {
	int fd;
	printf("FD To close: ");
	fd = getc();
	fd -= '0';
	printf("%d\n", fd);
	if (fd < 0 || fd > 9) {
		printf("Invalid FD\n");
		return -1;
	}
	close(fd);

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

int uread() {
	int fd, nbytes;
	char buf[128];
	printf("Which FD to read from: ");
	fd = getc();
	fd -= '0';
	printf("%d\n", fd);

	if (fd < 0 || fd > 9) {
		printf("Invalid FD\n");
		return 0;
	}
	while( (nbytes = read(fd, buf, 127)) > 0) {
		buf[nbytes] = 0;
		printf("Read %d from FD: %d %s\n", nbytes, fd, buf);
	}
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

int uwrite() {
	int fd, nbytes;
	char buf[128];

	printf("Which FD to write to: ");
	fd = getc(); 
	fd -= '0';
	printf("%d\n", fd);
	
	if (fd < 0 || fd > 9) {
		printf("Invalid FD\n");
		return 0;
	}
	
	printf("Buffer to write to %d: ", fd);
	gets(buf);
	printf("\n");
	nbytes = write(fd, buf, strlen(buf));
}

int close(int fd) {
	return syscall(33, fd, 0, 0);
}

int pfd() {
	return syscall(34, 0, 0, 0);
}

