#include "clock.h"


/**
  *  @说明  配置STM32F407的时钟系统
  * @参数  无
  * @返回  无
  * @说明  void Clock_Config(void) 按如下表格配置时钟
  *
*==================================================================
*        Supported STM32F4xx device revision    | Rev A
        *-----------------------------------------------------------------------------
*        System Clock source                             | PLL (HSE)
*-----------------------------------------------------------------------------
        *        SYSCLK(Hz)                              | 168000000
        *-----------------------------------------------------------------------------
*        HCLK(Hz)                                | 168000000
        *-----------------------------------------------------------------------------
*        AHB Prescaler                                       | 1
*-----------------------------------------------------------------------------
*        APB1 Prescaler                                  | 4
*-----------------------------------------------------------------------------
*        APB2 Prescaler                                | 2
*-----------------------------------------------------------------------------
*        HSE Frequency(Hz)                           | 8000000                                    
*-----------------------------------------------------------------------------
 *        PLL_M                                 |8
*-----------------------------------------------------------------------------
*        PLL_N                                   | 336
*-----------------------------------------------------------------------------
        *        PLL_P                                       | 2
*-----------------------------------------------------------------------------
*        PLL_Q                                  |7
*===================================================================
*/
void Clock_Config(void)
{ 
  ErrorStatus        State;
  uint32_t           PLL_M;      
  uint32_t           PLL_N;
  uint32_t           PLL_P;
  uint32_t           PLL_Q;
  /*配置前将所有RCC重置为初始值*/
  RCC_DeInit();
  /*这里选择 外部晶振（HSE）作为 时钟源，因此首先打开外部晶振*/
  RCC_HSEConfig(RCC_HSE_ON);
  /*等待外部晶振进入稳定状态*/
  while( RCC_WaitForHSEStartUp() != SUCCESS );
  /*
  **我们要选择PLL时钟作为系统时钟，因此这里先要对PLL时钟进行配置
  */
  /*选择外部晶振作为PLL的时钟源*/
  /* 到这一步为止，已有 HSE_VALUE = 8 MHz.
    PLL_VCO input clock = (HSE_VALUE or HSI_VALUE / PLL_M)，
    根据文档，这个值被建议在 1~2MHz，因此我们令 PLL_M = 8，
    即 PLL_VCO input clock = 1MHz */
  PLL_M         =    8;  
  /* 到这一步为止，已有 PLL_VCO input clock = 1 MHz.
    PLL_VCO output clock = (PLL_VCO input clock) * PLL_N,
    这个值要用来计算系统时钟，我们 令 PLL_N = 336,
    即 PLL_VCO output clock = 336 MHz.*/       
  PLL_N        =    336;
  /* 到这一步为止，已有 PLL_VCO output clock = 336 MHz.
    System Clock = (PLL_VCO output clock)/PLL_P ,
    因为我们要 SystemClock = 168 Mhz，因此令 PLL_P = 2.
    */
  PLL_P         =    2;
  /*这个系数用来配置SD卡读写，USB等功能，暂时不用，根据文档，暂时先设为7*/
  PLL_Q         =    7;
  /* 配置PLL并将其使能，获得 168Mhz 的 System Clock 时钟*/
  RCC_PLLConfig(RCC_PLLSource_HSE, PLL_M, PLL_N, PLL_P, PLL_Q);
  RCC_PLLCmd(ENABLE);
  /*到了这一步，我们已经配置好了PLL时钟。下面我们配置Syetem Clock*/
  /*选择PLL时钟作为系统时钟源*/
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /*到了这一步，我们已经配置好了系统时钟，频率为 168MHz. 下面我们可以对 AHB，APB，外设等的 时钟进行配置*/
  /*时钟的结构请参考用户手册*/
  /*首先配置 AHB时钟（HCLK）. 为了获得较高的频率，我们对 SYSCLK 1分频，得到HCLK*/
  RCC_HCLKConfig(RCC_HCLK_Div1);
  /*APBx时钟（PCLK）由AHB时钟（HCLK）分频得到，下面我们配置 PCLK*/
  /*APB1时钟配置. 4分频，即 PCLK1 = 42 MHz*/
  RCC_PCLK1Config(RCC_HCLK_Div4);
  /*APB2时钟配置. 2分频，即 PCLK2 = 84 MHz*/
  RCC_PCLK2Config(RCC_HCLK_Div2);
  /*****函数结束******/
 
/*以上函数可以大体上说明这些库函数的作用*/
}


