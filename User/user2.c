#include "usercode.h"
#include "print.h"

int syscall(int a,int b,int c,int d);

int main(int argc, char *argv[]) { 
	char name[64]; int pid, cmd;
	int i = 0;

	printf("There are %d parameter(s): ", argc);
	while(i < argc) printf("| %s |  ", argv[i++]);

	while(1){

		printf("\n----------------------------------------------\n");
	
		printf("I AM PROC %d IN UMODE: RUNNING SEGMENT=%x\n",getpid(), getcs());
		

		show_menu();
		printf("COMMAND? ");
		gets(name); 
		printf("\n");
		if (name[0]==0) 
		continue;

		cmd = find_cmd(name);
		switch(cmd){
			case 0:	getpid();	break;
			case 1:	ps();		break;
			case 2:	chname();	break;
			case 3:	kswitch();	break;
			case 4:	wait();		break;
			case 5:	ufork();	break;
			case 6:	uexec();	break;
			case 7: exit();		break;
			default:invalid(name);break;
		}
	}
}
