#include "debug_uart.h"

const char* ascii_table = "0123456789ABCDEF";

void debug_initialize(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

void debug_putchar(char c){
	
	USART1->DR = c;
	while((USART1->SR & USART_FLAG_TXE) == 0x00);
}

void debug_puts(char* s){
	
	while(*s){
		USART1->DR = *s++;
		while((USART1->SR & USART_FLAG_TXE) == 0x00);
	}
}

void debug_putlen(uint8_t* s,register uint32_t len){
	
	while(len--){
		USART1->DR = *s++;
		while((USART1->SR & USART_FLAG_TXE) == 0x00);
	}
}

void debug_puthex(void* data,register uint32_t len){
	
	uint8_t* c = (uint8_t*)data;
	
	while(len--){
		
		debug_putchar(ascii_table[((*c) & 0xf0) >> 4]);
		debug_putchar(ascii_table[((*c) & 0x0f)]);
		if(len)
			debug_putchar(0x20);
		c++;
	}
}

const Debug Output = {
	debug_initialize,
	debug_putchar,
	debug_puts,
	debug_puthex,
	debug_putlen
};
