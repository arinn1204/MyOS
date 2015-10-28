#ifndef _TIMER_H_
#define _TIMER_H_


u16 tick = 0;

int enable_irq(u16 irq_nr);
int timer_init();

#endif