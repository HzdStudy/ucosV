#include "clock.h"


void Clock_Config(void)
{ 
   if (SysTick_Config(SystemCoreClock / 100))
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

extern uint32_t tick ;
void SysTick_Handler(void)
{
 // TimingDelay_Decrement();
  tick++;
}

