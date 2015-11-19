

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
