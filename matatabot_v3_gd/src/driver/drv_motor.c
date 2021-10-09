#include <stdio.h>
#include "gd32f3x0.h"
#include "drv_motor.h"
#include "drv_gpio.h"
#include <stdlib.h>
#include "string.h"
#include "stdint.h"
#include "drv_pwm.h"



void motor_pin_config(void)
{
	  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_8);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);
    
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_9);
	
	
	  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3);
    
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_15);
    gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_3);
	
	
}

void motor_driver_pin_init(void)
{
	  gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_10); 
	  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_12);
	  gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    gpio_bit_write(GPIOA, GPIO_PIN_10, 1);
	  gpio_bit_write(GPIOA, GPIO_PIN_12, 1);
   
}

void motor_star(void)
{


	 gpio_bit_write(GPIOA, GPIO_PIN_8, 1);
	 gpio_bit_write(GPIOA, GPIO_PIN_9, 0);
	 gpio_bit_write(GPIOA, GPIO_PIN_15, 1);
	 gpio_bit_write(GPIOB, GPIO_PIN_3, 0);
	
	
}



void motor_init(void)
{
	motor_pin_config();
	motor_driver_pin_init();

 // motor_star();
}



