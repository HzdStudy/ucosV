/*
 * File      : serial.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-05-15     lgnq         first version.
 * 2012-05-28     bernard      change interfaces
 * 2013-02-20     bernard      use SERIAL_RB_BUFSZ to define
 *                             the size of ring buffer.
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "deviceInterface.h"

#define BAUD_RATE_2400                  2400
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_38400                 38400
#define BAUD_RATE_57600                 57600
#define BAUD_RATE_115200                115200
#define BAUD_RATE_230400                230400
#define BAUD_RATE_460800                460800
#define BAUD_RATE_921600                921600

#define DATA_BITS_5                     5
#define DATA_BITS_6                     6
#define DATA_BITS_7                     7
#define DATA_BITS_8                     8
#define DATA_BITS_9                     9

#define STOP_BITS_1                     0
#define STOP_BITS_2                     1
#define STOP_BITS_3                     2
#define STOP_BITS_4                     3

#define PARITY_NONE                     0
#define PARITY_ODD                      1
#define PARITY_EVEN                     2

#define BIT_ORDER_LSB                   0
#define BIT_ORDER_MSB                   1

#define NRZ_NORMAL                      0       /* Non Return to Zero : normal mode */
#define NRZ_INVERTED                    1       /* Non Return to Zero : inverted mode */


#define SERIAL_EVENT_RX_IND          0x01    /* Rx indication */
#define SERIAL_EVENT_TX_DONE         0x02    /* Tx complete   */
#define SERIAL_EVENT_RX_DMADONE      0x03    /* Rx DMA transfer done */
#define SERIAL_EVENT_TX_DMADONE      0x04    /* Tx DMA transfer done */
#define SERIAL_EVENT_RX_TIMEOUT      0x05    /* Rx timeout    */

#define SERIAL_DMA_RX                0x01
#define SERIAL_DMA_TX                0x02

#define SERIAL_RX_INT                0x01
#define SERIAL_TX_INT                0x02

#define SERIAL_ERR_OVERRUN           0x01
#define SERIAL_ERR_FRAMING           0x02
#define SERIAL_ERR_PARITY            0x03

#define SERIAL_TX_DATAQUEUE_SIZE     2048
#define SERIAL_TX_DATAQUEUE_LWM      30

struct serial_configure
{
    uint32_t baud_rate;

    uint32_t data_bits               :4;
    uint32_t stop_bits               :2;
    uint32_t parity                  :2;
    uint32_t bit_order               :1;
    uint32_t invert                  :1;
	uint32_t bufsz					:16;
    uint32_t reserved                :4;
};

struct serial_device
{
    struct device             parent;

    const struct uart_ops     *ops;
    struct serial_configure   config;

	uint8_t  *serial_rx;
	uint16_t put_index, get_index;
	void     *serial_tx;
};
typedef struct serial_device serial_t;

/**
 * uart operators
 */
struct uart_ops
{
    err_t (*configure)(struct serial_device *serial, struct serial_configure *cfg);
    err_t (*control)(struct serial_device *serial, int cmd, void *arg);

    int (*putc)(struct serial_device *serial, char c);
    int (*getc)(struct serial_device *serial);

    //size_t (*dma_transmit)(struct serial_device *serial, const uint8_t *buf, size_t size, int direction);
};

void hw_serial_isr(struct serial_device *serial, int event);

err_t hw_serial_register(struct serial_device *serial,
                               const char              *name,
                               uint32_t              flag,
                               void                    *data);

#endif
