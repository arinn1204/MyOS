#include "usercode.h"
#include "print.h"
#include "pipe.h"

int main(int argc, char *argv[]) {
	int n, pd[2], count = 0;
	int pid = getpid();
	char *s = "Hello there!";
	char line[256];


	printf("Parent: %d\n", pid);

	pipe(pd);

	if( fork() ) {
		printf("Parent %d close pd[0]\n", pid);
		close(pd[0]);

		while(1) {
			printf("Parent %d writing to pipe: %s\n", pid, s);
			write(pd[1], s, strlen(s));
			count++;
			if(count > 10) break;
		}
		close(pd[1]);
	}
	else {
		pid = getpid();
		printf("Child %d close pd[1]\n", pid);
		close(pd[1]);

		while(1) {
			printf("Child %d reading from pipe\n", pid);
			n = read(pd[0], line, 256);
			line[n] = 0;
			printf("%d read %d from pipe: %s\n", pid, n, line);
			count++;
			if(count > 10) break;
		}
		close(pd[0]);
	}



	return 0;
}