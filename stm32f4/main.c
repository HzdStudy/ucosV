#include "configs.h"
#include "includes.h" 	 
#include "_task_Include.h"


int main(void)
{
  board_init();
  
  OSInit();  	 			//初始化ucosII			  
  OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
  OSStart();	 
}




