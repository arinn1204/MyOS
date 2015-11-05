
#include "video.h"
#include "file.h"
#include "pipe.h"
#include "proc.h"
#include "io.h"

#define VDC_INDEX 		0x3D4
#define VDC_DATA 		0x3D5
#define CURSOR_SIZE 	10
#define VID_ORG			12
#define CURSOR 			14

#define LINE_WIDTH 		80
#define SCR_LINE 		24
#define SCR_BYTES 		SCR_LINE*LINE_WIDTH

#define CURSOR_SHAPE 	15

#define HGREEN 			0x0A
#define HCYAN 			0x0B
#define HRED 			0x0C
#define HPURPLE 		0x0D
#define HYELLOW 		0x0E

typedef unsigned short u16;

u16 base = 0xB800;
u16 offset = 0;
u16 vidmask = 0x3FFF;

int color = HGREEN;

int org,row,column;

void set_vdc(u16 reg, u16 val) {
	lock();
	out_byte(VDC_INDEX, reg);
	out_byte(VDC_DATA, (val >> 8) & 0xFF);
	out_byte(VDC_INDEX, reg + 1);
	out_byte(VDC_DATA, val & 0xFF);
	unlock();
}

int vid_init() {
	int i, w;
	org = column = 0;
	row = 1;
	set_vdc(CURSOR_SIZE, CURSOR_SHAPE);
	set_vdc(VID_ORG, 0);
	set_vdc(CURSOR, 0);

	w = 0x0700;
	for(i = 0; i < SCR_LINE*LINE_WIDTH; i++)	
		put_word(w, base, 2*i);

	printf("Video init complete..");
	displayTime(0,0,0);
}

//clear a specified row with blanks
void clearRow(int crow) {
	int off;
	int i;

	off = org + 2*crow*LINE_WIDTH;
	for(i = 0; i < LINE_WIDTH; i++) {
		put_word(0x0700, base,  off + 2*i);
	}
}

int scroll() {
	int i;
	u16 w, bytes;

	offset = org + SCR_BYTES + 2*LINE_WIDTH;
	if(offset <= vidmask) 
		org += 2*LINE_WIDTH;
	else { 
		for(i = 0; i < (SCR_LINE - 1)*LINE_WIDTH; i++)
			put_word(get_word(base, org+2*LINE_WIDTH+2*i), base, 2*i );
		org = 0;
	}
	offset = org + 2*(SCR_LINE - 1)*LINE_WIDTH;

	w = 0x0C00;
	for(i = 0; i < LINE_WIDTH; i++)
		put_word(w, base, offset + 2*i);
	set_vdc(VID_ORG, org >> 1);
}

int putc(char c) {
	int pos, w;
	extern PROC *running;

	switch(c) {
		case '\n':
			row++;
			if (row >= SCR_LINE) {
				row = SCR_LINE - 1;
				scroll();
			}
			pos = 2*(row*LINE_WIDTH + column);
			offset = (org + pos) & vidmask;
			set_vdc(CURSOR, offset >> 1);
			break;
		case '\r':
			column = 0;
			pos = 2*(row*LINE_WIDTH + column);
			offset = (org + pos) & vidmask;
			set_vdc(CURSOR, offset >> 1);
			break;
		case '\b':
			if (column > 0) {
				column--;
				pos = 2*(row*LINE_WIDTH + column);
				offset = (org + pos) & vidmask;
				put_word(0x0700, base, offset);
				set_vdc(CURSOR, offset >> 1);
			}
			break;
		default:
			//c is just a regular character
			pos = 2*(row*LINE_WIDTH + column);
			offset = (org + pos) & vidmask;
			w = color;
			w = (w << 8) + c;
			put_word(w, base, offset);
			column++;
			if (column >= LINE_WIDTH) {
				column = 0;
				row++;
				if (row >= SCR_LINE) {
					row = SCR_LINE - 1;
					scroll();
				}
			}
			pos = 2*(row*LINE_WIDTH + column);
			offset = (org + pos) & vidmask;
			set_vdc(CURSOR, offset >> 1);
			break;
	}
	return 0;
}

int displayTime(int hour, int minute, int second) {
	int oldrow = 	row;
	int oldcolumn = column;
	int oldorg = 	org;
	int oldoff = 	offset;
	int oldclr = 	color;
	int i, w = 0x0C00;

	color = HGREEN;
	column = LINE_WIDTH - 9;
	row = SCR_LINE;

	//clearRow(row);

	printf( (hour < 10) ? "0%d:" : "%d:", hour);
	printf( (minute < 10) ? "0%d:" : "%d:", minute);
	printf( (second < 10) ? "0%d" : "%d", second);

	column = 	oldcolumn;
	row = 		oldrow;
	org = 		oldorg;
	offset = 	oldoff;
	color = 	oldclr;
	set_vdc(CURSOR, offset >> 1);
}


