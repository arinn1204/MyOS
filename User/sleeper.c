#include "usercode.h"
#include "print.h"



int main(int argc, char *argv) {
	int i, pid, ppid;
	unsigned long int j;
	if ( fork() ) {
		pid = getpid(); ppid=getppid();
		for(i = 0; i < 25; i++) {
			j = 0;
			printf("%d) I am %d and my parent is %d\n", i, pid, ppid);
			while(++j <= 0xFFFFF);
		}
		exit(0);
	}
	else {
		pid = getpid(); ppid=getppid();
		for(i = 0; i < 25; i++) {
			j = 0;
			printf("%d) I am %d and my parent is %d\n", i, pid, ppid);
			while(++j <= 0xFFFFF);
		}
		exit(0);
	}
}