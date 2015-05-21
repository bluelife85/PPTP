#ifndef SYSTEM_TICK_H_
#define SYSTEM_TICK_H_

#include "stm32f10x.h"

void systick_init(void);
uint32_t get_systick(void);
uint32_t get_passed_tick(uint32_t prior);
void SysTick_Handler(void);

#endif
