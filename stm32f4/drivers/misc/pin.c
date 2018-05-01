/*
 * File      : pin.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
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
 * 2015-01-20     Bernard      the first version
 */

#include "pin.h"

static struct device_pin _hw_pin;

static size_t _pin_read(device_t dev, off_t pos, void *buffer, size_t size)
{
    struct device_pin_status *status;
    struct device_pin *pin = (struct device_pin *)dev;


    status = (struct device_pin_status *) buffer;
    if (status == NULL || size != sizeof(*status)) return 0;

    status->status = pin->ops->pin_read(dev, status->pin);
    return size;
}

static size_t _pin_write(device_t dev, off_t pos, const void *buffer, size_t size)
{
    struct device_pin_status *status;
    struct device_pin *pin = (struct device_pin *)dev;

    /* check parameters */
    //ASSERT(pin != RT_NULL);

    status = (struct device_pin_status *) buffer;
    if (status == NULL || size != sizeof(*status)) return 0;

    pin->ops->pin_write(dev, (base_t)status->pin, (base_t)status->status);

    return size;
}

static err_t  _pin_control(device_t dev, uint8_t cmd, void *args)
{
    struct device_pin_mode *mode;
    struct device_pin *pin = (struct device_pin *)dev;

    /* check parameters */
   // ASSERT(pin != RT_NULL);

    mode = (struct device_pin_mode *) args;
    if (mode == NULL) return -ERROR;

    pin->ops->pin_mode(dev, (base_t)mode->pin, (base_t)mode->mode);

    return 0;
}

int device_pin_register(const char *name, const struct pin_ops *ops, void *user_data)
{
    _hw_pin.parent.type         = Device_Class_Miscellaneous;
    _hw_pin.parent.rx_indicate  = NULL;
    _hw_pin.parent.tx_complete  = NULL;

    _hw_pin.parent.init         = NULL;
    _hw_pin.parent.open         = NULL;
    _hw_pin.parent.close        = NULL;
    _hw_pin.parent.read         = _pin_read;
    _hw_pin.parent.write        = _pin_write;
    _hw_pin.parent.control      = _pin_control;

    _hw_pin.ops                 = ops;
    _hw_pin.parent.user_data    = user_data;

    /* register a character device */
    device_register(&_hw_pin.parent, name, DEVICE_FLAG_RDWR);

    return 0;
}

