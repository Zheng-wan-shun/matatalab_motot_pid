#ifndef COMMUNICATION_LINK_H
#define COMMUNICATION_LINK_H

#include <stdint.h>
#include <stdbool.h>
#include "drv_ring_buf.h"

void communication_link_init(void);
void communication_write_data(uint8_t data);
void communication_read_data(uint16_t data_len, uint8_t* rx_data_buf);
void communication_data_empty(void);
uint16_t communication_get_count(void);
RING_BUF_DEF_STRUCT* communicattion_get_rxcfg(void);

#endif
