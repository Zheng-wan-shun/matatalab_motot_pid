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
#include <stdio.h>
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
#include "drv_time.h"
#include "drv_command.h"
float current_speed;
float err =0;
int32_t last_count;
int32_t count_err;
float last_err =0;
float previous_err = 0; 
int32_t speed_count;
float kp = 3.85;
float ki = 0.05;
float kd = 0.52;
float out_value = 0.0; 
float current_pwm=0.0;
int32_t count;
int32_t num[50];
int32_t i=0;
int32_t arr[100];
int16_t j;
float accrelarion;
int32_t Data[8]={0};
int32_t ring_buff[8]={0};
extern int32_t a;
int main(void)
{

    drv_systime_init();
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
//    EXTI4_15_IRQHandler();
	  int_config();
	  TIMER13_init();
	//  nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	 // drv_uart0_init(115200);
//    nvic_irq_enable(USART0_IRQn, 0, 1);
	 // nvic_irq_enable(USART0_IRQn, 0, 0);
//	  usart_interrupt_enable(USART0, USART_INT_TBE);
//	  nvic_irq_enable(USART0_IRQn, 1, 1);
//	  usart_interrupt_enable(USART0, USART_INT_TBE);
    motor_init();
 
	 			
    while(1)
    {
			timer0_config(20,0);
		timer1_config(20,0);
	   // drv_uart_tx_rx();
//	  while(RESET == usart_flag_get(USART0, USART_FLAG_TC)); 
//    usart_interrupt_enable(USART0, USART_INT_RBNE);
		 //	motor_go(motor_forward,50.0);
//      motor_speed_pid(motor_forward,32);
//			printf("current_speed:%f\t\n",(current_speed));
			//printf ("%d\t\n",data);
//			printf("current_pwm:%f\t\n",current_pwm);
//			printf("err:%f\t\n",err); 
//			printf("%d\t\n",(int)(speed_count));
//      printf("current_pwm:%f\t\n",current_pwm);
		// motor_go(motor_forward,current_pwm);
//		 motor_direction();
//			printf("%d\t\n",count);
	   // drv_delay_ms(1000);

    }
}







