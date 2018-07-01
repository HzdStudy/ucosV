#include "uart_task.h"

#if(USE_UART_TASK == 1)

UART_TASK_DATA  uart_task_data;

//开始任务
void uart_task(void *pdata)
{
  uart_task_data._uart1_device = NULL;
 
  uart_task_data._uart1_device = device_find("uart1");
  if(uart_task_data._uart1_device != NULL)//不加也可以，下层接口对设备做了判断
    device_open(uart_task_data._uart1_device, DEVICE_OFLAG_RDWR);
  
  printf("uart_task app\r\n");
  
  while(1)
  {
    OSTimeDlyHMSM(0, 0, 0, 1000);
    printf("uart_task test\r\n");
  }
}	  


int fputc(int ch, FILE *f) 
{ 
	if(uart_task_data._uart1_device != NULL)
      device_write(uart_task_data._uart1_device, 0, &ch, 1);
    return ch;
} 

#endif