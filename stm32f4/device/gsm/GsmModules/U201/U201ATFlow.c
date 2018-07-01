#include "U201ATFlow.h"
#include "string.h"

//AT指令
const  char c_AT_arr[]       = "AT\r\n";   //AT命令
const  char c_ATE_arr[]      = "ATE\r\n";  //关闭回显

//AT回复
const  char r_OK_arr[]       = "\r\nOK";
const  char r_Err_arr[]      = "\r\nError";

const  char wrap_arr[]       = "\r\n";

//指令集结构体
ATComData at_com_data[] = 
{
  {C_AT_NONE,         NULL,  0  },
  {C_AT     ,    c_AT_arr , 0   },
  {C_AT     ,    c_ATE_arr, 0   },
  {C_AT_MAX ,         NULL, 0   },
};


ATReplyData at_reply_data[] = 
{
  {R_NONE   ,         NULL  , 0,           NULL},
  {R_OK     ,      r_OK_arr , 0,           NULL},
  {R_ERROR  ,     r_Err_arr , 0,           NULL},
  {R_MAX    ,          NULL , 0,           NULL},
};

ATModulesData at_modules_data;



//AT指令流初始化，加载链表
void ATCom_Init(void)
{
  unsigned short i;
  at_modules_data.cur_at_call_back_func = NULL;
  at_modules_data.at_send_com           = NULL;

//初始化AT指令
  for(i=C_AT_NONE; i<C_AT_MAX; i++)
    at_com_data[i].at_len = strlen(at_com_data[i].at_entity);

//初始化回复
  for(i=R_NONE; i<R_MAX; i++)
    at_reply_data[i].at_len = strlen(at_reply_data[i].at_entity);

}

void ATCom_RegSendFunc(ATSendCom atSendFunc)
{
  at_modules_data.at_send_com = atSendFunc;
}

//解析AT指令流命令
void ATCom_RevAnalyze(unsigned char * pBuf, unsigned short iLen, void * pUserData)
{
  unsigned short   i;
  unsigned short   offset;     //偏移值
  unsigned char *  cur_pBuf = pBuf;//当前指针指向位置

  pBuf[iLen] = '\0'; //

//循环，每次进来解析完所有指令才退出
  while(iLen > 0)
  {
    //解析AT应答指令
    for(i=R_NONE; i<R_MAX; i++)
    {
      if(at_reply_data[i].at_entity != NULL)
      {
        //找出相应指令
        if( strncmp(cur_pBuf, at_reply_data[i].at_entity, at_reply_data[i].at_len) == 0)
        {
          at_modules_data.cur_at_reply = at_reply_data[i].at_reply;
          if(at_reply_data[i].at_reply_func != NULL)
          {
            at_reply_data[i].at_reply_func(cur_pBuf, iLen, pUserData);
          }

          break;//查找到当前AT回复指令，跳出循环
        }
      }      
    }

    //处理当前发送AT的命令
    if(i<R_MAX)
    {
      if(at_modules_data.cur_at_call_back_func != NULL)
        at_modules_data.cur_at_call_back_func(cur_pBuf, iLen, pUserData);
    }

    //清除指令
    if(iLen > 0)
    {
      cur_pBuf = strstr(cur_pBuf, wrap_arr);//搜索回车换行符
      if(cur_pBuf == NULL)//已经找不到了，退出
        return; 

      cur_pBuf += strlen(wrap_arr);
    }
    at_modules_data.cur_at_reply = R_NONE;//清除回复
  }
}

//AT指令流发送函数
void ATCom_SendATCom(ATCom atCom, ATCommandCallBack atCallBackFunc, unsigned short ovetTime100Ms)
{
  if(at_modules_data.cur_at_com != NULL)
    return;

  //发送函数
  at_modules_data.at_send_com(at_com_data[atCom].at_entity, at_com_data[atCom].at_len);
  //注册当前指令的回调函数
  at_modules_data.cur_at_call_back_func = atCallBackFunc; 
  //当前的AT指令
  at_modules_data.cur_at_com = atCom; 
  //当前指令最长存在时间
  at_modules_data.at_max_time = ovetTime100Ms*5;
  //清0计数器
  at_modules_data.at_tick = 0;

}

void ATCom_ATCallBack(unsigned char * pBuf, unsigned short iLen, void * pUserData)
{
  
}

void ATCom_ATECallBack(unsigned char * pBuf, unsigned short iLen, void * pUserData)
{
  
}

//AT模块初始化
//1.AT 应答
//2.ATE关闭回显
void ATCom_ModuleInit(void)
{
  switch(at_modules_data.at_modules_init_statu)
  {
    case AT_NONE:
      ATCom_SendATCom(C_AT, ATCom_ATCallBack, 10);
    break;

    case AT_OK:
      ATCom_SendATCom(C_ATE, ATCom_ATECallBack, 10);
    break;

    case AT_Complete:
      //ATCom_SendATCom(C_AT, ATCom_ATECallBack, 10);
    break;

    default:
    break;
  }
}

//AT模块网络注册初始化
//1.AT+CPIN  查询SIM卡
//2.AT+CSQ   查询信号量
//3.AT+CREG  查询是否附着到网络
//4.AT+CGREG 查询是否注册上网络
void ATCom_ModuleRegInit(void)
{
  if(at_modules_data.at_modules_init_statu != AT_Complete)
    return ;
/*
  switch()
  {

  }*/
}

//AT模块联网
//1. 配置APN
//2. 连接网络
void ATCom_ModuleConnectNet(void)
{
  
}

//超时处理函数
void ATCom_OverTiemDealWith(void)
{
  if(at_modules_data.cur_at_com == NULL)
    return;
  
  if(at_modules_data.at_tick > at_modules_data.at_max_time)
  {
    //清空AT指令流
    at_modules_data.cur_at_com = NULL;
    at_modules_data.at_tick = 0;

    //预留，清除其他操作


    
  }

  at_modules_data.at_tick ++;
}

//AT进程
//以50HZ频率调用
void ATCom_Process(unsigned char * pBuf, unsigned short iLen, void * pUserData)
{
  //AT指令流处理程序
  ATCom_RevAnalyze(pBuf, iLen, pUserData);

  //超时处理函数
  ATCom_OverTiemDealWith();

  //初始化模块
  ATCom_ModuleInit();

  //网络注册
  ATCom_ModuleRegInit();

  //连接服务器
  ATCom_ModuleConnectNet();
}

