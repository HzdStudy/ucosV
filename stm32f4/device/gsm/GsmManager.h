#ifndef __U201ATFLOW_H
#define __U201ATFLOW_H

#include "configs.h"




/*
typedef struct
{


}GsmManagerData;
*/

void GsmManager_Init(void);//
void GsmManager_SendData(uint8_t * pBuf, uint16_t iLen);
void GsmManager_RevData(uint8_t * pBuf, uint16_t iLen);



#endif


