#include "GsmManager.h"

device_t _gsm_device;

//初始化函数
void GsmManager_Init(void)
{
//注册串口函数
  _gsm_device = device_find("uart2");
  if(_gsm_device != NULL)//不加也可以，下层接口对设备做了判断
    device_open(_gsm_device, DEVICE_OFLAG_RDWR);

  //初始化AT指令流模块
  ATCom_Init();

  //注册串口发送函数
  ATCom_RegSendFunc(GsmManager_SendData);

  
}

//数据发送函数
void GsmManager_SendData(uint8_t * pBuf, uint16_t iLen)
{
  if(_gsm_device != NULL)
    device_write(_gsm_device, 0, pBuf, iLen);
}

//数据接收函数
void GsmManager_RevData(uint8_t * pBuf, uint16_t iLen)
{
  ATCom_Process(pBuf, iLen, NULL);

}
















