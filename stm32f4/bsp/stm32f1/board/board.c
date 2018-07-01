#include "clock.h"

void board_init(void)
{    
  Clock_Config();
  
  device_listInit();
  rt_hw_usart_init();
  stm32_hw_pin_init();

}

