#include "gd32f3x0.h"
#include <stdio.h>
#include "system_def.h"
#include "drv_gpio.h"
#include "drv_systime.h"
#include "message.h"
#include "drv_key.h"


void drv_key_init(void)
{
    drv_gpio_pin_mode(PIN_TURN_LEFT, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_TURN_RIGHT, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_TURN_FORWARD, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_TURN_BACKWARD, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_GD32_HW_VER, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_TURN_START, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_TURN_DELETE, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_TURN_LOOP, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_RECORD, GPIO_MODE_INPUT_NONE);
    drv_gpio_pin_mode(PIN_RANDOM_DANCE, GPIO_MODE_INPUT_NONE);

    
}

uint32_t drv_key_get_status(uint8_t key_pin)
{
    return (!drv_gpio_digital_read(key_pin));
}

uint32_t drv_key_status_all(void)
{
    uint32_t status = 0;

    status = (drv_key_get_status(PIN_TURN_LEFT) << KEY_TURN_LEFT) \
            | (drv_key_get_status(PIN_TURN_RIGHT) << KEY_TURN_RIGHT)  \
            | (drv_key_get_status(PIN_TURN_FORWARD) << KEY_TURN_FORWARD)  \
            | (drv_key_get_status(PIN_TURN_BACKWARD) << KEY_TURN_BACKWARD)  \
            | (drv_key_get_status(PIN_TURN_START) << KEY_START)  \
            | (drv_key_get_status(PIN_TURN_DELETE) << KEY_DELETE)  \
            | (drv_key_get_status(PIN_TURN_LOOP) << KEY_LOOP)  \
            | (drv_key_get_status(PIN_RECORD) << KEY_RECORD)  \
            | (drv_key_get_status(PIN_RANDOM_DANCE) << KEY_RANDOM_DANCE);
    return status;
}

