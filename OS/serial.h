#ifndef _SERIAL_H_
#define _SERIAL_H_


typedef struct stty stty;

int sinit();
int sgetline(stty *t, char *line);
int sgets(int port, int strADDR); //this is called via syscall


int sputline(stty *t, char *line);
int sputs(int port, int strADDR); //this is called via syscall


#endif