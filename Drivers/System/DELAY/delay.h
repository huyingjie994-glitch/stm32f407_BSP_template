#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32f4xx.h"

void delay_init(void);
void delay_us(uint32_t _us);
void delay_ms(uint32_t _ms);

#endif
