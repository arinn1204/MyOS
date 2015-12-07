#include "usercode.h"
#include "print.h"
#include "pipe.h"

int syscall(int a,int b,int c,int d);

int main(int argc, char *argv[]) { 
	char name[64];
	int pid, cmd;
	int pd[2];

	while(1){

		printf("\n----------------------------------------------\n");
		#ifndef _LAB_3_
			printf("I am proc %d in U mode with ppid %d: running segment=%x\n",getpid(), getppid(), getcs());
		#else
			printf("I am proc "); getpid(); printf(" in U mode: running segment=%x\n", getcs());
		#endif

		#ifdef _SLEEPER_
			while(1) {
				printf("PID: %d PPID: %d\n", getpid(), getppid());
				sleep(5);
				return 0;
			}
		#endif

		show_menu();
		printf("Command? ");
		gets(name);
		printf("\n"); 
		if (name[0]==0) 
		continue;

		cmd = find_cmd(name);
		switch(cmd){
			case 0:		getpid();	break;
			case 1:		ps();		break;
			case 2:		chname();	break;
			case 3:		kswitch();	break;
			case 4:		wait();		break;
			case 5:		ufork();	break;
			case 6:		uexec();	break;
			case 7: 	exit();		break;
			case 8: 	pipe(pd);	break;
			case 9: 	pfd();		break;
			case 10:	uclose();	break;
			case 11:	uread();	break;
			case 12:	uwrite();	break;
			case 13:	usleep();	break;
			case 14:	in();		break;
			case 15:	out();		break;
			default:invalid(name);break;
		}
	}
}
