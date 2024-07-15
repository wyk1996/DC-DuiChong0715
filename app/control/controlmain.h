/******************************************************************************

                  ��Ȩ���� (C), 2010-2016, �����Ƽ�����Դ���޹�˾

 ******************************************************************************
  �� �� ��   : ControlMain.h
  �� �� ��   : ����
  ��    ��   : Ҷϲ��
  ��������   : 2016��6��6��
  ����޸�   :
  ��������   : ��������ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��6��6��
    ��    ��   : Ҷϲ��
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __CONTROL_MAIN_H_
#define __CONTROL_MAIN_H_
/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include <string.h>
#include "sysconfig.h"
#include "gbstandard.h"
#include "bsp_RTC.h"
#include "PCUMain.h"
/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define CONTROL_Q_NUM         (50u)
#define CMD_TABLE_NUM         (14u)

#define MAX_DEVIATION         (100u)    //������Ϊ10,�Ŵ���10��
 
 
#define	BHMCMP_VOLT				(300)   //����BHM����ƫ��20V
 
//��v�Ŵ�/��Сm����m��0Ϊ�Ŵ�,0Ϊ��С
#define VAL_ADJUST(v,m,t)     ( (t) ? ( (v) * (m) ) : ( (v) / (m) ) )

//�Լ�ģ�������ѹ����������ʱ����Ƽ��Ĳ���
//�˴�������λС��
#define SIMULATE_DEMAND_CUR   VAL_ADJUST(5, 10, 1)   //5A 
#define SIMULATE_DEMAND_VOL   VAL_ADJUST(350, 10, 1) //4000V VAL_ADJUST(400, 100, 1)
#define SIMULATE_DEMAND_NUM   (0u)     //0��

	//Ӣ��Դ����ģ�����������ĵ�ѹ��������ֵ
	#define MAX_CUR_OUTPUT        1200
	#define MAX_VOL_OUTPUT        7500 //750V

	//��Ļ���õı���ֵ,�Ѿ����˽��棬��ʱ������¡�60KW��ʱ����
	#define LCDMAX_CUR_OUTPUT        (1608u) //13.4A*20*10
	#define LCDMAX_VOL_OUTPUT        (7500u) //750V
	#define MAX_VOL_OUTPUT_DEFAULT   (7500u)
	#define MAX_CUR_OUTPUT_DEFAULT   (1608u)

	#define THEROLD_CUR_OUTPUT        (2600u) //2600
	#define THEROLD_VOL_OUTPUT        (7600u) //760V
	

	//��С�����������ѹ
	#define MIN_CUR_OUTPUT        (0u)     //0A
	#define MIN_VOL_OUTPUT        (1500u)  //150V

//���º��Ǳ���ʵ���е�Ҫ�õ��Ĳ���
#define AC_POWER_UNDER_VAL    	(3300u)   //
#define AC_POWER_OVER_VAL     	(4500u)   //
#define DC_POWER_UNDER_VAL_10 	(10u)    //�����ѹ�������ֱ����ѹֵ��110%ʱ�澯���˴��Ŵ���10��

#define MODULE_WARNING_SHUTDOWN	(0u)   //����ģ��澯�Ƿ�Ҫ�ػ���0:���ػ�  1:�ػ�

#define DEF_OUTPUT_CUR           50		//Ĭ�ϵ���
#define DEF_OUTPUT_VOL           2000	//Ĭ�ϵ�ѹ

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
typedef INT32U CostType;  //���¶����������
typedef INT16U PowerType; //���¶����������


typedef enum
{
    APP_BMS_SHAKE_HAND = 0x10,
    APP_BMS_STATE,
    APP_BMS_CHARGE_STATE,
    APP_BMS_CHARGE_ERR_A,
    APP_BMS_CHARGE_ERR_B,
    APP_BMS_CHARGE_ERR_C,

    APP_ELEC_LOCKED = 0x20,     //��������������
    APP_ELEC_UNLOCKED,          //��������������
    APP_CHARGE_START,           //��ʼ���
    APP_CHARGE_END,             //�������
    APP_CHARGE_FAIL,            //����ʧ��
    APP_CHARGE_SUCCESS,         //�����ɹ�
    APP_CHARGE_RUNNING,         //���ڳ��
    APP_CHARGE_PAUSE,			//��ͣ���
    APP_CHARGE_BSD,             //���ͳ����Ϣ����BMS���ص�BSD����
    APP_INSULATIONG_CHECK,      //��ʼ��Ե���   
    APP_BCP_FIT,                //BMS���ͻ�����BCP�����Ƿ����
    APP_CHARGE_READY,           //��������״̬���
    APP_POWER_OUTPUT,           //�������
    APP_POWER_ENABLE,           //�ù���ģ�������ѹ������
    APP_POWER_DISABLE,          //�ù���ģ��ر������ѹ������
    APP_POWER_GRP_SET,          //�ù���ģ����з���
    APP_POWER_DELAY,            //����ģ����ʱ10s

    BSP_MSGID_READ_PLUS,        //���Ͷ���ĸ������
    BSP_MSGID_READ_MINUS,       //���Ͷ���ĸ������
    BSP_MSGID_READ_PM,          //���Ͷ�����ĸ������
    BSP_MSGID_WAIT,             //ֹͣ����Ե���
	
	APP_BCU_SET,				//TCU��PCU�Ľ�����ֵ����緽ʽ����
}_BMS_DIV_TYPE;

//ֹͣ����
typedef enum
{
    STOP_UNDEF = 0x00,             //δ����
    STOP_USER = 0x01,              //�û�����ֹͣ
    STOP_CONDITION = 0x02,         //�ﵽ����ֹͣ
    STOP_ERR = 0x04,               //���ֹ���ֹͣ
    STOP_WARN = 0x08,              //���ָ澯������ֹͣ���
    STOP_NORMAL = 0x10,            //����ֹͣ�����û�����ֹͣ
	STOP_TCU_HEART = 0x20,			//TCU����ֹͣ
}_STOP_TYPE;

//�澯���
#define WARN_CHECK(w)       ((w)&0x08)
//������
#define ERR_CHECK(e)        ((e)&0x04)
//�û�����ֹͣ���
#define USER_CHECK(u)       ((u)&0x01)

//Ԥ��׼��״̬
typedef enum{
	READY_ING = 0,   //׼����
	READY_SUCCESS,	//׼���ɹ�
	READY_FAIL,		//׼��ʧ��
}_CHARGE_READY_STATE;

typedef enum
{
    STATE_UNDEF,            //û�ж��壬��ʼֵ
    STATE_NO_ERR,           //û�д���
    STATE_ERR,              //�д���
    STATE_ABORT,            //��ֹ
}_CONTROL_STATE_TYPE;

//ϵͳ�ܴ�������
typedef enum
{
    ERR_NORMAL_TYPE = 0x00,         //0x00 ����
    //ERR_SELFCHECK_TYPE = 0x01,      //0x01 �Լ����
	BMS_STOP_NORMAL = 0x01,			//BMS����ֹͣ
    ERR_CHARGESTOP_TYPE = 0x02,     //0x02 ������ֹ����ԭ�򣬼�CST��Ĵ���
    ERR_CHARGERXTIMEOUT_TYPE = 0x04,//0x04 ��������BMS���ĳ�ʱ����CEM��Ĵ���
    ERR_BMSSTOP_TYPE = 0x08,        //0x08 BMS��ֹ����ԭ�򣬼�BST��Ĵ���,��BSM����Ĵ���
    ERR_BMSRXTIMEOUT_TYPE = 0x10,   //0x10 BMS���ճ������ĳ�ʱ����BEM��Ĵ���
    ERR_REMOTEMETER_TYPE = 0x20,    //0x20 ��������
    ERR_USER_STOP_TYPE = 0x40,      //0x40 �û�����ֹͣ
    ERR_REACH_CONDITION_TYPE = 0x80,//0x80 �ﵽ��������
}_CONTROL_ERR_TYPE;


//����Э�����
typedef enum
{	
   STOP_ERR_NONE = 0,
   STOP_TCUNORMAL =1,               // �Ʒѵ�Ԫ����ֹͣ
   STOP_TCUERR  ,                   //�Ʒѵ�Ԫ��������ֹͣ
   STOP_CCUERR,						//�Ʒѵ�Ԫ�жϳ�����������
   STOP_STARTEDTOUT ,               //�������״̬ȷ��֡��ʱ
   STOP_HANDERR,                    //��������
   STOP_HEARTTOUT,                  //��������
   STOP_WAITTOIT ,                  //�������ͣ��ʱ
   STOP_EMERGENCY,                  //��ͣ
   STOP_DOORERR,                    //�Ž�
   STOP_SURGEARRESTER,              //������

   STOP_SMOKEERR = 11,              //�̸�
   STOP_ACSWERR1,                   //���������·������
   STOP_ACSWERR2,                   //��������Ӵ�����
   STOP_ACSWERR3,                   //��������Ӵ���ճ��
   STOP_ACINERR ,                   //�����������
   STOP_CUPTEMPERATURE,             //���׮����
   STOP_GUNUPTEMPERATURE,           //���׮�ӿڹ���
   STOP_ELECLOCKERR,                //���׮���������
   STOP_OUTSWERR1,                  //����Ӵ����󶯹���
   STOP_OUTSWERR2,                  //����Ӵ���ճ��
   
   STOP_OUTSWERR3 = 21,              //����۶�������
   STOP_SAMELEVELSW1,               //�����Ӵ����󶯹���
   STOP_SAMELEVELSW2,               //�����Ӵ�������
   STOP_LEAKOUTTIMEOUT,                 //й�Ż�·
   STOP_BMSPOWERERR ,                   //������Դ����
   
   STOP_CHARGEMODULEERR ,               //����ģ�����
   STOP_OUTVOLTVORE ,                   //�����ѹ��ѹ
   STOP_OUTVOLTUNDER ,                  //���Ƿѹ
   STOP_OUTCURROVER ,                   //�����������
   STOP_SHORTCIRCUIT,                  //�����·
   
   STOP_BCLTIMTOUT = 31,						//BCL��ʱ
   STOP_BCSTIMTOUT ,						//BCS��ʱ
   STOP_BSMTIMTOUT ,						//BSM��ʱ
   STOP_BSMBATVOLTHIGH ,					//BSM�����ѹ����
   STOP_BSMBATVOLTLOW ,                     //����
   STOP_BSMSOCHIGH ,                      //BSMSOC����
   STOP_BSMSOCLOW ,                      //����
   STOP_BSMCURRUP ,                      //BMS����
   STOP_BSMTEMPUP ,                      //BMS����
   STOP_BSMINSOLUTION ,                 //BMS��Ե
   
   STOP_BSMSWERR  = 41,                      //BSM����������
   STOP_BSMNORMAL ,                     //BMS����ֹͣ
   STOP_BSMERR ,                        //BMS�쳣ֹͣ
   STOP_CCUBSMERR ,                     //���׮�ж�BMS����
   STOP_MATERFAIL,						//�������
   STOP_OTHERERR ,                      //���׮��������
   
	STOP_BSTINSULATIONERR,		 //��Ե����
	STOP_BSTSWOVERT,				//������������¹���	
	STOP_BSTELOVERT,				//BMSԪ�����¹���
	STOP_CERR,     //�������������
	STOP_BSTBATOVERT, //�������¹���
	STOP_BSTHIGHRLCERR, //��ѹ�̵�������
	STOP_BSTTPTWO,//����2��ѹ������
	STOP_BSTOTHERERR,//BST��������
	STOP_BALANCE,
	STOP_SOC,
        
}_CONTROL_ERR_BRANCH;

//���ʧ��ԭ��
//����TCUЭ���޸� 2020
typedef enum
{
	
	STARTEND_SUCCESS = 0,                   //
	ENDFAIL_HANDERR ,                       // ��������
	ENDFAIL_RECVTIMEOUT  ,                  //�Ʒѵ�Ԫͨ�ų�ʱ
	ENDFAIL_EMERGENCY,                      //��ͣ��ť
	ENDFAIL_DOORERR ,                       //�Ž�
	ENDFAIL_SURGEARRESTER,                  //������
	ENDFAIL_SMOKEERR,                       //�̸�
	ENDFAIL_ACSWERR ,                       //���������·������
	ENDFAIL_ACSWERR1,                        //��������Ӵ�������
	ENDFAIL_ACSWERR2,                       //��������Ӵ���ճ��
	ENDFAIL_ACINERR,                       //����������Ϲ�ѹǷѹ����ȱ��

	ENDFAIL_CUPTEMPERATURE = 11,                 //���׮����
	ENDFAIL_GUNUPTEMPERATURE,               //���ӿڹ���
	ENDFAIL_ELECLOCKERR,                    //���������
	ENDFAIL_INSOLUTIONERR,                  //��Ե������
	ENDFAIL_BATREVERSE,                     //��ط���
	ENDFAIL_OUTSWERR1,                   //����Ӵ����󶯹���
	ENDFAIL_OUTSWERR2,                   //����Ӵ���ճ��
	ENDFAIL_OUTSWERR3,                   //����۶�������
	ENDFAIL_SAMELEVELSW1,                //�����Ӵ����󶯹���
	ENDFAIL_SAMELEVELSW2,                   //�����Ӵ�������


   ENDFAIL_LEAKOUTTIMEOUT = 21,                  //й�Ż�·
   ENDFAIL_MODULEERR ,                   	//ģ�����
   ENDFAIL_CHARGEMODULEERR ,               //����ģ�����
   ENDFAIL_OUTVOLTVORE ,                   //�����ѹ��ѹ
   ENDFAIL_OUTVOLTUNDER ,                  //���Ƿѹ
   ENDFAIL_OUTCURROVER ,                   //�����������
   ENDFAIL_SHORTCIRCUIT ,                  //�����·
   ENDFAIL_BATVOLTERR1 ,                      //��������������ѹС�ڳ�����С�����ѹ
   ENDFAIL_BATVOLTERR2 ,                      //��Ե���ǰֱ���Ӵ����󼶴���10V
   ENDFAIL_BATVOLTERR3 ,                      //ͨ�ű����������ѹ����5%
   
   ENDFAIL_ADVOLTERR1 = 31 ,               	//����ǰ�󼶵�ѹС����С���
   ENDFAIL_ADVOLTERR2 ,                      //����ǰ�󼶵�ѹ����������
   ENDFAIL_ADVOLTERR3 ,                      //����ǰ�󼶵�ѹ������BMS������ߵ�ѹ
   ENDFAIL_BRMTIMEOUT ,                      //BRM��ʱ
   ENDFAIL_BRMDATAERR ,                      //BRM�����쳣
   ENDFAIL_BCPTIMEOUT ,                      //BCP��ʱ
   ENDFAIL_BCPDATAERR ,                      //BCP�����쳣
   ENDFAIL_BROWAITTIMEOUT ,                 //BRO00��ʱ
   ENDFAIL_BRORUNTIMEOUT ,                  //BROAA��ʱ
   ENDFAIL_VIN,								//VIN����ʧ��
   ENDFAIL_OTHERERR ,                      //��������
	
}_START_FAIL_TYPE;

//���ģʽ�����ֶ����Զ�
typedef enum
{
    MODE_UNDEF, //δ����ģʽ
    MODE_AUTO,  //�Զ�ģʽ
    MODE_MAN,   //�ֶ�ģʽ
	MODE_VIN,	//VIN��ģʽ
}_CHARGE_MODE_TYPE;



//�·���ʼ���ָ�����Ϣ��ʽ���룬pdata�е�����
__packed typedef struct
{
	_CHARGE_MODE_TYPE ChargeMode;	//���ģʽ
	INT16U ChargeVol;    			//��ѹ   �ֱ��ʣ�0.1V��0ƫ����,�Զ�ģʽΪ0
	INT16U ChargeCur;	 			//����	 �ֱ���0.1A��0ƫ�������Զ�ģʽΪ0
}_START_CHARGE_CMD;

//ǹӲ������״̬
typedef enum
{
    GUN_CONNECTED,           //0ǹ������
    GUN_DISCONNECTED,        //1ǹδ����
}_GUN_STATE;

//������Ӳ��״̬
typedef enum
{
    ELEC_LOCK_LOCKED,        //0����������
    ELEC_LOCK_UNLOCKED,      //1δ����
}_ELEC_LOCK_STATE;

//��ͣ��ť״̬
typedef enum
{

    EMERGENCY_UNPRESSED,       //0��ͣ��ťδ����
    EMERGENCY_PRESSED,     //1��ͣ��ť����
}_EMERGENCY_STATE;


//��ط���״̬
typedef enum
{
    BAT_UNREVERSE,       //0��ز�����
    BAT_REVERSE,         //1��ط���
}_BAT_STATE;

//ֱ������Ӵ���K1\K2״̬����״̬�޷���ȡ������Ϣ��ֻ���������ж�
typedef enum
{
    POWER_DC_OPEN,          //0ֱ������Ӵ���K1\K2�Ͽ�
    POWER_DC_CLOSE,         //1ֱ������Ӵ���K1\K2�պ�
}_POWER_DC_STATE;

//ǹ��λ״̬
typedef enum
{
    GUN_RETURN,            //0ǹ�ѹ�λ
    GUN_UNRETURN,          //ǹδ��λ
}_GUN_RETURN_STATE;

typedef enum
{
    GUN_ID_FAIL = 0,         //CMU ID���ظ�
    GUN_ID_NORMAL,          //CMU ID���ظ�
} GUN_ID_STATE;

//ң�����ͣ��˱�������BCU��������ȡϵͳ״̬
typedef enum
{
    REMOTE_SIGNALE_1,   //0��ң��1����ӦBCUYX_BYTE1����
    REMOTE_SIGNALE_2,   //1��ң��2����ӦBCUYX_BYTE2����
    REMOTE_SIGNALE_3,   //2��ң��3����ӦBCUYX_BYTE3����
    REMOTE_SIGNALE_4,   //3��ң��4����ӦBCUYX_BYTE4����
    REMOTE_SIGNALE_5,   //4��ң��5����ӦBCUYX_BYTE5����
	REMOTE_SIGNALE_6,   //5��ң��6����ӦBCUYX_BYTE6����
	REMOTE_SIGNALE_7,   //6��ң��7����ӦBCUYX_BYTE7����
    REMOTE_SIGNALE_ALL, //7��ȫ��ң�ţ���Ӧ�����ܺ�
}_REMOTE_SIGNALE_TYPE;

//����״̬
typedef enum
{
    WORK_STATE_IDLE,    //0����״̬
    WORK_STATE_RUN,     //1����״̬
    WORK_STATE_OVER,    //2����״̬
    WORK_STATE_WARN,    //3�澯״̬
    WORK_STATE_ERR,     //4����״̬
}_WORK_STATE;

//�����̲���
typedef enum
{
	BMS_CONNECT_DEFAULTVAL = 0,	//��ʼ��
	BMS_CONNECT_HANDSHARE , 	//���ֽ׶�
	BMS_CONNECT_PARACONFIG,	   	//���ý׶�
	BMS_CONNECT_RUN,		   	//���׶η��سɹ�֮ǰ
	BMS_CONNECT_RUNSUCCESS,	   	//���׶η��سɹ�֮��
	BMS_CONNECT_END,		   	//�������׶�
}_BMS_CONNECT_STEP;

//ֱ��ģ������״̬
typedef enum
{
    MODULE_RESET_ON,    //�ϵ�պ�
    MODULE_UNRESET_ON,  //��ǹ�պ�
}_MODULE_ON_STATE;


//�Լ����
__packed typedef struct
{
    INT8U NoErr;             //�����ȫ��û������
    INT8U GunDisconnect;     //ǹδ����
    INT8U Insulation;        //��Ե����
    INT8U ContactorVol;      //�Ӵ�����ѹ
    INT8U LeakVol;           //й�ŵ�·��ѹ
    INT8U LowVol;            //����ģ���ڹ涨��ʱ����δ�ﵽ����ֵ
    INT8U Unstable;          //����ģ���ڹ涨��ʱ���ڴ��ڲ��ȶ�״̬(��ʱ�ﵽ������ʱ��������)
    INT8U Adhesion;          //ճ������ 
    INT8U Other;             //����δ�������
}_SELF_CHECK_STATE;



//BMS״̬
__packed typedef struct
{
    INT8U TotalState;
    INT8U BCPFitState;       //BCP�����Ƿ����
}_BMS_STATE;

//ֹͣԭ��
__packed typedef struct
{
    //������BMSӲ������ֹͣԭ��
    _BMS_BST_CONTEXT BMSStop;    //BMSֹͣԭ��
    _BMS_CST_CONTEXT ChargeStop; //����ֹͣԭ�򣬸�BMSͨѶ��ʱ
    //������BMSͨѶ��ʱ�����
    _BMS_BEM_CONTEXT BMSErr;     //BMS����ԭ��
    _BMS_CEM_CONTEXT ChargeErr;  //��������ԭ�򣬸�BMSͨѶ��ʱ
}_STOP_REASON;

//���ü���
__packed typedef struct
{
   CostType CostTotal;          //�ܷ���
   CostType CostJian;           //�����
   CostType CostFeng;           //�����
   CostType CostPing;           //ƽ����
   CostType CostGu;             //�ȷ���
}_COST_VAL;

//��������
__packed typedef struct
{
    PowerType PowerTotal;       //�ܵ���
    PowerType PowerJian;        //�����
    PowerType PowerFeng;        //�����
    PowerType PowerPing;        //ƽ����
    PowerType PowerGu;          //�ȵ���
}_POWER_VAL;

//�����Ϣ
__packed typedef struct
{
    INT32U StartTick;           //��翪ʼ����
    INT32U CurTick;             //��ǰ����
    _BSPRTC_TIME StartTime;     //��翪ʼʱ��
    _BSPRTC_TIME EndTime;       //������ʱ��
    _POWER_VAL StartPower;      //��ʼ������
    _POWER_VAL EndPower;        //����������
    _POWER_VAL CostPower;       //���ĵ��ܵ���
    _COST_VAL Cost;             //����
    _BMS_BSD_CONTEXT BSDContext;//BSD��������
}_CHARGE_INFO_C;


//ң��״̬
__packed typedef struct
{
    _EMERGENCY_STATE EmergencyState; //��ͣ��ť״̬
    _ELEC_LOCK_STATE ElecLockState;  //����������״̬��ELEC_LOCK_UNLOCKED--δ���� ELEC_LOCK_LOCKED--����
    _BAT_STATE BatState;             //��ط���״̬
    _GUN_RETURN_STATE GunReturnState;//ǹ��λ״̬
    _POWER_DC_STATE PowerDCState;    //ֱ������Ӵ���K1\K2״̬
}_REMOTE_SIGNAL_STATE;

//ң��״̬
__packed typedef struct
{
    _GUN_STATE GunState;             //ǹ����״̬
}_REMOTE_METER_STATE;

typedef struct
{
    INT8U DivNum;
    INT8U (*Fun)(void *pdata);
}_CONTROL_CMD;

__packed typedef struct
{
    _CONTROL_ERR_TYPE 		TotalErrState; 		//�ܴ���,��״̬��һ����״̬                
    _SELF_CHECK_STATE 		SelfCheckState;		//�Լ�״̬���Լ�״̬
    _BMS_STATE 				BMSState;        	//BMS״̬��BCP֡�Ĳ����Ƿ��ʺ�
    _STOP_REASON 			StopReason;    		//ֹͣԭ��ʹ���ԭ��

    _REMOTE_METER_STATE 	RemoteMeterState;   //ң��״̬
    _REMOTE_SIGNAL_STATE 	RemoteSingalState;	//ң��״̬
     
    _CHARGE_INFO_C 			ChargeInfo;    		//���γ�������Ϣ  
    
    _WORK_STATE 			WorkState;			//��ǰ����״̬
    _MODULE_ON_STATE 		ModuleOnState;
	
	_START_FAIL_TYPE		StartFailType;		//���ʧ������(�������֡����)
	_CONTROL_ERR_BRANCH		ErrBranch;			//����ֹͣԭ��ֹͣ���֡���ͣ�
	GUN_ID_STATE			GUNIDState;			//CMP id�Ƿ��ظ�
}_SYS_STATE;

extern _SYS_STATE SysState[GUN_MAX];

//��������
typedef struct
{
    INT8U SelfCheck;            //�Ƿ���Ҫ�Լ�  TRUE--��Ҫ�Լ�   FALSE--����Ҫ�Լ�
    INT8U InsulationCheck;      //�Ƿ���Ҫ��Ե��� TRUE--��Ҫ��Ե��� FALSE--����Ҫ��Ե���
    INT8U BCPFitCheck;          //�Ƿ���ҪBCP������� TRUE---��Ҫ��� FALSE--����Ҫ���
    INT8U StartCharge;          //��ʼ������� TRUE--��ʼ��磨��ʼ�����������Ҳ�㣬APP_StartCharge���ÿ�ʼ��磩    FALSE---δ��ʼ���
    INT8U StopCharge;           //ֹͣ������� TRUE--����ֹͣ������  FALSE---δ����ֹͣ������
    INT8U StopOnce;             //ֹͣ��־ FALSE---����ִ��ֹͣ���� TRUE---�Ѿ���ִ��ֹͣ��������Ҫ��APP_StopCharge   ������ʹ���ˣ�
    INT8U ChargeReady;          //����׼����
    INT8U ChargeRunning;        //����ģ���Ѿ���� TRUE-��ǰ���ڳ����  FALSE-��ǰ���ڷǳ����
    INT8U DemandUpdate;         //�����Ѹı�(Ŀǰ�������ֶ�ģʽ) TRUE--���������� FALSE--δ����
	INT8U PauseCharge;			//��ͣ������� TRUE--��ͣ���    FALSE---δ��ͣ���
	INT8U DealFaultOver;        //�����������(���ǳ�ʱ����) TRUE---�Ѿ����  FALSE---δ���
	INT8U BMSStep;				//BMS���׶β���
}_CONTROL_STEP;

//��������Ϣ
typedef struct
{
    _CHARGE_MODE_TYPE ChargeMode;  //���ģʽ:�ֶ����Զ�
}_CONTROL_INFO;

/*********************************�������ݽṹ��**********************************/
typedef enum
{
	OPEN_QUICK = 1,   	//���ٿ�������Ե���׶�ʹ�ã�
	CLOSE_JYCHARGE,	  	//ֹͣ��磨���ģ����Ҫ��������ǰ���˿�ʹ�ã���Ե���ֹͣ
	SOFT_START,		  	//��������Ԥ��׶�ʹ�ã�
	SHOW_ADDR,		  	//��ʾ��ַ
	PAGE_ADJUST,	 	//�����޸�
	STOP_CHARGE,	 	//ֹͣ��磨���ģ���˳����У��ɸ��������˿�ʹ�ã�
}_OPERATE_CMD;			//����ָ��

__packed typedef struct
{
    INT8U  Operate_Cmd:4;       //_CONTROL_CMD
	INT8U VolOutRange:1;	    // ��ѹ�����Χ  0x00 --- �͵�ѹ��(200V-500V) 0x01 --- �ߵ�ѹ(500V-750V)
    INT8U  Reverse1:1;          //Ԥ��
	INT8U SWState:1;			//����·���ӽӴ���״̬ 0X00 --�ֶ�  0x01 �պ�
    INT8U  SuccessState:1;  	//�ɹ���ʶ  0x00-ʧ�� 0x01�ɹ�
}_TCCU_CONTROL_BYTE;

//ң�������
__packed typedef struct
{
	_TCCU_CONTROL_BYTE   Cmd;    //������
	INT8U  Res;	                //Ԥ��
	INT16U Vol;	                //���õ�ѹ0.1V
	INT16U Cur;	                //���õ���0.01A
	INT16U BatteryVol;	    	//��ص�ѹ0.1V	
}_ERCV_CONTROCMD;	

#warning "��ֵ���á���ֵ��ѯ��ʱ������ң��ң������δ��Ӧ��"
__packed typedef union
{
	INT16U AllBits;
	__packed struct
	{
		INT16U  Res:4; 					//Ԥ��
		INT16U  HortWorkState:1;		//���Ȳ�������״̬
		INT16U  Res1:1;					//Ԥ��
		INT16U  FANWorkState:1;			//�������״̬   0 ����  1 ����
		INT16U  Res2:7;					//Ԥ��
		INT16U  ACInSW3:1; 				//�����������Ӵ���״̬  0 �ֶ�  1�պ�
		INT16U  InSWState:1; 		   //�����ܿǶ�·��״̬  0�ֶ� 1�պ�		
	}TwoByte;
}_STATE_INFO;

__packed typedef union
{
	INT32U AllBits;
	__packed struct
	{
		INT32U Res:18; 		            //Ԥ��
		INT32U RYKYCTimeout:1;			//���ʿ��������ң��ң�ű��ĳ�ʱ����  0 ���� 1����
		INT32U TempOver:1;				//��������¹���
		INT32U SWModuleFail:1;         //����ģ�����
		INT32U ModuleFail:1;     //���ģ�����
		INT32U ModuleInDePhaseFail:1;  //����ģ������ȱ�����
		INT32U ModuleInUVFail:1;       //����ģ������Ƿѹ����
		INT32U ModuleInOVFail:1;       //����ģ�������ѹ����
		INT32U PCUWaterFail:1;         //���ʵ�Ԫˮ������
		INT32U PCUDoorFail:1;          //���ʵ�Ԫ���Ź���
		INT32U HotFail:1;             //���Ȳ�������
		INT32U ReHotFail:1;           //ɢ�ȷ������
		INT32U SmogFail:1;          	//������������
		INT32U ACInSWBFail:1; 			//������������Ӵ������� 
		INT32U ACInSWBreaker:1;        //�������������ܿǶ�·������
	}FourByte;
}_FAIL_INFO;

typedef enum
{
    STANDBY_STATE = 0x0,    //����״̬
    RUN_STATE,              //����״̬
}_WORK_STATUS;

typedef enum
{
    SERVER_PROHIBT = 0x0,   //�������
    SERVER_ALLOW,           //��������
    SERVER_WAIT,            //����ȴ�
}_SERVER_STATUS;

__packed typedef union
{
    INT8U  data;
    __packed struct
    {
        INT8U  Reverse:4;
        INT8U  Error_Status:1; //0 ����״̬  1����״̬
        INT8U  Warn_Status:1;	//0 ����״̬ 1�澯״̬
        INT8U  Work_Status:2;	//1 ����״̬ 2����״̬ 3�Լ�״̬
    }WORK_INFOR_LIST;
}_WORK_INFOR;


__packed typedef union
{
	INT32U AllBits;
	__packed struct
	{
		INT32U res:12;   				//Ԥ��
		INT32U ModuleXFFail:1;     		//���ģ��й�Ź��ϣ�������ģ�飩 0���� 1����
		INT32U ModuleSignalFail:1;  	//���ģ��ͨ�Ź��ϣ�������ģ�飩 0���� 1����
		INT32U ModuleInDePhaseFail:1;  //����ģ������ȱ����ϣ�������ģ�飩 0���� 1����
		INT32U ModuleInUVFail:1;       //����ģ������Ƿѹ���ϣ�������ģ�飩 0���� 1����
		INT32U ModuleInOVFail:1;       //����ģ�������ѹ���ϣ�������ģ�飩 0���� 1����
		INT32U ModuleOutUVFail:1;       //����ģ�����Ƿѹ���ϣ�������ģ�飩 0���� 1����
		INT32U ModuleOutOVFail:1;       //����ģ�������ѹ���ϣ�������ģ�飩 0���� 1����
		INT32U ModuleOutOIFail:1;       //����ģ������������ϣ�������ģ�飩 0���� 1����
		INT32U ModuleOutDLFail:1;       //����ģ�������·���ϣ�������ģ�飩 0���� 1����
		INT32U ModuleACInFail:1;		//����ģ�齻��������ϣ�������ģ�飩 0���� 1����
		INT32U ModuleOTempFail:1;	   //����ģ����¹��ϣ�������ģ�飩 0���� 1����
		INT32U ModuleFSFail:1;	   		//����ģ����ȣ�������ģ�飩 0���� 1����
		INT32U OTWarn:1;					//������¸澯
		INT32U SWModuleWarn:1;				//����ģ��澯
		INT32U InDePhaseWarn:1;  		//����������ȱ��澯
		INT32U  InUVWarn:1;       		//����������Ƿѹ�澯
		INT32U  InOVWarn:1;       		//�����������ѹ�澯
		INT16U  HortWorkWarn:1;			//���Ȳ����澯
		INT16U  FSWarn:1;				//���ȷ��ȸ澯
		INT16U  BLQWarn:1;				//�������澯
		
	}FourByte;
}_WARN_INFO;

//����ģ��ң��ң��֡1
__packed typedef struct
{
	_WORK_INFOR     WorkStatus;     //����״̬
	_SERVER_STATUS  ServiceStatus;  //����״̬
	_STATE_INFO     StatusInfo;	    //״̬��Ϣ
	_FAIL_INFO      FailInfo;	    //������Ϣ
}_ERCV_INFO1;

//ң������֡
__packed typedef struct
{
	INT8U ModuleGrupNum;				//ģ��������    	1��/λ
	INT16U ModuleAllPower;				//ģ���ܹ���		1kw/λ
	INT8U IdleModuleGrupNum;			//����ģ������		1��/λ
	INT16U IdleModuleAllPower;			//����ģ���ܹ���	1kw/λ
	INT16U Res;
}_ERCV_INFO2;
//����ģ��澯����֡
__packed typedef struct
{
	_WARN_INFO  WarnInfo;  //�澯��Ϣ
	 INT8U  Reverse[4];
}_ERCV_INFO3;	

//����֡
__packed typedef struct
{
	INT8U	ifGunUser;			//������Ƿ������		0 ����  	1������
	INT8U	PDUSingnal;			//PDUͨ���Ƿ�ʱ		0 δ��ʱ 	1��ʱ
	INT8U	OpenCloseState;		//���ػ�״̬ 0�ػ�  1����
	INT8U	DCSWFail;			//���ֱ���Ӵ���������¶�Ӧ��ǹ������
	INT8U Resp[4];
}_ERCV_HEART;

//�Զ������ݷ��ؽṹ��
//ģ����Ϣ
__packed typedef struct
{
	INT16U	Vol;		//�����ѹ(1V)
	INT16U	Cur;		//�������(1A)
	INT8U	ModuleT;	//ģ���¶�
	_MODULE_STATE ModuleState; //ģ��״̬
}_ERCV_MODULEINFO;

//ģ����Ϣ
__packed typedef struct
{
	INT16U AC_Vab;		//���� Vab ��ѹ(0.1V)  
	INT16U AC_Vbc;		//���� Vbc ��ѹ(0.1V)  
	INT16U AC_Vca;		//���� Vca ��ѹ(0.1V)  
	INT8U  Res;
}_RECV_ACVOL;

__packed typedef struct
{
	INT16U Output_GunVol;		//ֱ�� ��ѹ(0.1V)  
	INT16U Output_GunCur;		//ֱ�� ��ѹ(1V)
	INT8U  IfGunUser;			//��ǹ�Ƿ����	   1��ʾ������ ��0��ʾ����ʹ��
	INT8U  Res[2];
}_RECV_GUNDC;

typedef struct 
{
	_ERCV_CONTROCMD    RecvConterlCmd; 				//ң��֡
	_ERCV_INFO1	   RecvInfo1;				   //����ģ��ң��ң��֡1
	_ERCV_INFO2	   RecvInfo2;				   //ң������֡
	_ERCV_INFO3	   RecvInfo3;				   //����ģ��澯����֡
	_ERCV_HEART	   RecvHeart;				   //���� 
	_ERCV_MODULEINFO RecvModuleInfo[MODULE_MAX_NUM];			//ģ����Ϣ
	_RECV_ACVOL		RecvACVol;					//������ѹ
	_RECV_GUNDC		RecGunDC;				//ǹ�����ѹ����
}_CCU_RECV_DATA;	//CCU��������


typedef enum
{
	RECV_NOTTIOMOUT = 0,
	RECV_TIOMOUT,
}_RECV_TIOMOUT_STATE;
//PCU������յ������������Ϣ�����ṹ
typedef struct
{
    INT32U ID;
    INT32U IfTimeout;			//�����Ƿ�ʱ   0��ʶδ��ʱ
    const INT32U TimeOut;   	// ��ʱʱ�䣬0��ʾ�����жϳ�ʱ   
    INT32U OldTime;           	//��һ��ʱ��
    INT8U (*Fun)(INT8U,INT8U*,INT8U);   //���ݽ���������ǹ��
}CCU_RECV_TABLE;


//PCU���������Է���֡������ģ��
typedef struct
{
	const INT32U SendID;          //����
    const INT32U CycleTime;   	// ִ������
    INT32U OldTime;           	//��һ��ʱ��
	INT8U (*SendFun)(INT32U);    //void
}_CCU_SEND_TABLE;


/*********************************�������ݽṹ��**********************************/
__packed typedef struct
{
    INT8U  Operate_Cmd:4;       //_CONTROL_CMD
	INT8U VolOutRange:1;	    // ��ѹ�����Χ  0x00 --- �͵�ѹ��(200V-500V) 0x01 --- �ߵ�ѹ(500V-750V)
    INT8U  Reverse1:1;          //Ԥ��
	INT8U SWState:1;			//����·���ӽӴ���״̬
    INT8U  Reverse2:1;  		//Ԥ��
}_CONTROL_BYTE;

//ң������
__packed typedef struct
{
	_CONTROL_BYTE   Cmd;    	//������
	INT8U Res;					    //Ԥ��
	INT16U SetVol;				    //���õ�ѹ  0.1V
	INT16U SetCur;				    //���õ���	0.01A
	INT16U BatteryVol;			    //��ص�ѹ	0.1V
}_SEND_CONTROLCMD;	


#warning "��ֵ���á���ֵ��ѯ��ʱ����"


//ң��ң������֡1
__packed typedef struct
{
	INT8U GunStatus;   //��������״̬ 0 δ����  1������
	INT8U EmergStop;   //��ͣ״̬ 0δ����  1�Ѷ���
	INT8U GateError;   //�Ž�״̬ 0 ���� 1����
	INT8U WaterError; //ˮ������  0���� 1����
	INT8U Res[4];	    //Ԥ��4��
}_SEND_INFO1;	


//ң��ң������֡2
__packed typedef struct
{
	INT16U DCOutVol;		//ֱ�������ѹ  0.1V
	INT16U DCOutCur;		//ֱ���������	0.01A
	INT16U DCOutPower;		//ֱ���������	0.1KW
	INT16U ChargeTime;		//���ʱ�䳤��	1min
}_SEND_INFO2;	

//����֡
__packed typedef struct
{
	INT8U DCSWFaile;		//ֱ���Ӵ����Ƿ���� 1��ʾ���  0��ʾδ���
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
	_SEND_CONTROLCMD SendControlCmd;	//ң������
	_SEND_INFO1		 SendInfo1;			//ң��ң������֡1
	_SEND_INFO2		 SendInfo2;			//ң��ң������֡1
	_SEND_HEART		 SendHeart;			//��������
	_SEND_USERDEF	 SendUserdef;		//�����Զ���Э������		
}_CCU_SEND_DATA;	//CCU��������
extern _CONTROL_INFO ControlInfo[GUN_MAX];
extern _CCU_RECV_DATA CCURecvData[GUN_MAX];				//CCU�������ݽṹ��



__packed typedef	struct
{
	INT16U	OverACVolTherold;	//���������ѹ��ֵ
	INT16U	UnderACVolTherold;	//��������Ƿѹ��ֵ
	INT16U	OverOutVolTherold;	//�����ѹ��ѹ��ֵ
	INT16U	OverOutCurTherold;	//�������������ֵ
}_THEROLD_VALUE;


__packed typedef	struct
{
	_THEROLD_VALUE	Therold_Value;			//��ֵ����
	INT8U			PowerModelNumber;		//ģ������
	INT16U			VolMaxOutput;			//��������ѹ
	INT16U			CurMaxOutput;			//����������
	INT16U			VolMinOutput;			//��С�����ѹ
	INT8U			ChargeType;				//��緽ʽ,01-���䣻02-�ֳ䣻00-�������
}_PRESET_VALUE;

extern _PRESET_VALUE PresetValue;











typedef struct
{
    INT8U System_State: 4;         //����״̬ 0000:������0001��������0010������ 0011���澯 0100������
    INT8U Stop_Pluse: 1;           //��ͣ0���� 1δ����
    INT8U PCU_Link: 1;           //��PCU����װ�� 0���� 1�澯(��)
    INT8U ACCircuitBreaker_Err: 1; //������·������ 0���� 1�澯(��)
    INT8U DCContactor_Err: 1;      //ֱ��ĸ������Ӵ������� 0���� 1�澯(��)
} BCUYX_BYTE1;

typedef struct
{
    INT8U DCOutFuse_err: 1;        //ֱ��ĸ������۶������� 0���� 1�澯(��)
    INT8U DZLock_Err: 1;           //���������� 0���� 1�쳣
    INT8U Fan_Warn: 1;             //���ȹ��� 0���� 1�澯(��)
    INT8U SurgeArrester_Err: 1;    //���������� 0���� 1����
    INT8U JyCheck_Err: 1;          //��Ե������ 0���� 1����
    INT8U BatReverse_Err: 1;       //��ط��ӹ��� 0���� 1����
    INT8U CarGuide_Err: 1;         //����г����������� 0���� 1����(��)
    INT8U CUpTemperature_Err: 1;   //���׮���¹���0���� 1����(��)
} BCUYX_BYTE2;

typedef struct
{
    INT8U CGunUpTemperature_Err: 1; //���ǹ���¹���(��)
    INT8U ChargGun_State: 1;        //���ǹδ��λ 0��λ 1δ��λ
    INT8U BMSCommunication_Err: 1;  //BMSͨ���쳣 0���� 1�澯
    INT8U AC_OVP: 1;                //�����ѹ��ѹ 0���� 1�澯(��)
    INT8U AC_UVP: 1;                //�����ѹǷѹ 0���� 1�澯(��)
    INT8U DCOVP_Warn: 1;            //ֱ��ĸ�������ѹ�澯 0���� 1�澯
    INT8U DCUVP_Warn: 1;            //ֱ��ĸ�����Ƿѹ�澯 0���� 1�澯
    INT8U UPCUR_Warn: 1;            //ֱ��ĸ�߹����澯 0���� 1�澯(BST��ĵ�������)
} BCUYX_BYTE3;

typedef struct
{
    INT8U Module_Err: 1;               //���ģ����� 0���� 1�澯
    INT8U ModuleAC_Warn: 1;            //���ģ�齻������澯 0���� 1�澯(��)
    INT8U ModuleACOVP_Warn: 1;         //���ģ�齻�������ѹ�澯 0���� 1�澯
    INT8U ModuleACUVP_Warn: 1;         //���ģ�齻������Ƿѹ�澯 0���� 1�澯
    INT8U ModuleACPhase_Warn: 1;       //���ģ�齻������ȱ��澯 0���� 1�澯(��)
    INT8U ModuleDCShortCircuit_Err: 1; //���ģ��ֱ�������·���� 0���� 1�澯
    INT8U ModuleDCUPCUR_Warn: 1;       //���ģ��ֱ����������澯 0���� 1�澯(BST��ĵ�������)
    INT8U ModuleDCOVP_Warn: 1;         //���ģ��ֱ�������ѹ�澯 0���� 1�澯
} BCUYX_BYTE4;

typedef struct
{
    INT8U ModuleDCUVP_Warn: 1;         //���ģ��ֱ�����Ƿѹ�澯 0���� 1�澯(��)
    INT8U ModuleUPOT_Warn: 1;          //���ģ����¸澯 0���� 1�澯
    INT8U ModuleCommunication_Warn: 1; //���ģ��ͨ�Ÿ澯 0���� 1�澯
    INT8U ModuleFan_Err: 1;            //���ģ����ȹ��� 0���� 1�澯
    INT8U ConnectionCar_State: 1;      //�Ƿ����ӵ��0���� 1δ����
    INT8U ChargGun_State: 1;           //���ǹ״̬ 0���� 1δ����(��)
    INT8U ChargDzLock_State: 1;        //���ӿڵ�����״̬  0���� 1δ����
    INT8U DCcontactor_State: 1;        //ֱ���Ӵ���״̬ 0�ֶ� 1 �պ�
} BCUYX_BYTE5;

typedef struct
{
    INT8U Model1_Comm_State: 1;	//��Դģ��1ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model2_Comm_State: 1;	//��Դģ��2ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model3_Comm_State: 1; //��Դģ��3ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model4_Comm_State: 1;	//��Դģ��4ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model5_Comm_State: 1;	//��Դģ��5ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model6_Comm_State: 1;	//��Դģ��6ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model7_Comm_State: 1;	//��Դģ��7ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
    INT8U Model8_Comm_State: 1;	//��Դģ��8ͨѶ״̬  0������ͨѶ��1��ͨѶʧ��
} BCUYX_BYTE6;

typedef struct
{
    INT8U Model1_Err_State: 1;	//��Դģ��1����״̬  0��ģ��������1��ģ�����
    INT8U Model2_Err_State: 1;	//��Դģ��2����״̬  0��ģ��������1��ģ�����
    INT8U Model3_Err_State: 1;	//��Դģ��3����״̬  0��ģ��������1��ģ�����
    INT8U Model4_Err_State: 1;	//��Դģ��4����״̬  0��ģ��������1��ģ�����
    INT8U Model5_Err_State: 1;	//��Դģ��5����״̬  0��ģ��������1��ģ�����
    INT8U Model6_Err_State: 1;	//��Դģ��6����״̬  0��ģ��������1��ģ�����
    INT8U Model7_Err_State: 1;	//��Դģ��7����״̬  0��ģ��������1��ģ�����
    INT8U Model8_Err_State: 1;	//��Դģ��8����״̬  0��ģ��������1��ģ�����
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
    BCU_YX_BYTE1 	state1; //״̬1
    BCU_YX_BYTE2 	state2; //״̬2
    BCU_YX_BYTE3 	state3; //״̬3
    BCU_YX_BYTE4 	state4; //״̬4
    BCU_YX_BYTE5 	state5; //״̬5
	BCU_YX_BYTE6 	state6; //״̬6
	BCU_YX_BYTE7 	state7; //״̬7
//	INT8U 			BCURemoteSignal[7];//������ʵ���Ͼ���BCUYX_BYTE1--BCUYX_BYTE7������
} BCU_YX_ImFormaTion;

extern BCU_YX_ImFormaTion BCURemoteSignal[GUN_MAX];   //������ʵ���Ͼ���BCUYX_BYTE1--BCUYX_BYTE5������
/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/
/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/
extern OS_EVENT  *Control_PeventA;        //��Ϣ����
extern OS_EVENT  *Control_PeventB;        //��Ϣ����
/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
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
* Description   : ��ȡԤ��ֵ
* Input         : None 
* Output        : _PRESET_VALUE 		PresetValue
* Note(s)       : 
* Contributor   : 2016��10��17��  Yxy
*****************************************************************************/
_PRESET_VALUE	APP_GetPresetValue(void);

/*****************************************************************************
* Function      : APP_GetSystemState
* Description   : ��ȡϵͳ����״̬
* Input         : None 
* Output        : SysState.WorkState
* Note(s)       : 
* Contributor   : 2016��10��26��  Yxy
*****************************************************************************/
//_WORK_STATE     APP_GetSystemState(INT8U gun);
/*****************************************************************************
* Function      : GetGunState
* Description   : ��ȡǹӲ������״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
_GUN_STATE GetGunState(INT8U gun);
/*****************************************************************************
* Function      : SetGunState
* Description   : ����ǹӲ������״̬
* Input         : _GUN_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
void SetGunState(INT8U gun,_GUN_STATE state);
/*****************************************************************************
* Function      : GetStartFailType
* Description   : ��ȡ����ʧ������
* Input         : void  
* Output        : _START_FAIL_TYPE
* Note(s)       : 
* Contributor   : 2016��10��21��  Yxy
*****************************************************************************/
_START_FAIL_TYPE GetStartFailType(INT8U gun);
/*****************************************************************************
* Function      : SetStartFailType
* Description   : ��������ʧ������
* Input         : _START_FAIL_TYPE FailType 
* Output        : None
* Note(s)       : 
* Contributor   : 2016��10��21��  Yxy
*****************************************************************************/
void SetStartFailType(INT8U gun,_START_FAIL_TYPE FailType);
/*****************************************************************************
* Function      : GetPowerDCState
* Description   : ��ȡֱ������Ӵ���K1\K2��״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��14��  Ҷϲ��
*****************************************************************************/
_POWER_DC_STATE GetPowerDCState(INT8U gun);

/*****************************************************************************
* Function      : SetPowerDCState
* Description   : ����ֱ������Ӵ���K1\K2��״̬
* Input         : _POWER_DC_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��14��  Ҷϲ��
*****************************************************************************/
void SetPowerDCState(INT8U gun,_POWER_DC_STATE state);


/*****************************************************************************
* Function      : GetBatState
* Description   : ��ȡǹ��λ״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��17��  Ҷϲ��
*****************************************************************************/
//_GUN_RETURN_STATE GetGunReturnState(INT8U gun);

/*****************************************************************************
* Function      : GetBatState
* Description   : ��ȡǹ��λ״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��17��  Ҷϲ��
*****************************************************************************/
//void SetGunReturnState(INT8U gun,_GUN_RETURN_STATE state);

/*****************************************************************************
* Function      : GetBMSStartCharge
* Description   : ��ȡ�Ƿ���Ҫ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSStartCharge(INT8U gun);

/*****************************************************************************
* Function      : SetBMSStartCharge
* Description   : �����Ƿ���Ҫ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSStartCharge(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSStopCharge
* Description   : ��ȡ�Ƿ���Ҫ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSStopCharge(INT8U gun);

/*****************************************************************************
* Function      : SetBMSStopCharge
* Description   : �����Ƿ���Ҫ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSStopCharge(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : SetBMSStopOnce
* Description   : �����Ƿ�����ֹͣ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSStopOnce(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSStopOnce
* Description   : ��ȡ�Ƿ���Ҫ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSStopOnce(INT8U gun);
/*****************************************************************************
* Function      : GetBMSInsulationCheck
* Description   : ��ȡ�Ƿ���Ҫ��Ե���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSInsulationCheck(INT8U gun);

/*****************************************************************************
* Function      : SetBMSInsulationCheck
* Description   : �����Ƿ���Ҫ��Ե���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSInsulationCheck(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSBCPFitCheck
* Description   : ��ȡ�Ƿ���ҪBCP�������
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSBCPFitCheck(INT8U gun);

/*****************************************************************************
* Function      : SetBMSBCPFitCheck
* Description   : �����Ƿ���ҪBCP�������
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSBCPFitCheck(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargeReady
* Description   : ��ȡ�����Ƿ����
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
_CHARGE_READY_STATE GetChargeReady(INT8U gun);

/*****************************************************************************
* Function      : SetChargeReady
* Description   : ���ó�������
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetChargeReady(INT8U gun,_CHARGE_READY_STATE state);

/*****************************************************************************
* Function      : GetBMSSelfCheck
* Description   : ��ȡBMS�Ƿ���Ҫ�����Լ�
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSSelfCheck(INT8U gun);

/*****************************************************************************
* Function      : SetBMSSelfCheck
* Description   : ����BMS�Ƿ���Ҫ�����Լ�
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSSelfCheck(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargeRunningState
* Description   : ��ȡ�Ƿ��Ѿ��������
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetChargeRunningState(INT8U gun);

/*****************************************************************************
* Function      : SetChargeRunningState
* Description   : �����Ƿ��������
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetChargeRunningState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargeDemandUpdateState
* Description   : ��ȡ�����Ƿ����
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��9��  Ҷϲ��
*****************************************************************************/
//INT8U GetChargeDemandUpdateState(INT8U gun);

/*****************************************************************************
* Function      : SetChargeDemandUpdateState
* Description   : ���������Ƿ�Ҫ����
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��9��  Ҷϲ��
*****************************************************************************/
void SetChargeDemandUpdateState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetDealFaultOverState
* Description   : ��ȡ��������״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
INT8U GetDealFaultOverState(INT8U gun);

/*****************************************************************************
* Function      : SetDealFaultOverState
* Description   : ���ô�������״̬
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
void SetDealFaultOverState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetChargePauseState
* Description   : ��ȡ��ͣ���״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetChargePauseState(INT8U gun);

/*****************************************************************************
* Function      : SetChargePauseState
* Description   : ��ͣ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetChargePauseState(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetBMSConnectStep
* Description   : ��ȡBMS���ֲ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
//INT8U GetBMSConnectStep(INT8U gun);

/*****************************************************************************
* Function      : SetBMSConnectStep
* Description   : ����BMS���ֲ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
void SetBMSConnectStep(INT8U gun,INT8U state);

/*****************************************************************************
* Function      : GetElecLockState
* Description   : ��ȡ������Ӳ��״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
_ELEC_LOCK_STATE GetElecLockState(INT8U gun);

/*****************************************************************************
* Function      : SetElecLockState
* Description   : ���õ�����Ӳ��״̬
* Input         : _ELEC_LOCK_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
void SetElecLockState(INT8U gun,_ELEC_LOCK_STATE state);

/*****************************************************************************
* Function      : GetEmergencyState
* Description   : ��ȡ��ͣ��ť״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
_EMERGENCY_STATE GetEmergencyState(INT8U gun);

/*****************************************************************************
* Function      : SetEmergencyState
* Description   : ���ü�ͣ��ť״̬
* Input         : _EMERGENCY_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
void SetEmergencyState(INT8U gun,_EMERGENCY_STATE state);

/*****************************************************************************
* Function      : GetChargeMode
* Description   : ��ȡ���ģʽ����Ϊ�Զ����ֶ�
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��4��  Ҷϲ��
*****************************************************************************/
_CHARGE_MODE_TYPE GetChargeMode(INT8U gun);

/*****************************************************************************
* Function      : SetChargeMode
* Description   : ���ó��ģʽ�����ֶ����Զ�
* Input         : _CHARGE_MODE_TYPE mode  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��4��  Ҷϲ��
*****************************************************************************/
void SetChargeMode(INT8U gun,_CHARGE_MODE_TYPE mode);

/*****************************************************************************
* Function      : APP_Get_ERR_Branch
* Description   : ��ȡֹͣԭ��С��
* Input         : None 
* Output        : SysState.ErrBranch
* Note(s)       : 
* Contributor   : 2017��1��6��  Yxy
*****************************************************************************/
_CONTROL_ERR_BRANCH APP_Get_ERR_Branch(INT8U gun);
/*****************************************************************************
* Function      : APP_Set_ERR_Branch
* Description   : ����ֹͣԭ��С��
* Input         : _CONTROL_ERR_BRANCH 
* Output        : None
* Note(s)       : 
* Contributor   : 2017��1��6��  Yxy
*****************************************************************************/
void APP_Set_ERR_Branch(INT8U gun,_CONTROL_ERR_BRANCH Err_Branch_Value);

void ControlParaInit(INT8U gun);


/*****************************************************************************
* Function      : CheckBSTErr
* Description   : ���BMS���ص�BST�Ƿ��д�
* Input         : INT8U *perr
* Output        : None
* Note(s)       :
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
INT8U CheckBSTErr(INT8U gun,INT8U *perr);

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

#endif
