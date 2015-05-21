#include "system_tick.h"

static uint32_t systick;

void systick_init(void){
	
	SysTick_Config(72000);
}

uint32_t get_systick(void){
	
	return systick;
}

uint32_t get_passed_tick(uint32_t prior){
	
	uint32_t current;
	
	current = get_systick();
	
	if(current < prior){
		return (current - prior - 1);
	}
	
	return (current - prior);
}

void SysTick_Handler(void){
	
	systick++;
}
