#include "usercode.h"
#include "print.h"
#include "pipe.h"

int main(int argc, char *argv[]) {
	int n, pd[2];
	int pid = getpid();
	char *s = "Hello there!";
	char line[256];


	printf("Parent: %d\n", pid);

	pipe(pd);

	if( fork() ) {
		printf("Parent %d close pd[0]\n", pid);
		close(pd[0]);

		while(1) {
			sleep(2);
			printf("Parent %d writing to pipe: %s\n", pid, s);
			write(pd[1], s, strlen(s));
		}
	}
	else {
		printf("Child %d close pd[1]\n", getpid());
		close(pd[1]);

		while(1) {
			sleep(2);
			printf("Child %d reading from pipe: ", pid);
			n = read(pd[0], line, 256);
			line[n] = 0;
			printf("%s\n", line);
		}
	}



	return 0;
}