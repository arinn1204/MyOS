#ifndef _VIDEO_H_
#define _VIDEO_H_

extern int color;

int displayTime(int hour, int minute, int second);
int vid_init();
int putc(char c);
int scroll();

#endif
