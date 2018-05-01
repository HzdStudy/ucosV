/*
 * File      : pin.h
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

#ifndef __PIN_H__
#define __PIN_H__

#include "deviceInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

/* pin device and operations for RT-Thread */
struct device_pin
{
    struct device parent;
    const struct pin_ops *ops;
};

#define PIN_LOW                 0x00
#define PIN_HIGH                0x01

#define PIN_MODE_OUTPUT         0x00
#define PIN_MODE_INPUT          0x01
#define PIN_MODE_INPUT_PULLUP   0x02

struct device_pin_mode
{
    uint16_t pin;
    uint16_t mode;
};
struct device_pin_status
{
    uint16_t pin;
    uint16_t status;
};

struct pin_ops
{
    void (*pin_mode)(struct device *device, base_t pin, base_t mode);
    void (*pin_write)(struct device *device, base_t pin, base_t value);
    int (*pin_read)(struct device *device, base_t pin);

    /* TODO: add GPIO interrupt */
};

int  device_pin_register(const char *name, const struct pin_ops *ops, void *user_data);

void pin_mode(base_t pin, base_t mode);
void pin_write(base_t pin, base_t value);
int  pin_read(base_t pin);

#ifdef __cplusplus
}
#endif

#endif
