#include <stdio.h>
#include "drv_gpio.h"
#include "drv_led.h"


void drv_led_init(void)
{
    drv_gpio_pin_mode(LED_PIN, GPIO_MODE_OUTPUT_PULLDOWN);
    drv_gpio_digital_write(LED_PIN, 0);
}


void drv_led_on(void)
{
    drv_gpio_digital_write(LED_PIN, 1);

}


void drv_led_off(void)
{
    drv_gpio_digital_write(LED_PIN, 0);

}

