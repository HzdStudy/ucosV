#ifndef __TASK_INCLUDE_H
#define __TASK_INCLUDE_H

#include "configs.h"

//�����������ȼ�

#define START_TASK_PRIO      			6 
#define UART_TASK_PRIO      			8 
#define DEBUG_TASK_PRIO      			10 

//���������ջ��С
#define START_STK_SIZE  				64
#define UART_STK_SIZE  				    128
#define DEBUG_STK_SIZE  				64

//���������ջ
#if(USE_DEBUG_TASK == 1)
extern OS_STK DEBUG_TASK_STK[];
#endif
#if(USE_UART_TASK == 1)
extern OS_STK UART_TASK_STK[];
#endif
extern OS_STK START_TASK_STK[];



void start_task(void *pdata);	

#endif