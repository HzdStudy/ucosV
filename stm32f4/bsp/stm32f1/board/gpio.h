/*
 * File      : gpio.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-01-05     Bernard      the first version
 */
#ifndef __GPIO_H__
#define __GPIO_H__

#include "stm32f10x.h"
#include <stdio.h>
#include "pin.h"

#define USING_PIN 

int stm32_hw_pin_init(void);

#endif

