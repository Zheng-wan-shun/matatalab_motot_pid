
#ifndef __DRV_SENSOR_H__
#define __DRV_SENSOR_H__

#define LIGHT_REPORT_PERIOD    (20)

void drv_sensor_light_init(void);
uint16_t drv_sensor_light_get(uint8_t index);

#endif
