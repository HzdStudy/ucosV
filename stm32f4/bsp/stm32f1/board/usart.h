#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include <stdio.h>
#include "serial.h"


#define USING_UART1
#define USING_UART2
#define USING_UART3



#define SERIAL1_RB_BUFSZ              64
#define SERIAL2_RB_BUFSZ              64
#define SERIAL3_RB_BUFSZ              64
#define SERIAL4_RB_BUFSZ              64

/* Default config for serial_configure structure */
#define SERIAL_CONFIG_DEFAULT           \
{                                          \
    BAUD_RATE_115200, /* 115200 bits/s */  \
    DATA_BITS_8,      /* 8 databits */     \
    STOP_BITS_1,      /* 1 stopbit */      \
    PARITY_NONE,      /* No parity  */     \
    BIT_ORDER_LSB,    /* LSB first sent */ \
    NRZ_NORMAL,       /* Normal mode */    \
    0,                /* Buffer size */  \
    0                                      \
}



#define UART_ENABLE_IRQ(n)            NVIC_EnableIRQ((n))
#define UART_DISABLE_IRQ(n)           NVIC_DisableIRQ((n))

void rt_hw_usart_init(void);

#endif
