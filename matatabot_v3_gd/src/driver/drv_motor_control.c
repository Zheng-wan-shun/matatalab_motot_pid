#include "gd32f3x0.h"
#include "drv_uart.h"
#include "drv_exit.h"
#include "drv_pwm.h"
#include "drv_motor_control.h"
#include "drv_time.h"
#define default_pwm 0

extern float current_speed;
extern int32_t count_err;
extern float pwm_value;
extern float err ;

extern float last_err ;
extern float previous_err; 
extern float kp;
extern float ki;
extern float kd;
extern float out_value;
extern float current_pwm;

extern int32_t count;
extern int32_t last_count;



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




extern float accrelarion;
extern int32_t arr[50];
extern int32_t j;
void motor_speed_pid(motor_transfer motor,float set_speed)
{

	
	  err = set_speed - current_speed;
	 
	  out_value = kp *  err  + (kd * ( err - 2 * last_err + previous_err ))	\
	                + ki * (err - last_err) ;	

	 last_err = err;
	 previous_err = last_err;
	 current_pwm  =  current_pwm + out_value;
	  
	 motor_go(motor,current_pwm);
	 
	accrelarion = (arr[j]-arr[j-1]);
	printf("accrelarion:%f\t\n",accrelarion);
	printf("arr[j]:%d\t\n",arr[j]);
  printf("arr[j-1]:%d\n\t",arr[j-1]);
	
}
extern int32_t i;

extern int32_t num[10];
	
void motor_direction(void)
{

		printf("%d\t\n",num[i-1]);
	  printf("%d\t\n",num[i]);
	
		if(num[i]-num[i-1]<0)
		{
			printf("fanzhuan\t\n");
			
		}
		else if(num[i]-num[i-1]>0)
		{
			
			printf("zhengzhuan\t\n");
			
		}
//		else if (current_pwm==0)
//		{
//			
//			printf("stop");
//			
//		}
	
}

//void cylinder_number(int32_t set_distance)
//{
//	float current_distance;
//	float distance_err;
//	float value;
//	float kp;
//	float ki;
//	float kd;
//	float distance_last_err,distance_privious_err;
//	distance_err = set_distance - current_distance;
//	
//	value = kp * distance_err + ki * (distance_err - distance_last_err ) \
//	+ kd *( distance_err -2 * distance_last_err + distance_privious_err);
//	
//	current_speed = current_speed + value;
//	
//	
//}






















