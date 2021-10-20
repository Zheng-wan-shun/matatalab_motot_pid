#include "drv_systime.h"
#include "gd32f3x0.h"
#include "gd32f3x0_timer.h"
#include "drv_exit.h"
#include "drv_pwm.h"
#include "drv_motor.h"
#include "drv_motor_control.h"
#include "drv_uart.h"
volatile unsigned long system_time = 0;
extern float current_speed;
long time_count;
extern int32_t speed_count;
extern volatile int32_t recevie_data;
unsigned long mill_time;
unsigned long drv_millis(void)
{
	
    return system_time;
}
unsigned long mill(void)
{
	return time_count;
	
}
void set_get_speed_time(unsigned long ms)
{
	unsigned long start_time = mill();
    mill_time = ms;
	
		if(start_time>=ms)
	{
		current_speed = speed_count/11 ;
		
	  time_count=0;
		speed_count=0;
		//motor_speed_pid(recevie_data);
		printf("recevie_data:%d",recevie_data);
	}
	
}


void drv_delay_us(uint32_t us)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
	
    reload = SysTick->LOAD;
    ticks = us * (SystemCoreClock / 1000000);
    tcnt = 0;
    told = SysTick->VAL;
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if(tcnt >= ticks)
            {
                break;
            }
        }
    }  
}

void drv_delay_ms(unsigned long ms)
{
    unsigned long start_time = drv_millis();
	  //sys_time=ms;
    while((drv_millis() - start_time) <= ms) 
    {
        //do nothing
    }
}

void drv_systime_init(void)
{
    system_time = 0;
    SysTick_Config(SystemCoreClock / sys_time);
	  NVIC_SetPriority(SysTick_IRQn, 0x00U);
}
void drv_speedtime_init(void)
{
    time_count = 0;
    SysTick_Config(SystemCoreClock / 2000);
	  NVIC_SetPriority(SysTick_IRQn, 0x00U);
}




void SysTick_Handler(void)
{
	  time_count++;
    system_time++;
	
//		if(system_time>=5000)
//	{
//		current_speed = speed_count/11 ;
//		
//	  system_time=0;
//		speed_count=0;
//	}

}











