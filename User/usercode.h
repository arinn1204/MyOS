#ifndef _USERCODE_H
#define _USERCODE_H

int show_menu();
int find_cmd(char *);
int getpid();
int ps();
int chname();
int fork();
int ufork();
int exec();
int uexec();
int kswitch();
int wait();
int exit();
int invalid(char *);

int sleep(int seconds);
int usleep();


#endif