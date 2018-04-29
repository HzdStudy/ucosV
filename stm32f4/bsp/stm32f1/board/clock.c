#include "clock.h"


void Clock_Config(void)
{ 
   if (SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC))//���õδ�ʱ��ʱ��Ƶ��
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
  OSIntEnter();		//�����ж�
  OSTimeTick();       //����ucos��ʱ�ӷ������
  OSIntExit();        //���������л����ж�
}

