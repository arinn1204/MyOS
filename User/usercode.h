#ifndef _USERCODE_H
#define _USERCODE_H

int show_menu();
int find_cmd(char *);
int getpid();
int ps();
int chname();
int kfork();
int kswitch();
int wait();
int exit();
int invalid(char *);
int gets(char str[]);
int printf(char fmt[], ...);
int getc();

#endif