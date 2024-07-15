/*****************************************Copyright(C)******************************************
******************************************杭州快电*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: WT588D.h
* Author			: WB
* Date First Issued	: 2018/04/01
* Version			: V
* Description		:  
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
#ifndef	__BSP_WT588D_H_
#define	__BSP_WT588D_H_

#include "sysconfig.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
/***********************************************************************************************
* Function		: BSP_LEDInit
* Description	: 驱动初始化
* Input			: 
* Output		: 
* Note(s)		: 
* Contributor	: 
***********************************************************************************************/
void WT588D_20SSInit(void);

/***********************************************************************************************
* Function		: BSP_LEDInit
* Description	: 驱动初始化
* Input			: 
* Output		: 
* Note(s)		: 
* Contributor	: 2018年7月9日
***********************************************************************************************/
void MT588D_Flash(INT8U addr);
#endif	//__BSP_LED_H_
/************************(C)COPYRIGHT 2018 杭州快电****END OF FILE****************************/