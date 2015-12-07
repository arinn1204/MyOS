
#include "serial.h"
//puts string FROM serial into line
int sgets(int port, char line[]) {
	int nbytes = -1;
	nbytes = syscall(50, port, line);
	return nbytes;
}


//PUTS line INTO serial
int sputs(int port, char line[]) {
	int nbytes = -1;
	nbytes = syscall(51, port, line);
	return nbytes;
}

int sgetc(int port) {
	return syscall(52,port,0,0);
}

int sputc(int port, int c) {
	return syscall(53,port,c,0);
}
