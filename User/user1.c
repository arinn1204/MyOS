#include "usercode.h"
#include "print.h"

int syscall(int a,int b,int c,int d);

int main(int argc, char *argv[]) { 
	char name[64]; int pid, cmd;

	while(1){

		printf("\n----------------------------------------------\n");
		#ifndef _LAB_3_
			printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
		#else
			printf("I am proc "); getpid(); printf(" in U mode: running segment=%x\n", getcs());
		#endif

		show_menu();
		printf("Command? ");
		gets(name);
		printf("\n"); 
		if (name[0]==0) 
		continue;

		cmd = find_cmd(name);
		switch(cmd){
			case 0:	getpid();	break;
			case 1:	ps();		break;
			case 2:	chname();	break;
			case 3:	kfork();	break;
			case 4:	kswitch();	break;
			case 5:	wait();		break;
			case 6:	ufork();	break;
			case 7:	uexec();	break;
			case 8: exit();		break;
			default:invalid(name);break;
		}
	}
}
