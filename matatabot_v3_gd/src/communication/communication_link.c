#include "stdio.h"
#include "string.h"
#include "system_def.h"
#include "drv_uart.h"
#include "drv_ring_buf.h"
#include "message.h"
#include "communication_link.h"

#define DATA_RX_BUFSIZE        256
#define CANCEL_CMD_LEN         (5)

static RING_BUF_DEF_STRUCT s_rx_ring_buf;
static uint8_t s_link_rx_buf[DATA_RX_BUFSIZE];


void communication_link_init(void)
{
    drv_ringbuf_init((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, s_link_rx_buf, DATA_RX_BUFSIZE);
}

uint8_t communication_check_cmd_cancel(uint8_t data)
{
    const uint8_t cancel[] ={0xFE, 0x03, CMD_CANCEL, 0x99, 0x50};
    uint8_t i;
    static uint8_t buffer[8];
    static uint8_t msg_index = 0;
    
    if(data == cancel[0])
    {
        msg_index = 0;
    }
    if(buffer[msg_index] != cancel[msg_index])
    {
        msg_index = 0;
        return MATA_FAIL;
    }
    buffer[msg_index] = data;
    msg_index++;
    if(msg_index == CANCEL_CMD_LEN)
    {
        communication_data_empty();
        cmd_cancel();
        for(i = 0; i < 8; i++)
        {
            buffer[i] = 0;
        }
        msg_index = 0;
        return MATA_OK;
    }
    return MATA_FAIL;
}

void communication_write_data(uint8_t data)
{
    static uint8_t translation_flag = false;

    if(communication_check_cmd_cancel(data) == MATA_OK)
    {
        /*if recive cancel*/
        return;
    }
    if(data == MSG_FRAMER_HEADER)
    {
        translation_flag = false;
    }
    if((data == MSG_FRAMER_TRANSLATION) && (message_is_rx_long_trans() == false))
    {
        translation_flag = true;
        return;
    }
    else
    {
        data += translation_flag * CHAR_TRANSLATION_OFFSET;
        translation_flag = false;
    }
    drv_ringbuf_write((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, &data, 1);
}

void communication_read_data(uint16_t data_len, uint8_t* rx_data_buf)
{
    drv_ringbuf_read((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf, data_len, rx_data_buf);
}

void communication_data_empty(void)
{
    drv_ringbuf_flush((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf);
}

uint16_t communication_get_count(void)
{
    uint16_t count;
    count = drv_ringbuf_count((RING_BUF_DEF_STRUCT*)&s_rx_ring_buf);
    return count;
}

RING_BUF_DEF_STRUCT* communicattion_get_rxcfg(void)
{
    return &s_rx_ring_buf;
}


