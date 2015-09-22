/**@file kernel.h
*@brief
*/

#ifndef _KERNEL_H
#define _KERNEL_H

int set_vec(unsigned short vector, unsigned short handler);
int do_ps();
int chname(char *);
int do_tswitch();
int do_kfork();
int do_exit();
int do_stop();
int do_continue();
int do_sleep();
int do_wakeup();
int do_wait();
int reschedule();
int chpriority(int pid, int pri);
int do_chpriority();

#endif 