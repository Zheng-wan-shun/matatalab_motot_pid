#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gd32f3x0.h"
#include "gd32f3x0_timer.h"
#include "drv_gpio.h"



#include <math.h>
#include "drv_uart.h"

//#include "system_management.h"
#include "drv_step_motor.h"


// generated using https://daycounter.com/Calculators/Sine-Generator-Calculator.phtml

// uint16_t sin_data[] =
// {
//     500,506,512,518,525,531,537,543,549,555,561,567,573,579,585,592,
//     598,604,610,616,621,627,633,639,645,651,657,663,668,674,680,686,
//     691,697,703,708,714,719,725,730,736,741,746,752,757,762,767,773,
//     778,783,788,793,798,803,808,812,817,822,827,831,836,840,845,849,
//     854,858,862,866,870,875,879,883,887,890,894,898,902,905,909,912,
//     916,919,922,926,929,932,935,938,941,944,947,949,952,955,957,960,
//     962,964,966,969,971,973,975,977,978,980,982,983,985,986,988,989,
//     990,992,993,994,995,995,996,997,998,998,999,999,999,1000,1000,1000,
//     1000,1000,1000,1000,999,999,999,998,998,997,996,995,995,994,993,992,
//     990,989,988,986,985,983,982,980,978,977,975,973,971,969,966,964,
//     962,960,957,955,952,949,947,944,941,938,935,932,929,926,922,919,
//     916,912,909,905,902,898,894,890,887,883,879,875,870,866,862,858,
//     854,849,845,840,836,831,827,822,817,812,808,803,798,793,788,783,
//     778,773,767,762,757,752,746,741,736,730,725,719,714,708,703,697,
//     691,686,680,674,668,663,657,651,645,639,633,627,621,616,610,604,
//     598,592,585,579,573,567,561,555,549,543,537,531,525,518,512,506,
//     500,494,488,482,475,469,463,457,451,445,439,433,427,421,415,408,
//     402,396,390,384,379,373,367,361,355,349,343,337,332,326,320,314,
//     309,303,297,292,286,281,275,270,264,259,254,248,243,238,233,227,
//     222,217,212,207,202,197,192,188,183,178,173,169,164,160,155,151,
//     146,142,138,134,130,125,121,117,113,110,106,102,98,95,91,88,
//     84,81,78,74,71,68,65,62,59,56,53,51,48,45,43,40,
//     38,36,34,31,29,27,25,23,22,20,18,17,15,14,12,11,
//     10,8,7,6,5,5,4,3,2,2,1,1,1,0,0,0,
//     0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,8,
//     10,11,12,14,15,17,18,20,22,23,25,27,29,31,34,36,
//     38,40,43,45,48,51,53,56,59,62,65,68,71,74,78,81,
//     84,88,91,95,98,102,106,110,113,117,121,125,130,134,138,142,
//     146,151,155,160,164,169,173,178,183,188,192,197,202,207,212,217,
//     222,227,233,238,243,248,254,259,264,270,275,281,286,292,297,303,
//     309,314,320,326,332,337,343,349,355,361,367,373,379,384,390,396,
//     402,408,415,421,427,433,439,445,451,457,463,469,475,482,488,494,
// };
uint16_t sin_data[] =
{
    500,512,525,537,549,561,573,585,598,610,621,633,645,657,668,680,
    691,703,714,725,736,746,757,767,778,788,798,808,817,827,836,845,
    854,862,870,879,887,894,902,909,916,922,929,935,941,947,952,957,
    962,966,971,975,978,982,985,988,990,993,995,996,998,999,999,1000,
    1000,1000,999,999,998,996,995,993,990,988,985,982,978,975,971,966,
    962,957,952,947,941,935,929,922,916,909,902,894,887,879,870,862,
    854,845,836,827,817,808,798,788,778,767,757,746,736,725,714,703,
    691,680,668,657,645,633,621,610,598,585,573,561,549,537,525,512,
    500,488,475,463,451,439,427,415,402,390,379,367,355,343,332,320,
    309,297,286,275,264,254,243,233,222,212,202,192,183,173,164,155,
    146,138,130,121,113,106,98,91,84,78,71,65,59,53,48,43,
    38,34,29,25,22,18,15,12,10,7,5,4,2,1,1,0,
    0,0,1,1,2,4,5,7,10,12,15,18,22,25,29,34,
    38,43,48,53,59,65,71,78,84,91,98,106,113,121,130,138,
    146,155,164,173,183,192,202,212,222,233,243,254,264,275,286,297,
    309,320,332,343,355,367,379,390,402,415,427,439,451,463,475,488,
};

int32_t motion_max_speed = 700 * STEP_SUBDIVISION;
motion_data_type motion_data;



void step_moto_pin_init(void)
{
    /* motor right config  timer0 CH0 CH1(PA8 PA9)*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_8);
    
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);
    
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_8);
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_9);
    
    /* motor left config  timer1 CH0 CH1(PA0 PA1)*/
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3);
    
    gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_15);
    gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_3);

    /*motor right nsleep*/
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_11);
    gpio_bit_set(GPIOA, GPIO_PIN_11);
    /*motor left nsleep*/
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_12);
    gpio_bit_set(GPIOA, GPIO_PIN_12);
}

static void set_spwm_freq(uint32_t timer_num, uint32_t freq_hz)
{
   
    timer_autoreload_value_config(timer_num,  SystemCoreClock/freq_hz);
    if(MOTOR_LEFT_TIME == timer_num)
    {
        motion_data.motor_data[MOTOR_LEFT].current_spwm_freq = freq_hz;
    }
    else
    {
        motion_data.motor_data[MOTOR_RIGHT].current_spwm_freq = freq_hz;
    }
}


static void spwm_stop(motor_configure_type motor)
{
    if(MOTOR_LEFT == motor)
    {
        timer_disable(MOTOR_LEFT_TIME);
    }
    else
    {
       timer_disable(MOTOR_RIGHT_TIME);
    }
}

static void spwm_resume(motor_configure_type motor)
{
    if(MOTOR_LEFT == motor)
    {
       timer_enable(MOTOR_LEFT_TIME);
    }
    else
    {
       timer_enable(MOTOR_RIGHT_TIME);
    }
}




void step_moto0_spwm_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK is 1MHz

    TIMER1 channel1 duty cycle = (4000/ 16000)* 100  = 25%
    TIMER1 channel2 duty cycle = (8000/ 16000)* 100  = 50%
    TIMER1 channel3 duty cycle = (12000/ 16000)* 100 = 75%
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);

    /* TIMER1 configuration */
#ifdef GD32F330
    timer_initpara.prescaler         = 0;
#endif /* GD32F330 */
#ifdef GD32F350
    timer_initpara.prescaler         = 107;
#endif /* GD32F350 */
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = SystemCoreClock/DEFAULT_MIN_SPEED;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER0,TIMER_CH_1,&timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,timer_initpara.period>>1);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_ENABLE);

    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,timer_initpara.period>>1);
    timer_channel_output_mode_config(TIMER0,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_1,TIMER_OC_SHADOW_ENABLE);

    timer_primary_output_config(TIMER0,ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    
    timer_update_source_config(TIMER0, TIMER_UPDATE_SRC_REGULAR);
    
    /* enable the TIMER channel0/1/2 interrupt */
    timer_interrupt_enable(TIMER0, TIMER_INT_CH0);
    
    /* auto-reload preload enable */
    timer_enable(TIMER0);
    
    nvic_irq_enable(TIMER0_Channel_IRQn, 0,0);

}



void step_moto1_spwm_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK is 1MHz

    TIMER1 channel1 duty cycle = (4000/ 16000)* 100  = 25%
    TIMER1 channel2 duty cycle = (8000/ 16000)* 100  = 50%
    TIMER1 channel3 duty cycle = (12000/ 16000)* 100 = 75%
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
#ifdef GD32F330
    timer_initpara.prescaler         = 0;
#endif /* GD32F330 */
#ifdef GD32F350
    timer_initpara.prescaler         = 107;
#endif /* GD32F350 */
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = SystemCoreClock/DEFAULT_MIN_SPEED;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
    
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,timer_initpara.period>>1);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_ENABLE);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,timer_initpara.period>>1);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    
    timer_update_source_config(TIMER1, TIMER_UPDATE_SRC_REGULAR);
    timer_channel_output_fast_config(TIMER1, TIMER_INT_CH0, TIMER_OC_FAST_ENABLE);
    
    /* enable the TIMER channel0/1/2 interrupt */
    timer_interrupt_enable(TIMER1, TIMER_INT_CH0);
    /* auto-reload preload enable */
    timer_enable(TIMER1);
    
    nvic_irq_enable(TIMER1_IRQn, 0,0);
}

void step_motor_spwm_init(void)
{
    step_moto0_spwm_config();
    step_moto1_spwm_config();

}



static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static long distance_to_go(motor_configure_type motor)
{
    if(motor < MOTOR_MAX)
    {
        return motion_data.motor_data[motor].target_pos - motion_data.motor_data[motor].current_pos;
    }
    else
    {
        return 0;
    }
}

void step_start(motor_configure_type motor)
{
    int16_t table_pos_ina;
    int16_t table_pos_inb;
    volatile uint32_t left_freq;
    uint16_t timer_ch0;
    uint16_t timer_ch1;
    uint32_t timer_periph;
    uint32_t timer_int_ch;

    step_sleep(motor, false);
    spwm_resume(motor);
    
    if(motor == MOTOR_LEFT)
    {
        timer_periph = TIMER1;
        timer_ch0 = TIMER_CH_0;
        timer_ch1 = TIMER_CH_1;
        timer_int_ch = TIMER_INT_CH0;
        /*if start from stop,set minimal pwm*/
        if(motion_data.motor_data[motor]._n == 0)
        {
            set_spwm_freq(MOTOR_LEFT_TIME, DEFAULT_MIN_SPEED);
        }
    }
    else
    {
        timer_periph = TIMER0;
        timer_ch0 = TIMER_CH_0;
        timer_ch1 = TIMER_CH_1;
        timer_int_ch = TIMER_INT_CH0;
        if(motion_data.motor_data[motor]._n == 0)
        {
            set_spwm_freq(MOTOR_RIGHT_TIME, DEFAULT_MIN_SPEED);
        }
    }

    table_pos_ina = motion_data.motor_data[motor].step_pos % MICRO_STEP;
    // update duty, shift the duty 4 bits to the left due to ESP32 register format
    
    left_freq = motion_data.motor_data[motor].current_spwm_freq;
    timer_channel_output_pulse_value_config(timer_periph, timer_ch0, map(sin_data[2 * table_pos_ina], 0, 1000,SystemCoreClock/10/left_freq, SystemCoreClock*9/10/left_freq));

    if(table_pos_ina < (MICRO_STEP >> 2) * 3)
    {
        table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
    }
    else
    {
        table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
    }
    timer_channel_output_pulse_value_config(timer_periph, timer_ch1, map(sin_data[table_pos_inb], 0, 1000, SystemCoreClock/10/left_freq, SystemCoreClock*9/10/left_freq));

    timer_enable(timer_periph);
    /* enable the TIMER channel0/1/2 interrupt */
    timer_interrupt_enable(timer_periph, timer_int_ch);
}


static void motor_move_to_cfg(motor_configure_type motor, long absolute)
{
    motion_data.motor_data[motor].motion_status = POS_MOVE;
    if(motion_data.motor_data[motor].target_pos != absolute)
    {
        motion_data.motor_data[motor].target_pos = absolute;
    }
    if(motion_data.motor_data[motor].target_pos != motion_data.motor_data[motor].current_pos)
    {
        compute_new_speed(motor);
        step_start(motor);
    }
}


static void compute_motor_new_speed(motor_configure_type motor)
{
    long motor_distance_to_go = distance_to_go(motor);
    // long motor_steps_to_stop = (long)(pow(motion_data.motor_data[motor].speed, 2)) / (2 * motion_data.motor_data[motor].acceleration);
    long motor_steps_to_stop = (long)((motion_data.motor_data[motor].speed * motion_data.motor_data[motor].speed)) / \
                                           (2 * motion_data.motor_data[motor].acceleration);

    if((motion_data.motor_data[motor].motion_status != STOP_MOVE) && \
       (abs(motor_distance_to_go) < 1) && \
       (motor_steps_to_stop <= (DEFAULT_MIN_SPEED * DEFAULT_MIN_SPEED / (2 * motion_data.motor_data[motor].acceleration))))
    {
        // We are at the target and its time to stop
        motion_data.motor_data[motor].speed = 0;
        motion_data.motor_data[motor]._n = 0;
        spwm_stop(motor);
        step_sleep(motor, true);
        motion_data.motor_data[motor].motion_status = STOP_MOVE;
        return;
    }
    if(motor_distance_to_go > 0)
    {
        // We are anticlockwise from the target
        // Need to go clockwise from here, maybe decelerate now
        if(motion_data.motor_data[motor]._n > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if((motor_steps_to_stop >= motor_distance_to_go) || motion_data.motor_data[motor].dir == DIR_CCW)
            {
                motion_data.motor_data[motor]._n = -motor_steps_to_stop;              // Start deceleration
            }
        }
        else if(motion_data.motor_data[motor]._n < 0)
        {
            // Currently decelerating, need to accel again?
            if((motor_steps_to_stop < motor_distance_to_go) && motion_data.motor_data[motor].dir == DIR_CW)
            {
                motion_data.motor_data[motor]._n = -motion_data.motor_data[motor]._n; // Start accceleration
            }
        }

    }
    else if(motor_distance_to_go < 0)
    {
        // We are clockwise from the target
        // Need to go anticlockwise from here, maybe decelerate
        if(motion_data.motor_data[motor]._n > 0)
        {
            // Currently accelerating, need to decel now? Or maybe going the wrong way?
            if((motor_steps_to_stop >= -motor_distance_to_go) || motion_data.motor_data[motor].dir  == DIR_CW)
            {
                motion_data.motor_data[motor]._n  = -motor_steps_to_stop;             // Start deceleration
            }
        }
        else if(motion_data.motor_data[motor]._n < 0)
        {
            // Currently decelerating, need to accel again?
            if((motor_steps_to_stop < -motor_distance_to_go) && motion_data.motor_data[motor].dir == DIR_CCW)
            {
                motion_data.motor_data[motor]._n = -motion_data.motor_data[motor]._n; // Start accceleration
            }
        }
    }
    // Need to accelerate or decelerate
    if(motion_data.motor_data[motor]._n == 0)
    { 
        // First step from stopped
        motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._c0;
        motion_data.motor_data[motor].dir = (motor_distance_to_go > 0) ? DIR_CW : DIR_CCW;
    }
    else
    {
        // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
        int32_t temp_ve;
        // temp_ve = 2 * motion_data.motor_data[MOTOR_LEFT]._cn / ((4 * motion_data.motor_data[MOTOR_LEFT]._n) + 1);
        if(motion_data.motor_data[motor]._n > 0)
        {
            temp_ve = max((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), 1);
        }
        else
        {
            temp_ve = min((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), -1);
        }
        motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._cn - temp_ve;    //Equation 13
        motion_data.motor_data[motor]._cn = max(motion_data.motor_data[motor]._cn, motion_data.motor_data[motor]._cmin);
    }
    motion_data.motor_data[motor]._n++ ;
    motion_data.motor_data[motor].speed = 1000000000 / motion_data.motor_data[motor]._cn;
    if(motion_data.motor_data[motor].dir == DIR_CCW)
    {
        motion_data.motor_data[motor].speed = -motion_data.motor_data[motor].speed;
    }
}

static void compute_motor_new_speed_by_speed(motor_configure_type motor)
{
    if(motion_data.motor_data[motor]._n == 0)
    { 
        // First step from stopped
        motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._c0;
    }
    else
    {
        // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
        int32_t temp_ve;
        if(abs(motion_data.motor_data[motor].speed) < abs(motion_data.motor_data[motor].target_speed))
        {
            temp_ve = max((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), 1);
            motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._cn - temp_ve;      //Equation 13
            motion_data.motor_data[motor]._cn = max(motion_data.motor_data[motor]._cn, (1000000000 / abs(motion_data.motor_data[motor].target_speed)));
        }
        else
        {
            temp_ve = min((2 * motion_data.motor_data[motor]._cn) / ((4 * motion_data.motor_data[motor]._n) + 1), -1);
            motion_data.motor_data[motor]._cn = motion_data.motor_data[motor]._cn - temp_ve;      //Equation 13
            motion_data.motor_data[motor]._cn = min(motion_data.motor_data[motor]._cn, (1000000000 / abs(motion_data.motor_data[motor].target_speed)));
        }
    }
    motion_data.motor_data[motor].speed = 1000000000 / motion_data.motor_data[motor]._cn;
    if(motion_data.motor_data[motor].dir == DIR_CCW)
    {
        motion_data.motor_data[motor].speed = -motion_data.motor_data[motor].speed;
    }
    if(motion_data.motor_data[motor].target_speed != motion_data.motor_data[motor].speed)
    {
        motion_data.motor_data[motor]._n++;
    }
}


void step_motor_init(void)
{
    motion_data.motor_data[MOTOR_LEFT].step_pos = 0;
    motion_data.motor_data[MOTOR_RIGHT].step_pos = 0;
    motion_data.motor_data[MOTOR_LEFT].dir = DIR_CW;
    motion_data.motor_data[MOTOR_RIGHT].dir = DIR_CW;
    motion_data.motor_data[MOTOR_LEFT]._n = 0;
    motion_data.motor_data[MOTOR_RIGHT]._n = 0;
    
    step_moto_pin_init();
    step_sleep(MOTOR_LEFT, true);
    step_sleep(MOTOR_RIGHT, true);
    step_motor_spwm_init();

}

void step_sleep(motor_configure_type motor, uint8_t enable)
{
    if(MOTOR_LEFT == motor)
    {
        drv_gpio_digital_write(MOTOR_LEFT_SLEEP_PIN, !enable);
    }
    else if(MOTOR_RIGHT == motor)
    {
        drv_gpio_digital_write(MOTOR_RIGHT_SLEEP_PIN, !enable);
    }
}

void compute_new_speed(motor_configure_type motor)
{
    if(motion_data.motor_data[motor].motion_status == SPEED_MOVE)
    {
        compute_motor_new_speed_by_speed(motor);
    }
    else
    {
        compute_motor_new_speed(motor);
    }
}

void set_max_speed(motor_configure_type motor, int32_t speed)
{
    int32_t speed_temp;
    if(speed < 0)
    {
        speed_temp = -speed;
    }
    else
    {
        speed_temp = speed;
    }
    if(motion_data.motor_data[motor].max_speed != speed_temp)
    {
        motion_data.motor_data[motor].max_speed = speed_temp;
        motion_data.motor_data[motor]._cmin = 1000000000 / speed_temp;
        // Recompute _n from current speed and adjust speed if accelerating or cruising
        if(motion_data.motor_data[motor]._n > 0)
        {
            motion_data.motor_data[motor]._n  = (int32_t)round((motion_data.motor_data[motor].speed * motion_data.motor_data[motor].speed) / \
                                                      (2.0 * motion_data.motor_data[motor].acceleration)); // Equation 16
            compute_new_speed(motor);
        }
    }
}

void set_acceleration(motor_configure_type motor, int32_t acceleration)
{
    int32_t acceleration_temp;
    if(acceleration == 0)
    {
        return;
    }
    if(acceleration < 0)
    {
        acceleration_temp = -acceleration;
    }
    else
    {
        acceleration_temp = acceleration;
    }
    if(motion_data.motor_data[motor].acceleration != acceleration_temp)
    {
        // Recompute _n per Equation 17
        motion_data.motor_data[motor]._n = (motion_data.motor_data[motor]._n * (motion_data.motor_data[motor].acceleration  / acceleration_temp));
        // New c0 per Equation 7, with correction per Equation 15
        motion_data.motor_data[motor]._c0 = (int32_t)round(0.676 * sqrt(2.0 / acceleration_temp) * 1000000000.0); // Equation 15
        motion_data.motor_data[motor].acceleration = acceleration_temp;
        compute_new_speed(motor);
    }
}

long get_target_position(motor_configure_type motor)
{
    if((motor < MOTOR_MAX))
    {
        return motion_data.motor_data[motor].target_pos;
    }
    else
    {
        return 0;
    }
}

long get_current_position(motor_configure_type motor)
{
    if((motor < MOTOR_MAX))
    {
        return motion_data.motor_data[motor].current_pos;
    }
    else
    {
        return 0;
    }
}

void set_current_position(motor_configure_type motor, long position)
{
    if((motor < MOTOR_MAX))
    {
        motion_data.motor_data[motor].target_pos = position;
        motion_data.motor_data[motor].current_pos = position;
        motion_data.motor_data[motor]._n = 0;
        motion_data.motor_data[motor].speed = 0;
    }
}

void motor_move_to(motor_configure_type motor, long absolute, uint8_t sync)
{
    if(motion_data.motor_data[motor].current_pos == absolute)
    {
        motor_stop(motor);
        return;
    }
    set_max_speed(motor, motion_max_speed);
    set_acceleration(motor, 4000 * STEP_SUBDIVISION);
    motor_move_to_cfg(motor, absolute);

    timer_enable(TIMER0);
    
    timer_enable(TIMER1);
}

void motor_move(motor_configure_type motor, long relative, uint8_t sync)
{
    long absolute_position;
    relative = relative * -1;
    set_current_position(motor, 0);
    absolute_position = motion_data.motor_data[motor].current_pos + relative;
    motor_move_to(motor, absolute_position, sync);
}

void motor_set_move_speed_max(int32_t max_speed)
{
    motion_max_speed = max_speed;
    return;
}

void motor_set_speed(motor_configure_type motor, int32_t run_speed)
{
    int32_t need_n = 0;
    set_max_speed(motor, 1200 * STEP_SUBDIVISION);
    set_acceleration(motor, 4000 * STEP_SUBDIVISION); 
    if(run_speed == 0)
    {
        motor_stop(motor);
    }
    else
    {   
        if((motion_data.motor_data[motor].speed / run_speed) < 0)
        {
            motor_stop(motor);
        }
        motion_data.motor_data[motor].target_speed = run_speed;
        motion_data.motor_data[motor].dir = (motion_data.motor_data[motor].target_speed > 0) ? DIR_CW : DIR_CCW;
        motion_data.motor_data[motor].motion_status = SPEED_MOVE;
        need_n = ((motion_data.motor_data[motor].target_speed *  motion_data.motor_data[motor].target_speed) \
                                    - (motion_data.motor_data[motor].speed * motion_data.motor_data[motor].speed))  \
                                    / (2 * motion_data.motor_data[motor].acceleration);
        if(motion_data.motor_data[motor].speed == 0)
        {
            motion_data.motor_data[motor]._n = 0;

        }
        else
        {
            if(need_n == 0)
            {
                motion_data.motor_data[motor]._n = 1;
            }
            if(need_n < 0)
            {
                motion_data.motor_data[motor]._n = need_n;
            }
        }
        //drv_uart_printf(UART0_PORT, "target_speed %d,  _n: %d, speed:%d, need_n:%d \r\n", motion_data.motor_data[motor].target_speed, \
        motion_data.motor_data[motor]._n, motion_data.motor_data[motor].speed, need_n);
        step_start(motor);
    }
}

int32_t motor_get_speed(motor_configure_type motor)
{
    return motion_data.motor_data[motor].speed;
}

void motor_run_speed(int32_t left_run_speed, int32_t right_run_speed)
{
    motor_set_speed(MOTOR_LEFT, left_run_speed);
    motor_set_speed(MOTOR_RIGHT, right_run_speed);
}

void motor_stop(motor_configure_type motor)
{
    
    motion_data.motor_data[motor].speed = 0;
    motion_data.motor_data[motor]._n = 0;
    motion_data.motor_data[motor].target_pos = motion_data.motor_data[motor].current_pos;
    motion_data.motor_data[motor].motion_status = STOP_MOVE;
    spwm_stop(motor);
    step_sleep(motor, true);
    
}

uint8_t motor_get_status(motor_configure_type motor)
{
    return motion_data.motor_data[motor].motion_status;
}

void motor_run(void)
{
    static long previous_motor_left_pos = 0;
    static long previous_motor_right_pos = 0;
    if(motion_data.motor_data[MOTOR_LEFT].motion_status != STOP_MOVE)
    {
        if(abs(previous_motor_left_pos - motion_data.motor_data[MOTOR_LEFT].current_pos) >= 64)
        {
            volatile uint32_t left_freq = abs(motion_data.motor_data[MOTOR_LEFT].speed);
            if(left_freq < DEFAULT_MIN_SPEED)
            {
                left_freq = DEFAULT_MIN_SPEED;
            }
            else if(left_freq > DEFALUT_MAX_SPEED)
            {
                left_freq = DEFALUT_MAX_SPEED;
            }
            if(motion_data.motor_data[MOTOR_LEFT].current_spwm_freq != left_freq)
            {
                //drv_uart_printf(UART0_PORT, "left_freq:%d ", left_freq);
                set_spwm_freq(MOTOR_LEFT_TIME, left_freq);
            }
            //drv_uart_printf(UART0_PORT, "cn: %d. target: %d left_freq:%d _n: %d\r\n", motion_data.motor_data[MOTOR_LEFT]._cn, \
            //motion_data.motor_data[MOTOR_LEFT].target_speed,  motion_data.motor_data[MOTOR_LEFT].speed, motion_data.motor_data[MOTOR_LEFT]._n);
            previous_motor_left_pos = motion_data.motor_data[MOTOR_LEFT].current_pos;
        }
    }

    if(motion_data.motor_data[MOTOR_RIGHT].motion_status != STOP_MOVE)
    {
        if(abs(previous_motor_right_pos - motion_data.motor_data[MOTOR_RIGHT].current_pos) >= 64)
        {
            volatile uint32_t right_freq = abs(motion_data.motor_data[MOTOR_RIGHT].speed);
            if(right_freq < DEFAULT_MIN_SPEED)
            {
                right_freq = DEFAULT_MIN_SPEED;
            }
            else if(right_freq > DEFALUT_MAX_SPEED)
            {
                right_freq = DEFALUT_MAX_SPEED;
            }
            if(motion_data.motor_data[MOTOR_RIGHT].current_spwm_freq != right_freq)
            {
                set_spwm_freq(MOTOR_RIGHT_TIME, right_freq);
            }
            previous_motor_right_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos;
        }
    }
}

void TIMER0_Channel_IRQHandler(void)
{
    int16_t table_pos_ina;
    int16_t table_pos_inb;
    
    uint32_t pwm_pulse = 0;
    volatile uint32_t right_freq;

    if(SET == timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_CH0))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_CH0);
        if(motion_data.motor_data[MOTOR_LEFT].dir == DIR_CW)
        {
            motion_data.motor_data[MOTOR_LEFT].step_pos = motion_data.motor_data[MOTOR_LEFT].step_pos + 1;
            motion_data.motor_data[MOTOR_LEFT].current_pos = motion_data.motor_data[MOTOR_LEFT].current_pos + 1;
            if(motion_data.motor_data[MOTOR_LEFT].step_pos >= STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_LEFT].step_pos = 0;
            }
        }
        else
        {
            motion_data.motor_data[MOTOR_LEFT].step_pos = motion_data.motor_data[MOTOR_LEFT].step_pos - 1;
            motion_data.motor_data[MOTOR_LEFT].current_pos = motion_data.motor_data[MOTOR_LEFT].current_pos - 1;
            if(motion_data.motor_data[MOTOR_LEFT].step_pos <= -STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_LEFT].step_pos = 0;
            }
        }

        compute_new_speed(MOTOR_LEFT);

        table_pos_ina = motion_data.motor_data[MOTOR_LEFT].step_pos % MICRO_STEP;
        if(table_pos_ina < 0)
        {
            table_pos_ina = table_pos_ina + MICRO_STEP;
        }
        right_freq = motion_data.motor_data[MOTOR_LEFT].current_spwm_freq;
        pwm_pulse = (uint32_t)map(sin_data[2 * table_pos_ina], 0, 1000, SystemCoreClock/10/right_freq, SystemCoreClock*9/10/right_freq);
        
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, pwm_pulse);
        
        if(table_pos_ina < (MICRO_STEP >> 2) * 3)
        {
            table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
        }
        else
        {
            table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
        }

        
        pwm_pulse = (uint32_t)map(sin_data[table_pos_inb], 0, 1000, SystemCoreClock/10/right_freq, SystemCoreClock*9/10/right_freq);
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, pwm_pulse);
        
    }
    else if(SET == timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_CH1))
    {
        /* clear channel 1 interrupt bit */
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_CH1);
    }
    else if(SET == timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_CH2))
    {
        /* clear channel 1 interrupt bit */
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_CH2);
    }
    else if(SET == timer_interrupt_flag_get(TIMER0, TIMER_INT_FLAG_CH3))
    {
        /* clear channel 1 interrupt bit */
        timer_interrupt_flag_clear(TIMER0, TIMER_INT_FLAG_CH3);
    }
}



void TIMER1_IRQHandler(void)
{
    int16_t table_pos_ina;
    int16_t table_pos_inb;
    
    uint32_t pwm_pulse = 0;
    volatile uint32_t left_freq;
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_CH0))
    {
        /* clear channel 0 interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH0);
        if(motion_data.motor_data[MOTOR_RIGHT].dir == DIR_CW)
        {
            motion_data.motor_data[MOTOR_RIGHT].step_pos = motion_data.motor_data[MOTOR_RIGHT].step_pos + 1;
            motion_data.motor_data[MOTOR_RIGHT].current_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos + 1;
            if(motion_data.motor_data[MOTOR_RIGHT].step_pos >= STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_RIGHT].step_pos = 0;
            }
        }
        else
        {
            motion_data.motor_data[MOTOR_RIGHT].step_pos = motion_data.motor_data[MOTOR_RIGHT].step_pos - 1;
            motion_data.motor_data[MOTOR_RIGHT].current_pos = motion_data.motor_data[MOTOR_RIGHT].current_pos - 1;
            if(motion_data.motor_data[MOTOR_RIGHT].step_pos <= -STEP_POS_MAX)
            {
                motion_data.motor_data[MOTOR_RIGHT].step_pos = 0;
            }
        }

        compute_new_speed(MOTOR_RIGHT);

        table_pos_ina = motion_data.motor_data[MOTOR_RIGHT].step_pos % MICRO_STEP;
        if(table_pos_ina < 0)
        {
            table_pos_ina = table_pos_ina + MICRO_STEP;
        }
        left_freq = motion_data.motor_data[MOTOR_RIGHT].current_spwm_freq;
        pwm_pulse = (uint32_t)map(sin_data[2 * table_pos_ina], 0, 1000, SystemCoreClock/10/left_freq, SystemCoreClock*9/10/left_freq);

        // update duty, shift the duty 4 bits to the left due to ESP32 register format
        timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, pwm_pulse);
        
        if(table_pos_ina < (MICRO_STEP >> 2) * 3)
        {
            table_pos_inb = 2 * (table_pos_ina + (MICRO_STEP >> 2));
        }
        else
        {
            table_pos_inb = 2 * (table_pos_ina - (MICRO_STEP >> 2) * 3);
        }

        pwm_pulse = (uint32_t)map(sin_data[table_pos_inb], 0, 1000, SystemCoreClock/10/left_freq, SystemCoreClock*9/10/left_freq);
        timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, pwm_pulse);

    }else if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_CH1))
    {
        /* clear channel 1 interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH1);

        
    }else if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_CH2))
    {
        /* clear channel 2 interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH2);
    }else if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_CH3))
    {
        /* clear channel 3 interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH3);
    }
    
}

void step_motor_config(void)
{
    step_motor_init();
    set_max_speed(MOTOR_LEFT, 1000 * STEP_SUBDIVISION);
    set_max_speed(MOTOR_RIGHT, 1000 * STEP_SUBDIVISION);
    set_acceleration(MOTOR_LEFT, DEFALUT_ACCELERATION);
    set_acceleration(MOTOR_RIGHT, DEFALUT_ACCELERATION);
    set_current_position(MOTOR_LEFT, 0);
    set_current_position(MOTOR_RIGHT, 0);

}


