#include "gd32f3x0.h"
#include "drv_uart.h"
#include "drv_exit.h"
#include "drv_pwm.h"
#include "drv_motor_control.h"
#include "drv_systime.h"
#define default_pwm 0

extern float current_speed;

extern float pwm_value;
extern float err ;

extern float last_err ;
extern float previous_err; 
extern float kp;
extern float ki;
extern float kd;
extern float out_value;
extern float current_pwm;


void motor_go(motor_transfer motor,float pwm)
{
	
	if(motor == motor_forward)
	{
		
		
		if(pwm>0)
		{
		timer0_config(pwm,0);
		timer1_config(pwm,0);
		}
		else if(pwm<0)
		{
		timer0_config(-pwm,0);
		timer1_config(-pwm,0);
			
		}
		else
		{
			
		timer0_config(default_pwm,0);
		timer1_config(default_pwm,0);
			
		}
			
		
	}
  else if(motor == motor_backward)
	{
		if(pwm>0)
		{
		timer0_config(0,pwm);
		timer1_config(0,pwm);
		}
		else if(pwm<0)
		{
		timer0_config(0,-pwm);
		timer1_config(0,-pwm);
			
		}
		else
		{
			
		timer0_config(0,default_pwm);
		timer1_config(0,default_pwm);
			
		}
			
		
	}
 


}	



void motor_speed_pid(float set_speed)
{

	
	  err = set_speed - current_speed;
	  printf("err:%f\t\n",err); 
	 out_value = kp *  err  + (kd * ( err - 2 * last_err + previous_err ))	+ ki * (err - last_err) ;	
	
  //	  printf("err:%d\t\n",err); 
   current_pwm  =  current_pwm + out_value;
	
	 last_err = err;
	 previous_err = last_err;
	 printf("current_pwm:%f\t\n",current_pwm); 
	 motor_go(motor_forward,current_pwm);

	
	
}
