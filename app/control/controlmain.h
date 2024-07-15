/******************************************************************************

                  版权所有 (C), 2010-2016, 汇誉科技新能源有限公司

 ******************************************************************************
  文 件 名   : ControlMain.h
  版 本 号   : 初稿
  作    者   : 叶喜雨
  生成日期   : 2016年6月6日
  最近修改   :
  功能描述   : 控制任务头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年6月6日
    作    者   : 叶喜雨
    修改内容   : 创建文件

******************************************************************************/
#ifndef __CONTROL_MAIN_H_
#define __CONTROL_MAIN_H_
/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <string.h>
#include "sysconfig.h"
#include "gbstandard.h"
#include "bsp_RTC.h"
#include "PCUMain.h"
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define CONTROL_Q_NUM         (50u)
#define CMD_TABLE_NUM         (14u)

#define MAX_DEVIATION         (100u)    //最大误差为10,放大了10倍
 
 
#define	BHMCMP_VOLT				(300)   //允许BHM采样偏差20V
 
//把v放大/缩小m倍，m非0为放大,0为缩小
#define VAL_ADJUST(v,m,t)     ( (t) ? ( (v) * (m) ) : ( (v) / (m) ) )

//自检模拟需求电压、电流，暂时沿用萍乡的参数
//此处采用两位小数
#define SIMULATE_DEMAND_CUR   VAL_ADJUST(5, 10, 1)   //5A 
#define SIMULATE_DEMAND_VOL   VAL_ADJUST(350, 10, 1) //4000V VAL_ADJUST(400, 100, 1)
#define SIMULATE_DEMAND_NUM   (0u)     //0个

	//英飞源功率模块最大能输出的电压、电流阀值
	#define MAX_CUR_OUTPUT        1200
	#define MAX_VOL_OUTPUT        7500 //750V

	//屏幕设置的保护值,已经做了界面，到时候调试下。60KW暂时这样
	#define LCDMAX_CUR_OUTPUT        (1608u) //13.4A*20*10
	#define LCDMAX_VOL_OUTPUT        (7500u) //750V
	#define MAX_VOL_OUTPUT_DEFAULT   (7500u)
	#define MAX_CUR_OUTPUT_DEFAULT   (1608u)

	#define THEROLD_CUR_OUTPUT        (2600u) //2600
	#define THEROLD_VOL_OUTPUT        (7600u) //760V
	

	//最小输出电流、电压
	#define MIN_CUR_OUTPUT        (0u)     //0A
	#define MIN_VOL_OUTPUT        (1500u)  //150V

//以下宏是保护实验中的要用到的参数
#define AC_POWER_UNDER_VAL    	(3300u)   //
#define AC_POWER_OVER_VAL     	(4500u)   //
#define DC_POWER_UNDER_VAL_10 	(10u)    //输出电压超过最高直流电压值的110%时告警，此处放大了10倍

#define MODULE_WARNING_SHUTDOWN	(0u)   //功率模块告警是否要关机，0:不关机  1:关机

#define DEF_OUTPUT_CUR           50		//默认电流
#define DEF_OUTPUT_VOL           2000	//默认电压

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
typedef INT32U CostType;  //重新定义费率类型
typedef INT16U PowerType; //重新定义电量类型


typedef enum
{
    APP_BMS_SHAKE_HAND = 0x10,
    APP_BMS_STATE,
    APP_BMS_CHARGE_STATE,
    APP_BMS_CHARGE_ERR_A,
    APP_BMS_CHARGE_ERR_B,
    APP_BMS_CHARGE_ERR_C,

    APP_ELEC_LOCKED = 0x20,     //电子锁锁定命令
    APP_ELEC_UNLOCKED,          //电子锁解锁命令
    APP_CHARGE_START,           //开始充电
    APP_CHARGE_END,             //结束充电
    APP_CHARGE_FAIL,            //启动失败
    APP_CHARGE_SUCCESS,         //启动成功
    APP_CHARGE_RUNNING,         //正在充电
    APP_CHARGE_PAUSE,			//暂停充电
    APP_CHARGE_BSD,             //充电统计信息，即BMS返回的BSD报文
    APP_INSULATIONG_CHECK,      //开始绝缘检测   
    APP_BCP_FIT,                //BMS发送回来的BCP参数是否合适
    APP_CHARGE_READY,           //充电机就绪状态检测
    APP_POWER_OUTPUT,           //电能输出
    APP_POWER_ENABLE,           //让功率模块输出电压、电流
    APP_POWER_DISABLE,          //让功率模块关闭输出电压、电流
    APP_POWER_GRP_SET,          //让功率模块进行分组
    APP_POWER_DELAY,            //功率模块延时10s

    BSP_MSGID_READ_PLUS,        //发送读正母线命令
    BSP_MSGID_READ_MINUS,       //发送读负母线命令
    BSP_MSGID_READ_PM,          //发送读正负母线命令
    BSP_MSGID_WAIT,             //停止读绝缘检测
	
	APP_BCU_SET,				//TCU对PCU的交流阀值、充电方式设置
}_BMS_DIV_TYPE;

//停止类型
typedef enum
{
    STOP_UNDEF = 0x00,             //未定义
    STOP_USER = 0x01,              //用户主动停止
    STOP_CONDITION = 0x02,         //达到条件停止
    STOP_ERR = 0x04,               //出现故障停止
    STOP_WARN = 0x08,              //出现告警，但不停止充电
    STOP_NORMAL = 0x10,            //正常停止，非用户主动停止
	STOP_TCU_HEART = 0x20,			//TCU心跳停止
}_STOP_TYPE;

//告警检测
#define WARN_CHECK(w)       ((w)&0x08)
//错误检测
#define ERR_CHECK(e)        ((e)&0x04)
//用户主动停止检测
#define USER_CHECK(u)       ((u)&0x01)

//预充准备状态
typedef enum{
	READY_ING = 0,   //准备中
	READY_SUCCESS,	//准备成功
	READY_FAIL,		//准备失败
}_CHARGE_READY_STATE;

typedef enum
{
    STATE_UNDEF,            //没有定义，初始值
    STATE_NO_ERR,           //没有错误
    STATE_ERR,              //有错误
    STATE_ABORT,            //中止
}_CONTROL_STATE_TYPE;

//系统总错误类型
typedef enum
{
    ERR_NORMAL_TYPE = 0x00,         //0x00 正常
    //ERR_SELFCHECK_TYPE = 0x01,      //0x01 自检错误
	BMS_STOP_NORMAL = 0x01,			//BMS主动停止
    ERR_CHARGESTOP_TYPE = 0x02,     //0x02 充电机中止错误原因，即CST里的错误
    ERR_CHARGERXTIMEOUT_TYPE = 0x04,//0x04 充电机接收BMS报文超时，即CEM里的错误
    ERR_BMSSTOP_TYPE = 0x08,        //0x08 BMS中止错误原因，即BST里的错误,或BSM里面的错误
    ERR_BMSRXTIMEOUT_TYPE = 0x10,   //0x10 BMS接收充电机报文超时，即BEM里的错误
    ERR_REMOTEMETER_TYPE = 0x20,    //0x20 充电机故障
    ERR_USER_STOP_TYPE = 0x40,      //0x40 用户主动停止
    ERR_REACH_CONDITION_TYPE = 0x80,//0x80 达到充电机条件
}_CONTROL_ERR_TYPE;


//根据协议调整
typedef enum
{	
   STOP_ERR_NONE = 0,
   STOP_TCUNORMAL =1,               // 计费单元正常停止
   STOP_TCUERR  ,                   //计费单元自身故障停止
   STOP_CCUERR,						//计费单元判断充电控制器故障
   STOP_STARTEDTOUT ,               //启动完成状态确认帧超时
   STOP_HANDERR,                    //导引故障
   STOP_HEARTTOUT,                  //心跳超市
   STOP_WAITTOIT ,                  //充电中暂停超时
   STOP_EMERGENCY,                  //急停
   STOP_DOORERR,                    //门禁
   STOP_SURGEARRESTER,              //避雷器

   STOP_SMOKEERR = 11,              //烟感
   STOP_ACSWERR1,                   //交流输入断路器故障
   STOP_ACSWERR2,                   //交流输入接触器误动
   STOP_ACSWERR3,                   //交流输入接触器粘连
   STOP_ACINERR ,                   //交流输入故障
   STOP_CUPTEMPERATURE,             //充电桩过温
   STOP_GUNUPTEMPERATURE,           //充电桩接口过温
   STOP_ELECLOCKERR,                //充电桩电磁锁故障
   STOP_OUTSWERR1,                  //输出接触器误动故障
   STOP_OUTSWERR2,                  //输出接触器粘连
   
   STOP_OUTSWERR3 = 21,              //输出熔断器故障
   STOP_SAMELEVELSW1,               //并联接触器误动故障
   STOP_SAMELEVELSW2,               //并联接触器故障
   STOP_LEAKOUTTIMEOUT,                 //泄放回路
   STOP_BMSPOWERERR ,                   //辅助电源故障
   
   STOP_CHARGEMODULEERR ,               //充电机模块故障
   STOP_OUTVOLTVORE ,                   //输出电压过压
   STOP_OUTVOLTUNDER ,                  //输出欠压
   STOP_OUTCURROVER ,                   //输出电流过流
   STOP_SHORTCIRCUIT,                  //输出短路
   
   STOP_BCLTIMTOUT = 31,						//BCL超时
   STOP_BCSTIMTOUT ,						//BCS超时
   STOP_BSMTIMTOUT ,						//BSM超时
   STOP_BSMBATVOLTHIGH ,					//BSM单体电压过高
   STOP_BSMBATVOLTLOW ,                     //过低
   STOP_BSMSOCHIGH ,                      //BSMSOC过高
   STOP_BSMSOCLOW ,                      //过低
   STOP_BSMCURRUP ,                      //BMS过流
   STOP_BSMTEMPUP ,                      //BMS过温
   STOP_BSMINSOLUTION ,                 //BMS绝缘
   
   STOP_BSMSWERR  = 41,                      //BSM连接器故障
   STOP_BSMNORMAL ,                     //BMS正常停止
   STOP_BSMERR ,                        //BMS异常停止
   STOP_CCUBSMERR ,                     //充电桩判断BMS故障
   STOP_MATERFAIL,						//电表故障
   STOP_OTHERERR ,                      //充电桩其他故障
   
	STOP_BSTINSULATIONERR,		 //绝缘故障
	STOP_BSTSWOVERT,				//输出连接器过温故障	
	STOP_BSTELOVERT,				//BMS元件过温故障
	STOP_CERR,     //输出连接器故障
	STOP_BSTBATOVERT, //电池组过温故障
	STOP_BSTHIGHRLCERR, //高压继电器故障
	STOP_BSTTPTWO,//检测点2电压检测故障
	STOP_BSTOTHERERR,//BST其他故障
	STOP_BALANCE,
	STOP_SOC,
        
}_CONTROL_ERR_BRANCH;

//充电失败原因
//对照TCU协议修改 2020
typedef enum
{
	
	STARTEND_SUCCESS = 0,                   //
	ENDFAIL_HANDERR ,                       // 导引故障
	ENDFAIL_RECVTIMEOUT  ,                  //计费单元通信超时
	ENDFAIL_EMERGENCY,                      //急停按钮
	ENDFAIL_DOORERR ,                       //门禁
	ENDFAIL_SURGEARRESTER,                  //避雷器
	ENDFAIL_SMOKEERR,                       //烟感
	ENDFAIL_ACSWERR ,                       //交流输入断路器故障
	ENDFAIL_ACSWERR1,                        //交流输入接触器故障
	ENDFAIL_ACSWERR2,                       //交流输入接触器粘连
	ENDFAIL_ACINERR,                       //交流输入故障过压欠压过流缺相

	ENDFAIL_CUPTEMPERATURE = 11,                 //充电桩过温
	ENDFAIL_GUNUPTEMPERATURE,               //充电接口过温
	ENDFAIL_ELECLOCKERR,                    //电磁锁故障
	ENDFAIL_INSOLUTIONERR,                  //绝缘检测故障
	ENDFAIL_BATREVERSE,                     //电池反接
	ENDFAIL_OUTSWERR1,                   //输出接触器误动故障
	ENDFAIL_OUTSWERR2,                   //输出接触器粘连
	ENDFAIL_OUTSWERR3,                   //输出熔断器故障
	ENDFAIL_SAMELEVELSW1,                //并联接触器误动故障
	ENDFAIL_SAMELEVELSW2,                   //并联接触器故障


   ENDFAIL_LEAKOUTTIMEOUT = 21,                  //泄放回路
   ENDFAIL_MODULEERR ,                   	//模块故障
   ENDFAIL_CHARGEMODULEERR ,               //充电机模块故障
   ENDFAIL_OUTVOLTVORE ,                   //输出电压过压
   ENDFAIL_OUTVOLTUNDER ,                  //输出欠压
   ENDFAIL_OUTCURROVER ,                   //输出电流过流
   ENDFAIL_SHORTCIRCUIT ,                  //输出短路
   ENDFAIL_BATVOLTERR1 ,                      //电池最高允许充电电压小于充电机最小输出电压
   ENDFAIL_BATVOLTERR2 ,                      //绝缘监测前直流接触器后级大于10V
   ENDFAIL_BATVOLTERR3 ,                      //通信报文与采样电压大于5%
   
   ENDFAIL_ADVOLTERR1 = 31 ,               	//启动前后级电压小于最小输出
   ENDFAIL_ADVOLTERR2 ,                      //启动前后级电压大于最大输出
   ENDFAIL_ADVOLTERR3 ,                      //启动前后级电压大于最BMS允许最高电压
   ENDFAIL_BRMTIMEOUT ,                      //BRM超时
   ENDFAIL_BRMDATAERR ,                      //BRM数据异常
   ENDFAIL_BCPTIMEOUT ,                      //BCP超时
   ENDFAIL_BCPDATAERR ,                      //BCP数据异常
   ENDFAIL_BROWAITTIMEOUT ,                 //BRO00超时
   ENDFAIL_BRORUNTIMEOUT ,                  //BROAA超时
   ENDFAIL_VIN,								//VIN启动失败
   ENDFAIL_OTHERERR ,                      //其他故障
	
}_START_FAIL_TYPE;

//充电模式，分手动、自动
typedef enum
{
    MODE_UNDEF, //未定义模式
    MODE_AUTO,  //自动模式
    MODE_MAN,   //手动模式
	MODE_VIN,	//VIN码模式
}_CHARGE_MODE_TYPE;



//下发开始充电指令，以消息形式传入，pdata中的数据
__packed typedef struct
{
	_CHARGE_MODE_TYPE ChargeMode;	//充电模式
	INT16U ChargeVol;    			//电压   分辨率：0.1V，0偏移量,自动模式为0
	INT16U ChargeCur;	 			//电流	 分辨率0.1A，0偏移量，自动模式为0
}_START_CHARGE_CMD;

//枪硬件连接状态
typedef enum
{
    GUN_CONNECTED,           //0枪已连接
    GUN_DISCONNECTED,        //1枪未连接
}_GUN_STATE;

//电子锁硬件状态
typedef enum
{
    ELEC_LOCK_LOCKED,        //0电子锁锁定
    ELEC_LOCK_UNLOCKED,      //1未锁定
}_ELEC_LOCK_STATE;

//急停按钮状态
typedef enum
{

    EMERGENCY_UNPRESSED,       //0急停按钮未按下
    EMERGENCY_PRESSED,     //1急停按钮按下
}_EMERGENCY_STATE;


//电池反接状态
typedef enum
{
    BAT_UNREVERSE,       //0电池不反接
    BAT_REVERSE,         //1电池反接
}_BAT_STATE;

//直流输出接触器K1\K2状态，此状态无法读取反馈信息，只是软件上判断
typedef enum
{
    POWER_DC_OPEN,          //0直流输出接触器K1\K2断开
    POWER_DC_CLOSE,         //1直流输出接触器K1\K2闭合
}_POWER_DC_STATE;

//枪归位状态
typedef enum
{
    GUN_RETURN,            //0枪已归位
    GUN_UNRETURN,          //枪未归位
}_GUN_RETURN_STATE;

typedef enum
{
    GUN_ID_FAIL = 0,         //CMU ID不重复
    GUN_ID_NORMAL,          //CMU ID不重复
} GUN_ID_STATE;

//遥信类型，此变量仅供BCU任务来获取系统状态
typedef enum
{
    REMOTE_SIGNALE_1,   //0，遥信1，对应BCUYX_BYTE1类型
    REMOTE_SIGNALE_2,   //1，遥信2，对应BCUYX_BYTE2类型
    REMOTE_SIGNALE_3,   //2，遥信3，对应BCUYX_BYTE3类型
    REMOTE_SIGNALE_4,   //3，遥信4，对应BCUYX_BYTE4类型
    REMOTE_SIGNALE_5,   //4，遥信5，对应BCUYX_BYTE5类型
	REMOTE_SIGNALE_6,   //5，遥信6，对应BCUYX_BYTE6类型
	REMOTE_SIGNALE_7,   //6，遥信7，对应BCUYX_BYTE7类型
    REMOTE_SIGNALE_ALL, //7，全部遥信，对应上述总和
}_REMOTE_SIGNALE_TYPE;

//工作状态
typedef enum
{
    WORK_STATE_IDLE,    //0待机状态
    WORK_STATE_RUN,     //1工作状态
    WORK_STATE_OVER,    //2充满状态
    WORK_STATE_WARN,    //3告警状态
    WORK_STATE_ERR,     //4故障状态
}_WORK_STATE;

//充电过程步骤
typedef enum
{
	BMS_CONNECT_DEFAULTVAL = 0,	//初始化
	BMS_CONNECT_HANDSHARE , 	//握手阶段
	BMS_CONNECT_PARACONFIG,	   	//配置阶段
	BMS_CONNECT_RUN,		   	//充电阶段返回成功之前
	BMS_CONNECT_RUNSUCCESS,	   	//充电阶段返回成功之后
	BMS_CONNECT_END,		   	//充电结束阶段
}_BMS_CONNECT_STEP;

//直流模块输入状态
typedef enum
{
    MODULE_RESET_ON,    //上电闭合
    MODULE_UNRESET_ON,  //插枪闭合
}_MODULE_ON_STATE;


//自检故障
__packed typedef struct
{
    INT8U NoErr;             //下面的全部没有问题
    INT8U GunDisconnect;     //枪未连接
    INT8U Insulation;        //绝缘故障
    INT8U ContactorVol;      //接触器电压
    INT8U LeakVol;           //泄放电路电压
    INT8U LowVol;            //功率模块在规定的时间内未达到需求值
    INT8U Unstable;          //功率模块在规定的时间内处于不稳定状态(有时达到需求，有时低于需求)
    INT8U Adhesion;          //粘连故障 
    INT8U Other;             //其他未定义错误
}_SELF_CHECK_STATE;



//BMS状态
__packed typedef struct
{
    INT8U TotalState;
    INT8U BCPFitState;       //BCP参数是否合适
}_BMS_STATE;

//停止原因
__packed typedef struct
{
    //充电机跟BMS硬件故障停止原因
    _BMS_BST_CONTEXT BMSStop;    //BMS停止原因
    _BMS_CST_CONTEXT ChargeStop; //充电机停止原因，跟BMS通讯超时
    //充电机跟BMS通讯超时类错误
    _BMS_BEM_CONTEXT BMSErr;     //BMS错误原因
    _BMS_CEM_CONTEXT ChargeErr;  //充电机错误原因，跟BMS通讯超时
}_STOP_REASON;

//费用计算
__packed typedef struct
{
   CostType CostTotal;          //总费用
   CostType CostJian;           //尖费用
   CostType CostFeng;           //峰费用
   CostType CostPing;           //平费用
   CostType CostGu;             //谷费用
}_COST_VAL;

//电量计算
__packed typedef struct
{
    PowerType PowerTotal;       //总电量
    PowerType PowerJian;        //尖电量
    PowerType PowerFeng;        //峰电量
    PowerType PowerPing;        //平电量
    PowerType PowerGu;          //谷电量
}_POWER_VAL;

//充电信息
__packed typedef struct
{
    INT32U StartTick;           //充电开始心跳
    INT32U CurTick;             //当前心跳
    _BSPRTC_TIME StartTime;     //充电开始时间
    _BSPRTC_TIME EndTime;       //充电结束时间
    _POWER_VAL StartPower;      //开始充电电量
    _POWER_VAL EndPower;        //结束充电电量
    _POWER_VAL CostPower;       //消耗的总电能
    _COST_VAL Cost;             //费用
    _BMS_BSD_CONTEXT BSDContext;//BSD报文内容
}_CHARGE_INFO_C;


//遥信状态
__packed typedef struct
{
    _EMERGENCY_STATE EmergencyState; //急停按钮状态
    _ELEC_LOCK_STATE ElecLockState;  //电子锁连接状态，ELEC_LOCK_UNLOCKED--未锁定 ELEC_LOCK_LOCKED--锁定
    _BAT_STATE BatState;             //电池反接状态
    _GUN_RETURN_STATE GunReturnState;//枪归位状态
    _POWER_DC_STATE PowerDCState;    //直流输出接触器K1\K2状态
}_REMOTE_SIGNAL_STATE;

//遥测状态
__packed typedef struct
{
    _GUN_STATE GunState;             //枪连接状态
}_REMOTE_METER_STATE;

typedef struct
{
    INT8U DivNum;
    INT8U (*Fun)(void *pdata);
}_CONTROL_CMD;

__packed typedef struct
{
    _CONTROL_ERR_TYPE 		TotalErrState; 		//总错误,此状态是一个或状态                
    _SELF_CHECK_STATE 		SelfCheckState;		//自检状态，自检状态
    _BMS_STATE 				BMSState;        	//BMS状态，BCP帧的参数是否适合
    _STOP_REASON 			StopReason;    		//停止原因和错误原因

    _REMOTE_METER_STATE 	RemoteMeterState;   //遥测状态
    _REMOTE_SIGNAL_STATE 	RemoteSingalState;	//遥信状态
     
    _CHARGE_INFO_C 			ChargeInfo;    		//本次充电基本信息  
    
    _WORK_STATE 			WorkState;			//当前工作状态
    _MODULE_ON_STATE 		ModuleOnState;
	
	_START_FAIL_TYPE		StartFailType;		//充电失败类型(启动完成帧发送)
	_CONTROL_ERR_BRANCH		ErrBranch;			//具体停止原因（停止完成帧发送）
	GUN_ID_STATE			GUNIDState;			//CMP id是否重复
}_SYS_STATE;

extern _SYS_STATE SysState[GUN_MAX];

//控制流程
typedef struct
{
    INT8U SelfCheck;            //是否需要自检  TRUE--需要自检   FALSE--不需要自检
    INT8U InsulationCheck;      //是否需要绝缘检测 TRUE--需要绝缘检测 FALSE--不需要绝缘检测
    INT8U BCPFitCheck;          //是否需要BCP参数检测 TRUE---需要检查 FALSE--不需要检查
    INT8U StartCharge;          //开始充电命令 TRUE--开始充电（开始充电在启动中也算，APP_StartCharge中置开始充电）    FALSE---未开始充电
    INT8U StopCharge;           //停止充电命令 TRUE--正在停止充电操作  FALSE---未进行停止充电操作
    INT8U StopOnce;             //停止标志 FALSE---可以执行停止操作 TRUE---已经在执行停止操作（主要在APP_StopCharge   函数中使用了）
    INT8U ChargeReady;          //充电机准备好
    INT8U ChargeRunning;        //功率模块已经输出 TRUE-当前处于充电中  FALSE-当前处于非充电中
    INT8U DemandUpdate;         //需求已改变(目前仅用于手动模式) TRUE--更新了需求 FALSE--未更新
	INT8U PauseCharge;			//暂停充电命令 TRUE--暂停充电    FALSE---未暂停充电
	INT8U DealFaultOver;        //处理故障完成(就是超时重连) TRUE---已经完成  FALSE---未完成
	INT8U BMSStep;				//BMS充电阶段步骤
}_CONTROL_STEP;

//充电控制信息
typedef struct
{
    _CHARGE_MODE_TYPE ChargeMode;  //充电模式:手动或自动
}_CONTROL_INFO;

/*********************************接收数据结构体**********************************/
typedef enum
{
	OPEN_QUICK = 1,   	//快速开机（绝缘检测阶段使用）
	CLOSE_JYCHARGE,	  	//停止充电（充电模块需要保留给当前充电端口使用）绝缘监测停止
	SOFT_START,		  	//软启动（预充阶段使用）
	SHOW_ADDR,		  	//显示地址
	PAGE_ADJUST,	 	//参数修改
	STOP_CHARGE,	 	//停止充电（充电模块退出运行，可给其他充电端口使用）
}_OPERATE_CMD;			//操作指令

__packed typedef struct
{
    INT8U  Operate_Cmd:4;       //_CONTROL_CMD
	INT8U VolOutRange:1;	    // 电压输出范围  0x00 --- 低电压段(200V-500V) 0x01 --- 高电压(500V-750V)
    INT8U  Reverse1:1;          //预留
	INT8U SWState:1;			//充电回路主接接触器状态 0X00 --分断  0x01 闭合
    INT8U  SuccessState:1;  	//成功标识  0x00-失败 0x01成功
}_TCCU_CONTROL_BYTE;

//遥控命令返回
__packed typedef struct
{
	_TCCU_CONTROL_BYTE   Cmd;    //控制字
	INT8U  Res;	                //预留
	INT16U Vol;	                //设置电压0.1V
	INT16U Cur;	                //设置电流0.01A
	INT16U BatteryVol;	    	//电池电压0.1V	
}_ERCV_CONTROCMD;	

#warning "定值设置、定值查询暂时不做，遥信遥测数据未对应上"
__packed typedef union
{
	INT16U AllBits;
	__packed struct
	{
		INT16U  Res:4; 					//预留
		INT16U  HortWorkState:1;		//加热部件工作状态
		INT16U  Res1:1;					//预留
		INT16U  FANWorkState:1;			//风机工作状态   0 待机  1 启动
		INT16U  Res2:7;					//预留
		INT16U  ACInSW3:1; 				//交流输入主接触器状态  0 分断  1闭合
		INT16U  InSWState:1; 		   //进线塑壳断路器状态  0分断 1闭合		
	}TwoByte;
}_STATE_INFO;

__packed typedef union
{
	INT32U AllBits;
	__packed struct
	{
		INT32U Res:18; 		            //预留
		INT32U RYKYCTimeout:1;			//功率空气其接收遥控遥信报文超时故障  0 正常 1故障
		INT32U TempOver:1;				//充电机柜过温故障
		INT32U SWModuleFail:1;         //开关模块故障
		INT32U ModuleFail:1;     //充电模块故障
		INT32U ModuleInDePhaseFail:1;  //充电机模块输入缺相故障
		INT32U ModuleInUVFail:1;       //充电机模块输入欠压故障
		INT32U ModuleInOVFail:1;       //充电机模块输入过压故障
		INT32U PCUWaterFail:1;         //功率单元水浸故障
		INT32U PCUDoorFail:1;          //功率单元开门故障
		INT32U HotFail:1;             //加热部件故障
		INT32U ReHotFail:1;           //散热风机故障
		INT32U SmogFail:1;          	//充电机烟雾故障
		INT32U ACInSWBFail:1; 			//充电机柜交流输入接触器故障 
		INT32U ACInSWBreaker:1;        //充电机柜交流进线塑壳断路器故障
	}FourByte;
}_FAIL_INFO;

typedef enum
{
    STANDBY_STATE = 0x0,    //待机状态
    RUN_STATE,              //运行状态
}_WORK_STATUS;

typedef enum
{
    SERVER_PROHIBT = 0x0,   //服务禁用
    SERVER_ALLOW,           //服务允许
    SERVER_WAIT,            //服务等待
}_SERVER_STATUS;

__packed typedef union
{
    INT8U  data;
    __packed struct
    {
        INT8U  Reverse:4;
        INT8U  Error_Status:1; //0 正常状态  1故障状态
        INT8U  Warn_Status:1;	//0 正常状态 1告警状态
        INT8U  Work_Status:2;	//1 待机状态 2工作状态 3自检状态
    }WORK_INFOR_LIST;
}_WORK_INFOR;


__packed typedef union
{
	INT32U AllBits;
	__packed struct
	{
		INT32U res:12;   				//预留
		INT32U ModuleXFFail:1;     		//充电模块泄放故障（个别充电模块） 0正常 1故障
		INT32U ModuleSignalFail:1;  	//充电模块通信故障（个别充电模块） 0正常 1故障
		INT32U ModuleInDePhaseFail:1;  //充电机模块输入缺相故障（个别充电模块） 0正常 1故障
		INT32U ModuleInUVFail:1;       //充电机模块输入欠压故障（个别充电模块） 0正常 1故障
		INT32U ModuleInOVFail:1;       //充电机模块输入过压故障（个别充电模块） 0正常 1故障
		INT32U ModuleOutUVFail:1;       //充电机模块输出欠压故障（个别充电模块） 0正常 1故障
		INT32U ModuleOutOVFail:1;       //充电机模块输出过压故障（个别充电模块） 0正常 1故障
		INT32U ModuleOutOIFail:1;       //充电机模块输出过流故障（个别充电模块） 0正常 1故障
		INT32U ModuleOutDLFail:1;       //充电机模块输出短路故障（个别充电模块） 0正常 1故障
		INT32U ModuleACInFail:1;		//充电机模块交流输入故障（个别充电模块） 0正常 1故障
		INT32U ModuleOTempFail:1;	   //充电机模块过温故障（个别充电模块） 0正常 1故障
		INT32U ModuleFSFail:1;	   		//充电机模块风扇（个别充电模块） 0正常 1故障
		INT32U OTWarn:1;					//充电柜过温告警
		INT32U SWModuleWarn:1;				//开关模块告警
		INT32U InDePhaseWarn:1;  		//充电机柜输入缺相告警
		INT32U  InUVWarn:1;       		//充电机柜输入欠压告警
		INT32U  InOVWarn:1;       		//充电机柜输入过压告警
		INT16U  HortWorkWarn:1;			//加热部件告警
		INT16U  FSWarn:1;				//扇热风扇告警
		INT16U  BLQWarn:1;				//避雷器告警
		
	}FourByte;
}_WARN_INFO;

//功率模块遥信遥测帧1
__packed typedef struct
{
	_WORK_INFOR     WorkStatus;     //工作状态
	_SERVER_STATUS  ServiceStatus;  //服务状态
	_STATE_INFO     StatusInfo;	    //状态信息
	_FAIL_INFO      FailInfo;	    //故障信息
}_ERCV_INFO1;

//遥测数据帧
__packed typedef struct
{
	INT8U ModuleGrupNum;				//模块总组数    	1组/位
	INT16U ModuleAllPower;				//模块总功率		1kw/位
	INT8U IdleModuleGrupNum;			//空闲模块组数		1组/位
	INT16U IdleModuleAllPower;			//空闲模块总功率	1kw/位
	INT16U Res;
}_ERCV_INFO2;
//功率模块告警数据帧
__packed typedef struct
{
	_WARN_INFO  WarnInfo;  //告警信息
	 INT8U  Reverse[4];
}_ERCV_INFO3;	

//心跳帧
__packed typedef struct
{
	INT8U	ifGunUser;			//充电器是否可以用		0 可用  	1不可用
	INT8U	PDUSingnal;			//PDU通信是否超时		0 未超时 	1超时
	INT8U	OpenCloseState;		//开关机状态 0关机  1开机
	INT8U	DCSWFail;			//输出直流接触器黏连导致对应的枪不可用
	INT8U Resp[4];
}_ERCV_HEART;

//自定义数据返回结构体
//模块信息
__packed typedef struct
{
	INT16U	Vol;		//输出电压(1V)
	INT16U	Cur;		//输出电流(1A)
	INT8U	ModuleT;	//模块温度
	_MODULE_STATE ModuleState; //模块状态
}_ERCV_MODULEINFO;

//模块信息
__packed typedef struct
{
	INT16U AC_Vab;		//交流 Vab 电压(0.1V)  
	INT16U AC_Vbc;		//交流 Vbc 电压(0.1V)  
	INT16U AC_Vca;		//交流 Vca 电压(0.1V)  
	INT8U  Res;
}_RECV_ACVOL;

__packed typedef struct
{
	INT16U Output_GunVol;		//直流 电压(0.1V)  
	INT16U Output_GunCur;		//直流 电压(1V)
	INT8U  IfGunUser;			//此枪是否可用	   1表示不可用 ，0表示可以使用
	INT8U  Res[2];
}_RECV_GUNDC;

typedef struct 
{
	_ERCV_CONTROCMD    RecvConterlCmd; 				//遥控帧
	_ERCV_INFO1	   RecvInfo1;				   //功率模块遥信遥测帧1
	_ERCV_INFO2	   RecvInfo2;				   //遥测数据帧
	_ERCV_INFO3	   RecvInfo3;				   //功率模块告警数据帧
	_ERCV_HEART	   RecvHeart;				   //心跳 
	_ERCV_MODULEINFO RecvModuleInfo[MODULE_MAX_NUM];			//模块信息
	_RECV_ACVOL		RecvACVol;					//交流电压
	_RECV_GUNDC		RecGunDC;				//枪输出电压电流
}_CCU_RECV_DATA;	//CCU接收数据


typedef enum
{
	RECV_NOTTIOMOUT = 0,
	RECV_TIOMOUT,
}_RECV_TIOMOUT_STATE;
//PCU任务接收到其他任务的消息处理结构
typedef struct
{
    INT32U ID;
    INT32U IfTimeout;			//接收是否超时   0标识未超时
    const INT32U TimeOut;   	// 超时时间，0表示无需判断超时   
    INT32U OldTime;           	//上一次时间
    INT8U (*Fun)(INT8U,INT8U*,INT8U);   //数据解析，传入枪号
}CCU_RECV_TABLE;


//PCU任务周期性发送帧给功率模块
typedef struct
{
	const INT32U SendID;          //命令
    const INT32U CycleTime;   	// 执行周期
    INT32U OldTime;           	//上一次时间
	INT8U (*SendFun)(INT32U);    //void
}_CCU_SEND_TABLE;


/*********************************发送数据结构体**********************************/
__packed typedef struct
{
    INT8U  Operate_Cmd:4;       //_CONTROL_CMD
	INT8U VolOutRange:1;	    // 电压输出范围  0x00 --- 低电压段(200V-500V) 0x01 --- 高电压(500V-750V)
    INT8U  Reverse1:1;          //预留
	INT8U SWState:1;			//充电回路主接接触器状态
    INT8U  Reverse2:1;  		//预留
}_CONTROL_BYTE;

//遥控命令
__packed typedef struct
{
	_CONTROL_BYTE   Cmd;    	//控制字
	INT8U Res;					    //预留
	INT16U SetVol;				    //设置电压  0.1V
	INT16U SetCur;				    //设置电流	0.01A
	INT16U BatteryVol;			    //电池电压	0.1V
}_SEND_CONTROLCMD;	


#warning "定值设置、定值查询暂时不做"


//遥信遥测数据帧1
__packed typedef struct
{
	INT8U GunStatus;   //车辆接连状态 0 未连接  1已连接
	INT8U EmergStop;   //急停状态 0未动作  1已动作
	INT8U GateError;   //门禁状态 0 正常 1故障
	INT8U WaterError; //水浸故障  0正常 1故障
	INT8U Res[4];	    //预留4个
}_SEND_INFO1;	


//遥信遥测数据帧2
__packed typedef struct
{
	INT16U DCOutVol;		//直流输出电压  0.1V
	INT16U DCOutCur;		//直流输出电流	0.01A
	INT16U DCOutPower;		//直流输出功率	0.1KW
	INT16U ChargeTime;		//充电时间长度	1min
}_SEND_INFO2;	

//心跳帧
__packed typedef struct
{
	INT8U DCSWFaile;		//直流接触器是否黏连 1表示黏连  0表示未黏连
	INT8U ACSW;        //jiaoliujiechu 2024?5?7?13:34:46
	INT8U Resp[6];
}_SEND_HEART;

//
__packed typedef struct
{
	INT8U cmd;
	INT8U res[7];
}_SEND_USERDEF;

typedef struct 
{
	_SEND_CONTROLCMD SendControlCmd;	//遥控命令
	_SEND_INFO1		 SendInfo1;			//遥信遥测数据帧1
	_SEND_INFO2		 SendInfo2;			//遥信遥测数据帧1
	_SEND_HEART		 SendHeart;			//发送心跳
	_SEND_USERDEF	 SendUserdef;		//发送自定义协议数据		
}_CCU_SEND_DATA;	//CCU发送数据
extern _CONTROL_INFO ControlInfo[GUN_MAX];
extern _CCU_RECV_DATA CCURecvData[GUN_MAX];				//CCU接收数据结构体



__packed typedef	struct
{
	INT16U	OverACVolTherold;	//交流输入过压阀值
	INT16U	UnderACVolTherold;	//交流输入欠压阀值
	INT16U	OverOutVolTherold;	//输出电压过压阀值
	INT16U	OverOutCurTherold;	//输出电流过流阀值
}_THEROLD_VALUE;


__packed typedef	struct
{
	_THEROLD_VALUE	Therold_Value;			//阀值参数
	INT8U			PowerModelNumber;		//模块数量
	INT16U			VolMaxOutput;			//最大输出电压
	INT16U			CurMaxOutput;			//最大输出电流
	INT16U			VolMinOutput;			//最小输出电压
	INT8U			ChargeType;				//充电方式,01-均充；02-轮充；00-按需分配
}_PRESET_VALUE;

extern _PRESET_VALUE PresetValue;











typedef struct
{
    INT8U System_State: 4;         //工作状态 0000:待机，0001：工作；0010：充满 0011：告警 0100：故障
    INT8U Stop_Pluse: 1;           //急停0按下 1未按下
    INT8U PCU_Link: 1;           //与PCU连接装填 0正常 1告警(无)
    INT8U ACCircuitBreaker_Err: 1; //交流断路器故障 0正常 1告警(无)
    INT8U DCContactor_Err: 1;      //直流母线输出接触器故障 0正常 1告警(无)
} BCUYX_BYTE1;

typedef struct
{
    INT8U DCOutFuse_err: 1;        //直流母线输出熔断器故障 0正常 1告警(无)
    INT8U DZLock_Err: 1;           //电子锁故障 0正常 1异常
    INT8U Fan_Warn: 1;             //风扇故障 0正常 1告警(无)
    INT8U SurgeArrester_Err: 1;    //避雷器故障 0正常 1故障
    INT8U JyCheck_Err: 1;          //绝缘监测故障 0正常 1故障
    INT8U BatReverse_Err: 1;       //电池反接故障 0正常 1反接
    INT8U CarGuide_Err: 1;         //充电中车辆引导故障 0正常 1故障(无)
    INT8U CUpTemperature_Err: 1;   //充电桩过温故障0正常 1故障(无)
} BCUYX_BYTE2;

typedef struct
{
    INT8U CGunUpTemperature_Err: 1; //充电枪过温故障(无)
    INT8U ChargGun_State: 1;        //充电枪未归位 0归位 1未归位
    INT8U BMSCommunication_Err: 1;  //BMS通信异常 0正常 1告警
    INT8U AC_OVP: 1;                //输入电压过压 0正常 1告警(无)
    INT8U AC_UVP: 1;                //输入电压欠压 0正常 1告警(无)
    INT8U DCOVP_Warn: 1;            //直流母线输出过压告警 0正常 1告警
    INT8U DCUVP_Warn: 1;            //直流母线输出欠压告警 0正常 1告警
    INT8U UPCUR_Warn: 1;            //直流母线过流告警 0正常 1告警(BST里的电流过大)
} BCUYX_BYTE3;

typedef struct
{
    INT8U Module_Err: 1;               //充电模块故障 0正常 1告警
    INT8U ModuleAC_Warn: 1;            //充电模块交流输入告警 0正常 1告警(无)
    INT8U ModuleACOVP_Warn: 1;         //充电模块交流输入过压告警 0正常 1告警
    INT8U ModuleACUVP_Warn: 1;         //充电模块交流输入欠压告警 0正常 1告警
    INT8U ModuleACPhase_Warn: 1;       //充电模块交流输入缺相告警 0正常 1告警(无)
    INT8U ModuleDCShortCircuit_Err: 1; //充电模块直流输出短路故障 0正常 1告警
    INT8U ModuleDCUPCUR_Warn: 1;       //充电模块直流输出过流告警 0正常 1告警(BST里的电流过大)
    INT8U ModuleDCOVP_Warn: 1;         //充电模块直流输出过压告警 0正常 1告警
} BCUYX_BYTE4;

typedef struct
{
    INT8U ModuleDCUVP_Warn: 1;         //充电模块直流输出欠压告警 0正常 1告警(无)
    INT8U ModuleUPOT_Warn: 1;          //充电模块过温告警 0正常 1告警
    INT8U ModuleCommunication_Warn: 1; //充电模块通信告警 0正常 1告警
    INT8U ModuleFan_Err: 1;            //充电模块风扇故障 0正常 1告警
    INT8U ConnectionCar_State: 1;      //是否连接电池0连接 1未连接
    INT8U ChargGun_State: 1;           //充电枪状态 0连接 1未连接(无)
    INT8U ChargDzLock_State: 1;        //充电接口电子锁状态  0锁定 1未锁定
    INT8U DCcontactor_State: 1;        //直流接触器状态 0分断 1 闭合
} BCUYX_BYTE5;

typedef struct
{
    INT8U Model1_Comm_State: 1;	//电源模块1通讯状态  0：正常通讯；1：通讯失败
    INT8U Model2_Comm_State: 1;	//电源模块2通讯状态  0：正常通讯；1：通讯失败
    INT8U Model3_Comm_State: 1; //电源模块3通讯状态  0：正常通讯；1：通讯失败
    INT8U Model4_Comm_State: 1;	//电源模块4通讯状态  0：正常通讯；1：通讯失败
    INT8U Model5_Comm_State: 1;	//电源模块5通讯状态  0：正常通讯；1：通讯失败
    INT8U Model6_Comm_State: 1;	//电源模块6通讯状态  0：正常通讯；1：通讯失败
    INT8U Model7_Comm_State: 1;	//电源模块7通讯状态  0：正常通讯；1：通讯失败
    INT8U Model8_Comm_State: 1;	//电源模块8通讯状态  0：正常通讯；1：通讯失败
} BCUYX_BYTE6;

typedef struct
{
    INT8U Model1_Err_State: 1;	//电源模块1故障状态  0：模块正常；1：模块故障
    INT8U Model2_Err_State: 1;	//电源模块2故障状态  0：模块正常；1：模块故障
    INT8U Model3_Err_State: 1;	//电源模块3故障状态  0：模块正常；1：模块故障
    INT8U Model4_Err_State: 1;	//电源模块4故障状态  0：模块正常；1：模块故障
    INT8U Model5_Err_State: 1;	//电源模块5故障状态  0：模块正常；1：模块故障
    INT8U Model6_Err_State: 1;	//电源模块6故障状态  0：模块正常；1：模块故障
    INT8U Model7_Err_State: 1;	//电源模块7故障状态  0：模块正常；1：模块故障
    INT8U Model8_Err_State: 1;	//电源模块8故障状态  0：模块正常；1：模块故障
} BCUYX_BYTE7;


typedef union
{
    INT8U byte;
    BCUYX_BYTE1 State;
} BCU_YX_BYTE1;

typedef union
{
    INT8U byte;
    BCUYX_BYTE2 State;
} BCU_YX_BYTE2;

typedef union
{
    INT8U byte;
    BCUYX_BYTE3 State;
} BCU_YX_BYTE3;
typedef union
{
    INT8U byte;
    BCUYX_BYTE4 State;
} BCU_YX_BYTE4;
typedef union
{
    INT8U byte;
    BCUYX_BYTE5 State;
} BCU_YX_BYTE5;
typedef union
{
    INT8U byte;
    BCUYX_BYTE6 State;
} BCU_YX_BYTE6;
typedef union
{
    INT8U byte;
    BCUYX_BYTE7 State;
} BCU_YX_BYTE7;

typedef struct
{
    BCU_YX_BYTE1 	state1; //状态1
    BCU_YX_BYTE2 	state2; //状态2
    BCU_YX_BYTE3 	state3; //状态3
    BCU_YX_BYTE4 	state4; //状态4
    BCU_YX_BYTE5 	state5; //状态5
	BCU_YX_BYTE6 	state6; //状态6
	BCU_YX_BYTE7 	state7; //状态7
//	INT8U 			BCURemoteSignal[7];//此数组实际上就是BCUYX_BYTE1--BCUYX_BYTE7的内容
} BCU_YX_ImFormaTion;

extern BCU_YX_ImFormaTion BCURemoteSignal[GUN_MAX];   //此数组实际上就是BCUYX_BYTE1--BCUYX_BYTE5的内容
/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/
extern OS_EVENT  *Control_PeventA;        //消息队列
extern OS_EVENT  *Control_PeventB;        //消息队列
/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
INT8U APP_GetCMLContext(_BMS_CML_CONTEXT *context);
INT8U APP_GetCSSContext(_GUN_NUM gun,_BMS_CCS_CONTEXT *context);
INT8U APP_GetCSTContext(_GUN_NUM gun,_BMS_CST_CONTEXT *context);
INT8U APP_GetCSDContext(_GUN_NUM gun,_BMS_CSD_CONTEXT *context);
INT8U APP_GetCEMContext(_BMS_CEM_CONTEXT *context);


INT8U APP_ElectronicLockLockedGunA(void * pdata);
INT8U APP_ElectronicLockLockedGunB(void * pdata);
INT8U APP_ElectronicLockUnlockedGunA(void * pdata);
INT8U APP_ElectronicLockUnlockedGunB(void * pdata);
INT8U APP_SelfCheckFailGunA(void * pdata);
INT8U APP_SelfCheckFailGunB(void * pdata);
INT8U APP_StartChargeGunA(void *pdata);
INT8U APP_StartChargeGunB(void *pdata);
INT8U APP_StopChargeGunA(void *pdata);
INT8U APP_StopChargeGunB(void *pdata);
INT8U APP_StopChargeErrAGunA(void *pdata);
INT8U APP_StopChargeErrAGunB(void *pdata);
INT8U APP_StopChargeErrBGunA(void *pdata);
INT8U APP_StopChargeErrBGunB(void *pdata);
INT8U APP_StopChargeErrCGunA(void *pdata);
INT8U APP_StopChargeErrCGunB(void *pdata);
INT8U APP_InsulationCheckGunA(void *pdata);
INT8U APP_InsulationCheckGunB(void *pdata);
INT8U APP_BCPFitCheckGunA(void *pdata);
INT8U APP_BCPFitCheckGunB(void *pdata);
INT8U APP_ChargeReadyCheckGunA(void *pdata);
INT8U APP_ChargeReadyCheckGunB(void *pdata);
INT8U APP_ChargeRunningGunA(void *pdata);
INT8U APP_ChargeRunningGunB(void *pdata);
INT8U APP_ChargePauseGunA(void *pdata);
INT8U APP_ChargePauseGunB(void *pdata);
INT8U APP_ChargeBSDGunA(void *pdata);
INT8U APP_ChargeBSDGunB(void *pdata);
INT8U APP_ChargeByMan(void *pdata);
INT8U SetBcuRemoteSignalState(INT8U gun,INT8U point, INT8U bit, INT8U val);
/*****************************************************************************
* Function      : APP_GetPresetValue
* Description   : 获取预设值
* Input         : None 
* Output        : _PRESET_VALUE 		PresetValue
* Note(s)       : 
* Contributor   : 2016年10月17日  Yxy
*****************************************************************************/
_PRESET_VALUE	APP_GetPresetValue(void);

/*****************************************************************************
* Function      : APP_GetSystemState
* Description   : 获取系统工作状态
* Input         : None 
* Output        : SysState.WorkState
* Note(s)       : 
* Contributor   : 2016年10月26日  Yxy
*****************************************************************************/
//_WORK_STATE     APP_GetSystemState(INT8U gun);
/*****************************************************************************
* Function      : GetGunState
* Description   : 获取枪硬件连接状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月13日  叶喜雨
*****************************************************************************/
_GUN_STATE GetGunState(INT8U gun);
/*****************************************************************************
* Function      : SetGunState
* Description   : 设置枪硬件连接状态
* Input         : _GUN_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月13日  叶喜雨
*****************************************************************************/
void SetGunState(INT8U gun,_GUN_STATE state);
/*****************************************************************************
* Function      : GetStartFailType
* Description   : 获取启动失败类型
* Input         : void  
* Output        : _START_FAIL_TYPE
* Note(s)       : 
* Contributor   : 2016年10月21日  Yxy
*****************************************************************************/
_START_FAIL_TYPE GetStartFailType(INT8U gun);
/*****************************************************************************
* Function      : SetStartFailType
* Description   : 设置启动失败类型
* Input         : _START_FAIL_TYPE FailType 
* Output        : None
* Note(s)       : 
* Contributor   : 2016年10月21日  Yxy
*****************************************************************************/
void SetStartFailType(INT8U gun,_START_FAIL_TYPE FailType);
/*****************************************************************************
* Function      : GetPowerDCState
* Description   : 获取直流输出接触器K1\K2的状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月14日  叶喜雨
*****************************************************************************/
_POWER_DC_STATE GetPowerDCState(INT8U gun);

/*****************************************************************************
* Function      : SetPowerDCState
* Description   : 设置直流输出接触器K1\K2的状态
* Input         : _POWER_DC_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月14日  叶喜雨
*****************************************************************************/
void SetPowerDCState(INT8U gun,_POWER_DC_STATE state);


/*****************************************************************************
* Function      : GetBatState
* Description   : 获取枪归位状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月17日  叶喜雨
*****************************************************************************/
//_GUN_RETURN_STATE GetGunReturnState(INT8U gun);

/*****************************************************************************
* Function      : GetBatState
* Description   : 获取枪归位状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月17日  叶喜雨
*****************************************************************************/
//void SetGunReturnState(INT8U gun,_GUN_RETURN_STATE state);

/*****************************************************************************
* Function      : GetBMSStartCharge
* Description   : 获取是否需要充电
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetBMSStartCharge(INT8U gun);

/*****************************************************************************
* Function      : SetBMSStartCharge
* Description   : 设置是否需要充电
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetBMSStartCharge(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSStopCharge
* Description   : 获取是否需要充电
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetBMSStopCharge(INT8U gun);

/*****************************************************************************
* Function      : SetBMSStopCharge
* Description   : 设置是否需要充电
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetBMSStopCharge(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : SetBMSStopOnce
* Description   : 设置是否正在停止充电
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetBMSStopOnce(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSStopOnce
* Description   : 获取是否需要充电
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetBMSStopOnce(INT8U gun);
/*****************************************************************************
* Function      : GetBMSInsulationCheck
* Description   : 获取是否需要绝缘检测
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetBMSInsulationCheck(INT8U gun);

/*****************************************************************************
* Function      : SetBMSInsulationCheck
* Description   : 设置是否需要绝缘检测
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetBMSInsulationCheck(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSBCPFitCheck
* Description   : 获取是否需要BCP参数检测
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetBMSBCPFitCheck(INT8U gun);

/*****************************************************************************
* Function      : SetBMSBCPFitCheck
* Description   : 设置是否需要BCP参数检测
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetBMSBCPFitCheck(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargeReady
* Description   : 获取充电机是否就绪
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
_CHARGE_READY_STATE GetChargeReady(INT8U gun);

/*****************************************************************************
* Function      : SetChargeReady
* Description   : 设置充电机就绪
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetChargeReady(INT8U gun,_CHARGE_READY_STATE state);

/*****************************************************************************
* Function      : GetBMSSelfCheck
* Description   : 获取BMS是否需要重新自检
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetBMSSelfCheck(INT8U gun);

/*****************************************************************************
* Function      : SetBMSSelfCheck
* Description   : 设置BMS是否需要重新自检
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetBMSSelfCheck(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargeRunningState
* Description   : 获取是否已经输出电能
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetChargeRunningState(INT8U gun);

/*****************************************************************************
* Function      : SetChargeRunningState
* Description   : 设置是否输出电能
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetChargeRunningState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargeDemandUpdateState
* Description   : 获取需求是否更新
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月9日  叶喜雨
*****************************************************************************/
//INT8U GetChargeDemandUpdateState(INT8U gun);

/*****************************************************************************
* Function      : SetChargeDemandUpdateState
* Description   : 设置需求是否要更新
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月9日  叶喜雨
*****************************************************************************/
void SetChargeDemandUpdateState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetDealFaultOverState
* Description   : 获取处理故障状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月29日  叶喜雨
*****************************************************************************/
INT8U GetDealFaultOverState(INT8U gun);

/*****************************************************************************
* Function      : SetDealFaultOverState
* Description   : 设置处理故障状态
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月29日  叶喜雨
*****************************************************************************/
void SetDealFaultOverState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargePauseState
* Description   : 获取暂停输出状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
INT8U GetChargePauseState(INT8U gun);

/*****************************************************************************
* Function      : SetChargePauseState
* Description   : 暂停输出
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月3日  叶喜雨
*****************************************************************************/
void SetChargePauseState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSConnectStep
* Description   : 获取BMS握手步骤
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月29日  叶喜雨
*****************************************************************************/
//INT8U GetBMSConnectStep(INT8U gun);

/*****************************************************************************
* Function      : SetBMSConnectStep
* Description   : 设置BMS握手步骤
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月29日  叶喜雨
*****************************************************************************/
void SetBMSConnectStep(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetElecLockState
* Description   : 获取电子锁硬件状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月13日  叶喜雨
*****************************************************************************/
_ELEC_LOCK_STATE GetElecLockState(INT8U gun);

/*****************************************************************************
* Function      : SetElecLockState
* Description   : 设置电子锁硬件状态
* Input         : _ELEC_LOCK_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年6月13日  叶喜雨
*****************************************************************************/
void SetElecLockState(INT8U gun,_ELEC_LOCK_STATE state);

/*****************************************************************************
* Function      : GetEmergencyState
* Description   : 获取急停按钮状态
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月6日  叶喜雨
*****************************************************************************/
_EMERGENCY_STATE GetEmergencyState(INT8U gun);

/*****************************************************************************
* Function      : SetEmergencyState
* Description   : 设置急停按钮状态
* Input         : _EMERGENCY_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月6日  叶喜雨
*****************************************************************************/
void SetEmergencyState(INT8U gun,_EMERGENCY_STATE state);

/*****************************************************************************
* Function      : GetChargeMode
* Description   : 获取充电模式，分为自动跟手动
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月4日  叶喜雨
*****************************************************************************/
_CHARGE_MODE_TYPE GetChargeMode(INT8U gun);

/*****************************************************************************
* Function      : SetChargeMode
* Description   : 设置充电模式，分手动跟自动
* Input         : _CHARGE_MODE_TYPE mode  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月4日  叶喜雨
*****************************************************************************/
void SetChargeMode(INT8U gun,_CHARGE_MODE_TYPE mode);

/*****************************************************************************
* Function      : APP_Get_ERR_Branch
* Description   : 获取停止原因小项
* Input         : None 
* Output        : SysState.ErrBranch
* Note(s)       : 
* Contributor   : 2017年1月6日  Yxy
*****************************************************************************/
_CONTROL_ERR_BRANCH APP_Get_ERR_Branch(INT8U gun);
/*****************************************************************************
* Function      : APP_Set_ERR_Branch
* Description   : 设置停止原因小项
* Input         : _CONTROL_ERR_BRANCH 
* Output        : None
* Note(s)       : 
* Contributor   : 2017年1月6日  Yxy
*****************************************************************************/
void APP_Set_ERR_Branch(INT8U gun,_CONTROL_ERR_BRANCH Err_Branch_Value);

void ControlParaInit(INT8U gun);


/*****************************************************************************
* Function      : CheckBSTErr
* Description   : 检测BMS返回的BST是否有错
* Input         : INT8U *perr
* Output        : None
* Note(s)       :
* Contributor   : 2016年7月6日  叶喜雨
*****************************************************************************/
INT8U CheckBSTErr(INT8U gun,INT8U *perr);

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

#endif

