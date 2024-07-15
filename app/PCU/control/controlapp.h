/*****************************************Copyright(C)******************************************
*******************************************���ݻ���*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          :
* Author              :
* Date First Issued : 2018��8��31��
* Version           : V0.1
* Description       : 
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
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

//ģ���ַ��0��ʼ��ģ����Ŵ�1��ʼ
/* Private define-----------------------------------------------------------------------------*/ 
#define MODULE_R_REPLY_CON				5 //ģ����ճ�ʱ����������5��δ�յ���Ϊ���ߣ�


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
}GUN_USERSTATE;   //���ǹ�Ƿ���� 



/* Private typedef----------------------------------------------------------------------------*/


/* Private macro------------------------------------------------------------------------------*/

//������Ϣ
__packed typedef struct 
{
    INT16U Vol;       					//0.1V
    INT16U Cur;       					//0.01A
    _OPERATE_CMD  cmd;					//��������
	_CHARGE_MODE	ChargeType;			//��緽ʽĿǰû��
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
