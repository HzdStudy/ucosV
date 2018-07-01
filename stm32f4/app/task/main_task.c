#include "_task_include.h"
#include "uart_task.h"

//���������ջ
#if(USE_DEBUG_TASK == 1)
OS_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];
#endif
#if(USE_UART_TASK == 1)
OS_STK UART_TASK_STK[UART_STK_SIZE];
#endif
OS_STK START_TASK_STK[START_STK_SIZE];

//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
  pdata = pdata; 		  		 			  
  OSStatInit();					//��ʼ��ͳ�������������ʱ1������
  OS_ENTER_CRITICAL();			//�����ٽ���
      
#if(USE_UART_TASK == 1)
  OSTaskCreate(uart_task,(void *)0,(OS_STK*)&UART_TASK_STK[UART_STK_SIZE-1],UART_TASK_PRIO);	
#endif
  
  OSTaskSuspend(START_TASK_PRIO);	//������ʼ����
  OS_EXIT_CRITICAL();				//�˳��ٽ���
}	  
