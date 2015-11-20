
#include "serial.h"
#include "usercode.h"

int main(int argc, char *argv[]) {
	int pid = getpid();
	int ppid;
	int port = 0;
	int nbytes = -1;
	//for sending and receiving
	//parents string/buffer
	char *par 	= "This is a test";
	char parent[64];
	//child string/buffer
	char *chd = "I am sending it back";
	char child[64];

	if( fork() ) { //parent
		ppid = getppid();
		printf("I am %d and my parent is %d\n", pid, ppid);
		printf("Sending \"%s\" on port %d\n", par, port);
		nbytes = sputs(port, par);
		printf("I sent %d bytes\n", nbytes);
		wait();
	}
	else { //child
		ppid = getppid();
		printf("I am %d and my parent is %d\n", pid, ppid);
		nbytes = sgets(port, child);
		printf("I just received \"%s\" from port %d\n", child, port);
	}

}
