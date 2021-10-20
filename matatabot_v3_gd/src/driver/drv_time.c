#include "gd32f3x0.h"
#include "drv_uart.h"
#include "drv_exit.h"
#include "drv_pwm.h"
#include "drv_motor_control.h"
#include "drv_time.h"
#include "drv_receive_buff.h"
extern float current_speed;
extern float previous_err;
extern int32_t speed_count;
void TIMER13_init(void)
{
    timer_parameter_struct timer_parameter;
 
    rcu_periph_clock_enable(RCU_TIMER13);
 
     timer_deinit(TIMER13);
    
    timer_parameter.prescaler = 83;
    timer_parameter.alignedmode = TIMER_COUNTER_EDGE;
    timer_parameter.counterdirection = TIMER_INT_UP;
    timer_parameter.period = 2000;
    timer_parameter.repetitioncounter = 0;
    timer_parameter.clockdivision = TIMER_CKDIV_DIV2;
    
    timer_init(TIMER13,&timer_parameter);
 
    timer_counter_value_config(TIMER13,0);
 
    timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);  
 
    timer_interrupt_enable(TIMER13,TIMER_INT_UP);
 
    nvic_irq_enable(TIMER13_IRQn,0,2);
 
    timer_enable(TIMER13);


}	
int32_t time=0;
void TIMER13_IRQHandler(void)
{
	 
	time++;

	if (time>=1000)
	{
	  current_speed = speed_count/11 ;

		speed_count=0;
		time=0;
	}
	timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP); 

	

	
	
}
