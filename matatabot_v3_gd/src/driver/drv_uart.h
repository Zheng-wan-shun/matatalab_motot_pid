#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include <stdint.h>
#include "gd32f3x0_usart.h"
#include "stdio.h"

#if 0

#define DEBUG_MSG(...)   uart_printf(UART0_PORT, __VA_ARGS__); uart_printf(UART1_PORT, __VA_ARGS__) 
#else
#define DEBUG_MSG(...)
#endif

enum{
  UART0_PORT = 0,
  UART1_PORT = 1
};

enum{
  RT_NORMAL = 0,
  RT_REVERSE,
};

void send_string(uint8_t *str, uint8_t len);

void drv_uart0_init(uint32_t baud_rate);
int fputc(int ch, FILE *f);
void drv_uart1_init(uint32_t baud_rate);
void drv_uart_write_byte(uint8_t port, uint8_t inputData);
void drv_uart_printf(uint8_t port, char *fmt,...);

#endif
