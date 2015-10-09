
#define NULL (void *)0
#define MAXPARAM 10


int main0(char *parameters) {
	int argc = 0;
	char *argv[MAXPARAM + 1];
	char *t;

	t = strtok(parameters, " ");
	while ( t != NULL && argc < MAXPARAM) {
		strcpy(argv[argc++], t);
		t = strtok(NULL, " ");
	}

	argv[ argc ] = 0;


	main(argc, argv);
}
