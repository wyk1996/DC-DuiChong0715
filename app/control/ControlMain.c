/*****************************************Copyright(C)******************************************
*******************************************�����Ƽ�*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName			: controlmain.c
* Author			: Ҷϲ��     
* Date First Issued	:  
* Version			: V0.1
* Description		: ���Դ���ģ���ͨѶ����ʵ�ֶԵ�Դģ��Ŀ��ƹ���
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History			:
* //    		    : V
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#include <string.h>
#include "ucos_ii.h"
#include "sysconfig.h"
#include "bsp.h"
#include "gbstandard.h"
#include "controlmain.h"
#include "connectbms.h"
#include "bmstimeout.h"
#include "DisplayMain.h"
#include "PCUMain.h"
#include  "ADSamplemain.h"
#include "charging_Interface.h"
#include "controlapp.h"
#include "PCUMain.h"
/* Private define-----------------------------------------------------------------------------*/
/* Private typedef----------------------------------------------------------------------------*/
//�澯��״̬
typedef enum
{
    ERR_LEDECLOSED = 0,  	//��
    ERR_LEDOPENED = 1,      //��
} ERR_LED_STATE;
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
OS_EVENT  *Control_PeventA;        //��Ϣ����
OS_EVENT  *Control_PeventB;        //��Ϣ����
void *ControlOSQA[CONTROL_Q_NUM];  //����������Ϣ��������
void *ControlOSQB[CONTROL_Q_NUM];  //����������Ϣ��������
_SYS_STATE 			SysState[GUN_MAX];			//ϵͳ״̬
_CONTROL_STEP 		ControlStep[GUN_MAX];
_CCU_DEMAND_INFO 		ManDemandInfo[GUN_MAX];  	//�û��ֶ���������
_CCU_DEMAND_INFO 		BMSDemandInfo[GUN_MAX]; 	//����PCU�����������Ϣ
_CONTROL_INFO 		ControlInfo[GUN_MAX];  	 	//��������Ϣ
_PRESET_VALUE 		PresetValue;		//Ԥ��ֵ
BCU_YX_ImFormaTion BCURemoteSignal[GUN_MAX];   //������ʵ���Ͼ���BCUYX_BYTE1--BCUYX_BYTE5������
extern _ADC_JUST_INFO ADCJustInfo[GUN_MAX];
_POWER_MODULE_INFO 	PowerModuleInfo[GUN_MAX] = {0}; 	//ģ��������Ϣ
extern _CCU_SEND_DATA CCUSendData[GUN_MAX];				//CCU�������ݽṹ��
static const _CONTROL_CMD ControlCmdTableA[CMD_TABLE_NUM] =
{
    //����BMS����
    {APP_ELEC_LOCKED       ,  APP_ElectronicLockLockedGunA      },  //����������

    {APP_ELEC_UNLOCKED     ,  APP_ElectronicLockUnlockedGunA    },  //����������

    {APP_INSULATIONG_CHECK ,  APP_InsulationCheckGunA           },  //��Ե�������

    {APP_BCP_FIT           ,  APP_BCPFitCheckGunA             },  //BCP�������

    {APP_CHARGE_READY      ,  APP_ChargeReadyCheckGunA          },  //����׼�����

    {APP_POWER_OUTPUT      ,  APP_ChargeRunningGunA             },  //��ʼ��磬�ù���ģ���������

    {APP_CHARGE_PAUSE	   ,  APP_ChargePauseGunA               },  //��ͣ��������

    {APP_CHARGE_BSD        ,  APP_ChargeBSDGunA                 },  //BSD��Ϣ

    {APP_BMS_CHARGE_ERR_A  ,  APP_StopChargeErrAGunA            },  //������A,�������
    
	{APP_BMS_CHARGE_ERR_B  ,  APP_StopChargeErrBGunA            },  //������B,�������
	
	{APP_BMS_CHARGE_ERR_C  ,  APP_StopChargeErrCGunA            },  //������C,

    {APP_CHARGE_FAIL       ,  APP_SelfCheckFailGunA             },  //���ʧ��
    
    //����BCU����
    {APP_CHARGE_START      ,  APP_StartChargeGunA               },  //��ʼ���

    {APP_CHARGE_END        ,  APP_StopChargeGunA                },  //�������
};

static const _CONTROL_CMD ControlCmdTableB[CMD_TABLE_NUM] =
{
    //����BMS����
    {APP_ELEC_LOCKED       ,  APP_ElectronicLockLockedGunB      },  //����������

    {APP_ELEC_UNLOCKED     ,  APP_ElectronicLockUnlockedGunB    },  //����������

    {APP_INSULATIONG_CHECK ,  APP_InsulationCheckGunB           },  //��Ե�������

    {APP_BCP_FIT           ,  APP_BCPFitCheckGunB               },  //BCP�������

    {APP_CHARGE_READY      ,  APP_ChargeReadyCheckGunB          },  //����׼�����

    {APP_POWER_OUTPUT      ,  APP_ChargeRunningGunB             },  //��ʼ��磬�ù���ģ���������

    {APP_CHARGE_PAUSE	   ,  APP_ChargePauseGunB               },  //��ͣ��������

    {APP_CHARGE_BSD        ,  APP_ChargeBSDGunB                 },  //BSD��Ϣ

    {APP_BMS_CHARGE_ERR_A  ,  APP_StopChargeErrAGunB            },  //������A,�������
    
	{APP_BMS_CHARGE_ERR_B  ,  APP_StopChargeErrBGunB            },  //������B,�������
	
	{APP_BMS_CHARGE_ERR_C  ,  APP_StopChargeErrCGunB            },  //������C,

    {APP_CHARGE_FAIL       ,  APP_SelfCheckFailGunB             },  //���ʧ��
    
    //����BCU����
    {APP_CHARGE_START      ,  APP_StartChargeGunB               },  //��ʼ���

    {APP_CHARGE_END        ,  APP_StopChargeGunB                },  //�������
};

/*****************************************************************************
* Function      : APP_GetPresetValue
* Description   : ��ȡԤ��ֵ
* Input         : None 
* Output        : _PRESET_VALUE 		PresetValue
* Note(s)       : 
* Contributor   : 2016��10��17��  Yxy
*****************************************************************************/
_PRESET_VALUE	APP_GetPresetValue(void)
{
	return	PresetValue;
}

/*****************************************************************************
* Function      : APP_GetSystemState
* Description   : ��ȡϵͳ����״̬
* Input         : None 
* Output        : SysState.WorkState
* Note(s)       : 
* Contributor   : 2016��10��26��  Yxy
*****************************************************************************/
//_WORK_STATE     APP_GetSystemState(INT8U gun)
//{
//	if(gun >= GUN_MAX)
//	{
//		return WORK_STATE_IDLE;
//	}
//	return	SysState[gun].WorkState;
//}
/*****************************************************************************
* Function      : APP_Get_ERR_Branch
* Description   : ��ȡֹͣԭ��С��
* Input         : None 
* Output        : SysState.ErrBranch
* Note(s)       : 
* Contributor   : 2017��1��6��  Yxy
*****************************************************************************/
_CONTROL_ERR_BRANCH APP_Get_ERR_Branch(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return STOP_BSTOTHERERR;
	}
	return	SysState[gun].ErrBranch;
}
/*****************************************************************************
* Function      : APP_Set_ERR_Branch
* Description   : ����ֹͣԭ��С��
* Input         : _CONTROL_ERR_BRANCH 
* Output        : None
* Note(s)       : 
* Contributor   : 2017��1��6��  Yxy
*****************************************************************************/
void APP_Set_ERR_Branch(INT8U gun,_CONTROL_ERR_BRANCH Err_Branch_Value)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
	SysState[gun].ErrBranch=Err_Branch_Value;
}

/*****************************************************************************
* Function      : APP_GetCMLContext
* Description   : ��ȡ�������
* Input         : _POWER_OUTPUT_PARA *para  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��15��  Ҷϲ��
*****************************************************************************/
INT8U APP_GetCMLContext(_BMS_CML_CONTEXT *context)
{
    _PRESET_VALUE		PresetValue_Tmp;
    
    if (context == NULL)
    {
        return FALSE;
    }
	
	PresetValue_Tmp=APP_GetPresetValue();
    //��������ѹ
    context->MaxOutVol = PresetValue_Tmp.VolMaxOutput;
    //��С�����ѹ
    context->MinOutVol = PresetValue_Tmp.VolMinOutput;
    //����������
    context->MaxOutCur = 4000 - PresetValue_Tmp.CurMaxOutput;
    //��С�������
    context->MinOutCur = 4000;
    
    return TRUE;
}

/*****************************************************************************
* Function      : APP_GetCSSContext
* Description   : ��ȡCCS��������
* Input         : _BMS_CCS_CONTEXT *context  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��15��  Ҷϲ��
*****************************************************************************/
INT8U APP_GetCSSContext(_GUN_NUM gun,_BMS_CCS_CONTEXT *pcontext)
{
	USERINFO * puserinfo  = GetChargingInfo(gun);
    if ((pcontext == NULL) || (gun >= GUN_MAX))
    {
        return FALSE;
    }
	pcontext->OutputVol = PowerModuleInfo[gun].OutputInfo.Vol;
	 pcontext->OutputCur = 4000 - (INT16U)(PowerModuleInfo[gun].OutputInfo.Cur);
	pcontext->TotalChargeTime = puserinfo->ChargeTime;
    if(APP_GetGBType(gun) == BMS_GB_2015) 
    {
    //    pcontext->ChargePause = 0x01;  //��ʱ��������״̬�� �����������ٳ���������Ҫ��̬ͣʱ������
        if (GetChargePauseState(gun) == FALSE) //������
        {
            pcontext->ChargePause = 0x01; 
        }
        else //��ͣ���
        {
            pcontext->ChargePause = 0x00; 
        }
        pcontext->ChargePause |= 0xFC;
    }
    return TRUE;
}

/*****************************************************************************
* Function      : APP_GetCSTContext
* Description   : ��ȡCST��������
* Input         : _BMS_CST_CONTEXT *context  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��15��  Ҷϲ��
*****************************************************************************/
INT8U APP_GetCSTContext(_GUN_NUM gun,_BMS_CST_CONTEXT *context)
{
    if ((context == NULL) ||(gun>= GUN_MAX))
    {
        return FALSE;
    }
    //��ϵͳ״ֹ̬ͣԭ��ֱ�Ӹ������
    *context = SysState[gun].StopReason.ChargeStop;
    return TRUE;
}

/*****************************************************************************
* Function      : APP_GetCSDContext
* Description   : ��ȡCSD��������
* Input         : _BMS_CSD_CONTEXT *context  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��15��  Ҷϲ��
*****************************************************************************/
INT8U APP_GetCSDContext(_GUN_NUM gun,_BMS_CSD_CONTEXT *context)
{
	USERINFO * puserinfo  = GetChargingInfo(gun);
    if (context == NULL)
    {
        return FALSE;
    }


	context->TotalChargeTime = puserinfo->ChargeTime;
	context->PowerOut = puserinfo->TotalPower4/1000;

    //�������
    if(APP_GetGBType(gun) == BMS_GB_2015)
    {
        context->ChargeSN = 8;//����Ŀǰ��ͳһΪ8�Ż���
    }
    return TRUE;
}


/*****************************************************************************
* Function      : GetGunState
* Description   : ��ȡǹӲ������״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
_GUN_STATE GetGunState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return GUN_DISCONNECTED;
	}
    return SysState[gun].RemoteMeterState.GunState;
}

/*****************************************************************************
* Function      : SetGunState
* Description   : ����ǹӲ������״̬
* Input         : _GUN_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
void SetGunState(INT8U gun,_GUN_STATE state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    SysState[gun].RemoteMeterState.GunState = state;
}

/*****************************************************************************
* Function      : GetStartFailType
* Description   : ��ȡ����ʧ������
* Input         : void  
* Output        : _START_FAIL_TYPE
* Note(s)       : 
* Contributor   : 2016��10��21��  Yxy
*****************************************************************************/
_START_FAIL_TYPE GetStartFailType(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return ENDFAIL_OTHERERR;
	}
	return	SysState[gun].StartFailType;
}
/*****************************************************************************
* Function      : SetStartFailType
* Description   : ��������ʧ������
* Input         : _START_FAIL_TYPE FailType 
* Output        : None
* Note(s)       : 
* Contributor   : 2016��10��21��  Yxy
*****************************************************************************/
void SetStartFailType(INT8U gun,_START_FAIL_TYPE FailType)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
	SysState[gun].StartFailType=FailType;
	
}

/*****************************************************************************
* Function      : GetPowerDCState
* Description   : ��ȡֱ������Ӵ���K1\K2��״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��14��  Ҷϲ��
*****************************************************************************/
_POWER_DC_STATE GetPowerDCState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return POWER_DC_CLOSE;
	}
    return SysState[gun].RemoteSingalState.PowerDCState;
}

/*****************************************************************************
* Function      : SetPowerDCState
* Description   : ����ֱ������Ӵ���K1\K2��״̬
* Input         : _POWER_DC_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��14��  Ҷϲ��
*****************************************************************************/
void SetPowerDCState(INT8U gun,_POWER_DC_STATE state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    SysState[gun].RemoteSingalState.PowerDCState = state;
}

/*****************************************************************************
* Function      : GetBMSStartCharge
* Description   : ��ȡ�Ƿ���Ҫ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSStartCharge(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].StartCharge;
}

/*****************************************************************************
* Function      : SetBMSStartCharge
* Description   : �����Ƿ���Ҫ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSStartCharge(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].StartCharge = state;
}

/*****************************************************************************
* Function      : GetBMSStopCharge
* Description   : ��ȡ�Ƿ���Ҫ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSStopCharge(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].StopCharge;
}

/*****************************************************************************
* Function      : SetBMSStopOnce
* Description   : �����Ƿ�����ֹͣ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSStopOnce(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].StopOnce = state;
}

/*****************************************************************************
* Function      : GetBMSStopOnce
* Description   : ��ȡ�Ƿ���Ҫ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSStopOnce(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].StopOnce;
}

/*****************************************************************************
* Function      : SetBMSStopCharge
* Description   : �����Ƿ�����ֹͣ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSStopCharge(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].StopCharge = state;
}

/*****************************************************************************
* Function      : GetBMSInsulationCheck
* Description   : ��ȡ�Ƿ���Ҫ��Ե���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSInsulationCheck(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].InsulationCheck;
}

/*****************************************************************************
* Function      : SetBMSInsulationCheck
* Description   : �����Ƿ���Ҫ��Ե���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSInsulationCheck(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].InsulationCheck = state;
}

/*****************************************************************************
* Function      : GetBMSBCPFitCheck
* Description   : ��ȡ�Ƿ���ҪBCP�������
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSBCPFitCheck(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].BCPFitCheck;
}

/*****************************************************************************
* Function      : SetBMSBCPFitCheck
* Description   : �����Ƿ���ҪBCP�������
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSBCPFitCheck(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].BCPFitCheck = state;
}

/*****************************************************************************
* Function      : GetChargeReady
* Description   : ��ȡ�����Ƿ����
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
_CHARGE_READY_STATE GetChargeReady(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return READY_FAIL;
	}
    return (_CHARGE_READY_STATE)ControlStep[gun].ChargeReady;
}

/*****************************************************************************
* Function      : SetChargeReady
* Description   : ���ó�������
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetChargeReady(INT8U gun,_CHARGE_READY_STATE state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].ChargeReady = state;
}

/*****************************************************************************
* Function      : GetBMSSelfCheck
* Description   : ��ȡBMS�Ƿ���Ҫ�����Լ�
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetBMSSelfCheck(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].SelfCheck;
}

/*****************************************************************************
* Function      : SetBMSSelfCheck
* Description   : ����BMS�Ƿ���Ҫ�����Լ�
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetBMSSelfCheck(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].SelfCheck = state;
}

/*****************************************************************************
* Function      : GetChargeRunningState
* Description   : ��ȡ�Ƿ��Ѿ��������
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetChargeRunningState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].ChargeRunning;
}

/*****************************************************************************
* Function      : SetChargeRunningState
* Description   : �����Ƿ��������
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetChargeRunningState(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].ChargeRunning = state;
}

/*****************************************************************************
* Function      : GetChargeDemandUpdateState
* Description   : ��ȡ�����Ƿ����
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��9��  Ҷϲ��
*****************************************************************************/
//INT8U GetChargeDemandUpdateState(INT8U gun)
//{
//	if(gun >= GUN_MAX)
//	{
//		return FALSE;
//	}
//    return ControlStep[gun].DemandUpdate;
//}

/*****************************************************************************
* Function      : SetChargeDemandUpdateState
* Description   : ���������Ƿ�Ҫ����
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��9��  Ҷϲ��
*****************************************************************************/
void SetChargeDemandUpdateState(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].DemandUpdate = state;
}

/*****************************************************************************
* Function      : GetChargePauseState
* Description   : ��ȡ��ͣ���״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
INT8U GetChargePauseState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].PauseCharge;
}

/*****************************************************************************
* Function      : SetChargePauseState
* Description   : ��ͣ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��3��  Ҷϲ��
*****************************************************************************/
void SetChargePauseState(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].PauseCharge = state;
}

/*****************************************************************************
* Function      : GetDealFaultOverState
* Description   : ��ȡ�������״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
INT8U GetDealFaultOverState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
    return ControlStep[gun].DealFaultOver;
}

/*****************************************************************************
* Function      : SetDealFaultOverState
* Description   : ���ô������״̬
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
void SetDealFaultOverState(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].DealFaultOver = state;
}

/*****************************************************************************
* Function      : GetBMSConnectStep
* Description   : ��ȡBMS���ֲ���
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
//INT8U GetBMSConnectStep(INT8U gun)
//{
//	if(gun >= GUN_MAX)
//	{
//		return FALSE;
//	}
//    return ControlStep[gun].BMSStep;
//}

/*****************************************************************************
* Function      : SetBMSConnectStep
* Description   : ����BMS���ֲ���
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��29��  Ҷϲ��
*****************************************************************************/
void SetBMSConnectStep(INT8U gun,INT8U state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    ControlStep[gun].BMSStep = state;
}

/*****************************************************************************
* Function      : GetElecLockState
* Description   : ��ȡ������Ӳ��״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
_ELEC_LOCK_STATE GetElecLockState(INT8U gun)
{

	if(SYSSet.LockState == 1)   //����
	{
		return SysState[gun].RemoteSingalState.ElecLockState;
	}
	else
	{
		if( SysState[gun].RemoteSingalState.ElecLockState == ELEC_LOCK_LOCKED)
		{
			return ELEC_LOCK_UNLOCKED;
		}

		else
		{
			return ELEC_LOCK_LOCKED;
		}
	}
}

/*****************************************************************************
* Function      : SetElecLockState
* Description   : ���õ�����Ӳ��״̬
* Input         : _ELEC_LOCK_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
void SetElecLockState(INT8U gun,_ELEC_LOCK_STATE state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    SysState[gun].RemoteSingalState.ElecLockState = state;
}

/*****************************************************************************
* Function      : GetEmergencyState
* Description   : ��ȡ��ͣ��ť״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
_EMERGENCY_STATE GetEmergencyState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return EMERGENCY_PRESSED;
	}
    return SysState[gun].RemoteSingalState.EmergencyState;
}

/*****************************************************************************
* Function      : SetEmergencyState
* Description   : ���ü�ͣ��ť״̬
* Input         : _EMERGENCY_STATE state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
void SetEmergencyState(INT8U gun,_EMERGENCY_STATE state)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    SysState[gun].RemoteSingalState.EmergencyState = state;
}


/*****************************************************************************
* Function      : GetBatState
* Description   : ��ȡǹ��λ״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��17��  Ҷϲ��
*****************************************************************************/
//_GUN_RETURN_STATE GetGunReturnState(INT8U gun)
//{
//	if(gun >= GUN_MAX)
//	{
//		return GUN_UNRETURN;
//	}
//    return SysState[gun].RemoteSingalState.GunReturnState;
//}

/*****************************************************************************
* Function      : GetBatState
* Description   : ��ȡǹ��λ״̬
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��17��  Ҷϲ��
*****************************************************************************/
//void SetGunReturnState(INT8U gun,_GUN_RETURN_STATE state)
//{
//	if(gun >= GUN_MAX)
//	{
//		return;
//	}
//    SysState[gun].RemoteSingalState.GunReturnState = state;
//}


/*****************************************************************************
* Function      : GetChargeMode
* Description   : ��ȡ���ģʽ����Ϊ�Զ����ֶ�
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��4��  Ҷϲ��
*****************************************************************************/
_CHARGE_MODE_TYPE GetChargeMode(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return MODE_UNDEF;
	}
    return ControlInfo[gun].ChargeMode;
}

/*****************************************************************************
* Function      : SetChargeMode
* Description   : ���ó��ģʽ�����ֶ����Զ�
* Input         : _CHARGE_MODE_TYPE mode  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��4��  Ҷϲ��
*****************************************************************************/
void SetChargeMode(INT8U gun,_CHARGE_MODE_TYPE mode)
{
	if(gun >= GUN_MAX)
	{
		return ;
	}
    ControlInfo[gun].ChargeMode = mode;
}

/*****************************************************************************
* Function      : SetBcuRemoteSignalState
* Description   : ����BCUң��״̬
* Input         : INT8U point---BCU_YXPara.BCURemoteSignal[]����Ľű�
                  INT8U bit-----��һ���ֽ����λ    
                  INT8U val-----��λ��ֵ    
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��14��  Ҷϲ��
*****************************************************************************/
INT8U SetBcuRemoteSignalState(INT8U gun,INT8U point, INT8U bit, INT8U val)
{
	INT8U temp[7];
	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	OSSchedLock();
	memcpy(temp,&BCURemoteSignal[gun],7);
    if ( (point >= sizeof(temp)) || (bit > 7) || ( (val != 1) && (val != 0) )  || (gun >= GUN_MAX))
    {
        return FALSE;
    }
    if (val)
    {
        temp[point] |= (1 << bit);
    }
    else
    {
         temp[point]  &= ~(1 << bit);
    }
	memcpy(&BCURemoteSignal[gun],temp,7);
	OSSchedUnlock();
    return TRUE;
}
/*****************************************************************************
* Function      : APP_BatChargeAdjust
* Description   : �Ե�س����̵�������ʱֻ�ǶԵ�������򵥵ľ��ֵ���
* Input         : INT16U vol--������Ҫ����ĵ�ѹ  
                  INT16U cur--������Ҫ����ĵ���  
                  INT8U seq---�ֳɼ���ȥ����
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��4��  Ҷϲ��
*****************************************************************************/
void APP_BatChargeAdjust(INT8U gun,INT16U vol, INT16U cur, INT8U seq)
{
    INT8U i, temp;
    static _CCU_DEMAND_INFO DemandInfo[GUN_MAX];
    static _BSP_MESSAGE send_message[GUN_MAX];

	
	if(gun >= GUN_MAX)
	{
		return ;
	}
    DemandInfo[gun].Vol = vol;
    temp = cur / seq;
    send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
    send_message[gun].DivNum = APP_POWER_ENABLE;
    send_message[gun].GunNum = gun;  //ǹ��
    send_message[gun].pData = (INT8U *)&DemandInfo[gun].Vol;
    for (i = 1; i < seq; i++)
    {
        DemandInfo[gun].Cur = temp * i;
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
        OSTimeDly(SYS_DELAY_50ms);
    }
    DemandInfo[gun].Cur = cur;
    OSQPost(TaskPCU_pevent ,&send_message[gun]);
    OSTimeDly(SYS_DELAY_100ms);
	printf("Power Model Set Adjust\r\n");
}


/*****************************************************************************
* Function      : SetChargeInfo
* Description   : ���ó����Ϣ�ֶ�
* Input         : INT8U offset  
                  void *pdata   
                  INT8U len     
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��14��  Ҷϲ��
*****************************************************************************/
INT8U SetChargeInfo(INT8U gun,INT8U offset, void *pdata, INT8U len)
{
    if ( (pdata == NULL) || (!len) )
    {
        return FALSE;
    }
    switch (offset)
    {
        case 0:
            if (len != sizeof(_BSPRTC_TIME))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.StartTime, (const void *)pdata, len);
            return TRUE;
        case 1:
            if (len != sizeof(_BSPRTC_TIME))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.EndTime, (const void *)pdata, len);
            return TRUE;
        case 2:
            if (len != sizeof(_POWER_VAL))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.StartPower, (const void *)pdata, len);
            return TRUE;
        case 3:
            if (len != sizeof(_POWER_VAL))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.EndPower, (const void *)pdata, len);
            return TRUE;
        case 4:
            if (len != sizeof(_POWER_VAL))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.CostPower, (const void *)pdata, len);
            return TRUE;
        case 5:
            if (len != sizeof(_COST_VAL))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.Cost, (const void *)pdata, len);
            return TRUE;
        case 6:
            if (len != sizeof(_BMS_BSD_CONTEXT))
            {
                return FALSE;
            }
            memcpy((void *)&SysState[gun].ChargeInfo.BSDContext, (const void *)pdata, len);
            return TRUE;
        default:
            return FALSE;
        
    }
}



/*****************************************************************************
* Function      : APP_ElectronicLock
* Description   : ����������
* Input         : void *pdata
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_ElectronicLockLockedGunA(void *pdata)
{
	 _BSP_MESSAGE * pdisposedata = pdata;
	if (pdata == NULL)
	{
		return FALSE;
	}

    memset( (INT8U *)&SysState[GUN_A].TotalErrState, 0, \
        (INT8U *)&SysState[GUN_A].RemoteMeterState.GunState - (INT8U *)&SysState[GUN_A].TotalErrState);
    BSP_CloseLOCK(GUN_A);//

	printf("Electronic Lock Locked\r\n");
    return TRUE;
}

/*****************************************************************************
* Function      : APP_ElectronicLock
* Description   : ����������
* Input         : void *pdata
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_ElectronicLockLockedGunB(void *pdata)
{
	 _BSP_MESSAGE * pdisposedata = pdata;
	if (pdata == NULL)
	{
		return FALSE;
	}

    memset( (INT8U *)&SysState[GUN_B].TotalErrState, 0, \
        (INT8U *)&SysState[GUN_B].RemoteMeterState.GunState - (INT8U *)&SysState[GUN_B].TotalErrState);
    BSP_CloseLOCK(GUN_B);//

	printf("Electronic Lock Locked\r\n");
    return TRUE;
}

/*****************************************************************************
* Function      : APP_ElectronicLockUnlocked
* Description   : ����������
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
INT8U APP_ElectronicLockUnlockedGunA(void *pdata)
{
	 _BSP_MESSAGE * pdisposedata = pdata;

	if (pdata == NULL)
	{
		return FALSE;
	}
	
    //����������
    //�رյ�ѹ������Դ
  	BSP_CloseBMSPower(GUN_A);
	BSP_CloseLOCK(GUN_A);
	printf("Electronic Lock Unlocked\r\n");
    return TRUE;
}

/*****************************************************************************
* Function      : APP_ElectronicLockUnlocked
* Description   : ����������
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
INT8U APP_ElectronicLockUnlockedGunB(void *pdata)
{
	 _BSP_MESSAGE * pdisposedata = pdata;
	if (pdata == NULL)
	{
		return FALSE;
	}
	
    //����������
    //�رյ�ѹ������Դ
  	BSP_CloseBMSPower(GUN_B);
	BSP_CloseLOCK(GUN_B);
	printf("Electronic Lock Unlocked\r\n");
    return TRUE;
}


/*****************************************************************************
* Function      : APP_SelfCheckFail
* Description   : �Լ�ʧ�ܣ����ʧ��
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
INT8U APP_SelfCheckFailGunA(void *pdata)
{


    static _BSP_MESSAGE send_message[GUN_MAX];
	 _BSP_MESSAGE * pdisposedata = pdata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();

	if( (pdata == NULL) || (pdisevent == NULL))
	{
		return FALSE;
	}
	
   	send_message[GUN_A].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[GUN_A].DivNum = APP_CHARGE_FAIL; //����ʧ��
	send_message[GUN_A].GunNum = GUN_A;
	OSQPost(pdisevent, &send_message[GUN_A]);
	OSTimeDly(SYS_DELAY_10ms);
	SetBMSStartCharge(GUN_A,FALSE);
	printf("App Self Check Fail\r\n");
	BSP_CloseBMSPower(GUN_A);
	BSP_CloseLOCK(GUN_A);
    return TRUE;
}


/*****************************************************************************
* Function      : APP_SelfCheckFail
* Description   : �Լ�ʧ�ܣ����ʧ��
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
INT8U APP_SelfCheckFailGunB(void *pdata)
{


    static _BSP_MESSAGE send_message[GUN_MAX];
	 _BSP_MESSAGE * pdisposedata = pdata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
	if( (pdata == NULL) || (pdisevent == NULL))
	{
		return FALSE;
	}
	
   	send_message[GUN_B].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[GUN_B].DivNum = APP_CHARGE_FAIL; //����ʧ��
	send_message[GUN_B].GunNum = GUN_B;
	OSQPost(pdisevent, &send_message[GUN_B]);
	OSTimeDly(SYS_DELAY_10ms);
	SetBMSStartCharge(GUN_B,FALSE);
	printf("App Self Check Fail\r\n");
	BSP_CloseBMSPower(GUN_B);
	BSP_CloseLOCK(GUN_B);
    return TRUE;
}

/*****************************************************************************
* Function      : APP_SelfCheckEnd
* Description   : �����Լ�
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_SelfCheckEnd(void *pdata)
{
    return TRUE;
}


extern _DISP_CONTROL DispControl;       				//��ʾ������ƽṹ��
/*****************************************************************************
* Function      : APP_StartCharge
* Description   : ��ʼ���
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_StartChargeGunA(void *pdata)
{
	_START_CHARGE_CMD * pCharge;
	_START_CHARGE_CMD ChargeInfo[GUN_MAX] = {(_CHARGE_MODE_TYPE)0};
    static _BSP_MESSAGE send_message[GUN_MAX];
	 _BSP_MESSAGE * pdisposedata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
	INT8U gun;
	if( (pdata == NULL) || (pdisevent == NULL))
	{
		return FALSE;
	}
	pdisposedata = ( _BSP_MESSAGE *)pdata;
	pCharge = (_START_CHARGE_CMD *)pdisposedata->pData;

		//AC-jiechuqi
		OSTimeDly(SYS_DELAY_5s);
	gun = GUN_A;
	memcpy(&ChargeInfo[gun],pCharge,sizeof(_START_CHARGE_CMD));

	
	
 
    if (  ( (GetBMSStopCharge(gun) == TRUE) && (GetBMSStartCharge(gun) == TRUE) ) )
    {//pMsgΪ�ա�����ֹͣ�С���ʼ�����,PCUδ�������ܣ���������
        SysState[gun].SelfCheckState.Other = STATE_ERR;
        SysState[gun].SelfCheckState.NoErr = STATE_ERR;

		SetStartFailType(gun,ENDFAIL_OTHERERR);
		NB_WriterReason(gun,"F0",2);
		send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_CHARGE_FAIL; //����ʧ��
		send_message[gun].GunNum = gun;
        OSQPost(pdisevent, &send_message[gun]);
        OSTimeDly(SYS_DELAY_10ms);
		SetBMSStartCharge(gun,FALSE);
		printf("123123");
        return FALSE;
    }
	memset(ChargeRecodeInfo[gun].StopReason,0,sizeof(ChargeRecodeInfo[gun].StopReason));			//ֹͣԭ������ ��Ҫ�����ڲ���Ա�������ʹ��
	if (GetBMSStartCharge(gun) == FALSE)
    {//��ʼ���ɹ�,��Ե�����0
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 0);
        BMS_FrameContextClear(gun);
		BMS_CONNECT_ControlInit(gun);	//���³�ʼ��
	
		SetBMSConnectStep(gun,BMS_CONNECT_DEFAULTVAL);
		//��Ҫ�����Լ�
	    SetBMSSelfCheck(gun,TRUE);
	    //��Ҫ���BCP����
	    SetBMSBCPFitCheck(gun,TRUE);
		//�������
	    SetChargePauseState(gun,FALSE);
	    //���δ����
	    SetBMSStopCharge(gun,FALSE);
	    //����δ׼����
	    SetChargeReady(gun,READY_ING);
		//δ�������
		SetChargeRunningState(gun,FALSE);
		//���ƹر�
		BSP_CloseRunLed(gun);
		//���ϵƹر�
		BSP_CloseFailLed(gun);

		//ϵͳ״̬ȫΪ����
		memset((INT8U *)&SysState[gun].TotalErrState, 0, \
				   (INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
		//�ֶ����ģʽ��0
		memset( (INT8U *)&ManDemandInfo[gun].Vol, 0, sizeof(_CCU_DEMAND_INFO) );
		//PCU�����������Ϣ��0
		memset( (INT8U *)&BMSDemandInfo[gun].Vol, 0, sizeof(_CCU_DEMAND_INFO) );
		
		//����ģ�������Ϣ��0
		//memset( (INT8U *)&PowerModuleInfo.OutputInfo.Vol, 0, sizeof(_POWER_MODULE_INFO));
		
		memset(&CanFrameControl[gun], 0x00, sizeof(_CANFRAMECONTROL));
		memset(&BMS_RTS_FrameInf[gun], 0x00, sizeof(_BMS_RTS_FRAME_INF)); 
    }
	if(ChargeInfo[gun].ChargeMode == MODE_MAN)
	{
		if (GetBMSStartCharge(gun) == TRUE)
		{//�Ѿ��ڳ����
//			if ( (ManDemandInfo[gun].Vol != ChargeInfo[gun].ChargeVol) \
//				|| (ManDemandInfo[gun].Cur !=  ChargeInfo[gun].ChargeCur) )
//			{//�����µ�����
				ManDemandInfo[gun].Vol = ChargeInfo[gun].ChargeVol;
				ManDemandInfo[gun].Cur =  ChargeInfo[gun].ChargeCur;
				//����������
				SetChargeDemandUpdateState(gun,TRUE);
				printf("Manual Demand Update Done\r\n");
//			}
		}
    //�����Ƿ��Ѿ��ڳ���ˣ��ֶ�ģʽ����ʵʱ������ѹ������
      
		else
		{
			
			//��ȡ�ֶ����ĵ�ѹ
			ManDemandInfo[gun].Vol = ChargeInfo[gun].ChargeVol;
			ManDemandInfo[gun].Cur =  ChargeInfo[gun].ChargeCur;
			//�ֶ�ģʽ
			SetChargeMode(gun,MODE_MAN); 
			//ϵͳ״̬ȫΪ����
			memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
				(INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
			//�򿪵�ѹ������Դ
			BSP_OpenBMSPower(gun);
			SetBMSStartCharge(gun,TRUE);  //��ʼ���
			SetBMSStopOnce(gun,FALSE);
		}
		send_message[gun].GunNum = gun;
		if(gun == GUN_A)
		{
			APP_ChargeRunningGunA(&send_message[gun]); //֪ͨģ�鿪ʼ���
		}
		else
		{
			APP_ChargeRunningGunB(&send_message[gun]); //֪ͨģ�鿪ʼ���
		}
		printf("Manual Mode Start Done\r\n");
	}
	
	else //�Զ� VINģʽ
	{
		//ǹδ����
		if (GetGunState(gun) == GUN_DISCONNECTED)
		{
			//ǹδ����
			SysState[gun].SelfCheckState.GunDisconnect = STATE_ERR;
			SysState[gun].SelfCheckState.NoErr = STATE_ERR;
			SetStartFailType(gun,ENDFAIL_HANDERR);
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			NB_WriterReason(gun,"F1",2);
			send_message[gun].DivNum = APP_CHARGE_FAIL; //����ʧ��
			send_message[gun].GunNum = gun;
			OSQPost(pdisevent, &send_message[gun]);
			OSTimeDly(SYS_DELAY_10ms);
			printf("1114\r\n");
			SetBMSStartCharge(gun,FALSE);
			BSP_CloseBMSPower(gun);
			return FALSE; 
		}
		BSP_OpenLOCK(gun);   //�򿪵�����
		BSP_OpenBMSPower(gun);
		if(ChargeInfo[gun].ChargeMode == MODE_VIN)
		{
			SetChargeMode(gun,MODE_VIN); //VINģʽ
		}
		else
		{
			SetChargeMode(gun,MODE_AUTO); //�Զ�ģʽ
		}
		BSP_CloseK1K2(gun);	 //�����������ʼ���ѽӴ�����һ��
		SetBMSStartCharge(gun,TRUE);  //��ʼ���
		SetBMSStopOnce(gun,FALSE);
		DispControl.StopOverFrame[gun] = STATE_UNOVER;   //δ����
		printf("Auto Mode Start Done\r\n");
	}
    return TRUE;
}

/*****************************************************************************
* Function      : APP_StartCharge
* Description   : ��ʼ���
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_StartChargeGunB(void *pdata)
{
	_START_CHARGE_CMD * pCharge;
	_START_CHARGE_CMD ChargeInfo[GUN_MAX] = {(_CHARGE_MODE_TYPE)0};
    static _BSP_MESSAGE send_message[GUN_MAX];
	 _BSP_MESSAGE * pdisposedata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
	INT8U gun;
	if( (pdata == NULL) || (pdisevent == NULL))
	{
		return FALSE;
	}
	pdisposedata = ( _BSP_MESSAGE *)pdata;
	pCharge = (_START_CHARGE_CMD *)pdisposedata->pData;
	gun =pdisposedata->GunNum;
			//AC-jiechuqi
		OSTimeDly(SYS_DELAY_5s);
	gun = GUN_B;
	memcpy(&ChargeInfo[gun],pCharge,sizeof(_START_CHARGE_CMD));

	
	
 
    if (  ( (GetBMSStopCharge(gun) == TRUE) && (GetBMSStartCharge(gun) == TRUE) ) )
    {//pMsgΪ�ա�����ֹͣ�С���ʼ�����,PCUδ�������ܣ���������
        SysState[gun].SelfCheckState.Other = STATE_ERR;
        SysState[gun].SelfCheckState.NoErr = STATE_ERR;

		SetStartFailType(gun,ENDFAIL_OTHERERR);
		
		send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		NB_WriterReason(gun,"F2",2);
		send_message[gun].DivNum = APP_CHARGE_FAIL; //����ʧ��
		send_message[gun].GunNum = gun;
        OSQPost(pdisevent, &send_message[gun]);
        OSTimeDly(SYS_DELAY_10ms);
		SetBMSStartCharge(gun,FALSE);
		printf("123123");
        return FALSE;
    }
	memset(ChargeRecodeInfo[gun].StopReason,0,sizeof(ChargeRecodeInfo[gun].StopReason));			//ֹͣԭ������ ��Ҫ�����ڲ���Ա�������ʹ��
	if (GetBMSStartCharge(gun) == FALSE)
    {//��ʼ���ɹ�,��Ե�����0
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 0);
        BMS_FrameContextClear(gun);
		BMS_CONNECT_ControlInit(gun);	//���³�ʼ��
	
		SetBMSConnectStep(gun,BMS_CONNECT_DEFAULTVAL);
		//��Ҫ�����Լ�
	    SetBMSSelfCheck(gun,TRUE);
	    //��Ҫ���BCP����
	    SetBMSBCPFitCheck(gun,TRUE);
		//�������
	    SetChargePauseState(gun,FALSE);
	    //���δ����
	    SetBMSStopCharge(gun,FALSE);
	    //����δ׼����
	    SetChargeReady(gun,READY_ING);
		//δ�������
		SetChargeRunningState(gun,FALSE);
		//���ƹر�
		BSP_CloseRunLed(gun);
		//���ϵƹر�
		BSP_CloseFailLed(gun);

		//ϵͳ״̬ȫΪ����
		memset((INT8U *)&SysState[gun].TotalErrState, 0, \
				   (INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
		//�ֶ����ģʽ��0
		memset( (INT8U *)&ManDemandInfo[gun].Vol, 0, sizeof(_CCU_DEMAND_INFO) );
		//PCU�����������Ϣ��0
		memset( (INT8U *)&BMSDemandInfo[gun].Vol, 0, sizeof(_CCU_DEMAND_INFO) );
		
		//����ģ�������Ϣ��0
		//memset( (INT8U *)&PowerModuleInfo.OutputInfo.Vol, 0, sizeof(_POWER_MODULE_INFO));
		
		memset(&CanFrameControl[gun], 0x00, sizeof(_CANFRAMECONTROL));
		memset(&BMS_RTS_FrameInf[gun], 0x00, sizeof(_BMS_RTS_FRAME_INF)); 
    }
	if(ChargeInfo[gun].ChargeMode == MODE_MAN)
	{
		if (GetBMSStartCharge(gun) == TRUE)
		{//�Ѿ��ڳ����
//			if ( (ManDemandInfo[gun].Vol != ChargeInfo[gun].ChargeVol) \
//				|| (ManDemandInfo[gun].Cur !=  ChargeInfo[gun].ChargeCur) )
//			{//�����µ�����
				ManDemandInfo[gun].Vol = ChargeInfo[gun].ChargeVol;
				ManDemandInfo[gun].Cur =  ChargeInfo[gun].ChargeCur;
				//����������
				SetChargeDemandUpdateState(gun,TRUE);
				printf("Manual Demand Update Done\r\n");
//			}
		}
    //�����Ƿ��Ѿ��ڳ���ˣ��ֶ�ģʽ����ʵʱ������ѹ������
      
		else
		{
			
			//��ȡ�ֶ����ĵ�ѹ
			ManDemandInfo[gun].Vol = ChargeInfo[gun].ChargeVol;
			ManDemandInfo[gun].Cur =  ChargeInfo[gun].ChargeCur;
			//�ֶ�ģʽ
			SetChargeMode(gun,MODE_MAN); 
			//ϵͳ״̬ȫΪ����
			memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
				(INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
			//�򿪵�ѹ������Դ
			BSP_OpenBMSPower(gun);
			SetBMSStartCharge(gun,TRUE);  //��ʼ���
			SetBMSStopOnce(gun,FALSE);
		}
		send_message[gun].GunNum = gun;
		if(gun == GUN_A)
		{
			APP_ChargeRunningGunA(&send_message[gun]); //֪ͨģ�鿪ʼ���
		}
		else
		{
			APP_ChargeRunningGunB(&send_message[gun]); //֪ͨģ�鿪ʼ���
		}
		printf("Manual Mode Start Done\r\n");
	}
	
	else //�Զ� VINģʽ
	{
		//ǹδ����
		if (GetGunState(gun) == GUN_DISCONNECTED)
		{
			//ǹδ����
			SysState[gun].SelfCheckState.GunDisconnect = STATE_ERR;
			SysState[gun].SelfCheckState.NoErr = STATE_ERR;
			NB_WriterReason(gun,"F3",2);
			SetStartFailType(gun,ENDFAIL_HANDERR);
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			send_message[gun].DivNum = APP_CHARGE_FAIL; //����ʧ��
			send_message[gun].GunNum = gun;
			OSQPost(pdisevent, &send_message[gun]);
			OSTimeDly(SYS_DELAY_10ms);
			printf("1114\r\n");
			SetBMSStartCharge(gun,FALSE);
			BSP_CloseBMSPower(gun);
			return FALSE; 
		}
		BSP_OpenLOCK(gun);   //�򿪵�����
		BSP_OpenBMSPower(gun);
		if(ChargeInfo[gun].ChargeMode == MODE_VIN)
		{
			SetChargeMode(gun,MODE_VIN); //VINģʽ
		}
		else
		{
			SetChargeMode(gun,MODE_AUTO); //�Զ�ģʽ
		}
		SetBMSStartCharge(gun,TRUE);  //��ʼ���
		SetBMSStopOnce(gun,FALSE);
		DispControl.StopOverFrame[gun] = STATE_UNOVER;   //δ����
		printf("Auto Mode Start Done\r\n");
	}
    return TRUE;
}



/*****************************************************************************
* Function      : APP_StopCharge
* Description   : ֹͣ���
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeGunA(void *pdata)
{

	OS_EVENT* pdisevent =  APP_GetDispEvent();
	static _BSP_MESSAGE send_message[GUN_MAX],send_messagebms[GUN_MAX],send_messagedip[GUN_MAX];
     _BSP_MESSAGE* ptr = (_BSP_MESSAGE *)pdata;
	INT8U gun;
	
    if ( ptr == NULL  )
    {
        return FALSE;
    }
	gun	= GUN_A;

	//����  ��Ҫ�������ﲻ����ζ�Ҫ�ر�ģ��
	  //֪ͨPCUֹͣ���
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
	if (GetBMSStopOnce(gun) == TRUE) //�Ѿ�ֹͣ���
    {
        return FALSE;
    }
    
    if (GetBMSStopCharge(gun) == FALSE)
    {
        SetBMSStopCharge(gun,TRUE);  //����ֹͣ���
		SetBMSStartCharge(gun,FALSE);//δ��ʼ���
		printf("656654655\r\n");
        SetChargeReady(gun,READY_ING);   //����δ׼����
  		OSTimeDly(SYS_DELAY_5ms);
      
        //֪ͨBMS����ͨѶ
		send_messagebms[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_messagebms[gun].DivNum = APP_CHARGE_END;
		send_messagebms[gun].DataLen = sizeof(_SYS_STATE);
		send_messagebms[gun].pData = (INT8U *)&SysState[gun].TotalErrState;
		if(gun == GUN_A)
		{
			OSQPost(TaskConnectBMS_peventA, &send_messagebms[gun]);
		}
		else
		{
			OSQPost(TaskConnectBMS_peventB, &send_messagebms[gun]);
		}
		OSTimeDly(SYS_DELAY_5ms);
        if (ERR_CHECK(ptr->pData[0]) == STOP_ERR) //���ֹ���ֹͣ
        {
            NOP();
        }
        else
        {
            BMS_TIMEOUT_ENTER(gun,BMS_OTH2_STEP, SYS_DELAY_100ms);
            //��������Ƿ�Ϊ5A����     
			while (PowerModuleInfo[gun].OutputInfo.Cur >= DEF_OUTPUT_CUR)
            {
                 OSTimeDly(SYS_DELAY_5ms);
                //��ʱ100ms
                if (BMS_TIMEOUT_CHECK(gun,BMS_OTH2_STEP, SYS_DELAY_100ms) == BMS_TIME_OUT)
                {
                    break;
                }
            }
        }
        //(1)�Ͽ�������DC�̵���K1\K2
		
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
        //(3)�Ͽ�������ԴK3\K4
       BSP_CloseBMSPower(gun);
		
		
        //���������DC�̵�������ѹ����60V���£���ʱΪ�����ڲ��ѹ��Ͷ��й�ŵ�·
        if (ERR_CHECK(ptr->pData[0]) == STOP_ERR) //���ֹ���ֹͣ
        {
           OSTimeDly(SYS_DELAY_250ms);
        }
        else
        {
			//K1/K2�Ѿ��Ͽ��������󼶵ĵ�ѹֻ��Ϊ�˷�ֹǹͷ����Ȼ�д���60V�ĵ�ѹ������й�� 
			//�����1s��ǿ��й��1s���Ա�֤�ػ���ģ��Ĳ�������õ���ȫй�ţ�������ε����ʹ���
			//�������й�Ų���������ʣ��200V���ҾͲ�й�ţ�
            OSTimeDly(SYS_DELAY_1s);
            BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_5s);
            //�ж�DC�̵�������ѹ�Ƿ�С��60V
            while (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
            //while(PowerModuleInfo.OutputInfo.Vol >= VAL_ADJUST(60, 10, 1)) 
            {
                OSTimeDly(SYS_DELAY_5ms);
                if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_5s) == BMS_TIME_OUT)
                {
                     ChargeErrDeal(gun,ERR_LF_METHOD_A);
                     break;
                }
            }
        }

	
	BSP_CloseLOCK(gun);
	//(5)�رճ��ָʾ��
	BSP_CloseRunLed(gun); 
		
		
        //(6)���д���,����Ҫ�򿪹��ϵ� ERR_CHECK
        if (ERR_CHECK(ptr->pData[0]) == STOP_ERR) //���ֹ���ֹͣ
        {
//          BSP_RLCOpen(BSPRLC_ERR_LED); 
//			printf("ERR LED 2,%d\r\n",ptr->pData[0]);
        }
        else if (USER_CHECK(ptr->pData[0]) == STOP_USER) //�û�����ֹͣ
        {
            //�û�����ֹͣ���
            SysState[gun].StopReason.ChargeStop.State0.OneByte.UserStopBits = STATE_TROUBLE;
			if(SysState[gun].TotalErrState==ERR_NORMAL_TYPE)
			{
				SysState[gun].TotalErrState |= ERR_USER_STOP_TYPE;
			}
			printf("User Stop2\r\n");
        }
        //���������ʱ��
        SysState[gun].ChargeInfo.CurTick = OSTimeGet();
        BSP_RTCGetTime((_BSPRTC_TIME *)&SysState[gun].ChargeInfo.EndTime.Second);
		
        //δ�������
        SetChargeRunningState(gun,FALSE);
        //���ģʽ����Ϊδ����
        SetChargeMode(gun,MODE_UNDEF);
        SetBMSStopCharge(gun,FALSE);  //δ����ֹͣ������

        //֪ͨ��ʾ����
		send_messagedip[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_messagedip[gun].DivNum = APP_CHARGE_END; //������
		send_messagedip[gun].GunNum = gun;
		OSQPost(pdisevent, &send_messagedip[gun]);
		
  
        OSTimeDly(SYS_DELAY_10ms);
        SetBMSStopOnce(gun,TRUE);
		printf("Stop\r\n");
    }
    SetDealFaultOverState(gun,TRUE);
	
	
	 //δ�������   20220902 ��ȫ������ٲ���һ��
	SetChargeRunningState(gun,FALSE);
	BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
	BSP_CloseRunLed(gun); 				//�رճ��ָʾ��
    return TRUE;
}


/*****************************************************************************
* Function      : APP_StopCharge
* Description   : ֹͣ���
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeGunB(void *pdata)
{

	OS_EVENT* pdisevent =  APP_GetDispEvent();
	static _BSP_MESSAGE send_message[GUN_MAX],send_messagebms[GUN_MAX],send_messagedip[GUN_MAX];
     _BSP_MESSAGE* ptr = (_BSP_MESSAGE *)pdata;
	INT8U gun;
	
    if ( ptr == NULL  )
    {
        return FALSE;
    }
	gun	= GUN_B;

	//����  ��Ҫ�������ﲻ����ζ�Ҫ�ر�ģ��
	  //֪ͨPCUֹͣ���
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
	if (GetBMSStopOnce(gun) == TRUE) //�Ѿ�ֹͣ���
    {
        return FALSE;
    }
    
    if (GetBMSStopCharge(gun) == FALSE)
    {
        SetBMSStopCharge(gun,TRUE);  //����ֹͣ���
		SetBMSStartCharge(gun,FALSE);//δ��ʼ���
		printf("656654655\r\n");
        SetChargeReady(gun,READY_ING);   //����δ׼����
  		OSTimeDly(SYS_DELAY_5ms);
      
        //֪ͨBMS����ͨѶ
		send_messagebms[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_messagebms[gun].DivNum = APP_CHARGE_END;
		send_messagebms[gun].DataLen = sizeof(_SYS_STATE);
		send_messagebms[gun].pData = (INT8U *)&SysState[gun].TotalErrState;
		if(gun == GUN_A)
		{
			OSQPost(TaskConnectBMS_peventA, &send_messagebms[gun]);
		}
		else
		{
			OSQPost(TaskConnectBMS_peventB, &send_messagebms[gun]);
		}
		OSTimeDly(SYS_DELAY_5ms);
        if (ERR_CHECK(ptr->pData[0]) == STOP_ERR) //���ֹ���ֹͣ
        {
            NOP();
        }
        else
        {
            BMS_TIMEOUT_ENTER(gun,BMS_OTH2_STEP, SYS_DELAY_100ms);
            //��������Ƿ�Ϊ5A����     
			while (PowerModuleInfo[gun].OutputInfo.Cur >= DEF_OUTPUT_CUR)
            {
                 OSTimeDly(SYS_DELAY_5ms);
                //��ʱ100ms
                if (BMS_TIMEOUT_CHECK(gun,BMS_OTH2_STEP, SYS_DELAY_100ms) == BMS_TIME_OUT)
                {
                    break;
                }
            }
        }
        //(1)�Ͽ�������DC�̵���K1\K2
		
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
        //(3)�Ͽ�������ԴK3\K4
       BSP_CloseBMSPower(gun);
		
		
        //���������DC�̵�������ѹ����60V���£���ʱΪ�����ڲ��ѹ��Ͷ��й�ŵ�·
        if (ERR_CHECK(ptr->pData[0]) == STOP_ERR) //���ֹ���ֹͣ
        {
           OSTimeDly(SYS_DELAY_250ms);
        }
        else
        {
			//K1/K2�Ѿ��Ͽ��������󼶵ĵ�ѹֻ��Ϊ�˷�ֹǹͷ����Ȼ�д���60V�ĵ�ѹ������й�� 
			//�����1s��ǿ��й��1s���Ա�֤�ػ���ģ��Ĳ�������õ���ȫй�ţ�������ε����ʹ���
			//�������й�Ų���������ʣ��200V���ҾͲ�й�ţ�
            OSTimeDly(SYS_DELAY_1s);
            BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_5s);
            //�ж�DC�̵�������ѹ�Ƿ�С��60V
            while (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
            //while(PowerModuleInfo.OutputInfo.Vol >= VAL_ADJUST(60, 10, 1)) 
            {
                OSTimeDly(SYS_DELAY_5ms);
                if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_5s) == BMS_TIME_OUT)
                {
                     ChargeErrDeal(gun,ERR_LF_METHOD_A);
                     break;
                }
            }
        }

	
	BSP_CloseLOCK(gun);
	//(5)�رճ��ָʾ��
	BSP_CloseRunLed(gun); 
		
		
        //(6)���д���,����Ҫ�򿪹��ϵ� ERR_CHECK
        if (ERR_CHECK(ptr->pData[0]) == STOP_ERR) //���ֹ���ֹͣ
        {
//          BSP_RLCOpen(BSPRLC_ERR_LED); 
//			printf("ERR LED 2,%d\r\n",ptr->pData[0]);
        }
        else if (USER_CHECK(ptr->pData[0]) == STOP_USER) //�û�����ֹͣ
        {
            //�û�����ֹͣ���
            SysState[gun].StopReason.ChargeStop.State0.OneByte.UserStopBits = STATE_TROUBLE;
			if(SysState[gun].TotalErrState==ERR_NORMAL_TYPE)
			{
				SysState[gun].TotalErrState |= ERR_USER_STOP_TYPE;
			}
			printf("User Stop2\r\n");
        }
        //���������ʱ��
        SysState[gun].ChargeInfo.CurTick = OSTimeGet();
        BSP_RTCGetTime((_BSPRTC_TIME *)&SysState[gun].ChargeInfo.EndTime.Second);
		
        //δ�������
        SetChargeRunningState(gun,FALSE);
        //���ģʽ����Ϊδ����
        SetChargeMode(gun,MODE_UNDEF);
        SetBMSStopCharge(gun,FALSE);  //δ����ֹͣ������

        //֪ͨ��ʾ����
		send_messagedip[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_messagedip[gun].DivNum = APP_CHARGE_END; //������
		send_messagedip[gun].GunNum = gun;
		OSQPost(pdisevent, &send_messagedip[gun]);
		
  
        OSTimeDly(SYS_DELAY_10ms);
        SetBMSStopOnce(gun,TRUE);
		printf("Stop\r\n");
    }
    SetDealFaultOverState(gun,TRUE);
	
	
	 //δ�������   20220902 ��ȫ������ٲ���һ��
	SetChargeRunningState(gun,FALSE);
	BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
	BSP_CloseRunLed(gun); 				//�رճ��ָʾ��
    return TRUE;
}

/*****************************************************************************
* Function      : APP_StopChargeErr
* Description   : ����A�Ĵ���ʽ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��27��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeErrAGunA(void *pdata)
{
	_BSP_MESSAGE *pMsg = NULL;
	INT8U gun;
	static _BSP_MESSAGE send_message[GUN_MAX];
	if(pdata == NULL)
	{
		return FALSE;
	}
	OSSchedLock();
	pMsg = pdata;
	gun = GUN_A;
	OSSchedUnlock();
	
	//֪ͨPCUֹͣ���
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;		
	OSQPost(TaskPCU_pevent, &send_message[gun]);

	BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
	SetPowerDCState(gun,POWER_DC_OPEN);
	BMS_CONNECT_StepSet(gun,BMS_SEND_DEFAULT);			//���ٷ���BMS֡
    BSP_CloseBMSPower(gun);			
	BSP_CloseLOCK(gun);	//����������
    BSP_CloseRunLed(gun); 				//�رճ��ָʾ��
	SetBMSStartCharge(gun,FALSE);						//δ��ʼ���
	printf("6566655\r\n");
	SetChargeReady(gun,READY_ING);   						//����δ׼����
	SetChargeRunningState(gun,FALSE);					//δ�������
	SetChargeMode(gun,MODE_UNDEF);						//���ģʽ����Ϊδ����
    SetBMSSelfCheck(gun,TRUE);							//��Ҫ���ǹ�����Լ�
    SetBMSStopCharge(gun,FALSE);						//ֹͣ������
    SetDealFaultOverState(gun,TRUE);
	return TRUE;
}
/*****************************************************************************
* Function      : APP_StopChargeErr
* Description   : ����A�Ĵ���ʽ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��27��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeErrAGunB(void *pdata)
{
	_BSP_MESSAGE *pMsg = NULL;
	INT8U gun;
	static _BSP_MESSAGE send_message[GUN_MAX];
	if(pdata == NULL)
	{
		return FALSE;
	}
	OSSchedLock();
	pMsg = pdata;
	gun = GUN_B;
	OSSchedUnlock();
	
	//֪ͨPCUֹͣ���
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;		
	OSQPost(TaskPCU_pevent, &send_message[gun]);

	BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
	SetPowerDCState(gun,POWER_DC_OPEN);
	BMS_CONNECT_StepSet(gun,BMS_SEND_DEFAULT);			//���ٷ���BMS֡
    BSP_CloseBMSPower(gun);			
	BSP_CloseLOCK(gun);	//����������
    BSP_CloseRunLed(gun); 				//�رճ��ָʾ��
	SetBMSStartCharge(gun,FALSE);						//δ��ʼ���
	printf("6566655\r\n");
	SetChargeReady(gun,READY_ING);   						//����δ׼����
	SetChargeRunningState(gun,FALSE);					//δ�������
	SetChargeMode(gun,MODE_UNDEF);						//���ģʽ����Ϊδ����
    SetBMSSelfCheck(gun,TRUE);							//��Ҫ���ǹ�����Լ�
    SetBMSStopCharge(gun,FALSE);						//ֹͣ������
    SetDealFaultOverState(gun,TRUE);
	return TRUE;
}

/*****************************************************************************
* Function      : APP_StopChargeErrB
* Description   : ����B�Ĵ���ʽ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��12��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeErrBGunA(void *pdata)
{
    static _BSP_MESSAGE send_message[GUN_MAX], send_dismessage[GUN_MAX];
     _BSP_MESSAGE* ptr = (_BSP_MESSAGE *)pdata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
     INT8U gun;

	OSSchedLock();
	gun = GUN_A;
	OSSchedUnlock();
	
	//֪ͨPCUֹͣ���   ������ζ�Ҫ�ر�ģ��
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
    if (GetBMSStopCharge(gun) == FALSE)
    {
        SetBMSStopCharge(gun,TRUE);  //����ֹͣ���
		SetBMSStartCharge(gun,FALSE);//δ��ʼ���
        SetChargeReady(gun,READY_ING);   //����δ׼����
		printf("GUN = %d,654655\r\n",gun);
        //��������Ƿ�Ϊ5A����    
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_100ms);
		while (PowerModuleInfo[gun].OutputInfo.Cur >= DEF_OUTPUT_CUR)
        {
            OSTimeDly(SYS_DELAY_5ms);
            //��ʱ100ms
			
				if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_100ms) == BMS_TIME_OUT)
				{
					break;
				}
		
        }
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
        //(2)й�ŵ�·����
		//���������DC�̵�������ѹ����60V���£���ʱΪ�����ڲ��ѹ��Ͷ��й�ŵ�·
        OSTimeDly(SYS_DELAY_10ms); 
		
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_5s);
        //�ж�DC�̵�������ѹ�Ƿ�С��60V
        while (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
        {
            OSTimeDly(SYS_DELAY_5ms);
		
			 if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_5s) == BMS_TIME_OUT)
			{
				 ChargeErrDeal(gun,ERR_LF_METHOD_A);
				 break;
			}

        }
		BSP_CloseBMSPower(gun);			
		BSP_CloseLOCK(gun);	//����������
		BSP_CloseRunLed(gun); 				//�رճ��ָʾ��

        OSTimeDly(SYS_DELAY_1s);
        //ֹͣ����CEM
		BMS_CONNECT_StepSet(gun,BMS_SEND_DEFAULT);
        //���������ʱ��
        SysState[gun].ChargeInfo.CurTick = OSTimeGet();
        BSP_RTCGetTime((_BSPRTC_TIME *)&SysState[gun].ChargeInfo.EndTime.Second);

        //δ�������
        SetChargeRunningState(gun,FALSE);
        //���ģʽ����Ϊδ����
        SetChargeMode(gun,MODE_UNDEF);
        SetBMSStopCharge(gun,FALSE);  //δ����ֹͣ������

        //֪ͨBCU����
		send_dismessage[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_dismessage[gun].DivNum = APP_CHARGE_END; //������
		send_dismessage[gun].GunNum = gun;
        OSQPost(pdisevent, &send_dismessage[gun]);
		
    
		printf("Stop ErrorB\r\n");
    }
	BMS_CONNECT_StateSet(gun,BMS_CONNECT_DEFAULT);//���õ�ǰ״̬Ϊ��ʼֵ
    SetBMSSelfCheck(gun,TRUE);                    //��Ҫ���ǹ�����Լ�
    SetBMSStopCharge(gun,FALSE);
    SetDealFaultOverState(gun,TRUE);
	
	
	 //δ�������   20220902 ��ȫ������ٲ���һ��
	SetChargeRunningState(gun,FALSE);
	BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
	BSP_CloseRunLed(gun); 				//�رճ��ָʾ��
    return TRUE;
}

/*****************************************************************************
* Function      : APP_StopChargeErrB
* Description   : ����B�Ĵ���ʽ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��12��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeErrBGunB(void *pdata)
{
    static _BSP_MESSAGE send_message[GUN_MAX], send_dismessage[GUN_MAX];
     _BSP_MESSAGE* ptr = (_BSP_MESSAGE *)pdata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
     INT8U gun;

	OSSchedLock();
	gun = GUN_B;
	OSSchedUnlock();
	
	//֪ͨPCUֹͣ���   ������ζ�Ҫ�ر�ģ��
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
    if (GetBMSStopCharge(gun) == FALSE)
    {
        SetBMSStopCharge(gun,TRUE);  //����ֹͣ���
		SetBMSStartCharge(gun,FALSE);//δ��ʼ���
        SetChargeReady(gun,READY_ING);   //����δ׼����
		printf("GUN = %d,654655\r\n",gun);
        //��������Ƿ�Ϊ5A����    
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_100ms);
		while (PowerModuleInfo[gun].OutputInfo.Cur >= DEF_OUTPUT_CUR)
        {
            OSTimeDly(SYS_DELAY_5ms);
            //��ʱ100ms
			
				if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_100ms) == BMS_TIME_OUT)
				{
					break;
				}
		
        }
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
        //(2)й�ŵ�·����
		//���������DC�̵�������ѹ����60V���£���ʱΪ�����ڲ��ѹ��Ͷ��й�ŵ�·
        OSTimeDly(SYS_DELAY_10ms); 
		
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_5s);
        //�ж�DC�̵�������ѹ�Ƿ�С��60V
        while (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
        {
            OSTimeDly(SYS_DELAY_5ms);
		
			 if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_5s) == BMS_TIME_OUT)
			{
				 ChargeErrDeal(gun,ERR_LF_METHOD_A);
				 break;
			}

        }
		BSP_CloseBMSPower(gun);			
		BSP_CloseLOCK(gun);	//����������
		BSP_CloseRunLed(gun); 				//�رճ��ָʾ��

        OSTimeDly(SYS_DELAY_1s);
        //ֹͣ����CEM
		BMS_CONNECT_StepSet(gun,BMS_SEND_DEFAULT);
        //���������ʱ��
        SysState[gun].ChargeInfo.CurTick = OSTimeGet();
        BSP_RTCGetTime((_BSPRTC_TIME *)&SysState[gun].ChargeInfo.EndTime.Second);

        //δ�������
        SetChargeRunningState(gun,FALSE);
        //���ģʽ����Ϊδ����
        SetChargeMode(gun,MODE_UNDEF);
        SetBMSStopCharge(gun,FALSE);  //δ����ֹͣ������

        //֪ͨBCU����
		send_dismessage[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_dismessage[gun].DivNum = APP_CHARGE_END; //������
		send_dismessage[gun].GunNum = gun;
        OSQPost(pdisevent, &send_dismessage[gun]);
		
    
		printf("Stop ErrorB\r\n");
    }
	BMS_CONNECT_StateSet(gun,BMS_CONNECT_DEFAULT);//���õ�ǰ״̬Ϊ��ʼֵ
    SetBMSSelfCheck(gun,TRUE);                    //��Ҫ���ǹ�����Լ�
    SetBMSStopCharge(gun,FALSE);
    SetDealFaultOverState(gun,TRUE);
	
	
	 //δ�������   20220902 ��ȫ������ٲ���һ��
	SetChargeRunningState(gun,FALSE);
	BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
	BSP_CloseRunLed(gun); 				//�رճ��ָʾ��
    return TRUE;
}


/*****************************************************************************
* Function      : APP_StopChargeErrC
* Description   : ����C�Ĵ���ʽ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeErrCGunA(void *pdata)
{
    static _BSP_MESSAGE send_message[GUN_MAX];
     _BSP_MESSAGE* ptr = (_BSP_MESSAGE *)pdata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
     INT8U gun;
     
	OSSchedLock();
	if(ptr ==NULL)
	{
		OSSchedUnlock();
		return FALSE;
	}
	gun = GUN_A;
	OSSchedUnlock();
    
	
	//֪ͨPCUֹͣ���
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
    if (GetBMSStopCharge(gun) == FALSE)
    {
        SetBMSStopCharge(gun,TRUE);  //����ֹͣ���
        //֪ͨPCUֹͣ���
        OSTimeDly(SYS_DELAY_20ms);
#if 1
        //��������Ƿ�Ϊ5A����     
		
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_100ms);
		
		while (PowerModuleInfo[gun].OutputInfo.Cur >= DEF_OUTPUT_CUR)
        {
            OSTimeDly(SYS_DELAY_5ms);
            //��ʱ100ms
			
			if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_100ms) == BMS_TIME_OUT)
			{
				break;
			}
        }
#endif
        //�Ͽ�������DC�̵���K1\K2
        BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
        OSTimeDly(SYS_DELAY_1s);
		//�Ͽ�K1��K2���÷���CEM��Ҳ���÷���CST
		BMS_CONNECT_StepSet(gun,BMS_SEND_DEFAULT); 
    }
    SetBMSStopCharge(gun,FALSE);
    SetDealFaultOverState(gun,TRUE);
	return TRUE;
}

/*****************************************************************************
* Function      : APP_StopChargeErrC
* Description   : ����C�Ĵ���ʽ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_StopChargeErrCGunB(void *pdata)
{
    static _BSP_MESSAGE send_message[GUN_MAX];
     _BSP_MESSAGE* ptr = (_BSP_MESSAGE *)pdata;
	OS_EVENT* pdisevent =  APP_GetDispEvent();
     INT8U gun;
     
	OSSchedLock();
	if(ptr ==NULL)
	{
		OSSchedUnlock();
		return FALSE;
	}
	gun =GUN_B;
	OSSchedUnlock();
    
	
	//֪ͨPCUֹͣ���
	send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
    if (GetBMSStopCharge(gun) == FALSE)
    {
        SetBMSStopCharge(gun,TRUE);  //����ֹͣ���
        //֪ͨPCUֹͣ���
        OSTimeDly(SYS_DELAY_20ms);
#if 1
        //��������Ƿ�Ϊ5A����     
		
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_100ms);
		
		while (PowerModuleInfo[gun].OutputInfo.Cur >= DEF_OUTPUT_CUR)
        {
            OSTimeDly(SYS_DELAY_5ms);
            //��ʱ100ms
			
			if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_100ms) == BMS_TIME_OUT)
			{
				break;
			}
        }
#endif
        //�Ͽ�������DC�̵���K1\K2
        BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
        OSTimeDly(SYS_DELAY_1s);
		//�Ͽ�K1��K2���÷���CEM��Ҳ���÷���CST
		BMS_CONNECT_StepSet(gun,BMS_SEND_DEFAULT); 
    }
    SetBMSStopCharge(gun,FALSE);
    SetDealFaultOverState(gun,TRUE);
	return TRUE;
}


/*****************************************************************************
* Function      : APP_InsulationCheck
* Description   : ��Ե���
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_InsulationCheckGunA(void *pdata)
{
//���ݵ�·ԭ��ͼ����
#define	 ISO_R0   (double)(5*150000+2000)    //��ƽ�����R0
#define	 ISO_R1   (double)(5*15000+300)     //��ƽ�����R1
	
#define	 ISO_M   (double)(1.000 + ISO_R0/ISO_R1)    //(R1+R0)/R1  = 1+1/N
#define	 ISO_N   (double)(ISO_R1/ISO_R0)     		//R1/R0
#define  JY_RESERR		(50000u + 5000u)		    //��Ե��������ֵ ���ٷ�֮10

	INT16U modulevol = 0;
   INT8U err = BMS_STEP_DEFAULT;
	INT32U Vbus0 = 0,Viso0 = 0,Vbus1 = 0,Viso1 = 0,Vbus2 = 0,Viso2 = 0;
	INT32U Rp,Rn;			//��Ե������
		
	static _BSP_MESSAGE send_message[GUN_MAX];
	
    _BMS_BHM_CONTEXT BHMContext;
	_PRESET_VALUE		PresetValue_Tmp;
	_BSP_MESSAGE * pmsg;
	INT8U gun;
	if(pdata == NULL)
	{
		SetStartFailType(GUN_A,ENDFAIL_INSOLUTIONERR);	//��Ե���ʧ��
		ChargeErrDeal(GUN_A,ERR_LF_METHOD_B);
		SetBMSInsulationCheck(GUN_A,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
		printf("INSOLUTIONERR parameter1 err\r\n");
		 NB_WriterReason(GUN_A,"F17",3);
		return FALSE;
	}
	pmsg = pdata;
	gun = GUN_A;
	

	
    _SELF_CHECK_STATE *ptr = (_SELF_CHECK_STATE *)&SysState[gun].SelfCheckState.NoErr;
    

	if (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
	{//ͨ����1�;�2��������Щһ�ϵ�ͱպ��������صĳ���ȡ����Ե��� Test
		ptr->ContactorVol = STATE_ERR;
		ptr->NoErr = STATE_ERR;	
		SetStartFailType(gun,ENDFAIL_BATVOLTERR2);
		ChargeErrDeal(gun,ERR_LF_METHOD_B);
		printf("K1/K2 Batter Vol Large than 30V\r\n");
		SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
		 NB_WriterReason(gun,"F18",3);
		return FALSE;
	}
	
	if (GetRS_BatReverse(gun) == TRUE)
    {
       ptr->ContactorVol = STATE_ERR;
		ptr->NoErr = STATE_ERR;	
		SetStartFailType(gun,ENDFAIL_BATREVERSE);	//��ط���
		ChargeErrDeal(gun,ERR_LF_METHOD_B);
		SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
		 NB_WriterReason(gun,"F19",3);
		printf("��ط���\r\n");
		return FALSE;
    }
   


    //�˴�һ���Ƿ���Ϣ����Ե�������
    //һ����ֱ�ӵ��þ�Ե���ӿ�
    //(1)��Ե����·ON,�պϳ�����DC�̵���K1��K2
//    BSP_RLCOpen(BSPRLC_INSULATION); //��Ե����·ON
	printf("Insulation Check Begin\r\n");
#if 1
        BMSDemandInfo[gun].Vol = VAL_ADJUST(200, 10, 1);//Ĭ�ϵĻ���200����
        BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR; //5.1A
        send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_ENABLE;
		send_message[gun].GunNum = gun;  //ǹ��
		send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
  
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
        OSTimeDly(SYS_DELAY_100ms);
        PowerModuleInfo[gun].OutputInfo.Vol = 0;                   //������ϴ����û���㣬���¾�Ե����ѹû����
        //��ʱ20s��20���ڱ���Ҫ�ﵽ200V  ,    ����ģ�����л���ʱ���ȽϾã�֮ǰΪ10s��
		modulevol = 200;
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_30s);
		while ((PowerModuleInfo[gun].OutputInfo.Vol<= (BMSDemandInfo[gun].Vol-BHMCMP_VOLT))\
        ||(PowerModuleInfo[gun].OutputInfo.Vol>= (BMSDemandInfo[gun].Vol+BHMCMP_VOLT))) 
        {//ģ�������ѹ20s��û�����������ͳ�ʱ
            if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_30s) == BMS_TIME_OUT)
            {
                err = BMS_TIME_OUT;
                ptr->Insulation = STATE_ERR;   //��Ե������
                ptr->NoErr = STATE_ERR;
                SetStartFailType(gun,ENDFAIL_CHARGEMODULEERR);	
				NB_WriterReason(gun,"F20",3);				
                printf("20S TIMEOUT\r\n");
                break;
            }
			modulevol++;
			if(modulevol > 205)
			{
				modulevol = 200;
			}
			BMSDemandInfo[gun].Vol = VAL_ADJUST(modulevol, 10, 1);//Ĭ�ϵĻ���200����
			BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR; //5.1A
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			send_message[gun].DivNum = APP_POWER_ENABLE;
			send_message[gun].GunNum = gun;  //ǹ��
			send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
	  
			OSQPost(TaskPCU_pevent ,&send_message[gun]);
            OSTimeDly(SYS_DELAY_500ms);
        }
		if (err != BMS_TIME_OUT)
		{
			if (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
			{//�̵������������
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F21",3);		
				SetStartFailType(gun,ENDFAIL_OUTSWERR2);
//				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 7, 1);
	//            BSP_RLCOpen(BSPRLC_ERR_LED);
				printf("DCOUT ERR1\r\n");
					//֪ͨPCU����ֹͣ���
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
				send_message[gun].DivNum = APP_POWER_DISABLE;
				send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
				send_message[gun].GunNum = gun;  //ǹ��
				OSQPost(TaskPCU_pevent, &send_message[gun]);
				SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
				OSTimeDly(SYS_DELAY_50ms);
				return FALSE;
			}  
			else
			{
//				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 7, 0);
				//�պϳ�����DC�̵���K1��K2
				BSP_OpenK1K2(gun);				
				SetPowerDCState(gun,POWER_DC_CLOSE);
				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
				printf("k1K2 Open\r\n");
			}
		}
#endif
    //(2)��Ե����õ�ѹ���
    //����400V 5A����Ϣ��PCU���񣬿�ʼ�������
	 if (err != BMS_TIME_OUT)
	 {
		APP_GetBMSBHM(gun,&BHMContext);
		PresetValue_Tmp=APP_GetPresetValue();
		if (BHMContext.MaxChargeWholeVol <= DEF_OUTPUT_VOL)
		{
			BMSDemandInfo[gun].Vol = DEF_OUTPUT_VOL; //350V
		}
		else if (BHMContext.MaxChargeWholeVol <= PresetValue_Tmp.VolMaxOutput)
		{
			BMSDemandInfo[gun].Vol = BHMContext.MaxChargeWholeVol;
		}
		else
		{
			BMSDemandInfo[gun].Vol = VAL_ADJUST(500, 10, 1);//Ĭ�ϵĻ���500����
		}
		BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR; //5A
		send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_ENABLE;
		send_message[gun].GunNum = gun;  //ǹ��
		send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
		OSTimeDly(SYS_DELAY_100ms);
		printf("BMSDemandInfo.Vol = %d,BMSDemandInfo.Cur = %d\r\n",BMSDemandInfo[gun].Vol,BMSDemandInfo[gun].Cur);
		PowerModuleInfo[gun].OutputInfo.Vol = 0;                   //������ϴ����û���㣬���¾�Ե����ѹû����
		//��ʱ10s��10���ڱ���Ҫ�ﵽ300V
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_20s);

		while ((PowerModuleInfo[gun].OutputInfo.Vol<= (BMSDemandInfo[gun].Vol-BHMCMP_VOLT))\
			||(PowerModuleInfo[gun].OutputInfo.Vol>= (BMSDemandInfo[gun].Vol+BHMCMP_VOLT))) 
		{//ģ�������ѹ20s��û�����������ͳ�ʱ
			if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_20s) == BMS_TIME_OUT)
			{
				err = BMS_TIME_OUT;
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F22",3);		
				SetStartFailType(gun,ENDFAIL_CHARGEMODULEERR);		
				printf("20S TIMEOUT\r\n");
				printf("PowerModuleInfo[gun].OutputInfo.Vol =%d,BMSDemandInfo[gun].Vol =%d\r\n,PowerModuleInfo[gun].OutputInfo.Vol,BMSDemandInfo[gun].Vol");
				break;
			}
			OSTimeDly(SYS_DELAY_500ms);
		}
		
		if (err != BMS_TIME_OUT)
		{//ģ����20s�ڴﵽ�������ѹ
			//��Ե����ڴ˴�����  2020"
			//1����ȡVbus0,Viso0;
			Vbus0 = GetRM_DCOutVolt(gun)/10;
			Viso0 = GetJY_DCOutVolt(gun)/10;
			//2���պ�K300�ȴ�1s����ȡVbus1,Viso2;
			BSP_OpenPG1(gun);
			OSTimeDly(SYS_DELAY_1s);
			Vbus1 = GetRM_DCOutVolt(gun)/10;
			Viso1 = GetJY_DCOutVolt(gun)/10;
			//3���պ�K301�ȴ�1s����ȡVbus2,Viso2;
			BSP_OpenNG2(gun);
			OSTimeDly(SYS_DELAY_1s);
			Vbus2 = GetRM_DCOutVolt(gun)/10;
			Viso2 = GetJY_DCOutVolt(gun)/10;
			//4���Ͽ�K300��K301
		    BSP_ClosePG1(gun);
			BSP_CloseNG2(gun);
			printf("JY Vbus0 = %d,Viso0 = %d,JY Vbus1 = %d,Viso1 = %d,JY Vbus2 = %d,Viso2 = %d\r\n",Vbus0,Viso0,Vbus1,Viso1,Vbus2,Viso2);
			/******************�ж��Ƿ��쳣***************************/
			if(((Vbus0*100) > (Viso0*2*110)) || ((Vbus0*100) < (Viso0*2*90)))
			{
				//Vbus0 �� Viso0 * 2 ������ 10%ʱ�ж�λ��Ե����·�쳣��������ʱ��ѹ���պ�һ��
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F23",3);		
				SetStartFailType(gun,ENDFAIL_INSOLUTIONERR);	
                SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 1);				
				printf("��Ե����·����\r\n");
			}else if((Viso1 < 100) || (Viso1 > (Vbus1 - 200))){
				//����ĸ�߶Ե���ֵ��Сʱ���Ƚϵ�ѹ�������ݳ������Ա䶯��Ŀǰ�� С��10v����������ѹ-10v
				//С��10v����������ѹ-20v  500Vȥ�����������Ͽ��Բ�����30k�ĵ���  ��150K*5 + 2K||30K��/(150K*5 + 2K) Լ���� 25����  
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F24",3);		
				SetStartFailType(gun,ENDFAIL_INSOLUTIONERR);	
                SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 1);				
				printf("����ĸ�߶Ե���ֵ��С\r\n");
			}else{
			
				//�����Ե����Ƿ�����
				Rp = ISO_R0 *(((Vbus2 - Viso2)*Viso1 - (Vbus1 - Viso1)*Viso2) / \
				((Vbus1 - Viso1)*Viso2 + (ISO_M - 1) * Viso1 * Viso2- (Vbus2 - Viso2)*Viso1) );
				
				Rn = ISO_R0 *(((Vbus2 - Viso2)*Viso1 - (Vbus1 - Viso1)*Viso2) / \
				(ISO_M * (Vbus1 - Viso1)*Viso2 - (Vbus2 - Viso2)*Viso1) );
				
				printf("JY Rp = %d,Rn = %d\r\n",Rp,Rn);
				if((Rp < JY_RESERR) || (Rn < JY_RESERR) )
				{
					ptr->Insulation = STATE_ERR;   //��Ե������
					ptr->NoErr = STATE_ERR;
					NB_WriterReason(gun,"F25",3);		
					SetStartFailType(gun,ENDFAIL_INSOLUTIONERR);	
					SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 1);				
					printf("��Ե�����ϡ�\r\n");
				}
			}
			
		}
	}
    //(4)��Ե����ѹ���ֹͣ
    //֪ͨPCU����ֹͣ���
   send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 1;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
    OSTimeDly(SYS_DELAY_50ms);
    //��Ե����·OFF
//    BSP_RLCClose(BSPRLC_INSULATION);
    if(APP_GetGBType(gun) == BMS_GB_2011)
    {//�ɹ�����ܴ��ڽӴ���ֱ�ӱպϵ�����������ᵼ�°ѵ�صĵ������й��
     //(6)�Ͽ�������DC�̵���K1��K2
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
    } //#if (BMS_CUR_GB == BMS_OLD_GB2011)  //�ɹ��� 

    if(APP_GetGBType(gun) == BMS_GB_2015)  //�±�
    {
        BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_5s);
        //�ж�K1\K2�Ӵ�������ѹ�Ƿ�С��60v
        while (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1))
        {
            //��ʱ5��
            if (BMS_TIME_OUT == BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_5s))
            {
                ptr->LeakVol = STATE_ERR;
                ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F26",3);
				SetStartFailType(gun,ENDFAIL_LEAKOUTTIMEOUT);
                ChargeErrDeal(gun,ERR_LF_METHOD_A);
				printf("1113\r\n");
                break;
            }
            OSTimeDly(SYS_DELAY_100ms);
        }
        //(6)�Ͽ�������DC�̵���K1��K2
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
    }
    else
    {
        OSTimeDly(SYS_DELAY_1s); 
    }

    SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
    if (ptr->NoErr == STATE_ERR) //�д��󣬷��ش���
    {//�¹������й����ʱ��Ҫ��ĿǰΪ5s
		//ֻ��֪ͨǹ��Ԥ��
		 send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_DISABLE;
		send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
		send_message[gun].GunNum = gun;  //ǹ��
		OSQPost(TaskPCU_pevent, &send_message[gun]);
		OSTimeDly(SYS_DELAY_50ms);
        return FALSE;
    }
    else
    {
        ptr->NoErr = STATE_NO_ERR;
    }
    return TRUE;
}


/*****************************************************************************
* Function      : APP_InsulationCheck
* Description   : ��Ե���
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_InsulationCheckGunB(void *pdata)
{
//���ݵ�·ԭ��ͼ����
#define	 ISO_R0   (double)(5*150000+2000)    //��ƽ�����R0
#define	 ISO_R1   (double)(5*15000+300)     //��ƽ�����R1
	
#define	 ISO_M   (double)(1.000 + ISO_R0/ISO_R1)    //(R1+R0)/R1  = 1+1/N
#define	 ISO_N   (double)(ISO_R1/ISO_R0)     		//R1/R0
#define  JY_RESERR		(50000u + 5000u)		    //��Ե��������ֵ ���ٷ�֮10

   INT8U err = BMS_STEP_DEFAULT;
	INT32U Vbus0 = 0,Viso0 = 0,Vbus1 = 0,Viso1 = 0,Vbus2 = 0,Viso2 = 0;
	INT32U Rp,Rn;			//��Ե������
	INT16U	modulevol = 0;
	static _BSP_MESSAGE send_message[GUN_MAX];
	
    _BMS_BHM_CONTEXT BHMContext;
	_PRESET_VALUE		PresetValue_Tmp;
	_BSP_MESSAGE * pmsg;
	INT8U gun;
	if(pdata == NULL)
	{
		SetStartFailType(GUN_B,ENDFAIL_INSOLUTIONERR);	//��Ե���ʧ��
		ChargeErrDeal(GUN_B,ERR_LF_METHOD_B);
		SetBMSInsulationCheck(GUN_B,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
		printf("INSOLUTIONERR parameter1 err\r\n");
		NB_WriterReason(GUN_B,"F6",2);
		return FALSE;
	}
	pmsg = pdata;
	gun = GUN_B;
	

	
    _SELF_CHECK_STATE *ptr = (_SELF_CHECK_STATE *)&SysState[gun].SelfCheckState.NoErr;
    

	if (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
	{//ͨ����1�;�2��������Щһ�ϵ�ͱպ��������صĳ���ȡ����Ե��� Test
		ptr->ContactorVol = STATE_ERR;
		ptr->NoErr = STATE_ERR;	
		SetStartFailType(gun,ENDFAIL_BATVOLTERR2);
		ChargeErrDeal(gun,ERR_LF_METHOD_B);
		printf("K1/K2 Batter Vol Large than 30V\r\n");
		SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
		NB_WriterReason(gun,"F7",2);
		return FALSE;
	}
	
	if (GetRS_BatReverse(gun) == TRUE)
    {
       ptr->ContactorVol = STATE_ERR;
		ptr->NoErr = STATE_ERR;	
		SetStartFailType(gun,ENDFAIL_BATREVERSE);	//��ط���
		ChargeErrDeal(gun,ERR_LF_METHOD_B);
		SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
		printf("��ط���\r\n");
		NB_WriterReason(gun,"F8",2);
		return FALSE;
    }
   


    //�˴�һ���Ƿ���Ϣ����Ե�������
    //һ����ֱ�ӵ��þ�Ե���ӿ�
    //(1)��Ե����·ON,�պϳ�����DC�̵���K1��K2
//    BSP_RLCOpen(BSPRLC_INSULATION); //��Ե����·ON
	printf("Insulation Check Begin\r\n");
#if 1
        BMSDemandInfo[gun].Vol = VAL_ADJUST(200, 10, 1);//Ĭ�ϵĻ���200����
        BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR; //
        send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_ENABLE;
		send_message[gun].GunNum = gun;  //ǹ��
		send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
  
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
        OSTimeDly(SYS_DELAY_100ms);
        PowerModuleInfo[gun].OutputInfo.Vol = 0;                   //������ϴ����û���㣬���¾�Ե����ѹû����
        //��ʱ20s��20���ڱ���Ҫ�ﵽ200V  ,    ����ģ�����л���ʱ���ȽϾã�֮ǰΪ10s��
		modulevol = 200;
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_30s);
		while ((PowerModuleInfo[gun].OutputInfo.Vol<= (BMSDemandInfo[gun].Vol-BHMCMP_VOLT))\
        ||(PowerModuleInfo[gun].OutputInfo.Vol>= (BMSDemandInfo[gun].Vol+BHMCMP_VOLT))) 
        {//ģ�������ѹ20s��û�����������ͳ�ʱ
            if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_30s) == BMS_TIME_OUT)
            {
                err = BMS_TIME_OUT;
                ptr->Insulation = STATE_ERR;   //��Ե������
                ptr->NoErr = STATE_ERR;
                SetStartFailType(gun,ENDFAIL_CHARGEMODULEERR);	
				NB_WriterReason(gun,"F9",2);				
                printf("20S TIMEOUT\r\n");
                break;
            }
			modulevol++;
			if(modulevol > 205)
			{
				modulevol = 200;
			}
			BMSDemandInfo[gun].Vol = VAL_ADJUST(modulevol, 10, 1);//Ĭ�ϵĻ���200����
			BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR; //
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			send_message[gun].DivNum = APP_POWER_ENABLE;
			send_message[gun].GunNum = gun;  //ǹ��
			send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
	  
			OSQPost(TaskPCU_pevent ,&send_message[gun]);
            OSTimeDly(SYS_DELAY_500ms);
        }
		if (err != BMS_TIME_OUT)
		{
			if (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1)) 
			{//�̵������������
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				SetStartFailType(gun,ENDFAIL_OUTSWERR2);
//				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 7, 1);
	//            BSP_RLCOpen(BSPRLC_ERR_LED);
				printf("DCOUT ERR1\r\n");
					//֪ͨPCU����ֹͣ���
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
				send_message[gun].DivNum = APP_POWER_DISABLE;
				send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
				send_message[gun].GunNum = gun;  //ǹ��
				OSQPost(TaskPCU_pevent, &send_message[gun]);
				SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
				NB_WriterReason(gun,"F10",3);
				OSTimeDly(SYS_DELAY_50ms);
				return FALSE;
			}  
			else
			{
//				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 7, 0);
				//�պϳ�����DC�̵���K1��K2
				BSP_OpenK1K2(gun);				
				SetPowerDCState(gun,POWER_DC_CLOSE);
				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
				printf("k1K2 Open\r\n");
			}
		}
#endif
    //(2)��Ե����õ�ѹ���
    //����400V 5A����Ϣ��PCU���񣬿�ʼ�������
	 if (err != BMS_TIME_OUT)
	 {
		APP_GetBMSBHM(gun,&BHMContext);
		PresetValue_Tmp=APP_GetPresetValue();
		if (BHMContext.MaxChargeWholeVol <= DEF_OUTPUT_VOL)
		{
			BMSDemandInfo[gun].Vol = DEF_OUTPUT_VOL; //350V
		}
		else if (BHMContext.MaxChargeWholeVol <= PresetValue_Tmp.VolMaxOutput)
		{
			BMSDemandInfo[gun].Vol = BHMContext.MaxChargeWholeVol;
		}
		else
		{
			BMSDemandInfo[gun].Vol = VAL_ADJUST(500, 10, 1);//Ĭ�ϵĻ���500����
		}
		BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR; //5A
		send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_ENABLE;
		send_message[gun].GunNum = gun;  //ǹ��
		send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
		OSTimeDly(SYS_DELAY_100ms);
		printf("BMSDemandInfo.Vol = %d,BMSDemandInfo.Cur = %d\r\n",BMSDemandInfo[gun].Vol,BMSDemandInfo[gun].Cur);
		PowerModuleInfo[gun].OutputInfo.Vol = 0;                   //������ϴ����û���㣬���¾�Ե����ѹû����
		//��ʱ10s��10���ڱ���Ҫ�ﵽ300V
		BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_20s);

		while ((PowerModuleInfo[gun].OutputInfo.Vol<= (BMSDemandInfo[gun].Vol-BHMCMP_VOLT))\
			||(PowerModuleInfo[gun].OutputInfo.Vol>= (BMSDemandInfo[gun].Vol+BHMCMP_VOLT))) 
		{//ģ�������ѹ20s��û�����������ͳ�ʱ
			if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_20s) == BMS_TIME_OUT)
			{
				err = BMS_TIME_OUT;
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				SetStartFailType(gun,ENDFAIL_CHARGEMODULEERR);		
				printf("20S TIMEOUT\r\n");
				printf("PowerModuleInfo[gun].OutputInfo.Vol =%d,BMSDemandInfo[gun].Vol =%d\r\n,PowerModuleInfo[gun].OutputInfo.Vol,BMSDemandInfo[gun].Vol");
				NB_WriterReason(gun,"F11",3);
				break;
			}
			OSTimeDly(SYS_DELAY_500ms);
		}
		
		if (err != BMS_TIME_OUT)
		{//ģ����20s�ڴﵽ�������ѹ
			//��Ե����ڴ˴�����  2020"
			//1����ȡVbus0,Viso0;
			Vbus0 = GetRM_DCOutVolt(gun)/10;
			Viso0 = GetJY_DCOutVolt(gun)/10;
			//2���պ�K300�ȴ�1s����ȡVbus1,Viso2;
			BSP_OpenPG1(gun);
			OSTimeDly(SYS_DELAY_1s);
			Vbus1 = GetRM_DCOutVolt(gun)/10;
			Viso1 = GetJY_DCOutVolt(gun)/10;
			//3���պ�K301�ȴ�1s����ȡVbus2,Viso2;
			BSP_OpenNG2(gun);
			OSTimeDly(SYS_DELAY_1s);
			Vbus2 = GetRM_DCOutVolt(gun)/10;
			Viso2 = GetJY_DCOutVolt(gun)/10;
			//4���Ͽ�K300��K301
		    BSP_ClosePG1(gun);
			BSP_CloseNG2(gun);
			printf("JY Vbus0 = %d,Viso0 = %d,JY Vbus1 = %d,Viso1 = %d,JY Vbus2 = %d,Viso2 = %d\r\n",Vbus0,Viso0,Vbus1,Viso1,Vbus2,Viso2);
			/******************�ж��Ƿ��쳣***************************/
			if(((Vbus0*100) > (Viso0*2*110)) || ((Vbus0*100) < (Viso0*2*90)))
			{
				//Vbus0 �� Viso0 * 2 ������ 10%ʱ�ж�λ��Ե����·�쳣��������ʱ��ѹ���պ�һ��
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F12",3);
				SetStartFailType(gun,ENDFAIL_INSOLUTIONERR);	
                SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 1);				
				printf("��Ե����·����\r\n");
			}else if((Viso1 < 100) || (Viso1 > (Vbus1 - 200))){
				//����ĸ�߶Ե���ֵ��Сʱ���Ƚϵ�ѹ�������ݳ������Ա䶯��Ŀǰ�� С��10v����������ѹ-10v
				//С��10v����������ѹ-20v  500Vȥ�����������Ͽ��Բ�����30k�ĵ���  ��150K*5 + 2K||30K��/(150K*5 + 2K) Լ���� 25����  
				ptr->Insulation = STATE_ERR;   //��Ե������
				ptr->NoErr = STATE_ERR;
				NB_WriterReason(gun,"F13",3);
				SetStartFailType(gun,ENDFAIL_INSOLUTIONERR);	
                SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 1);				
				printf("����ĸ�߶Ե���ֵ��С\r\n");
			}else{
			
				//�����Ե����Ƿ�����
				Rp = ISO_R0 *(((Vbus2 - Viso2)*Viso1 - (Vbus1 - Viso1)*Viso2) / \
				((Vbus1 - Viso1)*Viso2 + (ISO_M - 1) * Viso1 * Viso2- (Vbus2 - Viso2)*Viso1) );
				
				Rn = ISO_R0 *(((Vbus2 - Viso2)*Viso1 - (Vbus1 - Viso1)*Viso2) / \
				(ISO_M * (Vbus1 - Viso1)*Viso2 - (Vbus2 - Viso2)*Viso1) );
				
				printf("JY Rp = %d,Rn = %d\r\n",Rp,Rn);
				if((Rp < JY_RESERR) || (Rn < JY_RESERR) )
				{
					ptr->Insulation = STATE_ERR;   //��Ե������
					ptr->NoErr = STATE_ERR;
					NB_WriterReason(gun,"F14",3);
					SetStartFailType(gun,ENDFAIL_INSOLUTIONERR);	
					SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 1);				
					printf("��Ե�����ϡ�\r\n");
				}
			}
			
		}
	}
    //(4)��Ե����ѹ���ֹͣ
    //֪ͨPCU����ֹͣ���
   send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
	send_message[gun].DivNum = APP_POWER_DISABLE;
	send_message[gun].DataLen = 1;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
	send_message[gun].GunNum = gun;  //ǹ��
	OSQPost(TaskPCU_pevent, &send_message[gun]);
    OSTimeDly(SYS_DELAY_50ms);
    //��Ե����·OFF
//    BSP_RLCClose(BSPRLC_INSULATION);
    if(APP_GetGBType(gun) == BMS_GB_2011)
    {//�ɹ�����ܴ��ڽӴ���ֱ�ӱպϵ�����������ᵼ�°ѵ�صĵ������й��
     //(6)�Ͽ�������DC�̵���K1��K2
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
    } //#if (BMS_CUR_GB == BMS_OLD_GB2011)  //�ɹ��� 

    if(APP_GetGBType(gun) == BMS_GB_2015)  //�±�
    {
        BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_5s);
        //�ж�K1\K2�Ӵ�������ѹ�Ƿ�С��60v
        while (GetRM_DCOutVolt(gun) >= VAL_ADJUST(100, 100, 1))
        {
            //��ʱ5��
            if (BMS_TIME_OUT == BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_5s))
            {
                ptr->LeakVol = STATE_ERR;
                ptr->NoErr = STATE_ERR;
				SetStartFailType(gun,ENDFAIL_LEAKOUTTIMEOUT);
                ChargeErrDeal(gun,ERR_LF_METHOD_A);
				NB_WriterReason(gun,"F15",3);
				printf("1113\r\n");
                break;
            }
            OSTimeDly(SYS_DELAY_100ms);
        }
        //(6)�Ͽ�������DC�̵���K1��K2
		BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
		SetPowerDCState(gun,POWER_DC_OPEN);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
    }
    else
    {
        OSTimeDly(SYS_DELAY_1s); 
    }

    SetBMSInsulationCheck(gun,FALSE);//���þ�Ե�����ɣ�������Ҫ��Ե���
    if (ptr->NoErr == STATE_ERR) //�д��󣬷��ش���
    {//�¹������й����ʱ��Ҫ��ĿǰΪ5s
		//ֻ��֪ͨǹ��Ԥ��
		 send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_DISABLE;
		send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
		send_message[gun].GunNum = gun;  //ǹ��
		OSQPost(TaskPCU_pevent, &send_message[gun]);
		OSTimeDly(SYS_DELAY_50ms);
        return FALSE;
    }
    else
    {
        ptr->NoErr = STATE_NO_ERR;
    }
    return TRUE;
}

/*****************************************************************************
* Function      : APP_BCPFitCheck
* Description   : ���BCP�����Ƿ����
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_BCPFitCheckGunA(void *pdata)
{
	 _BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_A;
    if (GetBMSBCPFitCheck(gun) == TRUE)
    {
//���BCP�����Ƿ����,�����Ǽ��ʲô???
        SysState[gun].BMSState.BCPFitState = STATE_NO_ERR;
        SetBMSBCPFitCheck(gun,FALSE);
    }
    return TRUE;
}


/*****************************************************************************
* Function      : APP_BCPFitCheck
* Description   : ���BCP�����Ƿ����
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_BCPFitCheckGunB(void *pdata)
{
	 _BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_B;
    if (GetBMSBCPFitCheck(gun) == TRUE)
    {
//���BCP�����Ƿ����,�����Ǽ��ʲô???
        SysState[gun].BMSState.BCPFitState = STATE_NO_ERR;
        SetBMSBCPFitCheck(gun,FALSE);
    }
    return TRUE;
}
/*****************************************************************************
* Function      : APP_ChargeReadyCheck
* Description   : ���׮����
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
  INT8U APP_ChargeReadyCheckGunA(void *pdata)
{
    INT32U vol;
    static _BSP_MESSAGE send_message[GUN_MAX];
	_PRESET_VALUE		PresetVal_Tmp;
	_BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	PresetVal_Tmp=APP_GetPresetValue();
    if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_A;
    if (GetChargeReady(gun) == READY_ING)
    {
        //"���DC�̵������ĵ�ѹ����?"
        //��ѹ�����ж�����:
        //(1):��ͨ�ű��ĵ�ص�ѹ���<=����5%
        //(2):�ڳ����������С�����ѹ֮��
		printf("Charge Ready Check\r\n");
        vol = GetRM_DCOutVolt(gun) / 10;//GetRM_DCOutVoltΪ��ص�ѹ
		//��ص�ѹ���ж� 2020"
		if (GetRS_BatReverse(gun) == TRUE)
		{
			SetStartFailType(gun,ENDFAIL_BATREVERSE);	//��ط���
			printf("��ط���\r\n");
			SetChargeReady(gun,READY_FAIL);
			return FALSE;
		}
		//2020Ԥ���ѹδδ�����Ƿ���Ҫ�����ӳ��"
		if(vol>1000)
		{//�����volȡ���ڳ������һ�������Ƿ�պϣ����û�бպϣ����vol���С
			if (vol >= BMS_BCP_Context[gun].BatteryPresentVol)
			{
				if (vol * 100 > BMS_BCP_Context[gun].BatteryPresentVol * 115)
				{
					OSTimeDly(SYS_DELAY_1s);  //�ȴ�1s�ٴλ�ȡ
					vol = GetRM_DCOutVolt(gun) / 10;//GetRM_DCOutVoltΪ��ص�ѹ
					if (vol * 100 > BMS_BCP_Context[gun].BatteryPresentVol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						SetChargeReady(gun,READY_FAIL);
						return FALSE;
					}
					if (BMS_BCP_Context[gun].BatteryPresentVol * 100 > vol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetChargeReady(gun,READY_FAIL);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						return FALSE;
					}
				}
			}
			else
			{
				if (BMS_BCP_Context[gun].BatteryPresentVol * 100 > vol * 115)
				{
					OSTimeDly(SYS_DELAY_1s);  //�ȴ�1s�ٴλ�ȡ
					vol = GetRM_DCOutVolt(gun) / 10;//GetRM_DCOutVoltΪ��ص�ѹ
					if (BMS_BCP_Context[gun].BatteryPresentVol * 100 > vol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetChargeReady(gun,READY_FAIL);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						return FALSE;
					}
					if (vol * 100 > BMS_BCP_Context[gun].BatteryPresentVol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						SetChargeReady(gun,READY_FAIL);
						return FALSE;
					}
				}
			}
		}
        //�жϵ�ѹ�Ƿ��׮��������ѹС
        if (vol > PresetVal_Tmp.VolMaxOutput)
        {
			printf("vol:%d\r\n",vol);
			SetChargeReady(gun,READY_FAIL);
			SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
            return FALSE;
        }  
		printf("Charge Ready Check Done\r\n");		
		OSTimeDly(SYS_DELAY_500ms); //���ܴ��ڿ��ĹصĶ�ͬʱ����20211116
        //�������������ѹ,ʹ�������ѹ�ȵ�ص�ѹ��(1V~10V)
        //������Ϣ��PCU���񣬲��õ�ص�ѹ��5A����Ԥ��
        BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR;
        BMSDemandInfo[gun].Vol = BMS_BCP_Context[gun].BatteryPresentVol - 50; //�ȵ�ص�ѹ��5V���
        //���������PCU���������
		send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_ENABLE;
		send_message[gun].GunNum = gun;  //ǹ��
		send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
  
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
		
        OSTimeDly(SYS_DELAY_10ms); //20211116
#if 1
		//�����д��ڿ���ģ���ѹ��֮ǰ�ľ�Ե����ѹ��Ŀǰ����ֻ����2��ǹͬʱ����������£���
		//��ʱ�������Ƿ���ģ���ѹ����BCP��ѹ�������·�һ�ιػ�ָ��
        BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_30s); 
		while(1)
		{
			if(PowerModuleInfo[gun].OutputInfo.Vol < BMS_BCP_Context[gun].BatteryPresentVol - VAL_ADJUST(20, 10, 1))
			{
				 OSTimeDly(SYS_DELAY_250ms);
			}
			else if(PowerModuleInfo[gun].OutputInfo.Vol > BMS_BCP_Context[gun].BatteryPresentVol + VAL_ADJUST(20, 10, 1))
			{
				//�����д��ڿ���ģ���ѹ��֮ǰ�ľ�Ե����ѹ��Ŀǰ����ֻ����2��ǹͬʱ����������£���
				//��ʱ�������Ƿ���ģ���ѹ����BCP��ѹ�������·�һ�ιػ�ָ��
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		        send_message[gun].DivNum = APP_POWER_DISABLE;
				send_message[gun].GunNum = gun;	
				send_message[gun].DataLen = 1;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��				
		         OSQPost(TaskPCU_pevent ,&send_message[gun]);
		        OSTimeDly(SYS_DELAY_2s);
				
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
				send_message[gun].DivNum = APP_POWER_ENABLE;
				send_message[gun].GunNum = gun;  //ǹ��
				send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
		  
				OSQPost(TaskPCU_pevent ,&send_message[gun]);
				OSTimeDly(SYS_DELAY_10ms);

				
			}else{
				break;
			}
			if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_30s) == BMS_TIME_OUT)
            {
            	//���������PCU���ر����
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
				send_message[gun].DivNum = APP_POWER_DISABLE;
				send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
				send_message[gun].GunNum = gun;			
				OSQPost(TaskPCU_pevent ,&send_message[gun]);
		         OSTimeDly(SYS_DELAY_1s);
                //�����ѹ����
                SysState[gun].SelfCheckState.LowVol = STATE_ERR;
                SysState[gun].SelfCheckState.NoErr = STATE_ERR;
                ChargeErrDeal(gun,ERR_LF_METHOD_C);
				printf("1111  vol = %d\r\n",PowerModuleInfo[gun].OutputInfo.Vol);
				SetStartFailType(gun,ENDFAIL_CHARGEMODULEERR);
				SetChargeReady(gun,READY_FAIL);
                return FALSE;
            }
		}
#endif
        if(APP_GetGBType(gun) == BMS_GB_2015)  //�±�
            BMS_BCP_Context[gun].MaxVoltage = BMS_BHM_Context[gun].MaxChargeWholeVol;//�ƺ�����BRM��BCP����
        //�պ�DC�̵���K1\K2
		BSP_CloseK1K2(gun);
		SetPowerDCState(gun,POWER_DC_CLOSE);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
        //��������
        SetChargeReady(gun,READY_SUCCESS);
		printf("Charger Ready Done\r\n");
    }

    return TRUE;
}


/*****************************************************************************
* Function      : APP_ChargeReadyCheck
* Description   : ���׮����
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
  INT8U APP_ChargeReadyCheckGunB(void *pdata)
{
    INT32U vol;
    static _BSP_MESSAGE send_message[GUN_MAX];
	_PRESET_VALUE		PresetVal_Tmp;
	_BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	PresetVal_Tmp=APP_GetPresetValue();
    if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_B;
    if (GetChargeReady(gun) == READY_ING)
    {
        //"���DC�̵������ĵ�ѹ����?"
        //��ѹ�����ж�����:
        //(1):��ͨ�ű��ĵ�ص�ѹ���<=����5%
        //(2):�ڳ����������С�����ѹ֮��
		printf("Charge Ready Check\r\n");
        vol = GetRM_DCOutVolt(gun) / 10;//GetRM_DCOutVoltΪ��ص�ѹ
		//��ص�ѹ���ж� 2020"
		if (GetRS_BatReverse(gun) == TRUE)
		{
			SetStartFailType(gun,ENDFAIL_BATREVERSE);	//��ط���
			printf("��ط���\r\n");
			SetChargeReady(gun,READY_FAIL);
			return FALSE;
		}
		//2020Ԥ���ѹδδ�����Ƿ���Ҫ�����ӳ��"
		if(vol>1000)
		{//�����volȡ���ڳ������һ�������Ƿ�պϣ����û�бպϣ����vol���С
			if (vol >= BMS_BCP_Context[gun].BatteryPresentVol)
			{
				if (vol * 100 > BMS_BCP_Context[gun].BatteryPresentVol * 115)
				{
					OSTimeDly(SYS_DELAY_1s);  //�ȴ�1s�ٴλ�ȡ
					vol = GetRM_DCOutVolt(gun) / 10;//GetRM_DCOutVoltΪ��ص�ѹ
					if (vol * 100 > BMS_BCP_Context[gun].BatteryPresentVol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						SetChargeReady(gun,READY_FAIL);
						return FALSE;
					}
					if (BMS_BCP_Context[gun].BatteryPresentVol * 100 > vol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetChargeReady(gun,READY_FAIL);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						return FALSE;
					}
				}
			}
			else
			{
				if (BMS_BCP_Context[gun].BatteryPresentVol * 100 > vol * 115)
				{
					OSTimeDly(SYS_DELAY_1s);  //�ȴ�1s�ٴλ�ȡ
					vol = GetRM_DCOutVolt(gun) / 10;//GetRM_DCOutVoltΪ��ص�ѹ
					if (BMS_BCP_Context[gun].BatteryPresentVol * 100 > vol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetChargeReady(gun,READY_FAIL);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						return FALSE;
					}
					if (vol * 100 > BMS_BCP_Context[gun].BatteryPresentVol * 115)
					{
						printf("vol:%d,BatteryPresentVol:%d\r\n",vol,BMS_BCP_Context[gun].BatteryPresentVol);
						SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
						SetChargeReady(gun,READY_FAIL);
						return FALSE;
					}
				}
			}
		}
        //�жϵ�ѹ�Ƿ��׮��������ѹС
        if (vol > PresetVal_Tmp.VolMaxOutput)
        {
			printf("vol:%d\r\n",vol);
			SetChargeReady(gun,READY_FAIL);
			SetStartFailType(gun,ENDFAIL_BATVOLTERR3);
            return FALSE;
        }  
		printf("Charge Ready Check Done\r\n");		
		OSTimeDly(SYS_DELAY_500ms); //���ܴ��ڿ��ĹصĶ�ͬʱ����20211116
        //�������������ѹ,ʹ�������ѹ�ȵ�ص�ѹ��(1V~10V)
        //������Ϣ��PCU���񣬲��õ�ص�ѹ��5A����Ԥ��
        BMSDemandInfo[gun].Cur = DEF_OUTPUT_CUR;
        BMSDemandInfo[gun].Vol = BMS_BCP_Context[gun].BatteryPresentVol - 50; //�ȵ�ص�ѹ��5V���
        //���������PCU���������
		send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		send_message[gun].DivNum = APP_POWER_ENABLE;
		send_message[gun].GunNum = gun;  //ǹ��
		send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
  
        OSQPost(TaskPCU_pevent ,&send_message[gun]);
		
        OSTimeDly(SYS_DELAY_10ms); //20211116
#if 1
		//�����д��ڿ���ģ���ѹ��֮ǰ�ľ�Ե����ѹ��Ŀǰ����ֻ����2��ǹͬʱ����������£���
		//��ʱ�������Ƿ���ģ���ѹ����BCP��ѹ�������·�һ�ιػ�ָ��
        BMS_TIMEOUT_ENTER(gun,BMS_OTH1_STEP, SYS_DELAY_30s); 
		while(1)
		{
			if(PowerModuleInfo[gun].OutputInfo.Vol < BMS_BCP_Context[gun].BatteryPresentVol - VAL_ADJUST(20, 10, 1))
			{
				 OSTimeDly(SYS_DELAY_250ms);
			}
			else if(PowerModuleInfo[gun].OutputInfo.Vol > BMS_BCP_Context[gun].BatteryPresentVol + VAL_ADJUST(20, 10, 1))
			{
				//�����д��ڿ���ģ���ѹ��֮ǰ�ľ�Ե����ѹ��Ŀǰ����ֻ����2��ǹͬʱ����������£���
				//��ʱ�������Ƿ���ģ���ѹ����BCP��ѹ�������·�һ�ιػ�ָ��
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
		        send_message[gun].DivNum = APP_POWER_DISABLE;
				send_message[gun].GunNum = gun;	
				send_message[gun].DataLen = 1;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��				
		         OSQPost(TaskPCU_pevent ,&send_message[gun]);
		        OSTimeDly(SYS_DELAY_2s);
				
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
				send_message[gun].DivNum = APP_POWER_ENABLE;
				send_message[gun].GunNum = gun;  //ǹ��
				send_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
		  
				OSQPost(TaskPCU_pevent ,&send_message[gun]);
				OSTimeDly(SYS_DELAY_10ms);

				
			}else{
				break;
			}
			if (BMS_TIMEOUT_CHECK(gun,BMS_OTH1_STEP, SYS_DELAY_30s) == BMS_TIME_OUT)
            {
            	//���������PCU���ر����
				send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
				send_message[gun].DivNum = APP_POWER_DISABLE;
				send_message[gun].DataLen = 0;     //0��ʾ��Ԥ��ģ��  1��ʾԤ��ģ��
				send_message[gun].GunNum = gun;			
				OSQPost(TaskPCU_pevent ,&send_message[gun]);
		         OSTimeDly(SYS_DELAY_1s);
                //�����ѹ����
                SysState[gun].SelfCheckState.LowVol = STATE_ERR;
                SysState[gun].SelfCheckState.NoErr = STATE_ERR;
                ChargeErrDeal(gun,ERR_LF_METHOD_C);
				printf("1111  vol = %d\r\n",PowerModuleInfo[gun].OutputInfo.Vol);
				SetStartFailType(gun,ENDFAIL_CHARGEMODULEERR);
				SetChargeReady(gun,READY_FAIL);
                return FALSE;
            }
		}
#endif
        if(APP_GetGBType(gun) == BMS_GB_2015)  //�±�
            BMS_BCP_Context[gun].MaxVoltage = BMS_BHM_Context[gun].MaxChargeWholeVol;//�ƺ�����BRM��BCP����
        //�պ�DC�̵���K1\K2
		BSP_CloseK1K2(gun);
		SetPowerDCState(gun,POWER_DC_CLOSE);
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
        //��������
        SetChargeReady(gun,READY_SUCCESS);
		printf("Charger Ready Done\r\n");
    }

    return TRUE;
}
/*****************************************************************************
* Function      : APP_ChargeRunning
* Description   : ��翪ʼ����Ҫ�ù���ģ���������
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_ChargeRunningGunA(void *pdata)
{
    static _BSP_MESSAGE  pcusend_message[GUN_MAX];
    _CCU_DEMAND_INFO DemandInfo1[GUN_MAX];
	_PRESET_VALUE	PresetValueTmp;
	INT16U cur;
	INT8U gun;
	_BSP_MESSAGE * pdispose = pdata;
	if(pdata == NULL)
	{
		return FALSE;
	}
	static _BSP_MESSAGE send_message[GUN_MAX];
	OS_EVENT* pdisevent =  APP_GetDispEvent();
	gun = GUN_A;
 
	PresetValueTmp=APP_GetPresetValue();
    
    if (GetChargeMode(gun) == MODE_MAN) //�ֶ�ģʽ
    {
        if (ManDemandInfo[gun].Vol > PresetValueTmp.VolMaxOutput)
        {
            //���ʧ�ܣ���������
            SysState[gun].SelfCheckState.Other = STATE_ERR;
            SysState[gun].SelfCheckState.NoErr = STATE_ERR;
			SetStartFailType(gun,ENDFAIL_OTHERERR);
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			NB_WriterReason(gun,"F4",2);
			send_message[gun].DivNum = APP_CHARGE_FAIL; //����ʧ��
			send_message[gun].GunNum = gun;
			OSQPost(pdisevent, &send_message[gun]);
            OSTimeDly(SYS_DELAY_10ms);
            SetBMSStartCharge(gun,FALSE);
			printf("Demand Vol Too Large\r\n");
            return FALSE;
        }
        else
        {
            if (ManDemandInfo[gun].Cur > PresetValueTmp.CurMaxOutput)
            {
                ManDemandInfo[gun].Cur = PresetValueTmp.CurMaxOutput; 
            }
            //֪ͨPCU�����������
            pcusend_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
            pcusend_message[gun].DivNum = APP_POWER_ENABLE;
            pcusend_message[gun].GunNum = gun;  //ǹ��
            pcusend_message[gun].pData = (INT8U *)(&ManDemandInfo[gun].Vol);
            OSQPost(TaskPCU_pevent, &pcusend_message[gun]);
            OSTimeDly(SYS_DELAY_10ms);
			
			
            //�Ѿ��ڳ���ˣ�˵���Ǹ���������
            if (GetChargeRunningState(gun) == TRUE)
            {
                return TRUE;
            }
            //�ֶ�ģʽ��Ҫ�պ�DC�̵���K1\K2
            BSP_OpenK1K2(gun);
			SetPowerDCState(gun,POWER_DC_CLOSE);
            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
            memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
                (INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
            SetChargeRunningState(gun,TRUE);
            //�����ɹ�
            send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
            send_message[gun].DivNum = APP_CHARGE_SUCCESS;
			send_message[gun].GunNum = gun;
            OSQPost(pdisevent, &send_message[gun]);
            OSTimeDly(SYS_DELAY_10ms);
        }
    }
    else
    {
        //����BMS�����PCU��������Ϣ
        //����ֵ����Ч��Ҳ���������
        DemandInfo1[gun].Cur = 4000 - BMS_BCL_Context[gun].DemandCur; 
		cur = 4000 - BMS_BCP_Context[gun].MaxCurrent;
		if(DemandInfo1[gun].Cur > cur)
		{
			DemandInfo1[gun].Cur = cur;
		}
        DemandInfo1[gun].Vol = BMS_BCL_Context[gun].DemandVol;


        BMSDemandInfo[gun].Vol = DemandInfo1[gun].Vol;
        BMSDemandInfo[gun].Cur = DemandInfo1[gun].Cur;
        if (BMSDemandInfo[gun].Cur > PresetValueTmp.CurMaxOutput)
        {
            BMSDemandInfo[gun].Cur = PresetValueTmp.CurMaxOutput; 
        }
        if (BMSDemandInfo[gun].Vol > PresetValueTmp.VolMaxOutput)
        {
            BMSDemandInfo[gun].Vol = PresetValueTmp.VolMaxOutput;
        }
		else if(BMSDemandInfo[gun].Vol<PresetValueTmp.VolMinOutput)
		{
			printf("AAAMINMIN,%x\r\n",BMSDemandInfo[gun].Vol);

            if(BMSDemandInfo[gun].Vol<=0)
			{
				if(APP_GetGBType(gun) == BMS_GB_2015)
				{
					BMSDemandInfo[gun].Vol=PresetValueTmp.VolMinOutput;
				}
				else
				{
					BMSDemandInfo[gun].Vol=2000;
				}
			}
			else
			{
				BMSDemandInfo[gun].Vol=PresetValueTmp.VolMinOutput;
                printf("AAAMINMIN2,%x\r\n",BMSDemandInfo[gun].Vol);
			}
		}
		
        //���������PCU����  
        if (GetChargeRunningState(gun) == TRUE)
        {
			
            //�Ѿ��ڳ���ˣ�ֱ��һ���Ե�λ
             pcusend_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
             pcusend_message[gun].DivNum = APP_POWER_ENABLE;
             pcusend_message[gun].GunNum = gun;    //ǹ��
             pcusend_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
            OSQPost(TaskPCU_pevent ,&pcusend_message[gun]);
            OSTimeDly(SYS_DELAY_20ms);
			//����������bug����������û�д� 20210627
			BSP_OpenK1K2(gun);
            return TRUE;
        }
        else 
        {//��δ��ʼ��磬���ý��ݵ����ķ�ʽ���ӵ���
            APP_BatChargeAdjust(gun,BMSDemandInfo[gun].Vol, BMSDemandInfo[gun].Cur, 3);
	
            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
            memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
                (INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
            SetChargeRunningState(gun,TRUE);
            //�����ɹ�
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
            send_message[gun].DivNum = APP_CHARGE_SUCCESS;
			send_message[gun].GunNum = gun;
            OSQPost(pdisevent, &send_message[gun]);
			SetBMSConnectStep(gun,BMS_CONNECT_RUNSUCCESS);
            OSTimeDly(SYS_DELAY_10ms);
			printf("Charge Begin\r\n");
        }
    }
	//����������bug����������û�д� 20210627
	BSP_OpenK1K2(gun);
	SetPowerDCState(gun,POWER_DC_CLOSE);
    //��������
    BSP_OpenRunLed(gun); 
    //��ȡ��ʼ����
    SysState[gun].ChargeInfo.StartTick = OSTimeGet();
    //��ȡ���ʱ��
    BSP_RTCGetTime((_BSPRTC_TIME *)&SysState[gun].ChargeInfo.StartTime.Second);
    SetBMSStopCharge(gun,FALSE);  //��ʼ���
    return TRUE;
}


/*****************************************************************************
* Function      : APP_ChargeRunning
* Description   : ��翪ʼ����Ҫ�ù���ģ���������
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��8��  Ҷϲ��
*****************************************************************************/
INT8U APP_ChargeRunningGunB(void *pdata)
{
    static _BSP_MESSAGE  pcusend_message[GUN_MAX];
    _CCU_DEMAND_INFO DemandInfo1[GUN_MAX];
	_PRESET_VALUE	PresetValueTmp;
	INT16U cur;
	INT8U gun;
	_BSP_MESSAGE * pdispose = pdata;
	if(pdata == NULL)
	{
		return FALSE;
	}

	static _BSP_MESSAGE send_message[GUN_MAX];
	OS_EVENT* pdisevent =  APP_GetDispEvent();
	gun = GUN_B;
 
	PresetValueTmp=APP_GetPresetValue();
    
    if (GetChargeMode(gun) == MODE_MAN) //�ֶ�ģʽ
    {
        if (ManDemandInfo[gun].Vol > PresetValueTmp.VolMaxOutput)
        {
            //���ʧ�ܣ���������
            SysState[gun].SelfCheckState.Other = STATE_ERR;
            SysState[gun].SelfCheckState.NoErr = STATE_ERR;
			SetStartFailType(gun,ENDFAIL_OTHERERR);
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			send_message[gun].DivNum = APP_CHARGE_FAIL; //����ʧ��
			NB_WriterReason(gun,"F30",3);
			send_message[gun].GunNum = gun;
			OSQPost(pdisevent, &send_message[gun]);
            OSTimeDly(SYS_DELAY_10ms);
            SetBMSStartCharge(gun,FALSE);
			printf("Demand Vol Too Large\r\n");
            return FALSE;
        }
        else
        {
            if (ManDemandInfo[gun].Cur > PresetValueTmp.CurMaxOutput)
            {
                ManDemandInfo[gun].Cur = PresetValueTmp.CurMaxOutput; 
            }
            //֪ͨPCU�����������
            pcusend_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
            pcusend_message[gun].DivNum = APP_POWER_ENABLE;
            pcusend_message[gun].GunNum = gun;  //ǹ��
            pcusend_message[gun].pData = (INT8U *)(&ManDemandInfo[gun].Vol);
            OSQPost(TaskPCU_pevent, &pcusend_message[gun]);
            OSTimeDly(SYS_DELAY_10ms);
			
			
            //�Ѿ��ڳ���ˣ�˵���Ǹ���������
            if (GetChargeRunningState(gun) == TRUE)
            {
                return TRUE;
            }
            //�ֶ�ģʽ��Ҫ�պ�DC�̵���K1\K2
            BSP_OpenK1K2(gun);
			SetPowerDCState(gun,POWER_DC_CLOSE);
            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
            memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
                (INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
            SetChargeRunningState(gun,TRUE);
            //�����ɹ�
            send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
            send_message[gun].DivNum = APP_CHARGE_SUCCESS;
			send_message[gun].GunNum = gun;
            OSQPost(pdisevent, &send_message[gun]);
            OSTimeDly(SYS_DELAY_10ms);
        }
    }
    else
    {
        //����BMS�����PCU��������Ϣ
        //����ֵ����Ч��Ҳ���������
        DemandInfo1[gun].Cur = 4000 - BMS_BCL_Context[gun].DemandCur; 
		cur = 4000 - BMS_BCP_Context[gun].MaxCurrent;
		if(DemandInfo1[gun].Cur > cur)
		{
			DemandInfo1[gun].Cur = cur;
		}
        DemandInfo1[gun].Vol = BMS_BCL_Context[gun].DemandVol;


        BMSDemandInfo[gun].Vol = DemandInfo1[gun].Vol;
        BMSDemandInfo[gun].Cur = DemandInfo1[gun].Cur;
        if (BMSDemandInfo[gun].Cur > PresetValueTmp.CurMaxOutput)
        {
            BMSDemandInfo[gun].Cur = PresetValueTmp.CurMaxOutput; 
        }
        if (BMSDemandInfo[gun].Vol > PresetValueTmp.VolMaxOutput)
        {
            BMSDemandInfo[gun].Vol = PresetValueTmp.VolMaxOutput;
        }
		else if(BMSDemandInfo[gun].Vol<PresetValueTmp.VolMinOutput)
		{
			printf("AAAMINMIN,%x\r\n",BMSDemandInfo[gun].Vol);

            if(BMSDemandInfo[gun].Vol<=0)
			{
				if(APP_GetGBType(gun) == BMS_GB_2015)
				{
					BMSDemandInfo[gun].Vol=PresetValueTmp.VolMinOutput;
				}
				else
				{
					BMSDemandInfo[gun].Vol=2000;
				}
			}
			else
			{
				BMSDemandInfo[gun].Vol=PresetValueTmp.VolMinOutput;
                printf("AAAMINMIN2,%x\r\n",BMSDemandInfo[gun].Vol);
			}
		}
		
        //���������PCU����  
        if (GetChargeRunningState(gun) == TRUE)
        {
			
            //�Ѿ��ڳ���ˣ�ֱ��һ���Ե�λ
             pcusend_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
             pcusend_message[gun].DivNum = APP_POWER_ENABLE;
             pcusend_message[gun].GunNum = gun;    //ǹ��
             pcusend_message[gun].pData = (INT8U *)&BMSDemandInfo[gun].Vol;
            OSQPost(TaskPCU_pevent ,&pcusend_message[gun]);
            OSTimeDly(SYS_DELAY_20ms);
			//����������bug����������û�д� 20210627
			BSP_OpenK1K2(gun);
            return TRUE;
        }
        else 
        {//��δ��ʼ��磬���ý��ݵ����ķ�ʽ���ӵ���
            APP_BatChargeAdjust(gun,BMSDemandInfo[gun].Vol, BMSDemandInfo[gun].Cur, 3);
	
            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
            memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
                (INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
            SetChargeRunningState(gun,TRUE);
            //�����ɹ�
			send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
            send_message[gun].DivNum = APP_CHARGE_SUCCESS;
			send_message[gun].GunNum = gun;
            OSQPost(pdisevent, &send_message[gun]);
			SetBMSConnectStep(gun,BMS_CONNECT_RUNSUCCESS);
            OSTimeDly(SYS_DELAY_10ms);
			printf("Charge Begin\r\n");
        }
    }
	//����������bug����������û�д� 20210627
	BSP_OpenK1K2(gun);
	SetPowerDCState(gun,POWER_DC_CLOSE);
    //��������
    BSP_OpenRunLed(gun); 
    //��ȡ��ʼ����
    SysState[gun].ChargeInfo.StartTick = OSTimeGet();
    //��ȡ���ʱ��
    BSP_RTCGetTime((_BSPRTC_TIME *)&SysState[gun].ChargeInfo.StartTime.Second);
    SetBMSStopCharge(gun,FALSE);  //��ʼ���
    return TRUE;
}
/*****************************************************************************
* Function      : APP_ChargePause
* Description   : �յ�BMS�����BMS֡����Ҫ��ͣ�����¿������
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��14��  Ҷϲ��
*****************************************************************************/
INT8U APP_ChargePauseGunA(void *pdata)
{
	static _BSP_MESSAGE send_message[GUN_MAX];
	_BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_A;
	//��Ҫ��ͣ���
	if (BMS_BSM_Context[gun].State1.OneByte.ChargeAllowBits == 0x00)
	{//��δ��ͣ���
		if (GetChargePauseState(gun) == FALSE)
		{//����ģ�����
            SetChargePauseState(gun,TRUE);
			 send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			send_message[gun].DivNum = APP_POWER_DISABLE;
			send_message[gun].GunNum = gun;  //ǹ��
			send_message[gun].DataLen = 1;   //1��ʾԤ��ģ��
			OSQPost(TaskPCU_pevent, &send_message[gun]);
		    OSTimeDly(SYS_DELAY_50ms);
			printf("Power Disable Message Send\r\n");
		}
	}
	return TRUE;
}


/*****************************************************************************
* Function      : APP_ChargePause
* Description   : �յ�BMS�����BMS֡����Ҫ��ͣ�����¿������
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��14��  Ҷϲ��
*****************************************************************************/
INT8U APP_ChargePauseGunB(void *pdata)
{
	static _BSP_MESSAGE send_message[GUN_MAX];
	_BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_B;
	//��Ҫ��ͣ���
	if (BMS_BSM_Context[gun].State1.OneByte.ChargeAllowBits == 0x00)
	{//��δ��ͣ���
		if (GetChargePauseState(gun) == FALSE)
		{//����ģ�����
            SetChargePauseState(gun,TRUE);
			 send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_CONTROL;
			send_message[gun].DivNum = APP_POWER_DISABLE;
			send_message[gun].GunNum = gun;  //ǹ��
			send_message[gun].DataLen = 1;   //1��ʾԤ��ģ��
			OSQPost(TaskPCU_pevent, &send_message[gun]);
		    OSTimeDly(SYS_DELAY_50ms);
			printf("Power Disable Message Send\r\n");
		}
	}
	return TRUE;
}

/*****************************************************************************
* Function      : APP_ChargeBSD
* Description   : �յ�BMS����ĳ����Ϣ����
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��14��  Ҷϲ��
*****************************************************************************/
INT8U APP_ChargeBSDGunA(void *pdata)
{
	_BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_A;
    SetChargeInfo(gun,6, pdata, sizeof(_BMS_BSD_CONTEXT));
    return TRUE;
}

/*****************************************************************************
* Function      : APP_ChargeBSD
* Description   : �յ�BMS����ĳ����Ϣ����
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��14��  Ҷϲ��
*****************************************************************************/
INT8U APP_ChargeBSDGunB(void *pdata)
{
	_BSP_MESSAGE * pdisposedata = pdata;
	INT8U gun;
	if (pdata == NULL)
	{
		return FALSE;
	}
	gun = GUN_B;
    SetChargeInfo(gun,6, pdata, sizeof(_BMS_BSD_CONTEXT));
    return TRUE;
}

/*****************************************************************************
* Function      : APP_AnalyzeRxData
* Description   : �����������յ��ĺ���
* Input         : INT8U divnum  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
static INT8U APP_AnalyzeRxDataA(_BSP_MESSAGE *pMsg)
{
    INT8U i;
    const _CONTROL_CMD *p = ControlCmdTableA;

    for (i = 0; i < CMD_TABLE_NUM; i++)
    {
        if (pMsg->DivNum == p[i].DivNum)
        {
            if (p[i].Fun != NULL)
            {
                return (p[i].Fun((void *)pMsg) );
            }
        }
    }
    return FALSE;
}


/*****************************************************************************
* Function      : APP_AnalyzeRxData
* Description   : �����������յ��ĺ���
* Input         : INT8U divnum  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
static INT8U APP_AnalyzeRxDataB(_BSP_MESSAGE *pMsg)
{
    INT8U i;
    const _CONTROL_CMD *p = ControlCmdTableB;

    for (i = 0; i < CMD_TABLE_NUM; i++)
    {
        if (pMsg->DivNum == p[i].DivNum)
        {
            if (p[i].Fun != NULL)
            {
                return (p[i].Fun((void *)pMsg) );
            }
        }
    }
    return FALSE;
}

/*****************************************************************************
* Function      : GetWorkState
* Description   : ��ȡ����״̬
* Input         : INT8U state  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��14��  Ҷϲ��
*****************************************************************************/
_WORK_STATE GetWorkState(_GUN_NUM gun,INT8U state)
{
    if (ERR_CHECK(state)) //���ϼ��
    {
        return WORK_STATE_ERR;
    }
    else if (GetChargeRunningState(gun) == TRUE)
    {
        //�Ѿ�������������ڹ���
        return WORK_STATE_RUN;
    }
    else //�����������
    {
        return WORK_STATE_IDLE;
    }
}

/*****************************************************************************
* Function      : CheckBSTErr
* Description   : ���BMS���ص�BST�Ƿ��д�
* Input         : INT8U *perr
* Output        : None
* Note(s)       :
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
INT8U CheckBSTErr(INT8U gun,INT8U *perr)
{

	if(gun >= GUN_MAX)
	{
		return FALSE;
	}
	APP_Set_ERR_Branch(gun,STOP_BSMNORMAL); //��ʼ��Ϊ����ֹͣ
   
                    //����SOCĿ��ֵ���˴���û�и澯
	if ( (BMS_BST_Context[gun].State0.OneByte.SocTargetBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State0.OneByte.TolVolValBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State0.OneByte.VolSetValBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State0.OneByte.ChargeStopBits == STATE_TROUBLE) )
	{
		SysState[gun].StopReason.BMSStop.State0.EndChargeReason \
			= BMS_BST_Context[gun].State0.EndChargeReason;
		//BMS��ֹ����ԭ��
		SysState[gun].TotalErrState	|= ERR_BMSSTOP_TYPE;
		APP_Set_ERR_Branch(gun,STOP_BSMNORMAL);
		//�ﵽ����
		*perr |= STOP_CONDITION;
		printf("11146\r\n");
	}

	
	if ( (BMS_BST_Context[gun].State1.OneByte.InsulationErrBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.ConnectorOverTempBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.ElementOverTempBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.ConnectorErrBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.BatOverTempBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.HighPresRelayBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.TPTwoVolErrBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State1.OneByte.OtherErrBits == STATE_TROUBLE) )
	{
		if(BMS_BST_Context[gun].State1.OneByte.InsulationErrBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_BSTINSULATIONERR);
		}
		else if (BMS_BST_Context[gun].State1.OneByte.ConnectorOverTempBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_BSTSWOVERT);
		}
		else if(BMS_BST_Context[gun].State1.OneByte.ElementOverTempBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_BSTELOVERT);
		}
		else if(BMS_BST_Context[gun].State1.OneByte.ConnectorErrBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_CERR);
		}
		else if(BMS_BST_Context[gun].State1.OneByte.BatOverTempBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_BSTBATOVERT);
		}
		else if(BMS_BST_Context[gun].State1.OneByte.TPTwoVolErrBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_BSTTPTWO);
		}
		else if(BMS_BST_Context[gun].State1.OneByte.HighPresRelayBits == STATE_TROUBLE)
		{
			APP_Set_ERR_Branch(gun,STOP_BSTHIGHRLCERR);
		}
		else
		{
			APP_Set_ERR_Branch(gun,STOP_BSTOTHERERR);
		}
		SysState[gun].StopReason.BMSStop.State1.EndChargeTroubleReason \
			= BMS_BST_Context[gun].State1.EndChargeTroubleReason;
	
		//BMS��ֹ����ԭ��
		SysState[gun].TotalErrState	|= ERR_BMSSTOP_TYPE;
	//	APP_Set_ERR_Branch(STOP_BSMERR);
		//������ֹ
		*perr |= STOP_ERR; 
		printf("11145\r\n");
	}

	if ( (BMS_BST_Context[gun].State2.OneByte.OverCurErrBits == STATE_TROUBLE) \
	   || (BMS_BST_Context[gun].State2.OneByte.OverVolErrBits == STATE_TROUBLE) )
	{
		SysState[gun].StopReason.BMSStop.State2.EndChargeErroReason \
			= BMS_BST_Context[gun].State2.EndChargeErroReason;
		//BMS��������
		if (BMS_BST_Context[gun].State2.OneByte.OverCurErrBits)
		{
			//ֱ��ĸ�߹����澯1
			APP_Set_ERR_Branch(gun,STOP_OUTCURROVER);
			SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 7, 1);
		}
		else
		{
			//ֱ��ĸ�߲������澯0
			SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 7, 0);
		}
		//BMS��ѹ����
		if (BMS_BST_Context[gun].State2.OneByte.OverVolErrBits)
		{
			//ֱ��ĸ�������ѹ�澯1
         	APP_Set_ERR_Branch(gun,STOP_OUTVOLTVORE);  //��ѹ�쳣
			SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 5, 1);
		}
		else
		{
			//ֱ��ĸ���������ѹ�澯0
			SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 5, 0);
		}

		//BMS��ֹ����ԭ��
		SysState[gun].TotalErrState	|= ERR_BMSSTOP_TYPE;
//		APP_Set_ERR_Branch(STOP_BSMNORMAL);
		//������ֹ
		*perr |= STOP_ERR; 
		printf("11144\r\n");
	}

    return TRUE;
}

/*****************************************************************************
* Function      : APP_CheckPowerModule
* Description   : �ж�ģ���Ƿ��й���
* Input         : _BMS_CST_CONTEXT* const ptr  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��12��  Ҷϲ��
*****************************************************************************/
INT8U APP_CheckPowerModule(INT8U gun,_BMS_CST_CONTEXT* const ptr, INT8U *perr)
{
//    INT8U i;
//    static INT8U oldState[3] =  {0};
//    static INT8U oldState3 = 0;
// 

//	if(gun >= GUN_MAX)
//	{
//		return FALSE;
//	}
//    _APP_CHARGE_STATE2 *p2 = &PowerModuleInfo[gun].TotalState;
//	_MODULE_STATE *pstate = &PowerModuleInfo[gun].TotalState.ModuleState;
//	INT8U *p 				= &PowerModuleInfo[gun].TotalState.ModuleState.State1.allbits;
//	if ( (ptr == NULL) || (perr == NULL)  )
//    {
//        return FALSE;
//    }
//    
//  
//    for (i = 0; i < sizeof(_MODULE_STATE); i++)
//    {//�ж�ģ���3���ֽ�״̬�Ƿ���1
//        if ( oldState[i] != *(p + i) || (*(p + i)))
//        {
//            oldState[i] = *(p + i); 
//            switch (i)
//            {
//                case 0:
////                    if ( *(p + i) & 0x40) 
////                    {//ģ�������·
////                        ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;		//CST������Ϊ���ܲ��ܴ���
////                        SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;			//ֹͣԭ��Ϊ������ֹ����
////						APP_Set_ERR_Branch(gun,STOP_SHORTCIRCUIT);
////                        *perr |= STOP_ERR; 										//����ԭ��Ϊ����ֹͣ
////						printf("11119\r\n");
////                        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 5, 1);	//����ģ��ֱ�������·����1
////                    }
////                    else
////                    {//����ģ��ֱ�������·�޹���0
////                        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 5, 0);
////                    }
//                    if ( *(p + i) & 0xBE) //BE-10111110
//                    {//����ģ�����
//                        if (pstate->State1.onebits.Mod_Alarm)
//                        {//ģ����ϸ澯״̬
//#if 0 //ģ����״̬��������
//                            //CST������Ϊ���ܲ��ܴ���
//                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("11111119\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#endif
//                        }
//                        
//                        if (pstate->State1.onebits.Protected)
//                        {//ģ�鱣����ѹ�澯״̬
//#if 0
//                            //CST������Ϊ���ܲ��ܴ���
//                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("11111110\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#endif
//                        }
//                        
//                        if (pstate->State1.onebits.Tem_Over)
//                        {//����ģ����¸澯,��ֵΪ100��
//#if (MODULE_WARNING_SHUTDOWN > 0u)
//                            //CST������Ϊ�����ڲ����¹���
//                            ptr->State1.OneByte.ChargeOverTempInBits = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("1111111910\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#else
//                            //CST������Ϊ�����ڲ����¹���
//                            ptr->State1.OneByte.ChargeOverTempInBits = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            //SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//                            //����״̬������ֹͣ���
//                            *perr |= STOP_WARN; 
//							printf("11117\r\n");
//#endif
//                            //���ģ����¸澯1
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 1, 1);
//                        }
//                        else
//                        {//���ģ���޹��¸澯0
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 1, 0);
//                        }
//                    
//                        if (pstate->State1.onebits.Volt_Over)
//                        { //����ģ�������ѹ�澯״̬
//#if (MODULE_WARNING_SHUTDOWN > 0u)
//                            //CST�����ó���ѹ��ѹ
//                            ptr->State2.OneByte.OverVolErrBits = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("111111199\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#else
//                            //����״̬������ֹͣ���
//                            *perr |= STOP_WARN; 
//#endif
//                            //����ģ�������ѹ1
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 1);
//                        }
//                        else
//                        {//����ģ���������ѹ0
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 0);
//                        }
//                        
////                        if (pstate->State1.onebits.Commu_Aarm)
////                        {//����ģ��ͨѶ�ж�
////                            //CST������Ϊ���ܲ��ܴ���
////                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
////                            //ֹͣԭ��Ϊ������ֹ����
////                            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;
////							APP_Set_ERR_Branch(gun,STOP_CHARGEMODULEERR);  //����ģ�����
////                            //����ԭ��Ϊ����ֹͣ
////                            *perr |= STOP_ERR; 
////							printf("11116\r\n");							
////                            //���ģ��ͨ�Ÿ澯1
////                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 2, 1);
////                        }
////                        else
////                        {//���ģ����ͨ�Ÿ澯0
////                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 2, 0);
////                        }
//                    }
//                    break;
//                case 1:
//                    if ( *(p + i) & 0x7F)//7F-01111111
//                    {
//                        if (pstate->State2.onebits.Power_Limit)
//                        {//ģ�鴦�ڹ�������״̬
//#if (MODULE_WARNING_SHUTDOWN > 0u)
//                            //CST������Ϊ���ܲ��ܴ���
//                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("11111118\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#else
////                            //����״̬������ֹͣ���
////                            *perr |= STOP_WARN; 
//#endif
//                        }
//                        
////                        if (pstate->State2.onebits.ID_Repeat)
////                        {//ģ��ID�ظ�
////                            //CST������Ϊ���ܲ��ܴ���
////                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
////                            //ֹͣԭ��Ϊ������ֹ����
////                            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;
////							APP_Set_ERR_Branch(gun,STOP_CHARGEMODULEERR);
////                            //����ԭ��Ϊ����ֹͣ
////                            *perr |= STOP_ERR; 
////							printf("11115\r\n");
////                            //���ģ�����1
////                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 0, 1);
////                        }
////                        else
////                        {//���ģ���޹���0
////                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 0, 0);
////                        }
//                        
//                        if (pstate->State2.onebits.Current_Unbalance)
//                        {//ģ�����ز�����
//#if (MODULE_WARNING_SHUTDOWN > 0u)
//                            //CST������Ϊ���ܲ��ܴ���
//                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("111111197\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#else
////                            //����״̬������ֹͣ���
////                            *perr |= STOP_WARN; 
//#endif
//                        }
//                        
//                        if (pstate->State2.onebits.AC_UVP)
//                        {//ģ�齻������Ƿѹ�澯������Ƿѹ��Ϊ255V
//#if (MODULE_WARNING_SHUTDOWN > 0u)
//                            //CST������Ϊ���ܲ��ܴ���
//                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("111111196\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#else
//                            //CST������Ϊ���ܲ��ܴ���
////                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
////                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//                            //����״̬������ֹͣ���
//                            *perr |= STOP_WARN; 
//#endif
//                            //ģ�齻������Ƿѹ�澯1
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 3, 1);
//                        }
//                        else
//                        {
//                            //ģ�齻��������Ƿѹ�澯0
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 3, 0);
//                        }
//						
//                        if (pstate->State2.onebits.AC_Over)
//                        {//ģ�齻�������ѹ�澯,��ѹ��Ϊ535V
//#if (MODULE_WARNING_SHUTDOWN > 0u)
//                            //CST������Ϊ���ܲ��ܴ���
//                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//                            //ֹͣԭ��Ϊ������ֹ����
//                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//							printf("111111195\r\n");
//                            //����ԭ��Ϊ����ֹͣ
//                            *perr |= STOP_ERR; 
//#else
//                            //CST������Ϊ���ܲ��ܴ���
////                            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
////                            //ֹͣԭ��Ϊ������ֹ����
////                            SysState.TotalErrState |= ERR_CHARGESTOP_TYPE;
//                            //����״̬������ֹͣ���
//                            *perr |= STOP_WARN; 
//#endif
//                            //ģ�齻�������ѹ�澯1
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 2, 1);
//                        }
//                        else
//                        {
//                            //ģ���޽��������ѹ�澯0
//                            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 2, 0);
//                        }
//                    }
//                    break;
//            }
//        }
//    }
//	
//	#if (MODULE_WARNING_SHUTDOWN > 0u)   //20220812    ��ģ����
//    if ((p2->State3.AllBits != oldState3) || (p2->State3.AllBits))
//    {//�й���
//        oldState3 = p2->State3.AllBits;
//        if (p2->State3.OneByte.InACOverVolBit)
//        {//ģ�������ѹ�쳣�澯
//            //CST������ܲ��ܴ������
//            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//            //ֹͣԭ��Ϊ������ֹ����
//            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;
//			APP_Set_ERR_Branch(gun,STOP_ACINERR);
//            //����ԭ��Ϊ����ֹͣ
//            *perr |= STOP_ERR; 
//			printf("11114\r\n");
////			*perr |= STOP_WARN; 
//            //���������ѹ�澯
//            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 3, 1);
//        }
//        //ģ�鱾���ظ����չ����������������ѹ��û��ѹ����ȥ������
//        else
//        {//���������ѹ�澯
//            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 3, 0);
//        }
//        
//        if (p2->State3.OneByte.InACUnderVolBit)
//        {//ģ������Ƿѹ�澯(�Զ��巧ֵ)
//            //CST������ܲ��ܴ������
//            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
//            //ֹͣԭ��Ϊ������ֹ����
//            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;
//			APP_Set_ERR_Branch(gun,STOP_ACINERR);
//            //����ԭ��Ϊ����ֹͣ
//            *perr |= STOP_ERR;  
//			printf("11113\r\n");
////			*perr |= STOP_WARN; 			
//            //��������Ƿѹ�澯
//            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 4, 1);
//        }
//        //ģ�鱾���ظ����չ����������������ѹ��ûǷѹ����ȥ������
//        else
//        {
//            //��������Ƿѹ�澯
//            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 4, 0);
//        }
////        if(p2->State3.OneByte.RevBits)
////        {
////            ptr->State1.OneByte.PowerUnreach = STATE_TROUBLE;
////            //ֹͣԭ��Ϊ������ֹ����
////            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;
////			APP_Set_ERR_Branch(gun,STOP_CHARGEMODULEERR);
////            //����ԭ��Ϊ����ֹͣ
////            *perr |= STOP_ERR;  
////			printf("11113\r\n");
////        }
//    }
//	#endif
    
    return TRUE;
}


/*****************************************************************************
* Function      : APP_MonitorChargingState
* Description   : ��س������е�״̬
* Input         : void  
* Output        : None
* Note(s)       : static
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
static void APP_MonitorChargingState(INT8U gun)
{    
    INT8U 	err;
	INT32U	Vol_Judge=0;
	INT16U ACVol;
    _STOP_TYPE stoptype = STOP_UNDEF;
	_PRESET_VALUE	PresetVal_Temp;
 	static INT32U curtime[GUN_MAX] = {0},lastcurtime[GUN_MAX] = {0},voltime[GUN_MAX] = {0},lastvoltime[GUN_MAX] = {0};
    static _BSP_MESSAGE send_message[GUN_MAX];  
	static INT32U	LastTotalPower[GUN_MAX] = {0},powertime[GUN_MAX] = {0},powerlasttime[GUN_MAX] = {0};
	if(gun >= GUN_MAX)
	{
		return;
	}
    //ptrָ�����ֹͣԭ��CST
    _BMS_CST_CONTEXT* const ptr = (_BMS_CST_CONTEXT *)(&SysState[gun].StopReason.ChargeStop.State0.EndChargeReason);
    
    if (GetGunState(gun) == GUN_DISCONNECTED)
    {//(1)ǹ�γ�
        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
        {
           if((GetChargeMode(gun) == MODE_AUTO) || (GetChargeMode(gun) == MODE_VIN))
            {
                BSP_CloseK1K2(gun);					//�Ͽ�����Ӵ���K1\K2
				SetPowerDCState(gun,POWER_DC_OPEN);
                SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
               BSP_CloseBMSPower(gun);
                ptr->State1.OneByte.OtherErrBits = STATE_TROUBLE; //CST��û��ǹ�γ����ֶΣ��������������ֶ�
                SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;    //ֹͣԭ��Ϊ������ֹ����
					NB_WriterReason(gun,"E18",3);
				APP_Set_ERR_Branch(gun,STOP_HANDERR);
                stoptype |= STOP_ERR;							  //����ԭ��Ϊ����ֹͣ
				printf("51\r\n");
            }
        }
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 5, 1);   //ǹδ����
    }
    else
    {//ǹ������
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 5, 0);
    }
    
    if (GetEmergencyState(gun) == EMERGENCY_PRESSED)
    {//(2)��ͣ��ť�Ƿ���
        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
        {//��ͣ����
            ptr->State1.OneByte.UrgentStopBits = STATE_TROUBLE;
            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE; 		//ֹͣԭ��Ϊ������ֹ����
			NB_WriterReason(gun,"E17",3);
			SetStartFailType(gun,ENDFAIL_EMERGENCY);
			APP_Set_ERR_Branch(gun,STOP_EMERGENCY);
            stoptype |= STOP_ERR;								//����ԭ��Ϊ����ֹͣ
			printf("52\r\n");
        }
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 4, 1);	//��ͣ����0
    }
    else
    {//��ͣδ����1
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 4, 0);
    }

    //(3)������û��ס����������ʱ�������ʵ���ϲ�����������Ͳ��жϵ������Ĭ�ϵ��������
    if (GetElecLockState(gun) == ELEC_LOCK_UNLOCKED)
    {
		//�ٳ���ж��Ҳ���ֹͣ��
        if (GetChargeRunningState(gun) == TRUE && (GetBMSStopCharge(gun) == FALSE))
        {
        //�����Զ�ģʽ���жϵ����״̬
           if((GetChargeMode(gun) == MODE_AUTO) || (GetChargeMode(gun) == MODE_VIN))
            {//������û��ס��CST�����е���������
                ptr->State1.OneByte.OtherErrBits = STATE_TROUBLE;
                SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
					NB_WriterReason(gun,"E16",3);
				SetStartFailType(gun,ENDFAIL_ELECLOCKERR);
				APP_Set_ERR_Branch(gun,STOP_ELECLOCKERR);
                stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
				printf("53\r\n");
				//����������
				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 1, 1);
            }
        }
        //���ӿڵ�����״̬1
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 6, 0);
    }
    else
    {//��������������
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 1, 0);
        //���ӿڵ�����״̬1
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 6, 1);
    }
	if (GetGunState((_GUN_NUM)gun) == GUN_DISCONNECTED)
   {//ǹδ������״̬	
		SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 4, 0);//���̻��Ĵ������û�ȷ�Ϻ����
		SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_2, 5, 0);//���̻��Ĵ������û�ȷ�Ϻ����
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 2, 0);//���̻��Ĵ������û�ȷ�Ϻ����
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 2, 0);//ģ������Ƿѹ���
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 5, 0);//ģ�������·���
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 6, 0);//ģ���������
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 0);//ģ�������ѹ
		SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 7, 0);//����
	   	SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 5, 0);//��ѹ�쳣
	//	SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_1, 7, 0);   //�̵���ճ����ǹ���   20211118
    }
	PresetVal_Temp=APP_GetPresetValue();
	Vol_Judge = PowerModuleInfo[gun].OutputInfo.Vol;    //�ֳ����г���ֱ����ѹ��ѹ����   20210909 Ҷ

	if(Vol_Judge < 7600)   //����С��760V������760V���ܴ���У׼������
	{
		if (Vol_Judge > (PresetVal_Temp.Therold_Value.OverOutVolTherold + 200))
		{//(5)�ж�ģ�鷵�ص������ѹ�Ƿ�������1��ģ���������߷�ֵ��2���̵����󼶵�ѹ���ص�ѹ
			if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
			{//�����ѹ����
				ptr->State2.OneByte.OverVolErrBits = STATE_TROUBLE;
				SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
				NB_WriterReason(gun,"E15",3);
				APP_Set_ERR_Branch(gun,STOP_OUTVOLTVORE);
				SetStartFailType(gun,ENDFAIL_OUTVOLTVORE);
				stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
				printf("55,%x\r\n",Vol_Judge);
			}
			SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 1);	//ֱ��ĸ�������ѹ�澯1
		}
	}
	
	
	Vol_Judge = GetRM_DCOutVolt(gun) /10;			//��ģ���ѹ ���ױ�E14����   20240306
   if ( (Vol_Judge > (BMS_BCP_Context[gun].MaxVoltage + 600)) \
        && (BMS_BCP_Context[gun].MaxVoltage >= 3000) )       //��������е����bcp������ѹ�����ʱ����е����⣬�ʷ�Χ�Ӵ�
    {
        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
        {//�����ѹ����
            ptr->State2.OneByte.OverVolErrBits = STATE_TROUBLE;
            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
			NB_WriterReason(gun,"E14",3);
			APP_Set_ERR_Branch(gun,STOP_OUTVOLTVORE);
			SetStartFailType(gun,ENDFAIL_OUTVOLTVORE);
            stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
			printf("56,%x,%x\r\n",Vol_Judge,BMS_BCP_Context[gun].MaxVoltage);
        }
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 1);	//ֱ��ĸ�������ѹ�澯1
    }
    else 
    {
        if (Vol_Judge <= PresetVal_Temp.Therold_Value.OverOutVolTherold)
        {//ֱ��ĸ���������ѹ�澯0
            SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 0);
        }
    }
	
	
	//�������Ե�ʱ����ж������Ƿѹ
	//���������ѹ�ж�
	ACVol = GetModuleReturnACVol();
	if ((ACVol > (PresetVal_Temp.Therold_Value.OverACVolTherold)) && (ACVol < 6000) )  //��AC�� 20211021
    {//(6)�ж�ģ�鷵�ص���������Ƿ�����
        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
        {//�����ѹ��ѹ
            ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;
            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
				NB_WriterReason(gun,"E13",3);
			APP_Set_ERR_Branch(gun,STOP_ACINERR);
			SetStartFailType(gun,ENDFAIL_ACINERR);
            stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
			printf("ac57\r\n");
        }
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 3, 1);	//�����ѹ
    }
    else
    {//ֱ��ĸ������������澯0
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 3, 0);
    }
	if ((ACVol < (PresetVal_Temp.Therold_Value.UnderACVolTherold)) && (ACVol < 6000) )   //��AC�� 20211021
    {//(6)�ж�ģ�鷵�ص���������Ƿ�����
        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
        {//�����ѹǷѹ
            ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;
            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
				NB_WriterReason(gun,"E12",3);
			APP_Set_ERR_Branch(gun,STOP_ACINERR);
			SetStartFailType(gun,ENDFAIL_ACINERR);
            stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
			printf("ac57\r\n");
        }
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 4, 1);	//����Ƿѹ
    }
    else
    {//ֱ��ĸ������������澯0
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_3, 4, 0);
    }
	
	
	
	
	if(BCURemoteSignal[gun].state1.State.DCContactor_Err)   //ֱ���Ӵ���ճ��
	{
		 if((GetChargeMode(gun) == MODE_AUTO) || (GetChargeMode(gun) == MODE_VIN))
		 {
			//ֻҪ��һ��ճ����2��ǹ��Ҫֹͣ���
			 if (!( (GetBMSStartCharge(GUN_A) == FALSE) && (GetBMSStopCharge(GUN_A) == FALSE) ) )
			{//ֱ���Ӵ���ճ��
				ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;
				SysState[GUN_A].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
				NB_WriterReason(GUN_A,"E63",3);
				APP_Set_ERR_Branch(GUN_A,STOP_OUTSWERR2);
				SetStartFailType(GUN_A,ENDFAIL_OUTSWERR2);
				stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
				printf("dc57\r\n");
			}
			if (!( (GetBMSStartCharge(GUN_B) == FALSE) && (GetBMSStopCharge(GUN_B) == FALSE) ) )
			{//ֱ���Ӵ���ճ��
				ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;
				SysState[GUN_B].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
				NB_WriterReason(GUN_B,"E64",3);
				APP_Set_ERR_Branch(GUN_B,STOP_OUTSWERR2);
				SetStartFailType(GUN_B,ENDFAIL_OUTSWERR2);
				stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
				printf("dc57\r\n");
			}
		}
	}
	
	//	static INT32U	LastTotalPower[GUN_MAX] = {0},powertime[GUN_MAX] = {0},powerlasttime[GUN_MAX] = {0};
	//����ʱ�����2���ӵ���δ�����仯����ֹͣ���    20230511
	powertime[gun] = OSTimeGet();
	if((GetChargeMode(gun) == MODE_AUTO) || (GetChargeMode(gun) == MODE_VIN) )
	{
		 if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
		 {
			 if(LastTotalPower[gun] == GunBillInfo[gun].TotalPower4)
			 {
				 if((powertime[gun] >= powerlasttime[gun]) ? ((powertime[gun] - powerlasttime[gun]) >= SYS_DELAY_2M) : \
				((powertime[gun] + (INT32U_MAX_NUM - powerlasttime[gun])) >= SYS_DELAY_2M))
				 {
					 powerlasttime[gun] = powertime[gun]; 
					LastTotalPower[gun] =  GunBillInfo[gun].TotalPower4;
					 ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;
					SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
					NB_WriterReason(gun,"E65",3);
					APP_Set_ERR_Branch(gun,STOP_MATERFAIL);
					SetStartFailType(gun,ENDFAIL_OTHERERR);
					stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
					printf("power57\r\n");
				 }
			 }
			 else
			 {
				powerlasttime[gun] = powertime[gun]; 
				LastTotalPower[gun] =  GunBillInfo[gun].TotalPower4;
			 }
		 }
		 else
		 {
			powerlasttime[gun] = powertime[gun]; 
			LastTotalPower[gun] =  GunBillInfo[gun].TotalPower4;
		 }
	}
	else
	{
		powerlasttime[gun] = powertime[gun]; 
		LastTotalPower[gun] =  GunBillInfo[gun].TotalPower4;
	}

	
	
	
	
//	if (PowerModuleInfo[gun].OutputInfo.Cur > (PresetVal_Temp.Therold_Value.OverOutCurTherold + MAX_DEVIATION) )
//    {//(6)�ж�ģ�鷵�ص���������Ƿ�����
//        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
//        {//�����������
//            ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;

//            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
//			APP_Set_ERR_Branch(gun,STOP_OUTCURROVER);
//			SetStartFailType(gun,ENDFAIL_OUTCURROVER);
//            stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
//	
//			printf("57\r\n");
//        }
//        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 6, 1);	//ֱ��ĸ����������澯1
//    }
   
    err = STOP_UNDEF;
    if (APP_CheckPowerModule(gun,ptr, &err) == TRUE)
    {//(5)�ж�ģ�鷵�ص�״̬�Ƿ�����
        if (err == STOP_ERR)
        {
			NB_WriterReason(gun,"E11",3);
			printf("58\r\n");
        }
        stoptype |= err;
    }

	
	
	//�������У�����5s����ѹ���������������ѹ����������������ж�Ϊ�쳣
	if((GetChargeRunningState(gun) == TRUE) &&  ((GetChargeMode(gun) == MODE_AUTO) || ((GetChargeMode(gun) == MODE_VIN))))
	{
		//�����ж�
		curtime[gun] = OSTimeGet();
		//������������Ƶ�����½� 20210610
		if (PowerModuleInfo[gun].OutputInfo.Cur  > ((4000-BMS_BCL_Context[gun].DemandCur) + MAX_DEVIATION) )
		{
			if((curtime[gun] >= lastcurtime[gun]) ? ((curtime[gun] - lastcurtime[gun]) >= SYS_DELAY_15s) : \
				((curtime[gun] + (INT32U_MAX_NUM - lastcurtime[gun])) >= SYS_DELAY_15s))
			{
				//(6)�ж�ģ�鷵�ص���������Ƿ�����
				if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
				{
			
					//�����������
					ptr->State2.OneByte.OverCurErrBits = STATE_TROUBLE;

					SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
					NB_WriterReason(gun,"E10",3);
					APP_Set_ERR_Branch(gun,STOP_OUTCURROVER);
					SetStartFailType(gun,ENDFAIL_OUTCURROVER);
					stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
					SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 6, 1);	//ֱ��ĸ����������澯1
					printf("577\r\n ");
				}
	
			}
		}
		else
		{
			lastcurtime[gun] = curtime[gun];
		}
		
		//��ѹ�ж�
		voltime[gun] = OSTimeGet();
		
		if (PowerModuleInfo[gun].OutputInfo.Vol > (BMS_BCL_Context[gun].DemandVol + BHMCMP_VOLT) )
		{
			if((voltime[gun] >= lastvoltime[gun]) ? ((voltime[gun] - lastvoltime[gun]) >= SYS_DELAY_10s) : \
				((voltime[gun] + (INT32U_MAX_NUM - lastvoltime[gun])) >= SYS_DELAY_10s))
			{
				if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
				{//�����ѹ����
					ptr->State2.OneByte.OverVolErrBits = STATE_TROUBLE;
					SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE;		//ֹͣԭ��Ϊ������ֹ����
					NB_WriterReason(gun,"E9",2);
					APP_Set_ERR_Branch(gun,STOP_OUTVOLTVORE);
					SetStartFailType(gun,ENDFAIL_OUTVOLTVORE);
					stoptype |= STOP_ERR; 								//����ԭ��Ϊ����ֹͣ
					printf("55,Vol = %d,BCL = %d\r\n",Vol_Judge,BMS_BCL_Context[gun].DemandVol);
				}
				SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_4, 7, 1);	//ֱ��ĸ�������ѹ�澯1
			}
		}
		else
		{
			lastvoltime[gun] = voltime[gun];
		}
		
	}
	else
	{
		curtime[gun] = OSTimeGet();
		lastcurtime[gun] = curtime[gun];
		voltime[gun] = OSTimeGet();
		lastvoltime[gun] = voltime[gun];
	}
	
    if (GetPowerDCState(gun) == POWER_DC_OPEN)
    {//ֱ���Ӵ����Ͽ�
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_OPEN);
    }
    else 
    {//�պ�
        SetBcuRemoteSignalState(gun,(INT8U)REMOTE_SIGNALE_5, 7, POWER_DC_CLOSE);
    }

	//�Ž�  20220719
	if (BSP_MPLSState(BSP_DI_DOOR))
    {
        if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
        {//��ͣ����
            ptr->State1.OneByte.UrgentStopBits = STATE_TROUBLE;
            SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE; 		//ֹͣԭ��Ϊ������ֹ����
			NB_WriterReason(gun,"E8",2);
			SetStartFailType(gun,ENDFAIL_OTHERERR);
			APP_Set_ERR_Branch(gun,STOP_DOORERR);
            stoptype |= STOP_ERR;								//����ԭ��Ϊ����ֹͣ
			printf("52\r\n");
        }
    }
	
	
	if(SYSSet.GunTemp == 0)
	{
		//ǹ�¶ȣ�����120���ǿ�ƹػ�
		 if ((BSP_GetTemp1(gun) > ADCJustInfo[gun].T1_120) || (BSP_GetTemp2(gun) > ADCJustInfo[gun].T2_120) ) 
		{
			ptr->State1.OneByte.UrgentStopBits = STATE_TROUBLE;
			SysState[gun].TotalErrState	|= ERR_CHARGESTOP_TYPE; 		//ֹͣԭ��Ϊ������ֹ����
			NB_WriterReason(gun,"E7",2);
			SetStartFailType(gun,ENDFAIL_GUNUPTEMPERATURE);
			APP_Set_ERR_Branch(gun,STOP_GUNUPTEMPERATURE);
			stoptype |= STOP_ERR;								//����ԭ��Ϊ����ֹͣ
			//printf("62\r\n");
		}
	}
	
	
    if (!( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) ) )
    {//�д�����
        if (SysState[gun].TotalErrState != ERR_NORMAL_TYPE)
        {//֪ͨ��������ֹͣ���
			printf("GUN = %d,TotalErr:%x\r\n",gun,SysState[gun].TotalErrState);
            send_message[gun].MsgID = (_BSP_MSGID)BSP_MSGID_PERIOD;
            send_message[gun].DivNum = APP_CHARGE_END;
            send_message[gun].DataLen = 1;
            send_message[gun].pData = (INT8U *)&stoptype;
			send_message[gun].GunNum = gun;
			if(gun == GUN_A)
			{
				OSQPost(Control_PeventA, &send_message[gun]);	//��������������Ϣ
			}
			else
			{
				OSQPost(Control_PeventB, &send_message[gun]);	//��������������Ϣ
			}
            OSTimeDly(SYS_DELAY_1s);
        }
    }
}

/*****************************************************************************
* Function      : ControlParaInit
* Description   : ���Ʋ�����ʼ��
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��15��  Ҷϲ��
*****************************************************************************/
void ControlParaInit(INT8U gun)
{
		if(gun >= GUN_MAX)
		{
			return;
		}


		    //��Ҫ�����Լ�
		SetBMSSelfCheck(gun,TRUE);
		//��Ҫ���BCP����
		SetBMSBCPFitCheck(gun,TRUE);
		//��Ҫ��Ե���
		SetBMSInsulationCheck(gun,TRUE);
		//���δ��ʼ
		SetBMSStartCharge(gun,FALSE);
		printf("555659654655\r\n");
		//�������
		SetChargePauseState(gun,FALSE);
		//���δ����
		SetBMSStopCharge(gun,FALSE);
		//���Խ��н������
		SetBMSStopOnce(gun,FALSE);
		//����δ׼����
		SetChargeReady(gun,READY_ING);
		//δ�������
		SetChargeRunningState(gun,FALSE);
		//���ģʽ����Ϊδ����
		SetChargeMode(gun,MODE_UNDEF);
		//δ�������
		SetDealFaultOverState(gun,FALSE);
		memset( (INT8U *)&SysState[gun].TotalErrState, 0, \
			(INT8U *)&SysState[gun].RemoteMeterState.GunState - (INT8U *)&SysState[gun].TotalErrState);
			    //�ֶ����ģʽ��0
		memset( (INT8U *)&ManDemandInfo[gun].Vol, 0, sizeof(_CCU_DEMAND_INFO) );
		memset( (INT8U *)&BMSDemandInfo[gun].Vol, 0, sizeof(_CCU_DEMAND_INFO) );
		//����ģ�������Ϣ��0

		memset( (INT8U *)&PowerModuleInfo[gun].OutputInfo.Vol, 0, sizeof(_POWER_MODULE_INFO));

		//ǹδ����
		SetGunState(gun,GUN_DISCONNECTED);
		//���ƹر�
		BSP_CloseRunLed(gun);
		//���ϵƹر�
		BSP_CloseFailLed(gun);
		//�Ͽ�K1 K2
		BSP_CloseK1K2(gun);	
		//�Ͽ�������Դ
		BSP_CloseBMSPower(gun);
	
}

void APP_ControlHardwareInit(void)
{
	INT8U gun;
	for(gun = 0;gun < GUN_MAX;gun++)
	{
		BSP_CloseBMSPower(gun);
		BSP_CloseLOCK(gun);
		BSP_CloseK1K2(gun);
	}

}

/*****************************************************************************
* Function      : APP_ControlInit
* Description   : �������������ʼ��
* Input         : void  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��13��  Ҷϲ��
*****************************************************************************/
INT8U APP_ControlInit(void)
{  
    //������Ϣ����
    Control_PeventA = OSQCreate(ControlOSQA, CONTROL_Q_NUM);
    if (Control_PeventA == NULL)
    {
        while(1);
    }
	
	Control_PeventB = OSQCreate(ControlOSQB, CONTROL_Q_NUM);
    if (Control_PeventB == NULL)
    {
        while(1);
    }
    //������ʼ��
    ControlParaInit(GUN_A);
	ControlParaInit(GUN_B);
    return TRUE;
}

/*****************************************************************************
* Function      : APP_MonitorGunState
* Description   : ���ǹ״̬��ǹ��״̬��BCU����ʱ��ȡǹ״̬����������Ϣ��ʽ
* Input         : void  
* Output        : None
* Note(s)       : static
* Contributor   : 2016��6��15��  Ҷϲ��
*****************************************************************************/
static void APP_MonitorGunState(INT8U gun)
{
	if(gun >= GUN_MAX)
	{
		return;
	}
    if (GetRM_HandVolt(gun) == INTERFACE_CONNECTED)  
    {//??????
        if (GetGunState(gun) == GUN_DISCONNECTED)
        {//???????
        	SetGunState(gun,GUN_CONNECTED);
        }
    }
    else 
    {//?????
        if (GetGunState((_GUN_NUM)gun) == GUN_CONNECTED)
        {//??????
			SetGunState(gun,GUN_DISCONNECTED);
			//????,?????,???????????
			if ( (GetBMSStartCharge(gun) == FALSE) && (GetBMSStopCharge(gun) == FALSE) )
			{//?????????
	            ControlParaInit(gun);
                
                BMS_CONNECT_StepSet((_GUN_NUM)gun,BMS_SEND_DEFAULT);	//???????
                BMS_CONNECT_ControlInit(gun);  //?????
			}
			else 
			{//???????,??????????
			    SetBMSInsulationCheck(gun,TRUE);//??????
			}
        }
    }
}


/*****************************************************************************
* Function      : APP_MonitorRemoteSignalState
* Description   : ���ң��״̬
* Input         : void  
* Output        : None
* Note(s)       : static
* Contributor   : 2016��7��6��  Ҷϲ��
*****************************************************************************/
static void APP_MonitorRemoteSignalState(INT8U gun)
{
    INT8U state;

	state = BSP_MPLSState(BSP_DI_JT);//��ͣ״̬
	if ( (_EMERGENCY_STATE)state != GetEmergencyState(gun) )
	{
		if(EMERGENCY_PRESSED == (_EMERGENCY_STATE)state )
		{
			//���������ڽⲻ�������¼�ͣ�������������һ��  20210702
			BSP_OpenLOCK(gun);
			OSTimeDly(SYS_DELAY_500ms);
			BSP_CloseLOCK(gun);
		}
		SetEmergencyState( gun,(_EMERGENCY_STATE)state);
	}
   
	state = BSP_GetLockState(gun);//ǹ�����״̬
  //  if ( (_ELEC_LOCK_STATE)state != GetElecLockState() )
	{
		SetElecLockState(gun,(_ELEC_LOCK_STATE)state);
	}

}

/***********************************************************************************************
* Function      : Get_BCUYXPara
* Description   : �ж�����ң�Ź���״̬����BCU�ϴ�����Ϊ׼
* Input         :
* Output        : ����һ�����ϼ��ɱ��Ƶ�
* Note(s)       :
* Contributor   : 2020-9-16 Ҷϲ��
***********************************************************************************************/
ERR_LED_STATE Get_ErrLedState(INT8U gun)
{
	static INT8U state[GUN_MAX] = {0};
	if(gun >= GUN_MAX)
	{
		return  ERR_LEDOPENED;
	}
    if(((BCURemoteSignal[gun].state1.byte&0xf0)!=0)||((BCURemoteSignal[gun].state2.byte&0x3C)!=0)||\
		((BCURemoteSignal[gun].state3.byte&0xFC)!=0)||((BCURemoteSignal[gun].state4.byte&0xE4)!=0) || BSP_MPLSState(BSP_DI_DOOR) || \
	(SysState[gun].GUNIDState == GUN_ID_FAIL) || (CCURecvData[gun].RecvHeart.ifGunUser) || (CCURecvData[gun].RecvHeart.PDUSingnal) \
	|| (CCURecvData[gun].RecvHeart.DCSWFail))
	{
		if(state[gun])
		{
			state[gun] = 0;
			printf("state1 =%x,state2 =%x,state3 =%x,state4 =%x,state5 =%x,state6 =%x,state7 =%x",BCURemoteSignal[gun].state1.byte,\
			BCURemoteSignal[gun].state2.byte,BCURemoteSignal[gun].state3.byte,BCURemoteSignal[gun].state4.byte,BCURemoteSignal[gun].state5.byte,BCURemoteSignal[gun].state6.byte,BCURemoteSignal[gun].state7.byte);
		}
        return ERR_LEDOPENED;
	}
	state[gun] = 1;
    return ERR_LEDECLOSED;
}
/***********************************************************************************************
* Function      : APP_DCSWDispose
* Description   :��Ӧǹû�������Ӵ�������е�ѹ����Ҫ������PcuControl.OpenCloseState[gunnum] = GUN_CLOSE;	
* Input         :
* Output        :
* Note(s)       :
* Contributor   : 2023-5-10 Ҷϲ��
***********************************************************************************************/
void APP_DCSWDispose(INT8U gun)
{
	static INT32U curtime[GUN_MAX] = {0},lasttime[GUN_MAX] = {0};
	
	curtime[gun] = OSTimeGet();
	
	if(gun >= GUN_MAX)
	{
		return;
	}
	
	//����10s���ڹػ�״̬,���ҵ�ѹ������200V ˵����ճ������Ҫ�ϵ����������ϲ�����ʧ
	if((CCURecvData[gun].RecvHeart.PDUSingnal  ==  GUN_CLOSE)  && (GetRM_DCOutVolt(gun) > 20000) )
	{
		if((curtime[gun] >= lasttime[gun]) ? ((curtime[gun] - lasttime[gun]) >= SYS_DELAY_15s ) : \
		((curtime[gun] + (INT32U_MAX_NUM - lasttime[gun])) >= SYS_DELAY_15s ) )
		{
			//A Bǹ�������ϣ���ֹA Bǹ�������
			SetBcuRemoteSignalState(GUN_A,(INT8U)REMOTE_SIGNALE_1, 7, 1);  //ֱ���Ӵ������ϣ�����ϵ�����
			SetBcuRemoteSignalState(GUN_B,(INT8U)REMOTE_SIGNALE_1, 7, 1);  //ֱ���Ӵ������ϣ�����ϵ�����
			lasttime[gun] = curtime[gun];
			CCUSendData[gun].SendHeart.DCSWFaile  = 1;						//ֱ���Ӵ����������Ӧ��ǹ������
		}
	}
	else
	{
		lasttime[gun] = curtime[gun];
	}
}

/*****************************************************************************
* Function      : TaskPeriod
* Description   : ����������
* Input         : void *data  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��7��5��  Ҷϲ��
*****************************************************************************/
void TaskPeriod(void *data)
{
	INT8U gun;
    OSTimeDly(SYS_DELAY_1s);
    while (1)
    {
		OSTimeDly(SYS_DELAY_5ms);
		for(gun = 0;gun < GUN_MAX;gun++)
		{
			//�ж��Ƿ�δ����
			if(Get_ErrLedState(gun) == ERR_LEDOPENED)
			{
				OSSchedLock();
				SysState[gun].WorkState = GetWorkState((_GUN_NUM)gun,STOP_ERR);//���㹤��״̬
				BCURemoteSignal[gun].state1.byte &= 0xf0;
				BCURemoteSignal[gun].state1.byte |= SysState[gun].WorkState & 0x0f;
				OSSchedUnlock();
				BSP_OpenFailLed(gun);
			}else
			{
				OSSchedLock();
				SysState[gun].WorkState = GetWorkState((_GUN_NUM)gun,STOP_UNDEF);//���㹤��״̬
				BCURemoteSignal[gun].state1.byte &= 0xf0;
				BCURemoteSignal[gun].state1.byte |= SysState[gun].WorkState & 0x0f;
				OSSchedUnlock();
				BSP_CloseFailLed(gun);
			}
			//����ģ�����ݣ�����  PowerModuleInfo��
			Updata_ModuleInfo(gun);
			//ʵʱ��ȡǹ״̬
			APP_MonitorGunState(gun);
			//ʵʱ��ȡң��״̬
			APP_MonitorRemoteSignalState(gun);
			//��ʼ�����ɴ�����ʵʱ��������ѹ������
			APP_MonitorChargingState(gun);
			if((APP_GetWorkState(GUN_A) == WORK_CHARGE) || 	(APP_GetWorkState(GUN_B) == WORK_CHARGE))
			{
				BSP_IOOpen(IO_RLY_FAN);  //�򿪷���
			}
			else
			{
				BSP_IOClose(IO_RLY_FAN);  //�رշ���
			}
			//12��δ��磬���׮���е�ѹ����Ҫ�ر���Ӧ��ǹ	����Ҫ�ر���Ӧ��ǹ��3��ǹ �ܹر�2��ǹ ����һ��ǹ��ô�ر�?
			//���ܴ���һ��ǹ�󼶾��е�ѹ
			//APP_DCSWDispose(gun);
		}
    }
}
/*****************************************************************************
* Function      : TaskControl
* Description   : �������񣬴�����������������߼���BMSͨѶ���Ӳ��������
                  �������BMSͨѶ���񲢷ǲ�����Ϣ�ķ�ʽͬ�������������������Ϣ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��08��  Ҷϲ��
*****************************************************************************/
void TaskControlGunA(void *pdata)
{   
    INT8U err;
    _BSP_MESSAGE *pMsg;   // ������Ϣָ��
    
    pdata = pdata;
    
    OSTimeDly(SYS_DELAY_1s);  

    APP_ControlHardwareInit();
    OSTimeDly(SYS_DELAY_1s);  
    APP_ControlInit();
    OSTimeDly(SYS_DELAY_2s);
//	BSP_OpenLOCK(GUN_A);
//	BSP_OpenLOCK(GUN_B);
//	
//		BSP_RLCOpen(BSPRLC_GUN_LOCK);
////	
//	BSP_RLCClose(BSPRLC_GUN_LOCK);

    while(1)
    {	
        pMsg = OSQPend(Control_PeventA, SYS_DELAY_20ms, &err); 
        if (err == OS_ERR_NONE)
        {
            switch((_BSP_MSGID)pMsg->MsgID)
            {
                case BSP_MSGID_BMS:         //����BMS����
                case BSP_MSGID_BCU: 
                case BSP_MSGID_PCU:
                case BSP_MSGID_INSULATION:
                case BSP_MSGID_PERIOD:
                case BSP_MSGID_GUIYUE:
                    APP_AnalyzeRxDataA(pMsg);
                    break;
                default:
                    break;
            }
        }
    }
}
 
/*****************************************************************************
* Function      : TaskControl
* Description   : �������񣬴�����������������߼���BMSͨѶ���Ӳ��������
                  �������BMSͨѶ���񲢷ǲ�����Ϣ�ķ�ʽͬ�������������������Ϣ
* Input         : void *pdata  
* Output        : None
* Note(s)       : 
* Contributor   : 2016��6��08��  Ҷϲ��
*****************************************************************************/
void TaskControlGunB(void *pdata)
{   
    INT8U err;
    _BSP_MESSAGE *pMsg;   // ������Ϣָ��
    
    pdata = pdata;
    
    OSTimeDly(SYS_DELAY_5s);    //A��������������
//	BSP_OpenLOCK(GUN_A);
//	BSP_OpenLOCK(GUN_B);
//	
//		BSP_RLCOpen(BSPRLC_GUN_LOCK);
////	
//	BSP_RLCClose(BSPRLC_GUN_LOCK);

    while(1)
    {	
        pMsg = OSQPend(Control_PeventB, SYS_DELAY_20ms, &err); 
        if (err == OS_ERR_NONE)
        {
            switch((_BSP_MSGID)pMsg->MsgID)
            {
                case BSP_MSGID_BMS:         //����BMS����
                case BSP_MSGID_BCU: 
                case BSP_MSGID_PCU:
                case BSP_MSGID_INSULATION:
                case BSP_MSGID_PERIOD:
                case BSP_MSGID_GUIYUE:
                    APP_AnalyzeRxDataB(pMsg);
                    break;
                default:
                    break;
            }
        }
    }
}
/************************(C)COPYRIGHT 2010 �����Ƽ�****END OF FILE****************************/
