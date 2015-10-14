#include "print.h"


#define NULL (void *)0
#define MAXPARAM 10


int main0(char *parameters) {
	int argc = 0, i;
	char *argv[MAXPARAM + 1];
	char *t;


	if( ! parameters ) main(0,0);

	printf("Param: %s\n", parameters); getc();
	t = strtok(parameters, " ");
	while ( t != NULL && argc < MAXPARAM) {
		strcpy(argv[argc++], t);
		t = strtok(NULL, " ");
	}

	argv[ argc ] = 0;

	printf("argc = %d\n", ++argc);
	for(i = 0; i < argc; i++) printf("Argv[%d]: %s\n", i, argv[i]);
	main(argc, argv);
}
