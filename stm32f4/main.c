#include "configs.h"
#include "includes.h" 	 

RCC_ClocksTypeDef RCC_Clocks;


extern  uint32_t SystemCoreClock;

#define START_TASK_PRIO      			10 
#define START_STK_SIZE  				64
OS_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *pdata);	


#define PRINT_TASK_PRIO       			7 
#define PRINT_STK_SIZE  		   		64
OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
void print_task(void *pdata);

#define PRINT2_TASK_PRIO       			6 
#define PRINT2_STK_SIZE  		    	64
OS_STK PRINT2_TASK_STK[PRINT2_STK_SIZE];
void print2_task(void *pdata);

device_t uart_device = NULL;
device_t pin_devic   = NULL;

int main(void)
{
  struct device_pin_status  _pin_status;
    
  Clock_Config();
  
  device_listInit();
  
//  USART_Config(115200);
  rt_hw_usart_init();
  stm32_hw_pin_init();



  uart_device = device_find("uart1");
  if(uart_device != NULL)//不加也可以，下层接口对设备做了判断
    device_open(uart_device, DEVICE_OFLAG_RDWR);
    
  printf("\r\nUSART Printf Example: retarget the C library printf function to the USART\r\n");
  
  pin_devic = device_find("pin");
  if(pin_devic != NULL)
    device_open(pin_devic, DEVICE_OFLAG_RDWR);
  _pin_status.pin = 20;//PA4
  _pin_status.status =  PIN_LOW;
  device_write(pin_devic, 0, &_pin_status, sizeof(_pin_status));
  
  

  OSInit();  	 			//初始化ucosII			  
  OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
  OSStart();	 
}


//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//初始化统计任务，这里会延时1秒左右
 	OS_ENTER_CRITICAL();			//进入临界区
 	OSTaskCreate(print_task,(void *)0,(OS_STK*)&PRINT_TASK_STK[PRINT_STK_SIZE-1],PRINT_TASK_PRIO);	
    OSTaskCreate(print2_task,(void *)0,(OS_STK*)&PRINT2_TASK_STK[PRINT2_STK_SIZE-1],PRINT2_TASK_PRIO);	
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务
	OS_EXIT_CRITICAL();				//退出临界区
}	  

//打印任务
void print_task(void *pdata)
{   
  while(1)
  {  
    OSTimeDlyHMSM(0,0,0, 500);
    printf("delay 500 ms\r\n");
  }									 
}	  

//打印任务
void print2_task(void *pdata)
{   
  while(1)
  {  
    OSTimeDlyHMSM(0,0,0, 1000);
    printf("delay 1000 ms\r\n");
  }									 
}	


int fputc(int ch, FILE *f) 
{ 
	if(uart_device != NULL)
      device_write(uart_device, 0, &ch, 1);
    return ch;
} 


