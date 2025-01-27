/*****************************************Copyright(C)******************************************
*******************************************杭州快电*******************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: 
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* //2010		: V
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
#ifndef	__BSP_FM_H_
#define	__BSP_FM_H_					
/* Includes-----------------------------------------------------------------------------------*/
#include "sysconfig.h"
/* Private define-----------------------------------------------------------------------------*/
/*根据使用的芯片情况，选择使能相应的芯片型号,这里只做了512K的和8M的铁电存贮*/
#define FM25L04_ENABLE     0
#define FM25CL64_ENABLE    1
/* 片存贮容量由具体选择的芯片确定 */
#if FM25L04_ENABLE>0
	#define FM_CHIPSIZE	      512ul
#endif//FM25L04_ENABLE
#if FM25CL64_ENABLE>0
	#define FM_CHIPSIZE	      8192ul 
#endif//FM25CL64_ENABLE
#define MAX_FM_LEN        (1*FM_CHIPSIZE) /* 由具体焊接的芯片数确定 */	 
/*硬件配置*/
/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/
/*******************************************************************************
* Function Name  : BSP_InitFm
* Description    : FM25CL64 初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_InitFm(void);

/*******************************************************************************
* Function Name  : BSP_WriteDataToFm
* Description    : Write Data To Fm25CL64
* Input          : FlashAddr - 地址范围 MAX_FM_LEN
*				   Len       - 数据长度
*                  DataAddr -  数据首地址
* Output         : None.
* Return         : TRUE / FALSE
*******************************************************************************/
INT8U BSP_WriteDataToFm(INT32U FlashAddr,INT8U *DataAddr,INT32U Len);

/*******************************************************************************
* Function Name  : BSP_ReadDataFromFm
* Description    : Read Data From Fm25CL64
* Input          : FlashAddr - 地址范围 MAX_FM_LEN
*				   Len       - 数据长度
* Output         : DataAddr -  数据存放首地址
* Return         : TRUE / FALSE
*******************************************************************************/
INT8U BSP_ReadDataFromFm(INT32U FlashAddr,INT8U *DataAddr,INT32U Len);

#endif	//__BSP_FM_H_
/************************(C)COPYRIGHT 2018 杭州快电*****END OF FILE****************************/
