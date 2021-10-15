#ifndef __DRV_SYSTIME_H__
#define __DRV_SYSTIME_H__

#include <stdint.h>

#define SYSTEM_CLK_M      72

unsigned long drv_millis(void);
void drv_delay_us(uint32_t us);
void drv_delay_ms(unsigned long ms);
void drv_systime_init(void);
void set_get_speed_time(unsigned long ms);
unsigned long mill(void);
void drv_speedtime_init(void);




#endif //__SYSTIME_H__
