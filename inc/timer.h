#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

#define TIMER_MODE_INT_ON_TERMINAL_COUNT 0
#define TIMER_MODE_RATE_GENERATOR 2

void init_timer(uint8_t timer_mode, uint16_t counter_value);

#endif /*__TIMER_H__*/
