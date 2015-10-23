#include "usercode.h"
#include "print.h"
#include "pipe.h"

int main(int argc, char *argv[]) {
	int n, pd[2], count = 0;
	int pid = getpid();
	char c;
	char *s = "Hello!";
	char line[256];


	printf("Parent: %d\n", pid);

	pipe(pd);

	if( fork() ) {
		printf("Parent %d close pd[0]\n", pid);
		close(pd[0]);

		while(count++ < 10) {
			printf("Parent %d writing to pipe: %s\n", pid, s);
			n = write(pd[1], s, strlen(s));
			kswitch();
		}
		printf("Parent %d exiting\n", pid);
		exit(1);
	}
	else {
		pid = getpid();
		printf("Child %d close pd[1]\n", pid);
		close(pd[1]);

		while(count++ < 10) {
			printf("Child %d reading from pipe\n", pid);
			if ( (n = read(pd[0], line, 256)) == 0 ) exit(2);
			line[n] = 0;
			printf("%d read %d from pipe: %s\n", pid, n, line);
			getc();
		}

	}



	return 0;
}
