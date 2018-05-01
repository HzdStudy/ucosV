#include "configs.h"
#include "includes.h" 	 
#include "dfs_posix.h"

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

rt_device_t uart_device = RT_NULL;

int32_t uart_fd = -1;

int main(void)
{
  Clock_Config();
  
//  USART_Config(115200);
  rt_hw_usart_init();

  dfs_init();//
  devfs_init();
  if(dfs_mount(RT_NULL, "/dev", "devfs", 0, 0) == 0)
  	printf("Ok");
  else
  	printf("Fail");

  uart_fd = open("/dev/uart1", 0, 0);
  
  printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");

  OSInit();  	 			//��ʼ��ucosII			  
  OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
  OSStart();	 
}


//��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//��ʼ��ͳ�������������ʱ1������
 	OS_ENTER_CRITICAL();			//�����ٽ���
 	OSTaskCreate(print_task,(void *)0,(OS_STK*)&PRINT_TASK_STK[PRINT_STK_SIZE-1],PRINT_TASK_PRIO);	
    OSTaskCreate(print2_task,(void *)0,(OS_STK*)&PRINT2_TASK_STK[PRINT2_STK_SIZE-1],PRINT2_TASK_PRIO);	
 	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����
	OS_EXIT_CRITICAL();				//�˳��ٽ���
}	  

//��ӡ����
void print_task(void *pdata)
{   
  while(1)
  {  
    OSTimeDlyHMSM(0,0,0, 500);
    printf("delay 500 ms\r\n");
  }									 
}	  

//��ӡ����
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
	if(uart_fd > 0)
		write(uart_fd, &ch, 1);
    
    return ch;
} 


