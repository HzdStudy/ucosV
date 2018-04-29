#include "configs.h"

RCC_ClocksTypeDef RCC_Clocks;

uint32_t tick = 0;

extern  uint32_t SystemCoreClock;

int main(void)
{

  Clock_Config();
  
  USART_Config(115200);
       
  printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
  
  while (1)
  {
  }
}