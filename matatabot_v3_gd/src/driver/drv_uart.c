#include "gd32f3x0.h"
#include <stdio.h>
#include "string.h"
#include <stdarg.h>
#include "gd32f3x0_gpio.h"
#include "gd32f3x0_misc.h"
#include "stdlib.h"
#include "communication_link.h"
#include "drv_ring_buf.h"
#include "drv_uart.h"

#define DATA_TX_BUFSIZE      128
static RING_BUF_DEF_STRUCT s_tx_ring_buf;
volatile uint8_t txcount = 0; 
static uint8_t s_link_tx_buf[DATA_TX_BUFSIZE];





void drv_uart0_init(uint32_t baud_rate)
{   
//    drv_ringbuf_init((RING_BUF_DEF_STRUCT*)&s_tx_ring_buf, s_link_tx_buf, DATA_TX_BUFSIZE);
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_6);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_7);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_6);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_7);

    /* USART configure */
    usart_deinit(USART0);
		usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, baud_rate);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_overrun_disable(USART0);
    usart_enable(USART0);
    usart_receive_fifo_enable(USART0);
    nvic_irq_enable(USART0_IRQn, 1, 0);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}




void drv_uart1_init(uint32_t baud_rate)
{   
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_8);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_15);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_8);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_15);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    /* USART configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, baud_rate);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    
    usart_overrun_disable(USART1);
    usart_enable(USART1);   
    usart_receive_fifo_enable(USART1);
    nvic_irq_enable(USART1_IRQn, 1, 0);
    usart_interrupt_enable(USART1, USART_INT_RBNE);
}


void drv_uart_write_byte(uint8_t port, uint8_t inputData)
{
    uint32_t com;
    if(port == UART0_PORT)
    {
        com = USART0;
    }
    else if(port == UART1_PORT)
    {
        com = USART1;
    }
    usart_data_transmit(com, inputData);
    while(RESET == usart_flag_get(com, USART_FLAG_TBE));
}

void send_string(uint8_t *str, uint8_t len)
{
   drv_ringbuf_write(&s_tx_ring_buf, str, len);
   usart_interrupt_enable(USART0, USART_INT_TBE);
}

void drv_uart_printf(uint8_t port, char *fmt,...)
{
    uint32_t com;

    if(port == UART0_PORT)
    {
      com = USART0;
    }
    else if(port == UART1_PORT)
    {
      com = USART1;
    }
    va_list ap;
    char string[128]; 
    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    send_string((uint8_t*)string, strlen(string));
    usart_interrupt_enable(com, USART_INT_TBE);
    va_end(ap);
    (void)com;
}
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t) ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
//void USART0_IRQHandler(void)
//{
//    uint8_t receive_data = 0xff;
//    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
//    {
//        receive_data = usart_data_receive(USART0);
//        communication_write_data(receive_data);
//    }
//    else if(RESET != usart_flag_get(USART0, USART_FLAG_ORERR))
//    {
//        usart_flag_clear(USART0, USART_FLAG_ORERR); 
//    }
//		if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE))
//		{
//			  drv_ringbuf_read((RING_BUF_DEF_STRUCT*)&s_tx_ring_buf, 1, s_link_tx_buf);
//			  usart_data_transmit(USART0, s_link_tx_buf[txcount++]);
//			  if(drv_ringbuf_count((RING_BUF_DEF_STRUCT*)&s_tx_ring_buf) == 0)
//				{
//					txcount = 0;
//					usart_interrupt_disable(USART0, USART_INT_TBE);
//					drv_ringbuf_flush((RING_BUF_DEF_STRUCT*)&s_tx_ring_buf);
//				}
//		}
//}



int32_t a;
extern int32_t Data[];
void USART0_IRQHandler(void)
{
	//printf("a\t\n");
   if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE)){      
			a = usart_data_receive(USART0);
			usart_data_transmit(USART0, a);
      usart_interrupt_disable(USART0, USART_INT_RBNE);
				 
       }
    

    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TBE))
			{
				
            usart_interrupt_disable(USART0, USART_INT_TBE);
      }
    }

void drv_uart_tx_rx(void)
{
	
//	 nvic_irq_enable(USART0_IRQn, 0, 0);
//	 usart_interrupt_enable(USART0, USART_INT_TBE);
//	 while(RESET == usart_flag_get(USART0, USART_FLAG_TC))
//    usart_interrupt_enable(USART0, USART_INT_RBNE);
 while(RESET == usart_flag_get(USART0, USART_FLAG_TC)); 
    usart_interrupt_enable(USART0, USART_INT_RBNE);
	
}
