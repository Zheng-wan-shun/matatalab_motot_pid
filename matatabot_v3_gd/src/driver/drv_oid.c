
#include "gd32f3x0.h"
#include <stdio.h>
#include "system_def.h"
#include "drv_gpio.h"
#include "drv_systime.h"
#include "drv_uart.h"
#include "drv_ring_buf.h"
#include "drv_oid.h"
#include "message.h"


oid_status_t oid_status;

static oid_proc_state oid_proc;

static RING_BUF_DEF_STRUCT oid_tx_ringbuf_s;

#define OID_DATA_LEN        (8)
static uint8_t oid_data[OID_DATA_LEN];

#define OID_TX_BUFFER_LEN   (16)
static uint8_t oid_tx_buffer[OID_TX_BUFFER_LEN];
static void drv_oid_recive_config(uint8_t type)
{
    oid_proc_state* p_oid_proc;
    oid_status_t *p_oid_status;

    p_oid_status = &oid_status;
    p_oid_proc = &oid_proc;

    if(type == OID_RECIVE_ACK)
    {
        p_oid_proc->oid_isr_loop = OID_RESPONSE_ACK_BIT;
        p_oid_status->status = OID_PROC_RECIVE_ACK;
    }
    else
    {
        p_oid_proc->oid_isr_loop = OID_RESPONSE_DATA_BIT;
        p_oid_status->status = OID_PROC_RECIVE_DATA;
    }
    p_oid_proc->oid_isr_state = OID_PROCESS_START;
    p_oid_proc->oid_done_flag = 0;


}

void drv_oid_recive_ack(void)
{
    drv_oid_recive_config(OID_RECIVE_ACK);
}

void drv_oid_recive_data(void)
{
    drv_oid_recive_config(OID_RECIVE_DATA);
}


void drv_oid_transmitcmd(uint8_t cmd)
{
    oid_status_t *p_oid_status;
    oid_proc_state* p_oid_proc;
    
    p_oid_status = &oid_status;
    p_oid_proc = &oid_proc;

    p_oid_status->status = OID_PROC_SEND;
    p_oid_status->send_data = cmd;
    p_oid_proc->oid_done_flag = 0;
    p_oid_proc->oid_isr_state = OID_PROCESS_START;

}


uint8_t drv_oid_get_tx_len(void)
{
    return drv_ringbuf_count(&oid_tx_ringbuf_s);
}

uint8_t drv_oid_mode_set(uint8_t mode, uint8_t report_mode, uint8_t freq)
{
    if((mode != MODE_TALKING_PEN) && (mode != MODE_HANDWRITING_PEN))
    {
        return MATA_FAIL;
    }
    oid_status.mode = mode;
    if(report_mode == REPORT_CONTINUES)
    {
        oid_transmitcmd(CMD_ENABLEPOSITIONWORKINGMODE);
    }
    else
    {
        oid_transmitcmd(CMD_DISABLEPOSITIONWORKINGMODE);
    }

    switch (freq)
    {
        case OID_MIN_FRAME_RATE_5:
            oid_transmitcmd(CMD_MINFRAMERATE_5);
            break;
        
        case OID_MIN_FRAME_RATE_20:
            oid_transmitcmd(CMD_MINFRAMERATE_20);
            break;
        case OID_MIN_FRAME_RATE_30:
            oid_transmitcmd(CMD_MINFRAMERATE_30);
            break;
        case OID_MIN_FRAME_RATE_40:
            oid_transmitcmd(CMD_MINFRAMERATE_40);
            break;
        case OID_MIN_FRAME_RATE_50:
            oid_transmitcmd(CMD_MINFRAMERATE_50);
            break;
        
    }
    return MATA_OK;
}


uint8_t drv_oid_wakeup(void)
{
    uint32_t i=0;
    oid_status_t *p_oid_status;
    oid_proc_state* p_oid_proc;

    
    p_oid_status = &oid_status;
    p_oid_proc = &oid_proc;

    drv_gpio_digital_write(p_oid_status->sck_pin, 1);
    drv_delay_ms(30);
    drv_gpio_digital_write(p_oid_status->sck_pin, 0);

    while(i < 200)
    {
        drv_delay_ms(10);
        i = i + 1;
        if(drv_gpio_digital_read(p_oid_status->sdio_pin) == 0)
        {
            drv_oid_recive_data();
            while(p_oid_proc->oid_done_flag == 0)
            {
                drv_delay_ms(1);
            }
            
            drv_uart_printf(UART0_PORT, "recive 0x%08x %08x\r\n",  \
                       (uint32_t)(((p_oid_status->recive_data)>>32)&0xFFFFFFFF),  \
                       (uint32_t)(p_oid_status->recive_data&0xFFFFFFFF));
            if((p_oid_status->recive_data & 0xFFFF) == 0xFFF8)
            {
                
                drv_uart_printf(UART0_PORT, "wakeup OK\r\n" );
                return MATA_OK;
            }
            drv_uart_printf(UART0_PORT, "wakeup fail\r\n" );
            /* recive wrong ack*/
            return MATA_FAIL;
        }
    }
    /* time out*/
    drv_uart_printf(UART0_PORT, "wakeup timeout\r\n" );
    return MATA_FAIL;
    
}

void oid_recive_isr(void)
{

    static uint8_t sck_state = 0;
    oid_status_t *p_oid_status;
    oid_proc_state* p_oid_proc;
    
    
    p_oid_status = &oid_status;
    p_oid_proc = &oid_proc;
    
    switch (p_oid_proc->oid_isr_state)
    {
        case OID_PROCESS_NULL:
            
            break;
        
        case OID_PROCESS_START:
            if(sck_state)
            {
                sck_state = 0;
                drv_gpio_digital_write(p_oid_status->sck_pin, 0);
                p_oid_proc->oid_isr_state = OID_PROCESS_DOING;
                p_oid_status->recive_data = 0;
                
            }
            else
            {
                sck_state = 1;
                drv_gpio_digital_write(p_oid_status->sck_pin, 1);
                /*start bit*/
                drv_gpio_digital_write(p_oid_status->sdio_pin, 0);
                drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_OUTPUT_PULLUP);
            }
            break;
        
        case OID_PROCESS_DOING:
            if(sck_state)
            {
                sck_state = 0;
                drv_gpio_digital_write(p_oid_status->sck_pin, 0);
                if(drv_gpio_digital_read(p_oid_status->sdio_pin))
                {
                    p_oid_status->recive_data += 1;
                }
                p_oid_proc->oid_isr_loop--;
                if(p_oid_proc->oid_isr_loop == 0)
                {
                    p_oid_proc->oid_isr_state = OID_PROCESS_END;
                    /*step end need 2 cycle*/
                    p_oid_proc->oid_isr_loop = OID_END_LAST_TIME_INDEX;
                }
            }
            else
            {
                sck_state = 1;
                p_oid_status->recive_data <<= 1;
                drv_gpio_digital_write(p_oid_status->sck_pin, 1);
                drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_INPUT_PULLUP);
            }
            break;

        case OID_PROCESS_END:
            sck_state = 0;
            drv_gpio_digital_write(p_oid_status->sck_pin, 0);
            p_oid_proc->oid_isr_loop--;
            if(p_oid_proc->oid_isr_loop == 0)
            {
                p_oid_proc->oid_isr_state = OID_PROCESS_NULL;
                p_oid_proc->oid_done_flag = 1;
            }
            break;

        default:
            break;
        
    }

}

void oid_send_isr(void)
{
    static uint8_t sck_state = 0;
    static uint8_t send_index = 0;
    static uint8_t send_cmd = 0;
    oid_status_t *p_oid_status;
    oid_proc_state* p_oid_proc;
    
    
    p_oid_status = &oid_status;
    p_oid_proc = &oid_proc;
    
    switch (p_oid_proc->oid_isr_state)
    {
        case OID_PROCESS_NULL:
            
            break;
        
        case OID_PROCESS_START:
            if(sck_state)
            {
                sck_state = 0;
                drv_gpio_digital_write(p_oid_status->sck_pin, 0);
                p_oid_proc->oid_isr_state = OID_PROCESS_DOING;
                send_index = 8;
                send_cmd = p_oid_status->send_data;
            }
            else
            {
                sck_state = 1;
                drv_gpio_digital_write(p_oid_status->sck_pin, 1);
                /*start bit*/
                drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_INPUT_PULLUP);
            }
            break;

        case OID_PROCESS_DOING:
            
            if(sck_state)
            {
                sck_state = 0;
                drv_gpio_digital_write(p_oid_status->sck_pin, 0);
                send_index--;
                if(send_index == 0)
                {
                    drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_INPUT_PULLUP);
                    p_oid_proc->oid_isr_state = OID_PROCESS_END;
                    send_index = OID_END_LAST_TIME_INDEX;
                }
                send_cmd <<= 1;
            }
            else
            {
                sck_state = 1;
                drv_gpio_digital_write(p_oid_status->sck_pin, 1);
                if(send_cmd & 0x80)
                {
                    drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_INPUT_PULLUP);
                }
                else
                {
                    drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_OUTPUT_PULLUP);
                    drv_gpio_digital_write(p_oid_status->sdio_pin, 0);
                }

            }
            break;
        
        case OID_PROCESS_END:
            send_index--;
            sck_state = 0;
            drv_gpio_digital_write(p_oid_status->sck_pin, 0);
            if(send_index == 0)
            {
                p_oid_proc->oid_isr_state = OID_PROCESS_NULL;
                p_oid_proc->oid_done_flag = 1;
            }
            break;
        
        default:
            break;
    }

}

void oid_process_isr(void)
{
    oid_status_t *p_oid_status;
    //oid_proc_state* p_oid_proc;

    
    p_oid_status = &oid_status;
    //p_oid_proc = &oid_proc;

    
    if(p_oid_status->status == OID_PROC_SEND)
    {
        oid_send_isr();
    }
    else
    {
        oid_recive_isr();
    }
    

}

void drv_oid_data_clear(void)
{
    uint8_t i;
    for (i = 0; i < OID_DATA_LEN; ++i)
    {
        oid_data[i] = 0;
    }
}

void drv_oid_recive_time_update(void)
{
    oid_status_t *p_oid_status;

    p_oid_status = &oid_status;
    p_oid_status->recive_time = drv_millis();
    return;
}

void drv_oid_data_get(uint8_t *pdata)
{
    uint8_t i;
    oid_status_t *p_oid_status;

    p_oid_status = &oid_status;
    if((drv_millis() - p_oid_status->recive_time) > OID_VALID_LAST_TIME)
    {
        drv_oid_data_clear();
    }

    for (i = 0; i < OID_DATA_LEN; ++i)
    {
        *pdata++ = oid_data[i];
    }

}

void drv_oid_init(void)
{
    oid_status_t *p_oid_status;
    uint8_t ret;
    uint8_t i = 0;
    
    p_oid_status = &oid_status;
    p_oid_status->sck_pin = OID_SCK_PIN;
    p_oid_status->sdio_pin = OID_DATA_PIN;
    p_oid_status->mode = MODE_HANDWRITING_PEN; //MODE_TALKING_PEN;
    p_oid_status->status = OID_IDLE;        
    p_oid_status->recive_data = 0;
    p_oid_status->oid_retrans_flag = false;
    p_oid_status->oid_transmit_cnt = 0;
    p_oid_status->rst_pin = OID_RST_PIN;

    oid_proc.oid_isr_loop = 0;
    oid_proc.oid_isr_state = OID_PROCESS_NULL;
    oid_proc.oid_done_flag = 0;

    drv_ringbuf_init(&oid_tx_ringbuf_s, oid_tx_buffer, OID_TX_BUFFER_LEN);
    drv_oid_data_clear();
    
    drv_gpio_pin_mode(p_oid_status->rst_pin, GPIO_MODE_OUTPUT_PULLUP);
    drv_gpio_digital_write(p_oid_status->rst_pin, 0);
    drv_delay_ms(30);
    drv_gpio_digital_write(p_oid_status->rst_pin, 1);

    drv_gpio_pin_mode(p_oid_status->sck_pin, GPIO_MODE_OUTPUT_PULLDOWN);
    drv_gpio_pin_mode(p_oid_status->sdio_pin, GPIO_MODE_INPUT_PULLUP);

    do{
        ret = drv_oid_wakeup();
        i++;

    }while((ret != MATA_OK)&&(i < OID_RETRANSMIT_TIME_MAX));
    oid_normal_presetting();

    return;
    
}



void oid_transmitcmd(uint8_t cmd)
{
    drv_ringbuf_write(&oid_tx_ringbuf_s, &cmd, 1);
    return;
}

void oid_normal_presetting(void)
{
    oid_status_t *p_oid_status;

    p_oid_status = &oid_status;
    if(MODE_TALKING_PEN == p_oid_status->mode)
    {
        oid_transmitcmd(CMD_POSITIONDISABLE);
        oid_transmitcmd(CMD_AUTOSWITCHTOPOSDISABLE);
        oid_transmitcmd(CMD_MINFRAMERATE_20);
        oid_transmitcmd(CMD_ENABLEPOSITIONWORKINGMODE);
        
    }
    
    if(MODE_HANDWRITING_PEN == p_oid_status->mode)
    {
        oid_transmitcmd(CMD_MINFRAMERATE_20);
        oid_transmitcmd(CMD_POSITIONENABLE);
        //oid_transmitcmd(CMD_AUTOSWITCHTOPOSENABLE);
        //oid_transmitcmd(CMD_AUTOSWITCHTOGENENABLE);
        oid_transmitcmd(CMD_OUTPUTANGLEENABLE);
        
        oid_transmitcmd(CMD_ENABLEPOSITIONWORKINGMODE);
    }
}

void oid_data_update(uint32_t oid_data_h, uint32_t oid_data_l)
{
    uint8_t *pdata;

    pdata = oid_data;
    *pdata++ = (oid_data_h >> 24)&0xFF;
    *pdata++ = (oid_data_h >> 16)&0xFF;
    *pdata++ = (oid_data_h >> 8)&0xFF;
    *pdata++ = (oid_data_h)&0xFF;
    *pdata++ = (oid_data_l >> 24)&0xFF;
    *pdata++ = (oid_data_l >> 16)&0xFF;
    *pdata++ = (oid_data_l >> 8)&0xFF;
    *pdata++ = (oid_data_l)&0xFF;

    drv_oid_recive_time_update();
}

void oid_parse(void)
{
    
    uint32_t oid_data_h;
    uint32_t oid_data_l;
    uint32_t oid_payload;
    uint32_t oid_sign_flag = 0;
    uint32_t angle = 0;
    float position_x;
    float position_y;
    
    oid_status_t *p_oid_status;

    p_oid_status = &oid_status;

    oid_data_h = (uint32_t)((p_oid_status->recive_data >> 32) & 0xFFFFFFFF);
    oid_data_l = (uint32_t)(p_oid_status->recive_data & 0xFFFFFFFF);
    if(oid_data_h & BIT(28))  //bit60
    {
        /*invalid code*/
        return;
    }
    drv_uart_printf(UART0_PORT, "OID recive 0x%08x  0x%08x \r\n", oid_data_h, oid_data_l);
    oid_data_update(oid_data_h, oid_data_l);
    if(oid_data_h & BIT(30))
    {
        /* OID3 */
        if(oid_data_h & BIT(29))
        {
            /*position*/
            if(oid_data_h & BIT(3))  //bit35
            {
                oid_sign_flag = 1;
            }
            else
            {
                oid_sign_flag = 0;
            }
            position_x = (p_oid_status->recive_data & 0x3FFF) \
                        + (((p_oid_status->recive_data >> 28) & 0x7F) / 128.0) \
                        + (-1.0) * oid_sign_flag;

            if(oid_data_h & BIT(11))  //bit43
            {
                oid_sign_flag = 1;
            }
            else
            {
                oid_sign_flag = 0;
            }
            position_y = ((p_oid_status->recive_data >> 14) & 0x3FFF) \
                        + (((p_oid_status->recive_data >> 36) & 0x7F) / 128.0) \
                        + (-1.0) * oid_sign_flag;
            /*angle bit 45-53 */
            angle = (oid_data_h >> 13) & 0x1FF;
            
            oid_data_h = (int)(position_x*10);
            oid_data_l = (int)(position_y*10);
            //drv_uart_printf(UART0_PORT, "position x: %d, y: %d\r\n", oid_data_h, oid_data_l);
            //drv_uart_printf(UART0_PORT, "angle:%d, position x: %f, y: %f\r\n", angle, position_x, position_y);
        }
        else
        {
            /*genernal*/
            oid_payload = oid_data_l & 0xFFFFFFF;
        }
    
    }
    else
    {
        /*OID2*/
        oid_payload = oid_data_l & 0xFFFF;
    }
    (void)oid_payload;
    (void)position_x;
    (void)position_y;
    (void)angle;
}

void oid_task_loop(void)
{
    
    uint8_t cmd;
    static uint8_t state = OID_IDLE;
    static uint32_t time;
    oid_status_t *p_oid_status;
    oid_proc_state* p_oid_proc;

    p_oid_status = &oid_status;
    p_oid_proc = &oid_proc;
    
    switch (state)
    {
        case OID_IDLE:
            if(drv_gpio_digital_read(p_oid_status->sdio_pin) == 0)
            {
                drv_oid_recive_data();
                state = OID_PROC_RECIVE_DATA;
            }
            else
            {
                if((p_oid_status->oid_retrans_flag) && (p_oid_status->oid_transmit_cnt < OID_RETRANSMIT_TIME_MAX))
                {
                    cmd = p_oid_status->send_data;
                    drv_oid_transmitcmd(cmd);
                    state = OID_PROC_SEND;
                    drv_uart_printf(UART0_PORT, "oid retransmit 0x%x, t:%d", cmd, p_oid_status->oid_transmit_cnt);
                }
                else if(drv_oid_get_tx_len())
                {
                    drv_ringbuf_read(&oid_tx_ringbuf_s, 1, &cmd);
                    drv_oid_transmitcmd(cmd);
                    p_oid_status->oid_transmit_cnt = 0;
                    state = OID_PROC_SEND;
                    drv_uart_printf(UART0_PORT,"oid transmit 0x%x", cmd);
                }

            }
            break;
                
        case OID_PROC_RECIVE_DATA:
            if(p_oid_proc->oid_done_flag == 1)
            {
                oid_parse();
                
                //oid_report();
                state = OID_IDLE;
            }
            break;
            
        case OID_PROC_SEND:
            if(p_oid_proc->oid_done_flag == 1)
            {
                time = drv_millis();
                state = OID_PROC_WAIT_ACK;
            }
            break;
            
        case OID_PROC_WAIT_ACK:
            if((drv_millis() - time) > OID_SEND_ACK_TIMEOUT)
            {
                p_oid_status->oid_retrans_flag = true;
                p_oid_status->oid_transmit_cnt++;
                state = OID_IDLE;
            }
            if((drv_gpio_digital_read(p_oid_status->sdio_pin) == 0))
            {
                drv_oid_recive_ack();
                state = OID_PROC_RECIVE_ACK;
            }
            break;
        
        
        case OID_PROC_RECIVE_ACK:
            if(p_oid_proc->oid_done_flag == 1)
            {
                if((p_oid_status->recive_data - 1) == p_oid_status->send_data)
                {
                    p_oid_status->oid_retrans_flag = false;
                    p_oid_status->oid_transmit_cnt = 0;
                }
                else
                {
                    /* ack is wrong*/
                    p_oid_status->oid_retrans_flag = true;
                    p_oid_status->oid_transmit_cnt++;
                }
                state = OID_IDLE;
                
                drv_uart_printf(UART0_PORT,"oid recive ack 0x%x", p_oid_status->recive_data);
            }
            break;
        
        

    }
}

