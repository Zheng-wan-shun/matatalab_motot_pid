#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__
#include <stdlib.h>
#include "string.h"
#include "stdint.h"


typedef enum
{
	right_motor =0,
	left_motor =1,
	
	
}motor_right_left;

void timer0_config(int32_t pwm);
void timer1_config(int32_t pwm);
void motor_pwm(int32_t pwm);
















#endif


