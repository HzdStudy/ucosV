#ifndef __UART_TASK_H
#define __UART_TASK_H

#include "configs.h"

#if(USE_UART_TASK == 1)

typedef struct
{
  device_t _uart1_device;
  
}UART_TASK_DATA;


void uart_task(void *pdata);


#endif

#endif
