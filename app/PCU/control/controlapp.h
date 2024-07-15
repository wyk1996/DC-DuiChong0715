/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          :
* Author              :
* Date First Issued : 2018年8月31日
* Version           : V0.1
* Description       : 
*----------------------------------------历史版本信息-------------------------------------------
* History             :
* //2010            : V
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#ifndef	__CONTROLAPP_H_
#define	__CONTROLAPP_H_

/* Includes-----------------------------------------------------------------------------------*/
#include "sysconfig.h"
#include "appconfig.h"
#include "controlmain.h"

//模块地址从0开始，模块组号从1开始
/* Private define-----------------------------------------------------------------------------*/ 
#define MODULE_R_REPLY_CON				5 //模块接收超时计数（连续5次未收到即为离线）


typedef enum
{
    MODULE_CLOSE = 0,
    MODULE_OPEN,
	MODULE_STATE_MAX,
}MODULE_STATE;


typedef enum
{
    GUN_CLOSE = 0,
    GUN_OPEN,
	GUN_STATE_MAX,
}GUN_STATE;

typedef enum
{
    GUN_USER = 0,
    GUN_NOTUSER,
}GUN_USERSTATE;   //充电枪是否可用 



/* Private typedef----------------------------------------------------------------------------*/


/* Private macro------------------------------------------------------------------------------*/

//需求信息
__packed typedef struct 
{
    INT16U Vol;       					//0.1V
    INT16U Cur;       					//0.01A
    _OPERATE_CMD  cmd;					//操作命令
	_CHARGE_MODE	ChargeType;			//充电方式目前没有
 }_DEMAND_INFO;

 
 __packed typedef struct 
{
    INT16U Vol;       					//0.1V
    INT16U Cur;       					//0.1A
 }_CCU_DEMAND_INFO;
 
 typedef enum
{
	DEMAND_NOTCHANGE = 0,
	DEMAND_CHANGE,
}_DEMAND_CHANGE;


 
/* Private functions--------------------------------------------------------------------------*/
#endif	//__CONTROLAPP_H_

/************************(C)COPYRIGHT 2019 ????*****END OF FILE****************************/
