#ifndef __U201ATFLOW_H
#define __U201ATFLOW_H

#include "configs.h"

//每个AT指令的回调函数
typedef void (* ATCommandCallBack) (unsigned char * pBuf, unsigned short i_len, void * user_data);
typedef void (* ATReplyCallBack) (unsigned char * pBuf, unsigned short i_len, void * user_data);
typedef void (* ATSendCom) (unsigned char const* pBuf, unsigned short i_len);


//AT指令
typedef enum
{
  //每个模块需要实现的基础指令
  C_AT_NONE = 0,
  C_AT         ,
  C_ATE        ,
  
  //每个模块各自独有的扩展指令

  C_AT_MAX,
}ATCom;


//AT指令数据结构体
typedef struct
{
  ATCom                  at_com;    //AT指令
  char const *           at_entity; //指令实体
  unsigned short         at_len;
}ATComData;


//AT指令回复
typedef enum
{
  //每个模块需要实现的基础指令
  R_NONE    = 0,
  R_OK         ,
  R_ERROR      ,
  
  //每个模块各自独有的扩展指令

  R_MAX,
}ATReply;


//AT指令数据结构体
typedef struct
{
  ATReply                 at_reply;    //AT指令
  char const *            at_entity; //回复指令实体
  unsigned short          at_len;
  ATReplyCallBack         at_reply_func;
}ATReplyData;

//AT指令流初始化状态机
typedef enum
{
  AT_NONE = 0,
  AT_OK,
  AT_ATE,
  AT_Complete,
}ATModulesInitStatu;


//当前AT模块数据结构
typedef struct
{
  ATCommandCallBack   cur_at_call_back_func;//当前AT指令的函数
  ATReply             cur_at_reply;         //当前回复的AT指令
  ATCom               cur_at_com;           //当前发送的AT指令
  ATSendCom           at_send_com;          //发送函数

  //模块状态机
  ATModulesInitStatu  at_modules_init_statu; //初始化状态机

  unsigned short            at_max_time;          //每一条AT指令存在的最长时间
  unsigned short            at_tick;              //AT指令流计数器
}ATModulesData;




void ATCom_Init(void);
void ATCom_ATCallBack(unsigned char * pBuf, unsigned short iLen, void * pUserData);
void ATCom_ATECallBack(unsigned char * pBuf, unsigned short iLen, void * pUserData);




#endif

