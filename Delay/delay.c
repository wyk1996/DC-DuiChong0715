﻿/*****************************************Copyright(C)******************************************
*******************************************杭州快电*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: delay.c
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		:
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "ucos_ii.h"
#include "delay.h"

/* Private define-----------------------------------------------------------------------------*/

/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
static u8 fac_us;
static u16 fac_ms;
/* Private function prototypes----------------------------------------------------------------*/
/* Private function ----------------------------------------------------------------*/
/*****************************************************************************
* Function     : DelayUs
* Description  : 延时us
* Input        : u32 nus : 要延时的us数，必须要小于等于2^24/fac_us。
                           使用了死延时的方式，要确报调用此函数不会引起实时性问题 
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年5月25日
*****************************************************************************/
void DelayUs(u32 nus)
{
    u32 ticks;
    u32 told, tnow, tcnt=0;
    u32 reload = SysTick->LOAD;  //LOAD值,递减到0后会从此值开始重新递减
    ticks = nus * fac_us;        //计算需要的节拍数

    if (!nus)
    {
        return;
    }
    OSSchedLock();               //停止调度
    told = SysTick->VAL;         //保存开始的计数器值
    while (1)
    {
        tnow = SysTick->VAL;     //读取当前的节拍数
        if (tnow != told)
        {
            //计算差值
            tcnt = (tnow < told) ? (told - tnow) : (reload - tnow + told);
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
    OSSchedUnlock();               //允许调度
}

/*****************************************************************************
* Function     : DelayMs
* Description  : 延时ms
* Input        : u16 nms  : 要延时的ms数，取值为0~65535，此函数不能在中断中调用，
                            也可能存在死延时的方式，要确报调用此函数不会引起实时性问题
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年5月25日
*****************************************************************************/
void DelayMs(u16 nms)
{	
	if(OSRunning && OSIntNesting==0)        //只允许OS运行起来，且不在中断中调用
	{		 
		if (nms >= fac_ms)					//延时的时间大于OS的节拍
		{ 
   			OSTimeDly(nms / fac_ms);	    //OS延时
        }
		nms %= fac_ms;						//OS无法提供更小的延时，使用普通方式延时
	}
	DelayUs((u32)(nms*1000));				//普通方式延时
}

/*****************************************************************************
* Function     : Delay_Init
* Description  : 延时初始化
* Input        : u32 SYSCLK : 系统时钟频率 
* Output       : None
* Return       : 
* Note(s)      : SYSTICK时钟设置为AHB时钟
* Contributor  : 2018年5月25日
*****************************************************************************/
void DelayInit(u32 SYSCLK)
{
    fac_us = SYSCLK / 1000000;
    fac_ms = 1000 / OS_TICKS_PER_SEC;
}


