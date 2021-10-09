#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

#include <stdint.h>
#include "gd32f3x0_gpio.h"

#define HIGH        1
#define LOW         0

#define LSBFIRST    0
#define MSBFIRST    1

#define ANALOG_MIN                  (0)
#define ANALOG_MAX                  (1023)

#define GPIO_PORT_ADDRESS(port)   (GPIO_BASE +(port << 10))

enum{
    PA_0   =   0x00,
    PA_1   =   0x01,
    PA_2   =   0x02,
    PA_3   =   0x03,
    PA_4   =   0x04,
    PA_5   =   0x05,
    PA_6   =   0x06,
    PA_7   =   0x07,
    PA_8   =   0x08,
    PA_9   =   0x09,
    PA_10  =   0x0a,
    PA_11  =   0x0b,
    PA_12  =   0x0c,
    PA_13  =   0x0d,
    PA_14  =   0x0e,
    PA_15  =   0x0f, 
    PB_0   =   0x10,
    PB_1   =   0x11,
    PB_2   =   0x12,
    PB_3   =   0x13,
    PB_4   =   0x14,
    PB_5   =   0x15,
    PB_6   =   0x16,
    PB_7   =   0x17,
    PB_8   =   0x18,
    PB_9   =   0x19,
    PB_10  =   0x1a,
    P_TEMP =   0x20,
    P_REF  =   0x21,
    P_BAT  =   0x22
};

typedef enum
{
    GPIO_MODE_OUTPUT_NONE = 0,
    GPIO_MODE_OUTPUT_PULLUP,
    GPIO_MODE_OUTPUT_PULLDOWN,
    GPIO_MODE_INPUT_NONE,
    GPIO_MODE_INPUT_PULLUP,
    GPIO_MODE_INPUT_PULLDOWN
}gpio_mode_type;

void drv_gpio_pin_mode(uint8_t pin,uint8_t mode);
void drv_gpio_digital_write(uint8_t  pin, uint8_t val);
int drv_gpio_digital_read(uint8_t pin);        
int drv_gpio_analog_read(uint8_t pin);
uint8_t drv_gpio_shift_in(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order);
void drv_gpio_disable_vdda_protect(void);
#endif
