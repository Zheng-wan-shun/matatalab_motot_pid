

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MSG_CRC_LEN              (2)
#define MSG_TOTAL_LEN_MIN        (5)

#define MSG_HEAD_LEN                      (0x02)
#define MSG_FRAMER_HEADER                 (0XFE)
#define MSG_FRAMER_TRANSLATION            (0xFD)
#define CHAR_TRANSLATION_OFFSET           (0x20)


#define MESSAGE_RX_OK                           (0)
#define MESSAGE_ERROR_CRC                       (1)
#define MESSAGE_ERROR_BUFFER_OVERFLOW           (2)
#define MESSAGE_ERROR_NOT_SUPPORTED_CMD         (3)
#define MESSAGE_ERROR_INVALID_PARAM             (4)
#define MESSAGE_ERROR_INVALID_LENGTH            (5)
#define MESSAGE_ERROR_TIMEOUT                   (6)
#define MESSAGE_ERROR_CONTROL_MODE_ERROR        (7)

#define CMD_SYSTERM_INFO_LEN          (4)
#define CMD_READ_SN_LEN               (4)
#define CMD_MOTION_LEN                (6)
#define CMD_ANIMOTION_LEN             
#define CMD_CANCEL_LEN                (3)
#define CMD_PLAY_LEN                  (3)
#define CMD_HEART_LEN                 (4)
#define CMD_RESPONSE_LEN              (4)

#define MOTION_DIRECTION_POSITIVE    (0x01)
#define MOTION_DIRECTION_NEGATIVE    (0x02)

#define MESSAGE_REPORT_PERIOD         (20)

#define MSG_OK         (0)
#define MSG_FAIL       (1)
#define MSG_BUSY       (2)

#define MSG_BUFFER_LEN        (140)
#define MSG_TIMEOUT_DEFAULT  (4000)

typedef enum{
    SYSTEM_INFO_VERSION = 0X01,
    SYSYTEM_INFO_MAC,
    SYSTEM_INFO_PROTOCOL,
    SYSTEM_INFO_DEVICE_ID,

}SYSTEM_INFO_E;


typedef enum{
    MOTION_FORWARD = 0x01,
    MOTION_BACKWARD,
    MOTION_TURN_LEFT,
    MOTION_TURN_RIGHT,
}MOTION_TYPE_E;

typedef enum{
    ANIMOTION_SPEED_LEFT = 0x01,
    ANIMOTION_SPEED_RIGHT = 0x02,
    ANIMOTION_SPEED_BOTH = 0x03,

}ANIMOTION_TYPE_E;

typedef enum{
    OID_MODE_CFG = 0x01,
    OID_CMD_CFG  = 0x02,
    OID_DATA_REPORT = 0x10,

}OID_CFG_E;


typedef union{
    struct{
        uint8_t low_byte;
        uint8_t mlow_byte;
        uint8_t mhigh_byte;
        uint8_t high_byte;

    }float_byte;
    float value;
}FLOAT_UNION;


typedef enum {
    MSG_IDLE = 0,
    MSG_PROCECSSING,
}MSG_STATE_E;

typedef struct{
    uint32_t total_len;
    uint32_t packet_id;
    uint32_t recive_len;
}long_msg_recive_s;


typedef enum{
    LONG_MSG_RECV_START = 0x01,
    LONG_MSG_RECV_MSG,
    LONG_MSG_RECV_END,
}LONG_MSG_RECV_E;


typedef enum{
    CMD_SYSTERM_INFO = 0x01,

    CMD_MOTION = 0x10,
    CMD_ANIMOTION = 0x11,
    
    CMD_OID = 0x40,
    CMD_LED = 0x41,
    CMD_GD32_REPORT = 0x42,

    CMD_MOTION_MAX_SPEED = 0x44,
    
    CMD_LONG_TRANS = 0x81,
    
    CMD_CANCEL = 0X84,
    
    CMD_RESPONSE = 0x88,
}COMMEND_TYPE_E;



typedef struct{

    uint8_t state;
    uint8_t msg_len;
    uint8_t factory_flag;
    uint8_t reserve;
    uint8_t buffer[MSG_BUFFER_LEN];
    uint8_t (* pfunc)(void);
}msg_state_t;

void message_init(void);
void message_send(uint8_t *p_msg, uint8_t msg_len);
void message_normal_response( uint8_t error_type);
uint8_t message_is_rx_long_trans(void);
void cmd_cancel(void);

void message_process(void);
void message_gd32_report(void);

#endif

