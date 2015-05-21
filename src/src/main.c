#include "system_tick.h"
#include "debug_uart.h"
#include "module_uart.h"

void pptp_get_pkt(uint8_t c);
void pptp_process(void);

int main(void){
	
	uint8_t c;
	
	Output.initialize();
	Dev.initialize();
	
	while(GPIOB->IDR & GPIO_Pin_6);
	
	while(1){
		
		if(Dev.getchar(&c)){
			pptp_get_pkt(c);
		}
		
		pptp_process();
	}
}
