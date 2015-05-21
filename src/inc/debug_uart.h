#ifndef DEBUG_USART_H_
#define DEBUG_USART_H_

#include "stm32f10x.h"

typedef struct debug_out_t{
	void (*initialize)(void);
	void (*putchar)(char c);
	void (*puts)(char* s);
	void (*puthex)(void* data,register uint32_t len);
}Debug;

extern const char* ascii_table;
extern Debug Output;

#endif
