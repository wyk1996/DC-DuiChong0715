/*****************************************Copyright(C)******************************************
*******************************************汇誉科技*********************************************
*------------------------------------------文件信息-------------------------------------------
* FileName          : pcu.h
* Author              :
* Date First Issued : 2016-07-09
* Version             : V1.0
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
* History             :no
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
#ifndef __PCU_H_
#define __PCU_H_
/* Includes-----------------------------------------------------------------------------------*/
#include "OS_CPU.H"
#include "sysconfig.h"
#include "gbstandard.h"
//#include "controlmain.h"
/* Private define-----------------------------------------------------------------------------*/
#define CAN_USER_MCP2515    0    //使用MCP2515can    存在 MCP2515 发送发着发着数据会发送不出去
#define CAN_USER_TCU		1	 //与TCU工用一个CAN
#define PCU_USER_CAN		CAN_USER_TCU	




//表长度定义
#define PCU_CMD_TABLE_NUM          (2u)   //PCU任务接收到其他任务的消息处理表长度
#define PCU_CMD_TABLE_RX_NUM       (3u)  //PCU任务接收到串口的消息处理表长度
#define PCU_CMD_TABLE_TX_NUM       (10u)  //PCU任务发送到串口的消息处理表长度

#warning"模块最大个数 2020"
#define MODULE_MAX_NUM             (16u)   //模块最大数量，实际模块个数必须要小于等于此值
#define MODULE_ACTUAL_NUM          (6u)   //实际模块个数

#define PCU_Q_NUM                  (8u)  //PCU消息队列长度
#define PCU_TX_Q_NUM               (16u)  //PCU发送任务消息队列长度

#define PCU_EX_AMP                 (10u)  //输出电压、电流对外放大倍数



#define APP_OPEN_DCMODE      0xAA 
#define APP_CLOSE_DCMODE     0x55 

//模块开关机命令
typedef enum
{
    MODULE_CMD_OPEN,   //0开机命令
    MODULE_CMD_CLOSE,  //1关机命令
}_MODULE_OPEN_CLOSE_CMD;


__packed typedef struct
{
    INT16U Vol;   //功率模块电压，0.1V/位
    INT16U Cur;   //功率模块电流，0.1A/位
}_POWER_INFO;

//模块的告警状态
__packed typedef struct
{
    __packed union
    {
        INT8U allbits;
        __packed struct
        {
            INT8U DC_Off: 1;    //模块DC侧处于关机状态
            INT8U Mod_Alarm: 1; //模块故障告警状态
            INT8U Protected: 1; //模块是否处于保护过压告警状态
            INT8U Fan_Fault: 1; //模块是否处于风扇故障告警状态
            INT8U Tem_Over: 1;  //模块是否处于温度过热告警状态,阀值为100度
            INT8U Volt_Over: 1; //模块是否处于过压告警状态
            INT8U OutShorted: 1;   //输出短路
            INT8U Commu_Aarm: 1;//模块是否处于通信中断告警状态
        } onebits;
    } State1;
    
    __packed union
    {
        INT8U allbits;
        __packed struct
        {
            INT8U Power_Limit: 1;       //模块是否处于功率限制状态
            INT8U ID_Repeat: 1;         //模块ID重复
            INT8U Current_Unbalance: 1; //模块不均流
            INT8U AC_Phaseloss: 1;      //交流缺相告警
            INT8U AC_Unbalance: 1;      //交流不平衡
            INT8U AC_UVP: 1;            //交流欠压
            INT8U AC_Over: 1;           //交流过压
            INT8U PFC_Off: 1;           //模块PFC处于关机状态
        } onebits;
    } State2;
} _MODULE_STATE; 

//英飞源指令一览表
typedef enum 
{
    CMD_SETMODULEWALKIN = 0x13, //设置模块软启动指令
    CMD_SETLEDSTATUS = 0x14,    //设置LED灯状态指令
    CMD_SETMODULEGROUP = 0x16,  //设置模块分组指令
    CMD_SETOPENCLOSE = 0x1a,    //打开关闭指令
    CMD_SETVI = 0x1b,           //设置电流电压指令
      
    
    CMD_RDSYSVIOUT = 0x01,      //读取系统电流电压指令
    CMD_RDSYSMODULENUM = 0x02,  //读取系统模块数量指令
    CMD_RDMODULEVIOUT = 0x03,   //读取模块电流电压指令
    CMD_RDMODULESTATE = 0x04,   //读取模块状态表指令
    CMD_RDACVOUT = 0x06,        //读取模块三相电输入指令
}_MODULE_CMD_TYPE;

//模块总信息
__packed typedef struct
{
    INT32U ModuleVol;            //模块的电流
    INT32U ModuleCur;            //模块的电压
    INT16U ModuleACAVol;         //模块A相输入
    INT16U ModuleACBVol;         //模块B相输入
    INT16U ModuleACCVol;         //模块C相输入
    INT8U  ModuleGrpNum;         //模块的组号
    INT8U  ModuleTemp;           //模块温度
    _MODULE_STATE ModuleInfo;    //模块信息
} _MODULE_TOTAL_INFO;

//接收到UART转CAN数据后的基本信息
//PCU任务接收到其他任务的消息处理结构
typedef struct
{
    INT8U DivNum;
    INT8U (*Fun)(void *pdata);
}_PCU_DEAL_CMD;

typedef struct
{
    INT32U FrameID;
    INT8U (*Fun)(INT8U *pdata);
}_PCU_DEAL_FRAMEID;

//PCU任务周期性发送帧给功率模块
typedef struct
{
	const INT8U Cmd;          //命令
    const INT32U CycleTime;   // 执行周期
    INT32U OldTime;           //上一次时间
	INT8U (*SendFun)(void *pdata);    //id对应的操作
}_PCU_SEND_TABLE;

typedef struct
{
    INT8U GunASure ;   //枪A确认
    INT8U GunBSure ;   //枪B确认
}_APP_CHARGE_STATE0;
extern _APP_CHARGE_STATE0  APP_Charge_State0;

__packed typedef struct
{
    INT8U ModuleMaxTep;    //充电模块最高温度-50°
    INT8U ShutDownNum;     //模块关机模块号，异常为具体的模块号 正常为0
    INT8U ComErrNum ;      //通信异常模块号，异常为具体的模块号 正常为0
    INT8U ModuleErrnum ;   //故障异常模块号，异常为具体的模块号 正常为0
    _MODULE_STATE ModuleState;
    __packed union
    {
        INT8U AllBits;
        __packed struct
        {
            INT8U InACOverVolBit:1;           //模块输入过压异常告警
            INT8U InACUnderVolBit:1;          //模块输入欠压告警
            INT8U RevBits:6;                  //预留，写0
        }OneByte;
    }State3;
}_APP_CHARGE_STATE2;


//模块整体信息，后续需要补全
typedef struct
{
    _POWER_INFO OutputInfo;      				//模块输出信息
    _APP_CHARGE_STATE2 TotalState; 				//模块总状态
}_POWER_MODULE_INFO;
extern _POWER_MODULE_INFO PowerModuleInfo[GUN_MAX]; //模块整体信息

extern _APP_CHARGE_STATE2  APP_Charge_State2;
extern OS_EVENT *TaskPCU_pevent; 
extern OS_EVENT *TaskPCUTx_pevent;

void Set_Group_Param(INT8U groupnum, INT8U cmdtype, INT8U * data);
void Set_Module_Param(INT8U modulenum, INT8U cmdtype, INT8U *data);
void Set_Module_VIout(INT8U modulenum, FP32 vdata, FP32 idata);
void Set_Group_VIout(INT8U groupnum, FP32 vdata, FP32 idata);
void Set_Mod_WalkIn(INT8U modulenum);
void Set_Mod_blink(INT8U modulenum);
void Set_Mod_Group(INT8U modulenum, INT8U groupnum);
void Get_SYS_VIout(void);
void Get_SYS_Num(void);
void Get_Mod_VIout(INT8U modulenum);
void Get_Mod_State(INT8U modulenum);
void Get_AC_ABC(INT8U modulenum);
void OpenCloseOneModule(INT8U number, INT8U co);
void OpenCloseOneGroup(INT8U number, INT8U co);
void OpenCloseAllModule(INT8U co, INT8U num_start, INT8U num);
void PCU_param_init(void);
//void Read_Return_Data(INT8U WaitTime);
void GetAllModuleVI(void);
void PCU_Main_Task(void * pdata);

/*****************************************************************************
* Function      : GetModuleOutPutInfo
* Description   : 获取模块的总输出电压、电流
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016年7月9日  叶喜雨
*****************************************************************************/
INT8U GetModuleOutPutInfo(_POWER_INFO* pdata);
INT8U GetModuleOutPutVolandCur(_BMS_CCS_CONTEXT *pcontext);

/***********************************************************************************************
* Function      : Updata_ModuleInfo
* Description   : 跟新模块数据（跟新  PowerModuleInfo）
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2020年6月5日
***********************************************************************************************/
INT8U Updata_ModuleInfo(INT8U gun);
#endif

