/*****************************************Copyright(C)******************************************
*******************************************杭州快电*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: DispkeyFunction.h
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
#ifndef _DISP_KEY_FUNCTION_H
#define _DISP_KEY_FUNCTION_H
/* Includes-----------------------------------------------------------------------------------*/
#include "Display.h"
/* Private define-----------------------------------------------------------------------------*/
/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/
/*****************************************************************************
* Function     : GetCurMenu
* Description  : 获取液晶当前菜单结构体
* Input        : void  
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  :  2018年6月14日  
*****************************************************************************/
ST_Menu* GetCurMenu(void);

/*****************************************************************************
* Function     : GetCurMenu
* Description  : 设置液晶当前菜单结构使
* Input        : void  
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  :  2018年7月17日
*****************************************************************************/
void SetCurMenu(ST_Menu*  pmenu);


void DisplayMenu1(void);
/********************************************************************
* Function Name : DisplayMenu1
* Description   : 主界面页面显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年6月14日  
********************************************************************/
void DisplayMenu7(void);

/********************************************************************
* Function Name : DisplayMenu8
* Description   : 付费方式选择显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu8(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   : 二维码界面显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu9(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电方式选择界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu10(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电时间界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu11(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电电量界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu12(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电金额界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu13(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入卡密码界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu14(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu15(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu16(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu17(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu18(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu19(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu20(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 电卡密码输入错误界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu21(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 卡内余额不足
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu22(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 此卡已被锁提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu23(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 此卡无法识别提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu24(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu25(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu26(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 当前负荷已满，是否继续充电
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu27(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu28(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu29(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu30(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu31(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu32(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu33(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu34(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu35(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu36(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu37(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu38(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu39(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 当前负荷已满，是否等待充电确认界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu40(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 故障提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu41(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu42(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示连接充电枪界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu43(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪A充电界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu44(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪B充电界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu45(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪A车辆信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu46(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪B车辆信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu47(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu48(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu49(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu50(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu51(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu52(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu53(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示直接结算可能造成的后果
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu54(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示卡号不一致
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu55(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示充电停止中请勿把枪1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu56(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示充电停止中请勿把枪2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu57(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : A枪结算界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu58(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : B枪结算界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu59(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户查询选择界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu60(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入查询账户密码界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu61(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 未找到账户信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu62(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu63(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu64(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu65(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu66(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 当前负荷已满，是否继续充电
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu67(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu68(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 费率详情界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu69(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu70(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 手动模式枪选择界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu71(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 系统信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu72(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : A枪手动模式
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu73(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : B枪手动模式
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu74(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 系统配置1界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu75(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 系统配置2界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu76(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 系统配置3界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu77(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 系统配置4界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu78(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 系统配置5界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu79(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 当前负荷已满，是否等待充电确认界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu80(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 故障提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu81(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu82(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示连接充电枪界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu83(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪A充电界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu84(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪B充电界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu85(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪A车辆信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu86(void);

/********************************************************************
* Function Name : 程序升级中1
* Description   : 枪B车辆信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu87(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 程序升级中2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu88(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 程序升级中3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu89(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 程序升级中4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu90(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 程序升级中5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu91(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 程序升级中6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu92(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 升级失败界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu93(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : CCU1升级成功界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu94(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : CCU2升级成功界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu95(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : tcu升级成功界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu96(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : pcu升级成功界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu97(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 网络配置1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu98(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 网络配置2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu99(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电模式切换
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu100(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示无故障故障记录界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu101(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 故障记录1界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu102(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 故障记录2界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu103(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 解锁成功界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu104(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 解锁失败界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void DisplayMenu105(void);


/****************************************汇誉屏幕***********************/
/********************************************************************
* Function Name : DisplayMenu1
* Description   : 主界面页面显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年6月14日  
********************************************************************/
void HYDisplayMenu0(void);

/********************************************************************
* Function Name : DisplayMenu8
* Description   : 付费方式选择显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu1(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   : 二维码界面显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu2(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电方式选择界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu3(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电时间界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu4(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电电量界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu5(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电电量界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu6(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   : 主界面页面显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年6月14日  
********************************************************************/
void HYDisplayMenu7(void);

/********************************************************************
* Function Name : DisplayMenu8
* Description   : 付费方式选择显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu8(void);

/********************************************************************
* Function Name : DisplayMenu1
* Description   : 二维码界面显示
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu9(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电方式选择界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu10(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电时间界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu11(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电电量界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu12(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入充电金额界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu13(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入卡密码界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu14(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu15(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu16(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu17(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu18(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu19(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡界面6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu20(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 电卡密码输入错误界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu21(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 卡内余额不足
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu22(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 此卡已被锁提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu23(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 此卡无法识别提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu24(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu25(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu26(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 当前负荷已满，是否继续充电
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu27(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu28(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu29(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu30(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu31(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu32(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪A启动中界面6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu33(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu34(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu35(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu36(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu37(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu38(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面6
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu39(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 当前负荷已满，是否等待充电确认界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu40(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 故障提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu41(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu42(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示连接充电枪界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu43(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪A充电界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu44(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪B充电界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu45(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪A车辆信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu46(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 枪B车辆信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu47(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu48(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu49(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面3
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu50(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu51(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 刷卡结算界面5
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu52(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 充电枪B启动中界面4
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu53(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示直接结算可能造成的后果
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu54(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示卡号不一致
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu55(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示充电停止中请勿把枪1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu56(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 提示充电停止中请勿把枪2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu57(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : A枪结算界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu58(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : B枪结算界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu59(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户查询选择界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu60(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 输入查询账户密码界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu61(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 未找到账户信息界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu62(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu63(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu64(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu65(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu66(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面1
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu73(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu74(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu75(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu76(void);


/********************************************************************
* Function Name : DisplayMenu10
* Description   : 账户信息界面2
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu77(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu78(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu79(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 读卡超时提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu80(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu81(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu82(void);

/********************************************************************
* Function Name : DisplayMenu10
* Description   : 非发行方卡提示界面
* Input         : 
* Output        : None.
* Return        : None.
* Contributor   :  2018年7月17日
********************************************************************/
void HYDisplayMenu83(void);

/********************************************************************
* Function Name : DisplayCommonMenu()
* Description   : 普通菜单显示函数
* Input         : -*pMenu,当前待显菜单结构.
* 				  pPrepage,记录上一页 如不需要继续，则传入NULL
* Output        : None.
* Return        : None.
* Contributor   : 2018年7月13日
********************************************************************/
 void DisplayCommonMenu(struct st_menu* pMenu,struct st_menu * pPrepage);
#endif //_DISP_KEY_FUNCTION_H
/************************(C)COPYRIGHT 2018 杭州快电*****END OF FILE****************************/
