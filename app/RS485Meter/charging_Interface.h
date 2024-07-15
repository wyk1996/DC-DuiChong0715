﻿/*****************************************Copyright(C)******************************************
*******************************************杭州快电*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: chargeing_Interface.h
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
#ifndef __CHARGING_INTERFACE_H_
#define __CHARGING_INTERFACE_H_
/* Includes-----------------------------------------------------------------------------------*/
#include	"sysconfig.h"
#include  "app_conf.h"
#include  "RS485Meter.h"
/* Private define-----------------------------------------------------------------------------*/

/* Private struct-----------------------------------------------------------------------------*/
typedef enum
{
	_COM_NORMAL,    //通信正常
	_COM_FAILED,    //通信失败
}COMSTATUS;
/* Private functions--------------------------------------------------------------------------*/
/*****************************************************************************
* Function     : APP_GetPricEvent
* Description  : 获取计费事件控制块指针
* Input        : 
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018.8.22  Z
*****************************************************************************/
OS_EVENT* APP_GetPricEvent(void);

/****************************************************************************
* Function     : GetMeterStatus
* Description  : 获取电表状态
* Input        : gun 枪号
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年8月22日  
******************************************************************************/
COMSTATUS GetMeterStatus(_GUN_NUM gun);

/****************************************************************************
* Function     : GetChargingStatus
* Description  : 获取计费状态
* Input        : gun 枪号
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年8月22日  
******************************************************************************/
CHARGING_STATUS GetChargingStatus(_GUN_NUM gun);

/****************************************************************************
* Function     : GetChargingInfo
* Description  : 获取计费信息
* Input        : gun 枪号
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年8月22日  
******************************************************************************/
USERINFO* GetChargingInfo(_GUN_NUM gun);

/****************************************************************************
* Function     : APP_GetMeterPowerInfo
* Description  : 回去电表读数 2位小数
* Input        : gun 枪号
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年8月22日  
******************************************************************************/
TEMPDATA * APP_GetMeterPowerInfo(_GUN_NUM gun);

#endif 
/************************(C)COPYRIGHT 2018 杭州快电*****END OF FILE****************************/
