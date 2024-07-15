/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: GPRSMain.c
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* //2010		    : V
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#include "4GMain.h"
#include "4GRecv.h"
#include <string.h>
#include "sysconfig.h"
#include "HFQGFrame.h"
#include "bsp_debug_usart.h"
#include "DisplayMain.h"
/* Private define-----------------------------------------------------------------------------*/
#define GPRSRECV_Q_LEN      20

#define GPRS_SEND_BUF_LEN          (2000u)      //显示任务发送buf大小
#define GPRS_RECV_BUF_LEN          (2000u)      //显示任务接收buf大小
#define GPRS_Q_LEN              	(16u)       //显示任务消息队列个数
#define GPRS_TEMP_BUF_LEN          (2000u)      //显示任务保存一帧数据的buf大小

/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
static void *GPRSRECVOSQ[GPRSRECV_Q_LEN];					// 消息队列
static OS_EVENT *GPRSRECVTaskEvent;				            // 使用的事件

static INT8U GPRSSendBuf[GPRS_SEND_BUF_LEN];            // 显示发送缓存
static INT8U GPRSRecvBuf[GPRS_RECV_BUF_LEN];            // 显示接收缓存
static INT8U GPRSRecvSMem[GPRS_Q_LEN * sizeof(_BSP_MESSAGE)]; //消息实体的个数，实际上就是_BSP_MESSAGE结构体的个数
static INT8U GPRSTempBuf[GPRS_TEMP_BUF_LEN];            // 显示保存一帧数据缓存



//typedef enum{
//	RECV_NOT_DATA = 0,		//没有接收到数据
//	RECV_FOUND_DATA			//有数据
//}_RECV_DATA_STATUS;


_RECV_DATA_CONTROL RecvDataControl[LINK_NET_NUM];
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/
//专门做GPRS接收处理
/*****************************************************************************
* Function     : GPRSRecv_HardwareInit
* Description  : 
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年6月14日       
*****************************************************************************/
static INT8U GPRSRecv_HardwareInit(void)
{
    _BSP_UART_SET UartGPRSSet;                      
    
    GPRSRECVTaskEvent = OSQCreate(GPRSRECVOSQ, GPRSRECV_Q_LEN);//创建显示接收消息队列
    if (GPRSRECVTaskEvent == NULL)
    {
        OSTaskSuspend(OS_PRIO_SELF);    //挂起当前任务
        return FALSE;
    }
    memset(GPRSSendBuf, 0 ,sizeof(GPRSSendBuf));
    memset(GPRSRecvBuf, 0 ,sizeof(GPRSRecvBuf));

    UartGPRSSet.BaudRate = 115200;
    UartGPRSSet.DataBits = BSPUART_WORDLENGTH_8;
    UartGPRSSet.Parity   = BSPUART_PARITY_NO;
    UartGPRSSet.StopBits = BSPUART_STOPBITS_1;
    UartGPRSSet.RxBuf = GPRSRecvBuf;
    UartGPRSSet.RxBufLen = sizeof(GPRSRecvBuf);
    UartGPRSSet.TxBuf = GPRSSendBuf;
    UartGPRSSet.TxBufLen = sizeof(GPRSSendBuf);
    UartGPRSSet.Mode = UART_DEFAULT_MODE | UART_MSG_MODE;
    UartGPRSSet.RxQueue = GPRSRECVTaskEvent;
    UartGPRSSet.RxMsgMemBuf = GPRSRecvSMem;
    UartGPRSSet.RxMsgMemLen = sizeof(GPRSRecvSMem);
    if (BSP_UARTConfigTimeInit(GPRS_UART, &UartGPRSSet) == FALSE)  //和一般串口时间间隔不一样
    {
        return FALSE;
    }
	return  TRUE;
}

/*****************************************************************************
* Function     : APP_RecvDataControl
* Description  : 
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年6月14日       
*****************************************************************************/
_RECV_DATA_CONTROL	* APP_RecvDataControl(INT8U num)
{
	if(num >= NetConfigInfo[SYSSet.NetYXSelct].NetNum)
	{
		return NULL;
	}
	return &RecvDataControl[num];
}

//专门做GPRS接收处理
/*****************************************************************************
* Function     : TaskGPRSRecv
* Description  : 串口测试任务
* Input        : void
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018年6月16日        
*****************************************************************************/
void Task4GRecv(void *pdata)
{
    INT8U err,i;    
	INT16U 	len;
	//连续10次未收到数据，则主动读取数据,连续40次则重启
	static INT8U NetRset[LINK_NET_NUM] = {0};		
    _BSP_MESSAGE *pMsg;
	static _BSP_MESSAGE send_message[LINK_NET_NUM];
	
	static INT32U lastSysTime[LINK_NET_NUM] = {0}; 
	static INT32U nowSysTime[LINK_NET_NUM] = {0};		
        static uint32_t ZFnowSysTime = 0,ZFlastSysTime = 0;
	
	OS_EVENT * psendevent; 
	//临时使用UART1 (显示任务串口调试)
    GPRSRecv_HardwareInit();
	memset(RecvDataControl,0,sizeof(RecvDataControl));
	OSTimeDly(SYS_DELAY_100ms);
	psendevent =  APP_GetSendTaskEvent();
	for(i = 0; i < NetConfigInfo[SYSSet.NetYXSelct].NetNum;i++)
	{
		nowSysTime[i] = OSTimeGet();
		lastSysTime[i] = nowSysTime[i];
	}
	while(1)
    {
        //从串口读取一个消息
        pMsg = OSQPend(GPRSRECVTaskEvent, SYS_DELAY_250ms, &err);
		for(i = 0; i < NetConfigInfo[SYSSet.NetYXSelct].NetNum;i++)
		{
			nowSysTime[i] = OSTimeGet();
			ZFnowSysTime = OSTimeGet();  //ZF时间
		}
		
        if (OS_ERR_NONE == err)
        {
            switch (pMsg->MsgID)
            {
                case BSP_MSGID_UART_RXOVER:
					if (UART_MsgDeal(&pMsg, GPRSTempBuf, sizeof(GPRSTempBuf)) == TRUE)
                    {	
						if((APP_GetSIM7600Mode() == MODE_HTTP)  || (APP_GetSIM7600Mode() == MODE_FTP))    //远程升级其他无关数据帧都不不要发送和处理
						{
							SIM7600_RecvDesposeCmd(pMsg->pData,pMsg->DataLen); //未连接上服务器，AT指令处理
						}
						else
						{
							//pMsg->pData[pMsg->DataLen] = '\0';		  //保险，可能存在有些数据接收后没有/0
							//printf("GPRS recv [%s]", pMsg->pData);    //打印接收到的数据
							#if(NET_SENDRECV_PRINTF)
							printfchar_NETrecv(pMsg->pData,pMsg->DataLen); //接收打印
							#endif
							//如果已经连接上服务器
							if (NetConfigInfo[SYSSet.NetYXSelct].NetNum == 1)
							{
								if(APP_GetModuleConnectState(0) == STATE_OK) //已经连接上后台了
								{
									if(_4G_RecvFrameDispose(pMsg->pData,pMsg->DataLen))  //数据透传
									{
										lastSysTime[0] = nowSysTime[0];
									}
									printf("1OK4gR:");
									for(len = 0;len < pMsg->DataLen;len++)
									{
										printf("%02x",pMsg->pData[len]);
									}
								}
								else
								{
									SIM7600_RecvDesposeCmd(pMsg->pData,pMsg->DataLen); //未连接上服务器，AT指令处理
									printf("1NO4gR:");
									for(len = 0;len < pMsg->DataLen;len++)
									{
										printf("%c",pMsg->pData[len]);
									}
								}
							}
							else
							{
								//未连接上服务器，AT指令处理
								SIM7600_RecvDesposeCmd(pMsg->pData,pMsg->DataLen);
								for(i = 0;i < NetConfigInfo[SYSSet.NetYXSelct].NetNum;i++)
								{
									if(RecvDataControl[i].RecvStatus == RECV_FOUND_DATA)
									{
										RecvDataControl[i].RecvStatus = RECV_NOT_DATA;
										//接收数据处理
										
										//临时接收什么发送什么
										//ModuleSIM7600_SendData(i,RecvDataControl[i].DataBuf,RecvDataControl[i].len);
										
										if(i == 0)
										{
											if(_4G_RecvFrameDispose(RecvDataControl[i].DataBuf,RecvDataControl[i].len) )
											{
												lastSysTime[0] = nowSysTime[0];
											}
											printf("0PT4gR:");
											for(len = 0;len < pMsg->DataLen;len++)
											{
												printf("%02X",pMsg->pData[len]);
											}
										}
										else
										{
//											//政府平台,有数据返回就是注册成功
//											NOP();
//											APP_SetAppRegisterState(1,STATE_OK);
//											lastSysTime[1] = nowSysTime[1];
											
											if((RecvDataControl[i].DataBuf[0] == 0x68) &&(RecvDataControl[i].DataBuf[1] == 0x01) )   //简单判读下
											{
												APP_SetAppRegisterState(1,STATE_OK);  //政府平台,有数据返回就是注册成功
												lastSysTime[i] = nowSysTime[i];
											}
											
											//是否接收到心跳
											if((RecvDataControl[i].DataBuf[0] == 0x68)&&(RecvDataControl[i].DataBuf[1] == 0x04)&&(RecvDataControl[i].DataBuf[2] == 0x83))    
											{
												ZFlastSysTime = ZFnowSysTime;
												lastSysTime[i] = nowSysTime[i];
											}
											
											printf("ZF4gR:");
											for(len = 0;len < pMsg->DataLen;len++)
											{
												printf("%02X",pMsg->pData[len]);
											}
										}
										
										NetRset[i] = 0;
									}
								}
							}
						}	
					}						
                    UART_FreeOneMsg(GPRS_UART, pMsg);
                    break;
                default:
                    break;
            }
        }
		
//		 //2个平台下--如果说ZF平台30分钟内没有收到任务消息，就任务是离线，主平台开始重联
//        if(((ZFnowSysTime >= ZFlastSysTime) ? ((ZFnowSysTime - ZFlastSysTime) >= (SYS_DELAY_5M*6)) : ((ZFnowSysTime + (0xffffffff - ZFlastSysTime)) >= (SYS_DELAY_5M*6))) && (NetConfigInfo[SYSSet.NetYXSelct].NetNum == 2))
//        {
//            APP_SetNetNotConect(0);
//			printf("ZF30min_lixian\r\n"); //主平台离线
//			ZFlastSysTime = ZFnowSysTime;
//        }
		
		
		
		if((APP_GetSIM7600Mode() == MODE_HTTP)  || (APP_GetSIM7600Mode() == MODE_FTP))    //远程升级其他无关数据帧都不不要发送和处理
		{
			for(i = 0; i < NetConfigInfo[SYSSet.NetYXSelct].NetNum;i++)
			{
				nowSysTime[i] = OSTimeGet();
				lastSysTime[i] = nowSysTime[i];
			}
			continue; //未连接上服务器，AT指令处理
		}
		for(i = 0;i < NetConfigInfo[SYSSet.NetYXSelct].NetNum;i++)
		{
			if(APP_GetModuleConnectState(i) == STATE_OK) //已经连接上后台了
			{
				if (NetConfigInfo[SYSSet.NetYXSelct].NetNum > 1)
				{
					if(++NetRset[i] >= 50)  // 15s
					{
						NetRset[i] = 0;
						if(i == 0)
						{
							send_message[i].MsgID = BSP_4G_MAIN;
							send_message[i].DivNum = BSP_4G_SENDNET1;
						}else{
							send_message[i].MsgID = BSP_4G_MAIN;
							send_message[i].DivNum = BSP_4G_SENDNET2;
						}
						OSQPost(psendevent, &send_message[i]);
					}
				}
				
				//重启时间根据周期性发送数据再放余量
				if(SYSSet.NetYXSelct == XY_AP || (SYSSet.NetYXSelct == XY_66))
				{
					if((nowSysTime[i] >= lastSysTime[i]) ? ((nowSysTime[i] - lastSysTime[i]) >= SYS_DELAY_1M) : \
					((nowSysTime[i] + (0xffffffff - lastSysTime[i])) >= SYS_DELAY_1M))
					{
						lastSysTime[i] = nowSysTime[i];
						APP_SetNetNotConect(i);
					}
				}
				else
				{
//					if((nowSysTime[i] >= lastSysTime[i]) ? ((nowSysTime[i] - lastSysTime[i]) >= SYS_DELAY_30s) : \
//					((nowSysTime[i] + (0xffffffff - lastSysTime[i])) >= SYS_DELAY_30s))
//					{
//						lastSysTime[i] = nowSysTime[i];
//						printf("duannet_num:%d\r\n",i); //判断一下断网断的是第几个平台
//						APP_SetNetNotConect(i);
//					}
					
					
					if((nowSysTime[0] >= lastSysTime[0]) ? ((nowSysTime[0] - lastSysTime[0]) >= SYS_DELAY_30s) : \
					((nowSysTime[0] + (0xffffffff - lastSysTime[0])) >= SYS_DELAY_30s))
					{
						lastSysTime[0] = nowSysTime[0];
						printf("duannet_num111:%d\r\n",0); //判断第一个平台
						APP_SetNetNotConect(0);
					}
					
					
					if((nowSysTime[1] >= lastSysTime[1]) ? ((nowSysTime[1] - lastSysTime[1]) >= SYS_DELAY_5M) : \
					((nowSysTime[1] + (0xffffffff - lastSysTime[1])) >= (SYS_DELAY_5M)))
					{
						lastSysTime[1] = nowSysTime[1];
						printf("duannet_num222:%d\r\n",1); //判断第二个平台接收的时间拉长一些--先修改到5分钟
						APP_SetNetNotConect(1);
					}
						

					
				}
			}
			else
			{
				lastSysTime[i] = nowSysTime[i];
			}
		}

	}	
}
/************************(C)COPYRIGHT 2020 杭州汇誉*****END OF FILE****************************/

