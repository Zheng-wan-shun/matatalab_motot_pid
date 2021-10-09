

#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__

#include "drv_gpio.h"



#define PIN_TURN_LEFT             (PA_0)
#define PIN_TURN_RIGHT            (PA_1)
#define PIN_TURN_FORWARD          (PA_2)
#define PIN_TURN_BACKWARD         (PA_5)
#define PIN_GD32_HW_VER           (PA_4)
#define PIN_TURN_START            (PA_3)
#define PIN_TURN_DELETE           (PA_6)
#define PIN_TURN_LOOP             (PA_7)
#define PIN_RECORD                (PA_10)
#define PIN_RANDOM_DANCE          (PB_8)


#define KEY_REPORT_PEROID         (20)

typedef enum{
    KEY_TURN_LEFT = 0x0,
    KEY_TURN_RIGHT,
    KEY_TURN_FORWARD,
    KEY_TURN_BACKWARD,
    KEY_START,
    KEY_DELETE,
    KEY_LOOP,
    KEY_RECORD,
    KEY_RANDOM_DANCE,
    

}KEY_TYPE_E;

void drv_key_init(void);
uint32_t drv_key_status_all(void);


#endif

