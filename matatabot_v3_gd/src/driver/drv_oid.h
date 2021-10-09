


#ifndef __DRV_OID_H__
#define __DRV_OID_H__

#define OID_SCK_PIN             (PB_4)
#define OID_DATA_PIN            (PB_5)
#define OID_RST_PIN             (PB_2)

#define OID_RESPONSE_DATA_BIT   (64)
#define OID_RESPONSE_ACK_BIT    (16)

#define OID_PLUSE_TIME          (25)
#define OID_SEND_ACK_TIMEOUT    (300)
#define OID_SEND_WAIT_ACK_TIME_MAX ((OID_SEND_ACK_TIMEOUT+OID_PLUSE_TIME-1)/OID_PLUSE_TIME)

#define OID_END_LAST_TIME_MIN   (78)
#define OID_END_LAST_TIME_INDEX ((OID_END_LAST_TIME_MIN+OID_PLUSE_TIME-1)/OID_PLUSE_TIME)

#define OID_RETRANSMIT_TIME_MAX  (3)

#define OID_REPORT_PEROID        (20)

/*unit ms*/
#define OID_VALID_LAST_TIME      (500)

typedef enum{
    MODE_TALKING_PEN = 0x01,
    MODE_HANDWRITING_PEN,
}OID_MODE_E;

typedef enum{
    OID_RECIVE_DATA = 0,
    OID_RECIVE_ACK,
}OID_RECIVE_TYPE_E;

typedef enum{
    REPORT_CONTINUES = 0x01,
    REPORT_WHEN_CHANGE = 0x02,
}OID_REPORT_E;

typedef enum{
    OID_MIN_FRAME_RATE_5 = 0x0,
    OID_MIN_FRAME_RATE_20 = 0x1,
    OID_MIN_FRAME_RATE_30 = 0x2,
    OID_MIN_FRAME_RATE_40 = 0x3,
    OID_MIN_FRAME_RATE_50 = 0x4,
        
}OID_FLUSH_FREQ_E;


typedef enum{
    CMD_OUTPUTANGLEENABLE = 0X10,
    CMD_OUTPUTANGLEDISABLE = 0X11,
    CMD_POSITIONENABLE = 0X35,
    CMD_POSITIONDISABLE = 0X36,
    CMD_OIDENTERSUSPENDMODE = 0X57,
    CMD_MINFRAMERATE_5 = 0X21,
    CMD_MINFRAMERATE_20 = 0X24,
    CMD_MINFRAMERATE_30 = 0X25,
    CMD_MINFRAMERATE_40 = 0X26,
    CMD_MINFRAMERATE_50 = 0X27,
    CMD_AUTOSWITCHTOPOSENABLE = 0X43,
    CMD_AUTOSWITCHTOPOSDISABLE = 0X44,
    CMD_AUTOSWITCHTOGENENABLE = 0X45,
    CMD_AUTOSWITCHTOGENDISABLE = 0X46,
    CMD_ENABLEPOSITIONWORKINGMODE = 0X40,
    CMD_DISABLEPOSITIONWORKINGMODE = 0XB0,

}OID_CMD_E;


typedef struct{
    uint8_t sck_pin;
    uint8_t sdio_pin;
    uint8_t mode;
    uint8_t status;
    
    uint64_t recive_data;
    
    uint8_t send_data;
    uint8_t oid_transmit_cnt;
    uint8_t oid_retrans_flag;
    uint8_t rst_pin;
    uint32_t recive_time;
}oid_status_t;

typedef enum{
    OID_IDLE = 0,
    OID_PROC_SEND,
    OID_PROC_RECIVE_DATA,
    OID_PROC_RECIVE_ACK,
    OID_PROC_WAIT_ACK,
}OID_PROCESS_TYPE_E;

typedef enum{
    OID_PROCESS_NULL = 0,
    OID_PROCESS_START,
    OID_PROCESS_DOING,
    OID_PROCESS_END,
    OID_PROCESS_WAIT,
}OID_PROCESS_STATE_E;

typedef struct{
    uint8_t oid_isr_loop;
    uint8_t oid_isr_state;
    uint8_t oid_done_flag;
}oid_proc_state;


void drv_oid_recive_ack(void);
void drv_oid_recive_data(void);
void drv_oid_transmitcmd(uint8_t cmd);
uint8_t drv_oid_mode_set(uint8_t mode, uint8_t report_mode, uint8_t freq);
uint8_t drv_oid_wakeup(void);
void oid_recive_isr(void);
void oid_send_isr(void);
void oid_process_isr(void);
void drv_oid_data_get(uint8_t *pdata);
void drv_oid_init(void);
void oid_transmitcmd(uint8_t cmd);
void oid_normal_presetting(void);
void oid_task_loop(void);

#endif
