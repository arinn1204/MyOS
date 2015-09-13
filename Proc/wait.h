///@file wait.h
///@brief


#ifndef _WAIT_H
#define _WAIT_H

int ksleep(int event);
int kwakeup(int event);
int ready(PROC *p);
int kexit(int exitValue);
int kwait(int *status);


#endif //_WAIT_H