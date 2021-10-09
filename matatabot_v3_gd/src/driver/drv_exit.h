#ifndef __DRV_EXIT_H__
#define __DRV_EXIT_H__

#include "stdint.h"














void int_config(void);
void encode_gpio_init(void);
int get_encode_gpio_signal(uint16_t GPIO_PIN);
void EXTI4_15_IRQHandler(void);















#endif




