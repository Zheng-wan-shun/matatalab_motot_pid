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

void timer0_config(float pwm_value1,float pwm_value2);
void timer1_config(float pwm_value3,float pwm_value4);
















#endif


