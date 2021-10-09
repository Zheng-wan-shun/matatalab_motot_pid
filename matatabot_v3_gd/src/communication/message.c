
#include "gd32f3x0.h"
#include <stdio.h>
#include "system_def.h"
#include "drv_gpio.h"
#include "drv_systime.h"
#include "drv_uart.h"
#include "drv_oid.h"
#include "drv_step_motor.h"
#include "drv_ring_buf.h"
#include "drv_led.h"
#include "drv_key.h"
#include "drv_sensor.h"
#include "communication_link.h"
#include "crc16.h"
#include "message.h"

msg_state_t rx_msg;
uint8_t long_msg_send_flag = false;  //for transmit file
uint8_t long_msg_recive_flag = false;  //for recive file
long_msg_recive_s long_msg_recv;
uint8_t motion_wait_stop = false;

void message_init(void)
{
    uint8_t i;
    msg_state_t *pRxMsg;
    pRxMsg = &rx_msg;

    pRxMsg->state = MSG_IDLE;
    pRxMsg->msg_len = 0;
    pRxMsg->factory_flag = false;
    for (i = 0; i < MSG_BUFFER_LEN; ++i)
    {
        pRxMsg->buffer[i] = 0;        
    }
    pRxMsg->pfunc = NULL;
    return;
}


uint8_t message_translation(uint8_t data, uint8_t *p_send)
{
    uint8_t len;
    if((data == MSG_FRAMER_HEADER) || (data == MSG_FRAMER_TRANSLATION))
    {
        *p_send = MSG_FRAMER_TRANSLATION;
        p_send++;
        *p_send = data - CHAR_TRANSLATION_OFFSET;
        p_send++;
        len = 2;
    }
    else
    {
        *p_send = data;
        p_send++;
        len = 1;
    }
    return len;

}

void message_send(uint8_t *p_msg, uint8_t msg_len)
{
    uint16_t crc;
    uint8_t buffer[32];
    uint8_t i;
    uint8_t cnt;
    uint8_t temp;
    uint8_t *pbuf;

    //crc = crc16_compute(p_msg, len, NULL);
    
    cnt = 0;
    pbuf = buffer;
    *pbuf = MSG_FRAMER_HEADER;
    pbuf++;
    cnt++;
    *pbuf = msg_len + MSG_CRC_LEN;
    crc = crc16_compute(pbuf, 1, NULL);
    pbuf++;
    cnt++;
    crc = crc16_compute(p_msg, msg_len, &crc);
    for(i = 0; i < msg_len; i++)
    {
        temp = message_translation(p_msg[i], pbuf);
        cnt += temp;
        pbuf += temp;
    }
    
    temp = message_translation((crc >> 8), pbuf);
    cnt += temp;
    pbuf += temp;
    
    temp = message_translation((crc & 0xFF), pbuf);
    cnt += temp;

    pbuf = buffer;
    drv_uart_printf(UART0_PORT, "send msg!\r\n");
    for(i = 0; i < cnt; i++)
    {
        drv_uart_printf(UART0_PORT, "0x%x ", buffer[i]);
    }
    
    send_string(pbuf, cnt);
   
    drv_uart_printf(UART0_PORT, "send end!\r\n");
}

void message_normal_response( uint8_t error_type)
{
    uint8_t msg[2];

    msg[0] = CMD_RESPONSE;
    msg[1] = error_type;
    
    message_send(msg, sizeof(msg));
    return;
}

uint8_t message_is_rx_long_trans(void)
{
    return long_msg_recive_flag;
}

uint8_t cmd_pop_msg(uint8_t *pbuffer)
{
    
    uint32_t cmd_byte_cnt;
    uint32_t i;
    uint32_t msg_len;
    uint8_t * pdata;
    uint8_t ret;
    RING_BUF_DEF_STRUCT* p_ringbuf_s;

    p_ringbuf_s = communicattion_get_rxcfg();

    pdata = p_ringbuf_s->buff_addr;

    cmd_byte_cnt = communication_get_count();

    if(cmd_byte_cnt == 0)
    {
        return MATA_FAIL;
    }
    
    /************find message header***************************************/
    for(i = 0; i < cmd_byte_cnt; i++)
    {
        if(pdata[p_ringbuf_s->read_index] == MSG_FRAMER_HEADER)
        {
            break;
        }
        else
        {
            drv_ringbuf_read(p_ringbuf_s, 1, &ret);
        }
    }
    
    cmd_byte_cnt = communication_get_count();
    if(cmd_byte_cnt == 1)
    {
        /*only recive header*/
        return MATA_FAIL;
    }
    /* message include 1(0xFE) + 1(len) + payload + 2 byte crc*/
    msg_len = pdata[(p_ringbuf_s->read_index + 1)&p_ringbuf_s->max_size_mask] + MSG_HEAD_LEN;
    if(msg_len > cmd_byte_cnt)
    {
        /*have not recive enough message*/
        return MATA_FAIL;
    }
    drv_ringbuf_read(p_ringbuf_s, msg_len, pbuffer);
    return MATA_OK;

}



void cmd_system_info_get(uint8_t* p_msg, uint8_t msg_len)
{
    uint8_t buffer[20];
    uint8_t cnt;

    if(msg_len != CMD_SYSTERM_INFO_LEN)
    {
        message_normal_response(MESSAGE_ERROR_INVALID_LENGTH);
        return;
    }
    cnt = 0;
    buffer[cnt++] = CMD_SYSTERM_INFO;
    buffer[cnt++] = p_msg[0];
    switch(p_msg[0])
    {
        case SYSTEM_INFO_VERSION:
            buffer[cnt++] = GD32_VERSION0;
            buffer[cnt++] = GD32_VERSION1;
            buffer[cnt++] = GD32_VERSION2;
            break;
        
        case SYSYTEM_INFO_MAC:
            break;
        
        case SYSTEM_INFO_PROTOCOL:
            buffer[cnt++] = GD32_PROTOCOL_VERSION0;
            buffer[cnt++] = GD32_PROTOCOL_VERSION1;
            buffer[cnt++] = GD32_PROTOCOL_VERSION2;
            break;

        case SYSTEM_INFO_DEVICE_ID:
            break;

        default:
            message_normal_response(MESSAGE_ERROR_INVALID_PARAM);
            return;
            //break;
    }
    //ble_nus_string_send(&m_nus, buffer, i);
    //ble_write(buffer, i);
    message_send(buffer, cnt);

    return;
}

void message_motion_ack(void)
{
    uint8_t buffer[4];
    uint8_t cnt;
    cnt = 0;
    buffer[cnt++] = CMD_MOTION;
    buffer[cnt++] = 0xFF;
    message_send(buffer, cnt);
    return;

}

uint8_t cmd_motion_process(void)
{
    if(motor_get_status(MOTOR_LEFT) != STOP_MOVE)
    {
        return MATA_BUSY;
    }
    message_motion_ack();
    //message_normal_response(MESSAGE_RX_OK);
    return MATA_OK;

}

void cmd_motion(uint8_t* p_msg, uint8_t msg_len)
{
    uint32_t parameter;
    
    long pulse;
    if(msg_len != CMD_MOTION_LEN)
    {
        message_normal_response(MESSAGE_ERROR_INVALID_LENGTH);
        return;
    }
    parameter = (p_msg[1] << 8) | p_msg[2];
    switch (p_msg[0])
    {
        case MOTION_FORWARD:
            /* parameter is mm, MM_TO_PULSE is for mm*/
            pulse = (long)(MM_TO_PULSE * parameter);
            motor_move(MOTOR_LEFT, pulse, false);
            motor_move(MOTOR_RIGHT, -pulse, true);
            break;
        
        case MOTION_BACKWARD:
            pulse = (long)(MM_TO_PULSE * parameter);
            motor_move(MOTOR_LEFT, -pulse, false);
            motor_move(MOTOR_RIGHT, pulse, true);
            break;
        
        case MOTION_TURN_LEFT:
            pulse = (long)(DEGREE_TO_PULSE * parameter);
            motor_move(MOTOR_LEFT, -pulse, false);
            motor_move(MOTOR_RIGHT, -pulse, true);
            break;

        case MOTION_TURN_RIGHT:
            pulse = (long)(DEGREE_TO_PULSE * parameter);
            motor_move(MOTOR_LEFT, pulse, false);
            motor_move(MOTOR_RIGHT, pulse, true);
            break;

        default:
            message_normal_response(MESSAGE_ERROR_INVALID_PARAM);
            return;
            //break;
    }
    //rx_msg.pfunc = cmd_motion_process;
    motion_wait_stop = true;

}

int8_t msg_animotion_direction_get(uint8_t direct)
{
    if(direct == MOTION_DIRECTION_POSITIVE)
    {
        return 1;
    }
    if(direct == MOTION_DIRECTION_NEGATIVE)
    {
        return -1;
    }
    return 1;

}

void cmd_animotion(uint8_t* p_msg, uint8_t msg_len)
{
    
    int16_t left_setting, right_setting;
    long left_pulse_speed = 0;
    long right_pulse_speed = 0;
    int8_t sign;
    switch (p_msg[0])
    {
        case ANIMOTION_SPEED_LEFT:
            sign = msg_animotion_direction_get(p_msg[1]);
            left_setting = (p_msg[2] << 8) | p_msg[3];
            left_setting = left_setting * sign;
            left_pulse_speed = (long)(MM_TO_PULSE * left_setting);
            motor_set_speed(MOTOR_LEFT, -left_pulse_speed);
            
            break;
        
        case ANIMOTION_SPEED_RIGHT:
            sign = msg_animotion_direction_get(p_msg[1]);
            right_setting = (p_msg[2] << 8) | p_msg[3];
            right_setting = right_setting * sign;
            right_pulse_speed = (long)(MM_TO_PULSE * right_setting);
            motor_set_speed(MOTOR_RIGHT, right_pulse_speed);
            break;
            
        case ANIMOTION_SPEED_BOTH:
            sign = msg_animotion_direction_get(p_msg[1]);
            left_setting = (p_msg[2] << 8) | p_msg[3];
            left_setting = left_setting * sign;
            left_pulse_speed = (long)(MM_TO_PULSE * left_setting);
            sign = msg_animotion_direction_get(p_msg[4]);
            right_setting = (p_msg[5] << 8) | p_msg[6];
            right_setting = right_setting * sign;
            right_pulse_speed = (long)(MM_TO_PULSE * right_setting);
            motor_run_speed(-left_pulse_speed, right_pulse_speed);
            break;
        
        default:
            message_normal_response(MESSAGE_ERROR_INVALID_PARAM);
            return;
    }
    message_normal_response(MESSAGE_RX_OK);

}


void cmd_oid(uint8_t* p_msg, uint8_t msg_len)
{
    if(p_msg[0] == OID_MODE_CFG)
    {
        drv_oid_mode_set(p_msg[1], p_msg[2], p_msg[3]);
    }
    if(p_msg[0] == OID_CMD_CFG)
    {
        oid_transmitcmd(p_msg[1]);
    }
    
    message_normal_response(MESSAGE_RX_OK);
    (void)msg_len;
}

void cmd_led(uint8_t* p_msg, uint8_t msg_len)
{
    if(p_msg[0])
    {
        drv_led_on();
    }
    else
    {
        drv_led_off();
    }
}


void cmd_set_motion_max_speed(uint8_t* p_msg, uint8_t msg_len)
{
    int32_t max_speed;
    
    max_speed = (p_msg[0] << 8) | p_msg[1];
    motor_set_move_speed_max(max_speed);
}

void cmd_cancel(void)
{
    motor_stop(MOTOR_LEFT);
    motor_stop(MOTOR_RIGHT);

}

void long_msg_recive(uint8_t* p_msg, uint8_t msg_len)
{
    
    uint8_t buffer[8];
    uint8_t cnt;

    cnt = 0;
    buffer[cnt++] = CMD_LONG_TRANS;
    buffer[cnt++] = LONG_MSG_RECV_MSG;
    buffer[cnt++] = p_msg[0];
    buffer[cnt++] = p_msg[1];
    buffer[cnt++] = p_msg[2];
    
    long_msg_recv.packet_id = (p_msg[0] << 16) | (p_msg[1] << 8) | p_msg[2];
    /************* recive message **************************/
    long_msg_recv.recive_len += msg_len - 5;

    buffer[cnt++] = true;
    
    message_send(buffer, cnt);
}

void cmd_long_msg_recive(uint8_t* p_msg, uint8_t msg_len)
{
    uint32_t send_len;
    uint8_t buffer[8];
    uint8_t cnt;
    
    switch (p_msg[0])
    {
        case LONG_MSG_RECV_START:
            long_msg_recv.total_len = (p_msg[1] << 24) | (p_msg[2] << 16) | (p_msg[3] << 8) | p_msg[4];
            long_msg_recv.packet_id = 0;
            long_msg_recv.recive_len = 0;
            long_msg_recive_flag = true;
            message_normal_response(MESSAGE_RX_OK);
            break;
        
        case LONG_MSG_RECV_MSG:
            long_msg_recive(&p_msg[1], msg_len);
            break;
        
        case LONG_MSG_RECV_END:
            send_len = (p_msg[1] << 24) | (p_msg[2] << 16) | (p_msg[3] << 8) | p_msg[4];

            if(send_len == long_msg_recv.recive_len)
            {
                /*recive ok*/
            }
            else
            {
                /*recive fail*/
            }
            long_msg_recive_flag = false;
            cnt = 0;
            buffer[cnt++] = CMD_LONG_TRANS;
            buffer[cnt++] = LONG_MSG_RECV_END;
            buffer[cnt++] = (long_msg_recv.recive_len >> 24) & 0xFF;
            buffer[cnt++] = (long_msg_recv.recive_len >> 16) & 0xFF;
            buffer[cnt++] = (long_msg_recv.recive_len >> 8) & 0xFF;
            buffer[cnt++] = (long_msg_recv.recive_len) & 0xFF;
            message_send(buffer, cnt);
            break;
    }
    
}


uint8_t message_parser(void)
{
    uint32_t msg_len;
    uint16_t crc_calc;
    uint16_t crc_recive;
    uint8_t ret;
    
    uint8_t *p_data;
    msg_state_t *p_rx_msg;

    p_rx_msg = &rx_msg;
    p_data = p_rx_msg->buffer;
    msg_len = communication_get_count();
    if(msg_len == 0)
    {
        return MATA_BUSY;
    }

    ret = cmd_pop_msg(p_data);
    if(ret != MATA_OK)
    {
        return MATA_FAIL;
    }
#if 0
    drv_uart_printf(UART0_PORT, "recive msg!\r\n");
    for(ret = 0; ret < 10; ret++)
    {
        drv_uart_printf(UART0_PORT, "0x%x ", p_data[ret]);
    }
    drv_uart_printf(UART0_PORT, "\r\n");
#endif
    p_rx_msg->pfunc = NULL;
    msg_len = p_data[1];
    if(msg_len < 2)
    {
        /*message len error*/
        message_normal_response(MESSAGE_ERROR_INVALID_LENGTH);
        return MATA_FAIL;
    }
    crc_calc = crc16_compute( &p_data[1], msg_len - 1, NULL);
    crc_recive = (p_data[msg_len]<<8) | p_data[msg_len + 1];
    if(crc_calc != crc_recive)
    {
        /*crc error*/
        message_normal_response(MESSAGE_ERROR_CRC);
        return MATA_FAIL;
    }

    switch (p_data[2])
    {
        case CMD_SYSTERM_INFO:
            cmd_system_info_get(&p_data[3], msg_len);
            break;
        
        case CMD_MOTION:
            cmd_motion(&p_data[3], msg_len);
            break;
        
        case CMD_ANIMOTION:
            cmd_animotion(&p_data[3], msg_len);
            break;
            
        case CMD_OID:
            cmd_oid(&p_data[3], msg_len);
            break;
        
#if 0
        case CMD_LED:
            cmd_led(&p_data[3], msg_len);
            break;
#endif

        case CMD_MOTION_MAX_SPEED:
            cmd_set_motion_max_speed(&p_data[3], msg_len);
            break;
        
        case CMD_LONG_TRANS:
            cmd_long_msg_recive(&p_data[3], msg_len);
            break;
        
        case CMD_CANCEL:
            cmd_cancel();
            break;
            
        default:
            break;
        
    }

    return MATA_OK;
}

void message_process(void)
{
    
    //msg_state_t *pRxMsg;
    uint8_t ret;

    //pRxMsg = &rx_msg;
    ret = message_parser();
    if(motion_wait_stop)
    {
        ret = cmd_motion_process();
        if(ret == MATA_OK)
        {
            motion_wait_stop = false;
        }
    }
    (void)ret;
#if 0
    switch (pRxMsg->state)
    {
        case MSG_IDLE:
            ret = message_parser();
            if(ret == MATA_OK)
            {
                pRxMsg->state = MSG_PROCECSSING;
            }
            break;
        
        case MSG_PROCECSSING:
            if(pRxMsg->pfunc == NULL)
            {
                pRxMsg->state = MSG_IDLE;
                return;
            }
            ret = pRxMsg->pfunc();
            if(ret == MATA_OK)
            {
                pRxMsg->state = MSG_IDLE;
                pRxMsg->pfunc = NULL;
                drv_uart_printf(UART0_PORT, "process complete!");
            }
            break;
    }
#endif

    return;
}

void message_gd32_report(void)
{
    static uint32_t time;
    uint8_t msg[16];
    uint8_t len = 0;
    uint32_t key_status = 0;

    if((drv_millis() - time) < MESSAGE_REPORT_PERIOD)
    {
        return;
    }
    time = drv_millis();
    
    msg[len++] = CMD_GD32_REPORT;
    key_status = drv_key_status_all();
    msg[len++] = (key_status >> 8) & 0xFF;
    msg[len++] = (key_status) & 0xFF;

    
    drv_oid_data_get(&msg[len]);
    /*there have 8 byte oid data*/
    len = len + 8;
    message_send(msg, len);
    
    return;
    
}


