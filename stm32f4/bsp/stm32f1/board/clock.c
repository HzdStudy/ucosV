#include "clock.h"


void Clock_Config(void)
{ 
   if (SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC))//配置滴答定时器时钟频率
  { 
    /* Capture error */ 
    while (1);
  }
}


/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
 // TimingDelay_Decrement();
  OSIntEnter();		//进入中断
  OSTimeTick();       //调用ucos的时钟服务程序
  OSIntExit();        //触发任务切换软中断
}

