#include "drv_systime.h"
#include "gd32f3x0.h"
#include "gd32f3x0_timer.h"
#include "drv_exit.h"
#include "drv_pwm.h"
#include "drv_motor.h"
#include "drv_time.h"

volatile unsigned long system_time = 0;
extern float current_speed;
extern int32_t speed_count;
unsigned long drv_millis(void)
{
    return system_time;
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
    while((drv_millis() - start_time) <= ms) 
    {
        //do nothing
    }
}

void drv_systime_init(void)
{
    system_time = 0;
    SysTick_Config(SystemCoreClock / 1000);
	  NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

extern int32_t arr[];
extern int16_t j;
void SysTick_Handler(void)
{

    system_time++;
//	if(system_time>=1000)
//	{
//    current_speed = speed_count/11 ;
//		j++;
//		arr[j] = current_speed;
//			
//			if(j>=100)
//			{
//				j=0;
//				
//			}
//		
//    system_time=0;
//		speed_count=0;
//	}
}
