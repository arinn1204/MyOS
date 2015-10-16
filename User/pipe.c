
#include "print.h"
#include "usercode.h"

int pipe(int pd[]) {
	printf("Pipe system call\n");
	syscall(30, pd, 0, 0);
	printf("Proc %d returned with fd = %d %d\n", getpid(), pd[0], pd[1]);
}