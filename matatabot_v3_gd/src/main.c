/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 20/12/08 10:49a $
 * @brief    for mbuild product
 *
 * @note
 * Copyright (C) 2019 pengjs@matatalab.com. All rights reserved.
 *
 ******************************************************************************/
#include "gd32f3x0.h"
#include "gd32f3x0_usart.h"
#include "gd32f3x0_exti.h"
#include "gd32f3x0_rcu.h"
#include "gd32f3x0_gpio.h"
#include "drv_systime.h"
#include "drv_motor.h"
#include "drv_uart.h"
#include <stdio.h>
#include "drv_exit.h"
#include "drv_gpio.h"
#include "drv_pwm.h"
#include "drv_motor_control.h"
float current_speed;
float err =0;
int32_t count;
float last_err =0;
float previous_err = 0; 
int32_t speed_count;
float kp = 5.85;
float ki = 0.05;
float kd = 0.52;
float out_value = 0.0; 
float current_pwm=0;



int main(void)
{

    drv_systime_init();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
//    EXTI4_15_IRQHandler();
	  int_config();
//	  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	  drv_uart0_init(115200);
   
   motor_init();
 //  motor_pwm(100);
	 			
    while(1)
    {
    //  motor_speed_pid(32);
			printf("current_speed:%f\t\n",(current_speed));
//			printf("%d\t\n",(int)(speed_count));
//      printf("current_pwm:%f\t\n",current_pwm);
		 printf("%d",count);
//			printf("%f\t\n",value);
	    drv_delay_ms(84000);

    }
}


