#ifndef CSW_B85K_MODULE_H_
#define CSW_B85K_MODULE_H_

#include "stm32f10x.h"

typedef struct wifi_module_uart_t {
	void (*initialize)(void);
	void (*putchar)(uint8_t c);
	void (*puts)(uint8_t* s, register uint32_t len);
	uint8_t (*getchar)(uint8_t* c);
} DevUART;

extern DevUART Dev;

#endif
