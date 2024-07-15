/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: appconfig.h
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
#ifndef	__APPCONFIG_H_
#define	__APPCONFIG_H_
/* Includes-----------------------------------------------------------------------------------*/
/* Private define-----------------------------------------------------------------------------*/

#define INT32U_MAX_NUM          (0xFFFFFFFFu)

#define  ACIN_UVP	  2900					//交流输入欠压线电压  290V   
#define  ACIN_OVER	  4600			  		//交流输入过压线电压  460V


//充电模式选择
typedef enum
{

    MODE_CHARGE_AVER = 0x00, 	//独立运行
    MODE_CHARGE_DEMAND,   		//智能分配
    MODE_CHARGE_QZ,   		//强制分配 （必须2辆车都可以充）
    MODE_CHARGE_MAX,
}_CHARGE_MODE;



//模块类型
typedef enum
{
    YFY75020_MODULE  =0,    //YFY750模块
    YFY50025_MODULE ,   	 //YFY500模块
    YFY75035_MODULE   ,    //YFY750模块
	YFY75050_MODULE   ,    //YFY750模块
	YKR15075000_MODULE,		//英可瑞150V，7.5KW模块
	SH75015K3B_MODULE,	 //盛弘15KW模块
	YL75030_MODULE,		//永联75030 20KW模块
	YFY75040_MODULE   ,    //YFY750模块 15kw
	SH75020K3B_MODULE   ,    //YFY750模块 20kw
	TH30F10025C7_MODULE,	//通合30kw模块
}_MODULE_OUTLIMT_CMD;



//模块类型选择
//模块品牌 10 个
typedef enum
{
	YFY_MODULE =0, //英飞源
	YKR_MODULE, //英可瑞
	YYLN_MODULE, //优优绿能
	MGMT_MODULE, //麦格米特
	TRD_MODULE, //特锐德
	HUWEI_MODULE, //华为
	LEINENG_MODULE, //雷能
	YFY1_MODULE, //英飞源(待增加)
	YFY2_MODULE, //英飞源(待增加)
	YFY3_MODULE,  //英飞源(待增加)
	MAX_MODULE,
}_MODULE_BRAND;


typedef enum
{
	MK_20kW750V =0, //20kW750V 恒功率模块
	MK_20kW500V , //20kW500V 模块
	MK_10kW150V, //10kW150V 模块
	MK_B75025T, //15kW 双向模块
	MK1_20kW750V, //20kW750V 恒功率模块(待补充)
	MK2_20kW750V, //20kW750V 恒功率模块(待补充)
	MK3_20kW750V, //20kW750V 恒功率模块(待补充)
	MK4_20kW750V, //20kW750V 恒功率模块(待补充)
	MK5_20kW750V, //20kW750V 恒功率模块(待补充)
	MK6_20kW750V, //20kW750V 恒功率模块(待补充)
	MAX_TYPE
}_MODULE_TYPE;


#define POWER_MODULE_YKR           0x01              	 //英科瑞功率模块
#define POWER_MODULE_YFY           0x02              	 //英飞源的功率模块
#define POWER_MODULE_GW            0x03              	 //国网模块
#define POWER_MODULE_SH            0x05              	 //国网模块   

#define MODULENUM_MAX_USED 			8					//模块最大为8个
#define MODULENUM_SIGNAL_USED 		4					//单枪为最大为4个


//#define POWER_MODULE_TYPE_CUR      POWER_MODULE_YFY  		//选择的功率模块厂商
//#define POWER_MODULE_TYPE          YFY75050_MODULE     		//模块具体类型
//#define POWER_MODULE_TYPE          YL75030_MODULE     		//模块具体类型

//#define POWER_MODULE_TYPE_CUR      POWER_MODULE_SH  		//选择的功率模块厂商
//#define POWER_MODULE_TYPE          SH75015K3B_MODULE     		//模块具体类型

#define GUN_CHARGE_MODE			   MODE_CHARGE_QZ			//充电模式
 

/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/

#endif	//__APPCONFIG_H_
/************************(C)COPYRIGHT 2018 杭州汇誉*****END OF FILE****************************/
