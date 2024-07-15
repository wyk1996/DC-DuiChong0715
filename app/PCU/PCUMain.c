/*****************************************Copyright(C)******************************************
*******************************************汇誉科技*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : PCUMain.c
* Author            :
* Date First Issued : 2016-07-07
* Version           : V1.0
* Description       : 功率模块控制
*----------------------------------------历史版本信息-------------------------------------------
* History           :
* //2010            : V1.0
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/

#include <string.h>
#include "ucos_ii.h"
#include "controlmain.h"
#include "bmstimeout.h"
#include "bsp_can.h"
#include "Disinterface.h"
/* Private define-----------------------------------------------------------------------------*/
#define  MIN(a, b)      (((a) < (b)) ? (a) : (b))
/* Private typedef----------------------------------------------------------------------------*/

/* Private macro------------------------------------------------------------------------------*/
/* Private variables-----------------------------------------------------------------*/


/************************************CCU发送数据****************************/
#define _SEND_CCU_MAX       7
#define RECV_PCU_NUM		8

//0x30为充电主空模块基地址   0x30表示A枪， 0x31表示B枪，依次类推
#define SEND_ID_CCU_CONTROL 			0x18011030  //遥控控制帧
#define SEND_ID_CCU_SET 				0x18801030  //定值设置命令帧
#define SEND_ID_CCU_QUERY				0x18821030  //定值查询命令帧
#define SEND_ID_CCU_INFO1				0x18201030  //遥信遥测帧1
#define SEND_ID_CCU_INFO2				0x18211030  //遥信遥测帧2
#define SEND_ID_CCU_HEART		        0x18401030  //心跳帧 
//自定义协议接收部分
#define SEND_ID_CCU_USERDEF	        	0x18F01030  //设置信息


#define RECV_ID_CCU_CONTROL 			0x18023010  //遥控应答帧
#define RECV_ID_CCU_SET 				0x18813010  //定值设置应答帧
#define RECV_ID_CCU_QUERY	            0x18833010  //定值查询命令帧
#define RECV_ID_CCU_INFO1				0x18223010  //工作状态及故障信息帧		
#define RECV_ID_CCU_INFO2				0x18233010  //遥测帧
#define RECV_ID_CCU_INFO3				0x18243010  //告警信息帧
#define RECV_ID_CCU_HEART		        0x18413010  //心跳帧 
#define RECV_ID_CCU_USERDEF				0x18F13010	//自定义协议

#define RECV_ID_DISPOSE(a)  ((a)&(0xffffff00))




_CCU_SEND_DATA CCUSendData[GUN_MAX];				//CCU发送数据结构体
_CCU_RECV_DATA CCURecvData[GUN_MAX];				//CCU接收数据结构体
#if(USER_GUN != USER_SINGLE_GUN)
#define	MODULE_CAN	2  //CAN3
#else
#define	MODULE_CAN	1  //CAN2
#endif
OS_EVENT *CANMutex;                 //CAN互斥
#define CANMODULERX_BUFF_LEN 	30
BSPCANTxMsg   CANModuleTxbuff;      //发送缓冲
BSPCANRxMsg   CANModuleRxbuff[CANMODULERX_BUFF_LEN];//接收缓冲
BSP_CAN_Set  bxModuleCANSet;
#define MODULECANRX_BUFF_LEN     (16u)      //接收缓冲长度
#define MODULECANOSQ_NUM         (32u)      //连接BMS的CAN消息容量
OS_EVENT  *TaskPCU_pevent;    //CAN消息队列指针
void *bxMODULECANOSQ[MODULECANOSQ_NUM];    //指针数组bxCANOSQ[]容纳指向各个消息的指针
_BSP_MESSAGE  MODULERxMessage[MODULECANRX_BUFF_LEN];//消息接收缓冲


#warning "YXY 当前板子编号"
//INT8U   CurCCUNum = 2;   //0   1   2  3
extern _SYS_SET SYSSet;
/***********************************************************************************************
* Function      : InitCAN_Module
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 20101210
***********************************************************************************************/
void InitCAN_Module(void)
{


	TaskPCU_pevent = OSQCreate(bxMODULECANOSQ, MODULECANOSQ_NUM); // 建立事件(消息队列)
	/*初始化can收发控制体*/
	memset(&CANModuleTxbuff, 0x00, sizeof(CANModuleTxbuff));
	bxModuleCANSet.TxMessage =  &CANModuleTxbuff;
	bxModuleCANSet.RxMessage =  CANModuleRxbuff;
	bxModuleCANSet.BspMsg = MODULERxMessage;
	bxModuleCANSet.RxMaxNum = MODULECANRX_BUFF_LEN ;
	bxModuleCANSet.BaundRate = BSP_CAN_BAUNDRATE_125KBPS; //波特率为250K
	BSP_CAN_Init(MODULE_CAN, &bxModuleCANSet, TaskPCU_pevent);
}
/************************************接收到其他任务消息处理****************************/

/*****************************************************************************
* Function      : APP_PowerModuleStart
* Description   : 设置电压、电流信息给功率模块
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   : 2020年6月5日  叶喜雨
*****************************************************************************/
INT8U APP_PowerModuleStartA(void *pdata)
{
	INT8U	*ptr;

	_BSP_MESSAGE *pMsg = (_BSP_MESSAGE *)pdata;

	if (pMsg == NULL)
	{
		return FALSE;
	}

	ptr = pMsg->pData;
//	OSSchedLock();

	CCUSendData[GUN_A].SendControlCmd.SetVol = *( (__packed INT16U *)ptr);
	CCUSendData[GUN_A].SendControlCmd.SetCur = (*( (__packed INT16U *)&ptr[2]))*10; //放大100倍
	//CCUSendData.SendControlCmd.SetCur -= 200;		//有些车子满负荷运行，需求电流会下降 20210508

//	if(CCUSendData.SendControlCmd.SetCur > 8000)
//	{
//		CCUSendData.SendControlCmd.SetCur = CCUSendData.SendControlCmd.SetCur - 500;   //当电流大于100A时候，强制降低5A输出，主要为了解决接触器频繁跳动问题
//	}
//	OSSchedUnlock();

	CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = OPEN_QUICK;
	CCUSendData[GUN_A].SendControlCmd.BatteryVol = 0;			//电池电压就填写0

	return TRUE;
}


/*****************************************************************************
* Function      : APP_PowerModuleStart
* Description   : 设置电压、电流信息给功率模块
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   : 2020年6月5日  叶喜雨
*****************************************************************************/
INT8U APP_PowerModuleStopA(void *pdata)
{
	INT8U	*ptr;

	_BSP_MESSAGE *pMsg = (_BSP_MESSAGE *)pdata;

	if (pMsg == NULL)
	{
		return FALSE;
	}

	ptr = pMsg->pData;
	CCUSendData[GUN_A].SendControlCmd.SetVol = 0;
	CCUSendData[GUN_A].SendControlCmd.SetCur = 0;
	//0表示正常关机，告诉PCU无需预留该枪的模块 1：表示需要预留该枪的模块
	if( pMsg->DataLen == 0)
	{
		CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
	}
	else
	{
		CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = CLOSE_JYCHARGE;
	}
	//CCUSendData.SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
	CCUSendData[GUN_A].SendControlCmd.BatteryVol = 0;			//电池电压就填写0

	return TRUE;
}



/*****************************************************************************
* Function      : APP_PowerModuleStart
* Description   : 设置电压、电流信息给功率模块
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   : 2020年6月5日  叶喜雨
*****************************************************************************/
INT8U APP_PowerModuleStartB(void *pdata)
{
	INT8U	*ptr;

	_BSP_MESSAGE *pMsg = (_BSP_MESSAGE *)pdata;

	if (pMsg == NULL)
	{
		return FALSE;
	}

	ptr = pMsg->pData;
//	OSSchedLock();

	CCUSendData[GUN_B].SendControlCmd.SetVol = *( (__packed INT16U *)ptr);
	CCUSendData[GUN_B].SendControlCmd.SetCur = (*( (__packed INT16U *)&ptr[2]))*10; //放大100倍
	//CCUSendData.SendControlCmd.SetCur -= 200;		//有些车子满负荷运行，需求电流会下降 20210508

//	if(CCUSendData.SendControlCmd.SetCur > 8000)
//	{
//		CCUSendData.SendControlCmd.SetCur = CCUSendData.SendControlCmd.SetCur - 500;   //当电流大于100A时候，强制降低5A输出，主要为了解决接触器频繁跳动问题
//	}
//	OSSchedUnlock();

	CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = OPEN_QUICK;
	CCUSendData[GUN_B].SendControlCmd.BatteryVol = 0;			//电池电压就填写0

	return TRUE;
}


/*****************************************************************************
* Function      : APP_PowerModuleStart
* Description   : 设置电压、电流信息给功率模块
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   : 2020年6月5日  叶喜雨
*****************************************************************************/
INT8U APP_PowerModuleStopB(void *pdata)
{
	INT8U	*ptr;

	_BSP_MESSAGE *pMsg = (_BSP_MESSAGE *)pdata;

	if (pMsg == NULL)
	{
		return FALSE;
	}

	ptr = pMsg->pData;
	CCUSendData[GUN_B].SendControlCmd.SetVol = 0;
	CCUSendData[GUN_B].SendControlCmd.SetCur = 0;
	//0表示正常关机，告诉PCU无需预留该枪的模块 1：表示需要预留该枪的模块
	if( pMsg->DataLen == 0)
	{
		CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
	}
	else
	{
		CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = CLOSE_JYCHARGE;
	}
	//CCUSendData.SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
	CCUSendData[GUN_B].SendControlCmd.BatteryVol = 0;			//电池电压就填写0

	return TRUE;
}

/************************************CCU发送数据****************************/
/***********************************************************************************************
* Function      : CanSend
* Description   : BCU主任务
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2016-06-03 Yxy
***********************************************************************************************/
INT8U CanSend(INT32U extid, INT8U *pdata, INT8U len)
{
	static INT32U CanWModuleErr = 0;
	INT8U err;

	if((pdata == NULL) || (!len) || (len > 8) )
	{
		return FALSE;
	}


	OSMutexPend(CANMutex, 0, &err);					//获取锁

	CANModuleTxbuff.RTR = CAN_RTR_DATA;
	CANModuleTxbuff.IDE = CAN_ID_EXT;
	CANModuleTxbuff.ExtId = extid;
	CANModuleTxbuff.DLC = len;
	memcpy(CANModuleTxbuff.Data,pdata,len);
	while(BSP_CAN_Write(MODULE_CAN,&bxModuleCANSet) == FALSE)     //直到当前帧发送成功为止
	{
		if(++CanWModuleErr  == 5)
		{
			break;					//5次跳出
		}
		OSTimeDly(SYS_DELAY_2ms);
	}
	CanWModuleErr = 0;
	OSTimeDly(SYS_DELAY_10ms);
	OSMutexPost(CANMutex); 					//释放锁
	return TRUE;
}
/***********************************************************************************************
* Function      : Send_Fream_Control
* Description   : 发送遥控命令
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2018年8月3日
***********************************************************************************************/
static INT8U Send_Fream_Control(INT32U id)
{

	INT32U extid;
	static INT8U gun = 0;


	extid = id | (gun + SYSSet.PMUNUM*2);
	if(CCUSendData[gun].SendControlCmd.Cmd.Operate_Cmd == 0)
	{
		NOP();
	}

	CanSend(extid,(INT8U*)&CCUSendData[gun].SendControlCmd,sizeof(CCUSendData[gun].SendControlCmd));
#if(USER_GUN != USER_SINGLE_GUN)
	gun++;
	if(gun >= 2)
	{
		gun = 0;
	}
#endif
	return TRUE;
}

/***********************************************************************************************
* Function      : Send_Fream_INFO1
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Send_Fream_INFO1(INT32U id)
{
	INT32U extid;
	static INT8U gun = 0;

#warning "目前未填写，PCU也没有处理"
	memset(&CCUSendData[gun].SendInfo1,0,sizeof(CCUSendData[gun].SendInfo1));
	extid = id | (gun+ SYSSet.PMUNUM*2);

	CanSend(extid,(INT8U*)&CCUSendData[gun].SendInfo1,sizeof(CCUSendData[gun].SendInfo1));
#if(USER_GUN != USER_SINGLE_GUN)
	gun++;
	if(gun >= 2)
	{
		gun = 0;
	}
#endif
	return TRUE;
}

/***********************************************************************************************
* Function      : Send_Fream_INFO2
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Send_Fream_INFO2(INT32U id)
{
	INT32U extid;
	static INT8U gun = 0;
#warning "目前未填写，PCU也没有处理"
	memset(&CCUSendData[gun].SendInfo2,0,sizeof(CCUSendData[gun].SendInfo2));
	extid = id | (gun+ SYSSet.PMUNUM*2);

	CanSend(extid,(INT8U*)&CCUSendData[gun].SendInfo2,sizeof(CCUSendData[gun].SendInfo2));
#if(USER_GUN != USER_SINGLE_GUN)
	gun++;
	if(gun >= 2)
	{
		gun = 0;
	}
#endif
	return TRUE;
}


/***********************************************************************************************
* Function      : Send_Fream_Heart
* Description   : 发送心跳
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Send_Fream_Heart(INT32U id)
{
	INT32U extid;
	static INT8U gun = 0;
//	memset(&CCUSendData[gun].SendHeart,0,sizeof(CCUSendData[gun].SendHeart));
	extid = id | (gun + SYSSet.PMUNUM*2);

	CanSend(extid,(INT8U*)&CCUSendData[gun].SendHeart,sizeof(CCUSendData[gun].SendHeart));
#if(USER_GUN != USER_SINGLE_GUN)
	gun++;
	if(gun >= 2)
	{
		gun = 0;
	}
#endif
	return TRUE;
}


/***********************************************************************************************
* Function      : Send_Fream_Heart
* Description   : 发送自定义协议数据
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Send_Fream_Userdef(INT32U id)
{
	INT32U extid;
	static INT8U gun = 0;

	CCUSendData[gun].SendUserdef.cmd = 0x40;		//读取模块信息
	CCUSendData[gun].SendUserdef.res[0] = 1;  	//表示读取1-8个模块
	extid = id | (gun+ SYSSet.PMUNUM*2);

	CanSend(extid,(INT8U*)&CCUSendData[gun].SendUserdef,sizeof(CCUSendData[gun].SendUserdef));
#if(USER_GUN != USER_SINGLE_GUN)
	gun++;
	if(gun >= 2)
	{
		gun = 0;
	}
#endif
	return TRUE;
}
/****************************************接受函数*********************************************/
/***********************************************************************************************
* Function      : Recv_Fream_Control
* Description   : 定值接受
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_Control(INT8U gun,INT8U *pdata,INT8U len)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	if((pdata == NULL)|| (len != sizeof(CCURecvData[gun].RecvConterlCmd)))
	{
		OSSchedUnlock();
		return FALSE;
	}
	memcpy((INT8U*)&CCURecvData[gun].RecvConterlCmd,pdata,MIN(sizeof(CCURecvData[gun].RecvConterlCmd),8) );
	OSSchedUnlock();
	return TRUE;
}

/***********************************************************************************************
* Function      : Recv_Fream_Set
* Description   : 定值设置命令帧
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_Set(INT8U gun,INT8U *pdata,INT8U len)
{
	return TRUE;
}

/***********************************************************************************************
* Function      : Recv_Fream_Query
* Description   : 定值查询
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_Query(INT8U gun,INT8U *pdata,INT8U len)
{
	return TRUE;
}

/***********************************************************************************************
* Function      : Recv_Fream_INFO1
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_INFO1(INT8U gun,INT8U *pdata,INT8U len)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	if((pdata == NULL)|| (len != sizeof(CCURecvData[gun].RecvInfo1)))
	{
		OSSchedUnlock();
		return FALSE;
	}
	memcpy((INT8U*)&CCURecvData[gun].RecvInfo1,pdata,MIN(sizeof(CCURecvData[gun].RecvInfo1),8) );
	OSSchedUnlock();
	return TRUE;
}

/***********************************************************************************************
* Function      : Recv_Fream_INFO2
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_INFO2(INT8U gun,INT8U *pdata,INT8U len)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	if((pdata == NULL)|| (len != sizeof(CCURecvData[gun].RecvInfo2)))
	{
		OSSchedUnlock();
		return FALSE;
	}
	memcpy((INT8U*)&CCURecvData[gun].RecvInfo2,pdata,MIN(sizeof(CCURecvData[gun].RecvInfo2),8) );
	OSSchedUnlock();
	return TRUE;
}

/***********************************************************************************************
* Function      : Recv_Fream_INFO3
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_INFO3(INT8U gun,INT8U *pdata,INT8U len)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	if((pdata == NULL)|| (len != sizeof(CCURecvData[gun].RecvInfo3)))
	{
		OSSchedUnlock();
		return FALSE;
	}
	memcpy((INT8U*)&CCURecvData[gun].RecvInfo3,pdata,MIN(sizeof(CCURecvData[gun].RecvInfo3),8));
	OSSchedUnlock();
	return TRUE;
}

/***********************************************************************************************
* Function      : Recv_Fream_Heart
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_Heart(INT8U gun,INT8U *pdata,INT8U len)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	if((pdata == NULL)|| (len != sizeof(CCURecvData[gun].RecvHeart)))
	{
		OSSchedUnlock();
		//printf("No recv\r\n");
		return FALSE;
	}
	memcpy((INT8U*)&CCURecvData[gun].RecvHeart,pdata,MIN(sizeof(CCURecvData[gun].RecvHeart),8));
	//                                                                    	printf("recv\r\n");
	OSSchedUnlock();
	return TRUE;
}

INT32U TTTT1 = 0,TTTT2 = 0,TTTT3 = 0;

/***********************************************************************************************
* Function      : Recv_Fream_Heart
* Description   :
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
static INT8U Recv_Fream_Usedef(INT8U gun,INT8U *pdata,INT8U len)
{

	INT8U modulenum = 0;
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	if((pdata == NULL)|| (len != sizeof(CCURecvData[gun].RecvHeart)))
	{
		OSSchedUnlock();
		return FALSE;
	}
	if(pdata[0] == 0x60)  //三相电压电流
	{
		memcpy((INT8U*)&CCURecvData[gun].RecvACVol,&pdata[1],MIN(sizeof(CCURecvData[gun].RecvACVol),7));
		TTTT1++;
	}
	else if(pdata[0] == 0x80)  //枪的直流输出电压电流
	{
		memcpy((INT8U*)&CCURecvData[gun].RecGunDC,&pdata[1],MIN(sizeof(_RECV_GUNDC),7));
		TTTT2++;
	}
	else
	{
		modulenum = (pdata[0] & 0x0f);		//模块信息
		if(((pdata[0] & 0xf0) == 0x40) && (modulenum < MODULE_MAX_NUM))
		{
			memcpy((INT8U*)&CCURecvData[gun].RecvModuleInfo[modulenum],&pdata[1],MIN(sizeof(_ERCV_MODULEINFO),7));
		}
		TTTT3++;
	}
	OSSchedUnlock();
	return TRUE;
}
//static _APP_CHARGE_STATE2 module_state;
/***********************************************************************************************
* Function      : Updata_ModuleInfo
* Description   : 跟新模块数据（跟新  PowerModuleInfo）
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
INT8U Updata_ModuleInfo(INT8U gun)
{
	_APP_CHARGE_STATE2 module_state = {0};
	INT8U i = 0;
	static  INT32U curtime[GUN_MAX] = {0},lasttime[GUN_MAX] = {0};
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	PowerModuleInfo[gun].OutputInfo.Cur = CCURecvData[gun].RecGunDC.Output_GunCur;

	curtime[gun] = OSTimeGet();
	if((PowerModuleInfo[gun].OutputInfo.Cur ) > (4000 - BMS_BCL_Context[gun].DemandCur))
	{
		if((curtime[gun]  - lasttime[gun]) > SYS_DELAY_2s)   //持续2s
		{
			NOP();
		}
		else
		{
			PowerModuleInfo[gun].OutputInfo.Cur = (4000 - BMS_BCL_Context[gun].DemandCur);
		}
	}
	else
	{
		lasttime[gun] =curtime[gun];
	}
	PowerModuleInfo[gun].OutputInfo.Vol = CCURecvData[gun].RecGunDC.Output_GunVol;



	if(CCURecvData[gun].RecGunDC.Output_GunCur > 10)
	{
		NOP();
	}
	for(i = 0; i < MODULE_MAX_NUM; i++)
	{
		if(CCURecvData[gun].RecvModuleInfo[i].ModuleState.State1.onebits.Commu_Aarm != 1)
		{
			//所有的在线模块才处理
			if(CCURecvData[gun].RecvModuleInfo[i].ModuleT > module_state.ModuleMaxTep)
			{
				module_state.ModuleMaxTep = CCURecvData[gun].RecvModuleInfo[i].ModuleT;
			}
			//只要一个为异常，则为异常
			module_state.ModuleState.State1.allbits |= CCURecvData[gun].RecvModuleInfo[i].ModuleState.State1.allbits;
			module_state.ModuleState.State2.allbits |= CCURecvData[gun].RecvModuleInfo[i].ModuleState.State2.allbits;
			module_state.State3.OneByte.InACOverVolBit |= CCURecvData[gun].RecvModuleInfo[i].ModuleState.State2.onebits.AC_Over;
			module_state.State3.OneByte.InACUnderVolBit |= CCURecvData[gun].RecvModuleInfo[i].ModuleState.State2.onebits.AC_UVP;
		}

	}
	memcpy(&PowerModuleInfo[gun].TotalState,&module_state,sizeof(_APP_CHARGE_STATE2));
	OSSchedUnlock();
	return TRUE;
}

//接收到其他任务的命令处理表
const _PCU_DEAL_CMD PCUDealCmdTable[GUN_MAX][PCU_CMD_TABLE_NUM] =
{
	{
		{APP_POWER_ENABLE       ,       APP_PowerModuleStartA       },  //开启模块命令

		{APP_POWER_DISABLE      ,       APP_PowerModuleStopA        },  //关闭模块命令
	}
	,
	{
		{APP_POWER_ENABLE       ,       APP_PowerModuleStartB       },  //开启模块命令

		{APP_POWER_DISABLE      ,       APP_PowerModuleStopB        },  //关闭模块命令
	}
};

//发送处理表，当执行周期为0时，不周期性发送


//发送数据时，存在ID与数据错乱，目前先保证其他数据都为0，就算错乱了，PCU也有保护。
static _CCU_SEND_TABLE TableSendCCUDispose[_SEND_CCU_MAX] =
{
	{SEND_ID_CCU_CONTROL     	, SYS_DELAY_100ms	, 0, 			Send_Fream_Control	},   	//遥控控制帧

	{SEND_ID_CCU_SET     		, 0 				, 0, 			NULL				},	 	//定值设置命令帧

	{SEND_ID_CCU_QUERY     		, 0 				, 0, 			NULL				},	 	//定值查询命令帧

	{SEND_ID_CCU_INFO1     		, SYS_DELAY_500ms		, 0,		 	Send_Fream_INFO1	},	  	//遥信遥测帧1

	{SEND_ID_CCU_INFO2     		, SYS_DELAY_500ms 		, 0, 			Send_Fream_INFO2	},		//遥信遥测帧2

	{SEND_ID_CCU_HEART    	 	, SYS_DELAY_500ms 		, 0, 			Send_Fream_Heart	},		//心跳帧


	//自定义协议，模块信息
	{SEND_ID_CCU_USERDEF    	, SYS_DELAY_500ms 		, 0, 			Send_Fream_Userdef	},		//发送自定义数据

};

//接收处理表
static CCU_RECV_TABLE TableRecvPCUDispose[RECV_PCU_NUM] =
{
	{RECV_ID_CCU_CONTROL    ,  	RECV_NOTTIOMOUT,	0 		 				,0		,Recv_Fream_Control     },  //遥控控制帧

	{RECV_ID_CCU_SET  		, 	RECV_NOTTIOMOUT, 	0 		  				,0		,Recv_Fream_Set  		},  //定值设置命令帧

	{RECV_ID_CCU_QUERY  	,  	RECV_NOTTIOMOUT, 	0 		  				,0		,Recv_Fream_Query  		},  //定值查询命令帧

	{RECV_ID_CCU_INFO1  	,  	RECV_NOTTIOMOUT,	0 			 			,0		,Recv_Fream_INFO1  		},  //工作状态及故障信息帧

	{RECV_ID_CCU_INFO2  	,	RECV_NOTTIOMOUT	,  	0 						,0		,Recv_Fream_INFO2  		},   //遥测帧

	{RECV_ID_CCU_INFO3  	,  	RECV_NOTTIOMOUT, 	0 						,0		,Recv_Fream_INFO3 		},   //告警信息帧

	{RECV_ID_CCU_HEART  	,  	RECV_NOTTIOMOUT, 	SYS_DELAY_10s 			,0		,Recv_Fream_Heart  		},   //心跳帧 连续超时三次才关机

	{RECV_ID_CCU_USERDEF  	,  	RECV_NOTTIOMOUT, 	0 						,0		,Recv_Fream_Usedef  	},   //自定义协议接受处理
};


/*****************************************************************************
* Function      : APP_CCUTxPeriodSend
* Description   : 周期性发送函数
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   :2020年6月5日  叶喜雨
*****************************************************************************/
INT8U APP_CCUTxPeriodSend(void)
{
	INT8U i;
	static INT32U delaynowtime,delayoldtime;   //不能一次性放入很多帧，需要间隔50ms放入一帧
	INT32U nowsystime = 0;
	_CCU_SEND_TABLE* p = TableSendCCUDispose;

	delaynowtime = OSTimeGet();

	if(delaynowtime > (delayoldtime + SYS_DELAY_50ms))
	{
		nowsystime = OSTimeGet();
		for (i = 0; i < _SEND_CCU_MAX; i++)
		{
			if (p[i].CycleTime == 0)
			{
				//执行周期为0，表示不执行
				continue;
			}
			//判断时间是否已经到了
			if (nowsystime >= p[i].OldTime)
			{
				if (nowsystime - p[i].OldTime >= p[i].CycleTime)
				{
					if (p[i].SendFun != NULL)
					{
						delayoldtime = delaynowtime;
						p[i].OldTime = nowsystime; //更新时间
						p[i].SendFun(p[i].SendID);
						break;
					}
				}
			}
			else
			{
				if (INT32U_MAX_NUM - p[i].OldTime + nowsystime >= p[i].CycleTime)
				{
					if (p[i].SendFun != NULL)
					{
						delayoldtime = delaynowtime;
						p[i].OldTime = nowsystime; //更新时间
						p[i].SendFun(p[i].SendID);
						break;
					}
				}
			}

		}
	}
	else
	{
		if(delayoldtime  > delaynowtime)  //49天左右可能会进来一次
		{
			delayoldtime  = delaynowtime;
		}
	}
	return TRUE;
}



/*****************************************************************************
* Function      : APP_CCURxDispose
* Description   : 接收函数处理
* Input         :
				id  :CANid
			   pdata:CAN数据指针
* Output        : None
* Note(s)       :
* Contributor   :2019年11月4日  叶喜雨
*****************************************************************************/
static INT8U APP_CCURxDispose(INT32U id,INT8U* pdata,INT8U len)
{
	INT8U i;
	INT8U gun = 0;
	CCU_RECV_TABLE *p = TableRecvPCUDispose;
	INT32U cmdid = 0;
	INT32U nowsystime = 0;
	static INT8U buf[8];

	OSSchedLock();
	memcpy(buf,pdata,MIN(len,8));
	if((pdata == NULL) || !len)
	{
		OSSchedUnlock();
		return FALSE;
	}
	nowsystime = OSTimeGet();

	if((((id & 0x0000ff00) >> 8) !=  (SYSSet.PMUNUM*2 + 0x30))   && (((id & 0x0000ff00) >> 8) !=  (SYSSet.PMUNUM*2 + 0x31)))
	{
		OSSchedUnlock();
		//枪号不一致直接返回
		return FALSE;
	}
	if((id & 0x00000f00) >> 8 ==  (SYSSet.PMUNUM*2))
	{
		gun = GUN_A;
	}
	else
	{
		gun = GUN_B;
	}

	if((id & 0x000000f0) != 0x10)
	{
		OSSchedUnlock();
		//枪号不一致直接返回
		return FALSE;
	}
	cmdid = id &0xffff30ff;			//用命令基地址去比较
	for(i = 0; i < RECV_PCU_NUM; i++)
	{
		if(p[i].ID ==  cmdid)
		{
			if(p[i].Fun != NULL)
			{
				p[i].Fun(gun,buf,len);
				if (p[i].TimeOut != 0)		//需要超时判断
				{
					p[i].OldTime  =  nowsystime;   		//更新当前时间
					p[i].IfTimeout = 0;		//接收未超时
				}
				break;
			}
		}
	}
	OSSchedUnlock();
	return TRUE;
}


/*****************************************************************************
* Function      : Judge_RxPCUTimeOut
* Description   : 接收CCU超时判断
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   :2019年12月4日  叶喜雨
*****************************************************************************/
static INT8U Judge_RxPCUTimeOut(void)
{
	INT8U i;
	CCU_RECV_TABLE *p = TableRecvPCUDispose;
	INT32U nowsystime = 0;

	nowsystime = OSTimeGet();
	//超时判断
	for(i = 0; i < RECV_PCU_NUM; i++)
	{
		if (p[i].TimeOut == 0)
		{
			//超时时间0，表示不执行
			continue;
		}
		if (nowsystime >= p[i].OldTime)
		{
			//printf("%d\r\n",nowsystime - p[i].OldTime);
			if ((nowsystime - p[i].OldTime) >= p[i].TimeOut)
			{
				//printf("%d\r\n",nowsystime - p[i].OldTime);
				p[i].OldTime  =  nowsystime;   		//更新当前时间
				p[i].IfTimeout = 1;			//接收超时
			}
		}
		else
		{
			if ((INT32U_MAX_NUM - p[i].OldTime + nowsystime) >= p[i].TimeOut)
			{
				p[i].OldTime  =  nowsystime;   		//更新当前时间
				p[i].IfTimeout = 1;			//接收超时
			}
		}
	}
	return TRUE;
}

/*****************************************************************************
* Function      : Dispose_RxPCUTimeOut
* Description   : 超时处理
* Input         : void
* Output        : None
* Note(s)       :
* Contributor   :2019年12月4日  叶喜雨
*****************************************************************************/
static INT8U Dispose_RxPCUTimeOut(void)
{
	CCU_RECV_TABLE *p = TableRecvPCUDispose;
	static _BSP_MESSAGE send_message;
	INT8U i;
	static INT8U count = 0;   //连续超时3次才关机
	static INT32U  lasttime = 0,curtime = 0;

	curtime = OSTimeGet();
	for(i = 0; i < RECV_PCU_NUM; i++)
	{
		if(p[i].IfTimeout)
		{
			break;

		}
	}


	//真正得通信异常比较少，模块也没有输出，没有影响。
	//MCP2515 可能出现模块超时20201214 雨
	//是否需要屏蔽下面？？ 20210225

	if(i != RECV_PCU_NUM)  //说明超时了
	{
		//走关机流程
		if(curtime - lasttime > SYS_DELAY_10s)
		{
			lasttime = curtime;
			count++;
			if(count >= 3)
			{
				count = 0;
				SetBcuRemoteSignalState(GUN_A,(INT8U)REMOTE_SIGNALE_1, 5, 1);	//通信超时
				SetBcuRemoteSignalState(GUN_B,(INT8U)REMOTE_SIGNALE_1, 5, 1);	//通信超时

				if (!( (GetBMSStartCharge(GUN_A) == FALSE) && (GetBMSStopCharge(GUN_A) == FALSE) ) )
				{

					APP_Set_ERR_Branch(GUN_A,STOP_HEARTTOUT);   //心跳超时
					{
						//通知控制任务停止充电
#warning "故障原因没有处理 2020"
						printf("PCU Recv Timeout\r\n");
						//printf("TotalErr:%x\r\n",SysState.TotalErrState);
						SetStartFailType(GUN_A,ENDFAIL_OTHERERR);
						send_message.MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
						send_message.DivNum = APP_CHARGE_END;
						send_message.GunNum = GUN_A;
						send_message.DataLen = 1;
						send_message.pData = (INT8U*)STOP_ERR;   //出现故障停止
						OSQPost(Control_PeventA, &send_message);
						CCUSendData[GUN_A].SendControlCmd.SetCur = 0;
						CCUSendData[GUN_A].SendControlCmd.SetVol = 0;
						CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
						OSTimeDly(SYS_DELAY_1s);
					}
				}


				if (!( (GetBMSStartCharge(GUN_B) == FALSE) && (GetBMSStopCharge(GUN_B) == FALSE) ) )
				{

					APP_Set_ERR_Branch(GUN_B,STOP_HEARTTOUT);   //心跳超时
					{
						//通知控制任务停止充电
#warning "故障原因没有处理 2020"
						printf("PCU Recv Timeout\r\n");
						//printf("TotalErr:%x\r\n",SysState.TotalErrState);
						SetStartFailType(GUN_B,ENDFAIL_OTHERERR);
						send_message.MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
						send_message.DivNum = APP_CHARGE_END;
						send_message.DataLen = 1;
						send_message.GunNum = GUN_B;
						send_message.pData = (INT8U*)STOP_ERR;   //出现故障停止
						OSQPost(Control_PeventB, &send_message);
						CCUSendData[GUN_B].SendControlCmd.SetCur = 0;
						CCUSendData[GUN_B].SendControlCmd.SetVol = 0;
						CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
						OSTimeDly(SYS_DELAY_1s);
					}
				}
			}
		}
	}
	else
	{
		SetBcuRemoteSignalState(GUN_A,(INT8U)REMOTE_SIGNALE_1, 5, 0);	//通信超时
		SetBcuRemoteSignalState(GUN_B,(INT8U)REMOTE_SIGNALE_1, 5, 0);	//通信超时
		count = 0;
		lasttime = OSTimeGet();
	}
	return TRUE;
}



/*****************************************************************************
* Function      : APP_PowerModuleDataDeal
* Description   : 处理其他任务发送过来的命令
* Input         : _BSP_MESSAGE *pMsg
* Output        : None
* Note(s)       :
* Contributor   : 2016年7月7日  叶喜雨
*****************************************************************************/
INT8U APP_PowerModuleDataDeal(_BSP_MESSAGE *pMsg)
{

	INT8U i;
	INT8U gun;
	_PCU_DEAL_CMD* p = NULL;

	if (pMsg == NULL)
	{
		return FALSE;
	}
	gun = pMsg->GunNum;
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	p = (_PCU_DEAL_CMD*)&PCUDealCmdTable[gun];
	for (i = 0; i < PCU_CMD_TABLE_NUM; i++)
	{
		if (p[i].DivNum == pMsg->DivNum)
		{
			if (p[i].Fun != NULL)
			{
				return (p[i].Fun( (void *)pMsg) );
			}
		}
	}
	return FALSE;
}

/*
******************************************************************************
* 文件名         : PCU_ParaInit
* 描述           : PCU参数初始化
* 输入           :
* 输入           :
* 输入           :
* 输出           : 无
* 返回           : 无
******************************************************************************
*/
static void PCU_ParaInit(void)
{

	memset(&CCUSendData[GUN_A],0,sizeof(_CCU_SEND_DATA));
	memset(&CCURecvData[GUN_A],0,sizeof(_CCU_RECV_DATA));
	CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;

	memset(&CCUSendData[GUN_B],0,sizeof(_CCU_SEND_DATA));
	memset(&CCURecvData[GUN_B],0,sizeof(_CCU_RECV_DATA));
	CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
}


/*****************************************************************************
* 文件名         : PCU_ParaInit
* 描述           : PCU参数初始化
* 输入           :
* 输入           :
* 输入           :
* 输出           : 无
* 返回           : 无
******************************************************************************/
static void RECV_ID_Dispose(INT32U exteid)
{
	static _BSP_MESSAGE send_message[GUN_MAX];
	static INT32U curtime = 0,lasttime = 0;

	curtime = OSTimeGet();

//	0x18011030						//判断ID是否重复
	if(((exteid & 0x000000ff) == (0x30 + SYSSet.PMUNUM*2)) || ((exteid & 0x000000ff) == (0x30 + SYSSet.PMUNUM*2 +1)))  //其他CMU设置了同样的ID,需要显示ID重复故障
	{
		lasttime = curtime;
		SysState[GUN_A].GUNIDState = GUN_ID_FAIL;
		SysState[GUN_B].GUNIDState = GUN_ID_FAIL;
		if (!( (GetBMSStartCharge(GUN_A) == FALSE) && (GetBMSStopCharge(GUN_A) == FALSE) ) )
		{
#warning "故障原因没有处理 2020"
			printf("PCU Recv Timeout\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_A,ENDFAIL_OTHERERR);
			send_message[GUN_A].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_A].DivNum = APP_CHARGE_END;
			send_message[GUN_A].GunNum = GUN_A;
			send_message[GUN_A].DataLen = 1;
			send_message[GUN_A].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventA, &send_message[GUN_A]);
			CCUSendData[GUN_A].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_A].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}

#if(USER_GUN != USER_SINGLE_GUN)
		if (!( (GetBMSStartCharge(GUN_B) == FALSE) && (GetBMSStopCharge(GUN_B) == FALSE) ) )
		{

#warning "故障原因没有处理 2020"
			printf("PCU Recv Timeout\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_B,ENDFAIL_OTHERERR);
			send_message[GUN_B].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_B].DivNum = APP_CHARGE_END;
			send_message[GUN_B].DataLen = 1;
			send_message[GUN_B].GunNum = GUN_B;
			send_message[GUN_B].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventB, &send_message[GUN_B]);
			CCUSendData[GUN_B].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_B].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
#endif
	}
	else
	{
		if(SysState[GUN_B].GUNIDState != GUN_ID_NORMAL)
		{
			if((curtime - lasttime) > SYS_DELAY_3s)
			{
				SysState[GUN_B].GUNIDState = GUN_ID_NORMAL;
				SysState[GUN_A].GUNIDState = GUN_ID_NORMAL;
			}
		}
		else
		{
			lasttime = curtime;
		}
	}
}


/*****************************************************************************
* 文件名         : RECV_PCU_HearInfoDispose
* 描述           : PCU心跳信息处理
* 输入           :
* 输入           :
* 输入           :
* 输出           : 无
* 返回           : 无
******************************************************************************/
static void RECV_PCU_HearInfoDispose(void)
{
	static _BSP_MESSAGE send_message[GUN_MAX];
	if(CCURecvData[GUN_A].RecvHeart.ifGunUser)   //枪不可用，为相关接触器粘连,或默认模块离线
	{
		if (!( (GetBMSStartCharge(GUN_A) == FALSE) && (GetBMSStopCharge(GUN_A) == FALSE) ) )
		{
			printf("PCU GUNA GunNotUser\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_A,ENDFAIL_OTHERERR);
			send_message[GUN_A].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_A].DivNum = APP_CHARGE_END;
			send_message[GUN_A].GunNum = GUN_A;
			send_message[GUN_A].DataLen = 1;
			send_message[GUN_A].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventA, &send_message[GUN_A]);
			CCUSendData[GUN_A].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_A].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
	}
	if(CCURecvData[GUN_B].RecvHeart.ifGunUser)   //枪不可用，为相关接触器粘连,或默认模块离线
	{
		if (!( (GetBMSStartCharge(GUN_B) == FALSE) && (GetBMSStopCharge(GUN_B) == FALSE) ) )
		{
			printf("PCU GUNB GunNotUser\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_B,ENDFAIL_OTHERERR);
			send_message[GUN_B].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_B].DivNum = APP_CHARGE_END;
			send_message[GUN_B].DataLen = 1;
			send_message[GUN_B].GunNum = GUN_B;
			send_message[GUN_B].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventB, &send_message[GUN_B]);
			CCUSendData[GUN_B].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_B].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
	}
	if(CCURecvData[GUN_A].RecvHeart.PDUSingnal || CCURecvData[GUN_B].RecvHeart.PDUSingnal)   //PDU通信超时
	{
		if (!( (GetBMSStartCharge(GUN_A) == FALSE) && (GetBMSStopCharge(GUN_A) == FALSE) ) )
		{
			printf("PCU GUNA Singnal TimeOut\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_A,ENDFAIL_OTHERERR);
			send_message[GUN_A].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_A].DivNum = APP_CHARGE_END;
			send_message[GUN_A].GunNum = GUN_A;
			send_message[GUN_A].DataLen = 1;
			send_message[GUN_A].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventA, &send_message[GUN_A]);
			CCUSendData[GUN_A].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_A].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
		if (!( (GetBMSStartCharge(GUN_B) == FALSE) && (GetBMSStopCharge(GUN_B) == FALSE) ) )
		{
			printf("PCU GUNB Singnal TimeOut\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_B,ENDFAIL_OTHERERR);
			send_message[GUN_B].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_B].DivNum = APP_CHARGE_END;
			send_message[GUN_B].DataLen = 1;
			send_message[GUN_B].GunNum = GUN_B;
			send_message[GUN_B].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventB, &send_message[GUN_B]);
			CCUSendData[GUN_B].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_B].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
	}

	if(CCURecvData[GUN_A].RecvHeart.DCSWFail)   //枪不可用，为相关接触器粘连,或默认模块离线
	{
		if (!( (GetBMSStartCharge(GUN_A) == FALSE) && (GetBMSStopCharge(GUN_A) == FALSE) ) )
		{
			printf("PCU GUNA GunNotUser\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_A,ENDFAIL_OTHERERR);
			send_message[GUN_A].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_A].DivNum = APP_CHARGE_END;
			send_message[GUN_A].GunNum = GUN_A;
			send_message[GUN_A].DataLen = 1;
			send_message[GUN_A].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventA, &send_message[GUN_A]);
			CCUSendData[GUN_A].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_A].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_A].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
	}
	if(CCURecvData[GUN_B].RecvHeart.DCSWFail)   //枪不可用，为相关接触器粘连,或默认模块离线
	{
		if (!( (GetBMSStartCharge(GUN_B) == FALSE) && (GetBMSStopCharge(GUN_B) == FALSE) ) )
		{
			printf("PCU GUNB GunNotUser\r\n");
			//printf("TotalErr:%x\r\n",SysState.TotalErrState);
			SetStartFailType(GUN_B,ENDFAIL_OTHERERR);
			send_message[GUN_B].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
			send_message[GUN_B].DivNum = APP_CHARGE_END;
			send_message[GUN_B].DataLen = 1;
			send_message[GUN_B].GunNum = GUN_B;
			send_message[GUN_B].pData = (INT8U*)STOP_ERR;   //出现故障停止
			OSQPost(Control_PeventB, &send_message[GUN_B]);
			CCUSendData[GUN_B].SendControlCmd.SetCur = 0;
			CCUSendData[GUN_B].SendControlCmd.SetVol = 0;
			CCUSendData[GUN_B].SendControlCmd.Cmd.Operate_Cmd = STOP_CHARGE;
			OSTimeDly(SYS_DELAY_1s);
		}
	}
}

/*****************************************************************************
* Function      : TaskPCUTxMain
* Description   : 周期性发送消息给功率模块
* Input         : void *pdata
* Output        : None
* Note(s)       :
* Contributor   : 2016年7月8日  叶喜雨
*****************************************************************************/
void TaskPCUTxRxMain(void *pdata)
{
	INT8U err;
	INT32U exteid;
	_BSP_MESSAGE *pMsg;
	INT8U offset = 0;

	OSTimeDly(SYS_DELAY_1s);
	CANMutex = OSMutexCreate(1, &err);      //
	InitCAN_Module();
	PCU_ParaInit();
	OSTimeDly(SYS_DELAY_3s);
	while(1)
	{
		pMsg = OSQPend(TaskPCU_pevent, SYS_DELAY_2ms, &err); //接收数据
		if (err == OS_ERR_NONE)
		{
			switch(pMsg->MsgID)
			{
				case BSP_MSGID_CAN_RXOVER:
					offset = pMsg->DivNum;
					APP_CCURxDispose(bxModuleCANSet.RxMessage[offset].ExtId, \
					                 bxModuleCANSet.RxMessage[offset].Data, \
					                 bxModuleCANSet.RxMessage[offset].DLC);

					exteid = bxModuleCANSet.RxMessage[offset].ExtId;
					break;
				case BSP_MSGID_CONTROL: //收到控制任务的命令
				case BSP_MSGID_BCU:		//收到TCU的命令
					//printf("Msg\r\n");
					APP_PowerModuleDataDeal(pMsg);
					break;
				default:
					break;
			}
		}

		Judge_RxPCUTimeOut();			//  判断接受是否超时
		Dispose_RxPCUTimeOut();			//超时处理
		RECV_ID_Dispose(exteid);		//ID判断
		RECV_PCU_HearInfoDispose();		//PCU心跳返回处理

	}
}

/*****************************************************************************
* Function      : TaskPCUTxMain
* Description   : 周期性发送消息给功率模块
* Input         : void *pdata
* Output        : None
* Note(s)       :
* Contributor   : 2016年7月8日  叶喜雨
*****************************************************************************/
void TaskPCUTxMain(void *pdata)
{
	OSTimeDly(SYS_DELAY_3s);
	while(1)
	{
		APP_CCUTxPeriodSend();    		//周期性发送数据
		OSTimeDly(SYS_DELAY_1ms);
	}
}


/*****************************************************************************
* Function      : GetModuleReturnACVol
* Description   : 获取模块返回得三项电压
* Input         : void *pdata
* Output        : None
* Note(s)       :
* Contributor   : 2016年7月8日  叶喜雨
*****************************************************************************/
INT16U GetModuleReturnACVol(void)
{
	INT16U acvol = CCURecvData[GUN_A].RecvACVol.AC_Vab;

	if(acvol < CCURecvData[GUN_A].RecvACVol.AC_Vbc)
	{
		acvol = CCURecvData[GUN_A].RecvACVol.AC_Vbc;
	}
	if(acvol < CCURecvData[GUN_A].RecvACVol.AC_Vca)
	{
		acvol = CCURecvData[GUN_A].RecvACVol.AC_Vca;
	}

	if(acvol < CCURecvData[GUN_B].RecvACVol.AC_Vbc)
	{
		acvol = CCURecvData[GUN_B].RecvACVol.AC_Vbc;
	}
	if(acvol < CCURecvData[GUN_B].RecvACVol.AC_Vca)
	{
		acvol = CCURecvData[GUN_B].RecvACVol.AC_Vca;
	}
	if(acvol < CCURecvData[GUN_B].RecvACVol.AC_Vab)
	{
		acvol = CCURecvData[GUN_B].RecvACVol.AC_Vab;
	}
	return acvol;
}




/************************(C)COPYRIGHT 2010 汇誉科技****END OF FILE****************************/
