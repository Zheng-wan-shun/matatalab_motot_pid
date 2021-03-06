#include "gd32f3x0.h"
#include "drv_uart.h"
#include "drv_exit.h"
#include "drv_gpio.h"
#include "gd32f3x0_exti.h"
#include "drv_pwm.h"
#include "drv_motor.h"
#include "drv_motor_control.h"
#include "gd32f3x0_rcu.h"
#include "drv_receive_buff.h"
extern uint32_t speed_count;
extern uint32_t current_speed;
void encode_gpio_init(void)
{

   gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_4);
	 gpio_mode_set(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_5);
	 gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
	 gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_7);

}	

void int_config(void)
{
	 rcu_periph_clock_enable(RCU_CFGCMP);
	 rcu_periph_clock_enable(RCU_GPIOA);
   rcu_periph_clock_enable(RCU_GPIOB);
	 nvic_irq_enable(EXTI4_15_IRQn, 2U, 0U);
	
 
    /* enable and set key temper EXTI interrupt to the lower priority */
   

    /* connect key temper EXTI line to key GPIO pin */
  syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN4);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOB, EXTI_SOURCE_PIN5);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN6);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN7);
	
	
	

    /* configure key temper EXTI line */
    exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_4);
	  exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_5);
	  exti_init(EXTI_6, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_6);
	  exti_init(EXTI_7, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_7);
     

}	

void EXTI4_15_IRQHandler()
{
	
		if( 1 == exti_interrupt_flag_get(EXTI_4))
		{
       exti_interrupt_flag_clear(EXTI_4);
		   speed_count=speed_count+1;
			
     
		
		}
    
		if( 1== exti_interrupt_flag_get(EXTI_5))
		{  exti_interrupt_flag_clear(EXTI_5);
       
//        speed_count=speed_count+1;
//			printf("%d\t\n",speed_count);
		}
     
		if( 1== exti_interrupt_flag_get(EXTI_6))
		{  exti_interrupt_flag_clear(EXTI_6);
//      
//           speed_count=speed_count+1;
//			printf("%d\t\n",speed_count);
		}
     
		if( 1== exti_interrupt_flag_get(EXTI_7))
		{ exti_interrupt_flag_clear(EXTI_7);
       
//           speed_count=speed_count+1;
//			printf("%d\t\n",speed_count);
		}

	
}


