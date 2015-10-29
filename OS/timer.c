
#include "io.h"
#include "int.h"

#define LATCH_COUNT 0x00
#define SQUARE_WAVE 0x36

#define TIMER_FREQ 1193182L
#define TIMER_COUNT TIMER_FREQ/60

#define TIMER0 0x40
#define TIMER_MODE 0x43
#define TIMER_IRQ 0

int lock();
int unlock();

typedef unsigned short u16;
#include "timer.h"

int uptime;

int enable_irq(u16 irq_nr) {
	lock();
	out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr) );
}

int timer_init() {
	printf("Timer init..\n");
	out_byte(TIMER_MODE, SQUARE_WAVE);
	out_byte(TIMER0, TIMER_COUNT);		//set timer low byte
	out_byte(TIMER0, TIMER_COUNT >> 8); //set timer high byte
	enable_irq(TIMER_IRQ);
}


int tsleep(int seconds) {
	int sleepsec = uptime + seconds;
	ksleep(sleepsec);
}

