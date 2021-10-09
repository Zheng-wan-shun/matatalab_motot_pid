#ifndef __DRV_MOTOR_CONTROL_H__
#define __DRV_MOTOR_CONTROL_H__
#include <stdlib.h>
#include "string.h"
#include "stdint.h"




void motor_speed_pid(float set_speed);

void motor_pwm_set(int32_t pwm);












#endif




