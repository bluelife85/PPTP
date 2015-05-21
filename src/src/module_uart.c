#include "module_uart.h"

static uint8_t uart_ring[512];
static uint32_t uart_write_index;
static uint32_t uart_read_index;

void module_uart_initialize(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	USART_Init(UART5, &USART_InitStructure);
	USART_Cmd(UART5, ENABLE);
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

void module_uart_putchar(uint8_t c){
	
	UART5->DR = c;
	while((UART5->SR & USART_FLAG_TXE) == 0x00);
}

void module_uart_puts(uint8_t* str, register uint32_t len){
	
	while(len--){
		UART5->DR = *str++;
		while((UART5->SR & USART_FLAG_TXE) == 0x00);
	}
}

uint8_t module_uart_getchar(uint8_t* c){
	
	if(uart_write_index == uart_read_index)
		return 0;
	
	*c = uart_ring[uart_read_index++];
	
	if(uart_read_index == 512)
		uart_read_index = 0;
	
	return 1;
}

void UART5_IRQHandler(void){
	
	uint8_t c;
	
	c = UART5->DR;
	
	uart_ring[uart_write_index++] = c;
	
	if(uart_write_index == 512)
		uart_write_index = 0;
}

DevUART Dev = {
	module_uart_initialize,
	module_uart_putchar,
	module_uart_puts,
	module_uart_getchar
};
