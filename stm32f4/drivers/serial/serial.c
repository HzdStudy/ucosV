/*
 * File      : serial.c
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
 * 2006-03-13     bernard      first version
 * 2012-05-15     lgnq         modified according bernard's implementation.
 * 2012-05-28     bernard      code cleanup
 * 2012-11-23     bernard      fix compiler warning.
 * 2013-02-20     bernard      use SERIAL_RB_BUFSZ to define
 *                             the size of ring buffer.
 * 2014-07-10     bernard      rewrite serial framework
 * 2014-12-31     bernard      use open_flag for poll_tx stream mode.
 * 2015-05-19     Quintin      fix DMA tx mod tx_dma->activated flag !=FALSE BUG 
 *                             in open function.
 */

//#include <rthw.h>
//l#include <rtthread.h>
//#include <rtdevice.h>
#include "serial.h"

/*
 * Serial poll routines 
 */
int _serial_poll_rx(struct serial_device *serial, uint8_t *data, int length)
{
    int ch;
    int size;
    
    //ASSERT(serial != NULL);
    size = length;

    while (length)
    {
        ch = serial->ops->getc(serial);
        *data = ch; 
        data ++; length --;

        if (ch == '\n') break;
    }

    return size - length;
}

int _serial_poll_tx(struct serial_device *serial, const uint8_t *data, int length)
{
    int size;
    //ASSERT(serial != NULL);

    size = length;
    while (length)
    {
        /*
         * to be polite with serial console add a line feed
         * to the carriage return character
         */
        if (*data == '\n' && (serial->parent.open_flag & DEVICE_FLAG_STREAM))
        {
            serial->ops->putc(serial, '\r');
        }
    
        serial->ops->putc(serial, *data);
    
        ++ data;
        -- length;
    }

    return size - length;
}

/*
 * Serial interrupt routines
 */
int _serial_int_rx(struct serial_device *serial, uint8_t *data, int length)
{
    int size;
    size = length; 
    
    /* read from software FIFO */
    while (length)
    {
        int ch;

        if (serial->get_index != serial->put_index)
        {
            ch = serial->serial_rx[serial->get_index];
            serial->get_index += 1;
            if (serial->get_index >= serial->config.bufsz) serial->get_index = 0;
        }
        else
        {
            break;
        }

        *data = ch & 0xff;
        data ++; length --;
    }

    return size - length;
}

int _serial_int_tx(struct serial_device *serial, const uint8_t *data, int length)
{
    int size;
    //struct serial_tx_fifo *tx;
    
    //ASSERT(serial != NULL);

    size = length;
    //tx = (struct serial_tx_fifo*) serial->serial_tx;
    //ASSERT(tx != NULL);

    while (length)
    {
        if (serial->ops->putc(serial, *(char*)data) == -1)
        {
            //completion_wait(&(tx->completion), WAITING_FOREVER);
            continue;
        }

        data ++; length --;
    }

    return size - length;
}


/* RT-Thread Device Interface */
/*
 * This function initializes serial device.
 */
static err_t serial_init(struct device *dev)
{
    err_t result = EOK;
    struct serial_device *serial;

    //ASSERT(dev != NULL);
    serial = (struct serial_device *)dev;

    /* initialize tx */
    serial->serial_tx = NULL;

    /* apply configuration */
    if (serial->ops->configure)
        result = serial->ops->configure(serial, &serial->config);

    return result;
}

static err_t serial_open(struct device *dev, uint16_t oflag)
{
    struct serial_device *serial;

    //ASSERT(dev != NULL);
    serial = (struct serial_device *)dev;

    /* check device flag with the open flag */
    if ((oflag & DEVICE_FLAG_DMA_RX) && !(dev->flag & DEVICE_FLAG_DMA_RX)) 
        return -EIO;
    if ((oflag & DEVICE_FLAG_DMA_TX) && !(dev->flag & DEVICE_FLAG_DMA_TX))
        return -EIO;
    if ((oflag & DEVICE_FLAG_INT_RX) && !(dev->flag & DEVICE_FLAG_INT_RX))
        return -EIO;
    if ((oflag & DEVICE_FLAG_INT_TX) && !(dev->flag & DEVICE_FLAG_INT_TX))
        return -EIO;

    /* get open flags */
    dev->open_flag = oflag & 0xff;
    
    /* initialize the Rx/Tx structure according to open flag */
    if (serial->serial_rx == NULL)
    {
        if (oflag & DEVICE_FLAG_INT_RX)
        {
            serial->put_index = 0;
            serial->get_index = 0;//初始化计数器

            dev->open_flag |= DEVICE_FLAG_INT_RX;
            /* configure low level device */
            serial->ops->control(serial, DEVICE_CTRL_SET_INT, (void *)DEVICE_FLAG_INT_RX);
        }
        else
        {
            serial->serial_rx = NULL;
        }
    }

    return EOK;
}

static err_t serial_close(struct device *dev)
{
    struct serial_device *serial;

    //ASSERT(dev != NULL);
    serial = (struct serial_device *)dev;

    /* this device has more reference count */
    if (dev->ref_count > 1) return EOK;
    
    if (dev->open_flag & DEVICE_FLAG_INT_RX)
    {
        dev->open_flag &= ~DEVICE_FLAG_INT_RX;
        /* configure low level device */
        serial->ops->control(serial, DEVICE_CTRL_CLR_INT, (void*)DEVICE_FLAG_INT_TX);
    }


    return EOK;
}

static size_t serial_read(struct device *dev,
                                off_t          pos,
                                void             *buffer,
                                size_t         size)
{
    struct serial_device *serial;

    //ASSERT(dev != NULL);
    if (size == 0) return 0;

    serial = (struct serial_device *)dev;

    if (dev->open_flag & DEVICE_FLAG_INT_RX)
    {
        return _serial_int_rx(serial, buffer, size);
    }

    return _serial_poll_rx(serial, buffer, size);
}

static size_t serial_write(struct device *dev,
                                 off_t          pos,
                                 const void       *buffer,
                                 size_t         size)
{
    struct serial_device *serial;

    //ASSERT(dev != NULL);
    if (size == 0) return 0;

    serial = (struct serial_device *)dev;

    if (dev->open_flag & DEVICE_FLAG_INT_TX)
    {
        return _serial_int_tx(serial, buffer, size);
    }
    else
    {
        return _serial_poll_tx(serial, buffer, size);
    }
}

static err_t serial_control(struct device *dev,
                                  uint8_t        cmd,
                                  void             *args)
{
    struct serial_device *serial;

    //ASSERT(dev != NULL);
    serial = (struct serial_device *)dev;

    switch (cmd)
    {
        case DEVICE_CTRL_SUSPEND:
            /* suspend device */
            dev->flag |= DEVICE_FLAG_SUSPENDED;
            break;

        case DEVICE_CTRL_RESUME:
            /* resume device */
            dev->flag &= ~DEVICE_FLAG_SUSPENDED;
            break;

        case DEVICE_CTRL_CONFIG:
            /* configure device */
            serial->ops->configure(serial, (struct serial_configure *)args);
            break;

        default :
            /* control device */
            serial->ops->control(serial, cmd, args);
            break;
    }

    return EOK;
}

/*
 * serial register
 */
err_t hw_serial_register(struct serial_device *serial,
                               const char              *name,
                               uint32_t              flag,
                               void                    *data)
{
    struct device *device;
    //ASSERT(serial != NULL);

    device = &(serial->parent);

    device->type        = Device_Class_Char;
    device->rx_indicate = NULL;
    device->tx_complete = NULL;

    device->init        = serial_init;
    device->open        = serial_open;
    device->close       = serial_close;
    device->read        = serial_read;
    device->write       = serial_write;
    device->control     = serial_control;
    device->user_data   = data;

    /* register a character device */
    return device_register(device, name, flag);
}

/* ISR for serial interrupt */
void hw_serial_isr(struct serial_device *serial, int event)
{
    switch (event & 0xff)
    {
        case SERIAL_EVENT_RX_IND:
        {
            int ch = -1;
            /* interrupt mode receive */

            while (1)
            {
                ch = serial->ops->getc(serial);
                if (ch == -1) break;

                
                /* disable interrupt */
                //level = hw_interrupt_disable();
                
                serial->serial_rx[serial->put_index] = ch;
                serial->put_index += 1;
                if (serial->put_index >= serial->config.bufsz) serial->put_index = 0;
                
                /* if the next position is read index, discard this 'read char' */
                if (serial->put_index == serial->get_index)
                {
                    serial->get_index += 1;
                    if (serial->get_index >= serial->config.bufsz) serial->get_index = 0;
                }
                
                /* enable interrupt */
                //hw_interrupt_enable(level);
            }
            
            /* invoke callback */
            if (serial->parent.rx_indicate != NULL)
            {
                size_t rx_length;
            
                /* get rx length */
                //level = hw_interrupt_disable();
                rx_length = (serial->put_index >= serial->get_index)? (serial->put_index - serial->get_index):
                    (serial->config.bufsz - (serial->get_index - serial->put_index));
               // hw_interrupt_enable(level);

                serial->parent.rx_indicate(&serial->parent, rx_length);
            }
            break;
        }
        case SERIAL_EVENT_TX_DONE:
        {
           /* struct serial_tx_fifo* tx_fifo;

            tx_fifo = (struct serial_tx_fifo*)serial->serial_tx;
            //completion_done(&(tx_fifo->completion));
           */
            break;
        }
  
      
    }
}
