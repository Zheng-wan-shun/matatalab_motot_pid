#include "drv_gpio.h"
#include "gd32f3x0.h"
#include "gd32f3x0_adc.h"
#include "drv_systime.h"

void drv_gpio_pin_mode(uint8_t pin, uint8_t mode)
{
    uint32_t port_num, pin_num;

    port_num = pin >> 4;
    pin_num = BIT(pin & 0x0f);
    /* enable the led clock */
    rcu_periph_clock_enable((rcu_periph_enum)(RCU_REGIDX_BIT(IDX_AHBEN, 17U) + port_num));
    /* configure led GPIO port */ 
    if(mode == GPIO_MODE_OUTPUT_NONE)
    {
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pin_num);
        gpio_output_options_set(GPIO_PORT_ADDRESS(port_num), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin_num);
    }
    else if(mode == GPIO_MODE_OUTPUT_PULLUP)
    {
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, pin_num);
        gpio_output_options_set(GPIO_PORT_ADDRESS(port_num), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin_num);
    }
    else if(mode == GPIO_MODE_OUTPUT_PULLDOWN)
    {
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, pin_num);
        gpio_output_options_set(GPIO_PORT_ADDRESS(port_num), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, pin_num);
    }
    else if(mode == GPIO_MODE_INPUT_NONE)
    {
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_INPUT, GPIO_PUPD_NONE, pin_num);
    }
    else if(mode == GPIO_MODE_INPUT_PULLUP)
    {
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, pin_num);
    }
    else if(mode == GPIO_MODE_INPUT_PULLDOWN)
    {
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, pin_num);
    }
}

void drv_gpio_digital_write(uint8_t pin, uint8_t val)
{
    uint32_t port_num, pin_num;

    port_num = pin >> 4;
    pin_num = BIT(pin&0x0f);
    gpio_bit_write(GPIO_PORT_ADDRESS(port_num), pin_num, (bit_status)val);
}

int drv_gpio_digital_read(uint8_t pin)
{
    uint32_t port_num, pin_num;

    port_num = pin >> 4;
    pin_num = BIT(pin & 0x0f);
    return gpio_input_bit_get(GPIO_PORT_ADDRESS(port_num), pin_num);
}	

int drv_gpio_analog_read(uint8_t pin)
{
    uint32_t port_num, pin_num;
    uint8_t analog_channel;
    int ret_value = 0;

    port_num = pin >> 4;
    pin_num = BIT(pin & 0x0f);
    if(pin < PA_8)
    {
        analog_channel = pin;  
    }
    else if((pin == PB_0) || (pin == PB_1))
    {
        analog_channel = (pin & 0x0f) + PA_8;  
    }
    else if((pin == P_TEMP)||(pin == P_REF)||(pin == P_BAT))
    {
        analog_channel = (pin & 0x0f) + 0x10;
        adc_tempsensor_vrefint_enable();
    }
    else
    {
        return -1;
    }

    if(analog_channel < 0x10)
    /* enable the led clock */
    {
        rcu_periph_clock_enable((rcu_periph_enum)(RCU_REGIDX_BIT(IDX_AHBEN, 17U) + port_num));
        gpio_mode_set(GPIO_PORT_ADDRESS(port_num), GPIO_MODE_ANALOG, GPIO_PUPD_NONE, pin_num);
    }
    rcu_periph_clock_enable(RCU_ADC);
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
    /* ADC external trigger enable */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1);
    // TODO: should specific the analog channel.
    adc_regular_channel_config(0, analog_channel, ADC_SAMPLETIME_55POINT5);
    adc_special_function_config(ADC_CONTINUOUS_MODE, DISABLE);
    adc_resolution_config(ADC_RESOLUTION_12B);
    /* enable ADC interface */
    adc_enable();
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, DISABLE);
    adc_flag_clear(ADC_FLAG_EOC);
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
    while(SET != adc_flag_get(ADC_FLAG_EOC))
    {

    }
    ret_value = adc_regular_data_read();
    if(pin == P_TEMP)
    {
        return ret_value;
    }
    else
    {
        return (ret_value>>2);
    }
}

uint8_t drv_gpio_shift_in(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order) 
{
    uint8_t value = 0;

    for(uint8_t i = 0; i < 8; ++i) 
    {
        drv_gpio_digital_write(clock_pin, HIGH);
        drv_delay_us(2);
        if(bit_order == LSBFIRST)
        {
            value |= drv_gpio_digital_read(data_pin) << i;
        }
        else
        {
            value |= drv_gpio_digital_read(data_pin) << (7 - i);
        }
        drv_gpio_digital_write(clock_pin, LOW);
    }
    return value;
}

void drv_gpio_disable_vdda_protect(void)
{
    if(OB_USER & OB_VDDA_DISABLE)
    {
        /* unlock the main flash and option byte */
        fmc_unlock();
        ob_unlock();
        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR | FMC_FLAG_PGERR);
        ob_erase();
        ob_user_write(OB_VDDA_DISABLE);
        fmc_lock();
        ob_lock();
    }
}
