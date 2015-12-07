#ifndef _SERIAL_H_
#define _SERIAL_H_

int sgets(int port, char line[]);
int sgetc(int port);
int sputs(int port, char line[]);
int sputc(int port);

#endif