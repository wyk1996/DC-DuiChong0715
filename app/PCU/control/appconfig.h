/*****************************************Copyright(C)******************************************
*******************************************���ݻ���*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName			: appconfig.h
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
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

#define  ACIN_UVP	  2900					//��������Ƿѹ�ߵ�ѹ  290V   
#define  ACIN_OVER	  4600			  		//���������ѹ�ߵ�ѹ  460V


//���ģʽѡ��
typedef enum
{

    MODE_CHARGE_AVER = 0x00, 	//��������
    MODE_CHARGE_DEMAND,   		//���ܷ���
    MODE_CHARGE_QZ,   		//ǿ�Ʒ��� ������2���������Գ䣩
    MODE_CHARGE_MAX,
}_CHARGE_MODE;



//ģ������
typedef enum
{
    YFY75020_MODULE  =0,    //YFY750ģ��
    YFY50025_MODULE ,   	 //YFY500ģ��
    YFY75035_MODULE   ,    //YFY750ģ��
	YFY75050_MODULE   ,    //YFY750ģ��
	YKR15075000_MODULE,		//Ӣ����150V��7.5KWģ��
	SH75015K3B_MODULE,	 //ʢ��15KWģ��
	YL75030_MODULE,		//����75030 20KWģ��
	YFY75040_MODULE   ,    //YFY750ģ�� 15kw
	SH75020K3B_MODULE   ,    //YFY750ģ�� 20kw
	TH30F10025C7_MODULE,	//ͨ��30kwģ��
}_MODULE_OUTLIMT_CMD;



//ģ������ѡ��
//ģ��Ʒ�� 10 ��
typedef enum
{
	YFY_MODULE =0, //Ӣ��Դ
	YKR_MODULE, //Ӣ����
	YYLN_MODULE, //��������
	MGMT_MODULE, //�������
	TRD_MODULE, //�����
	HUWEI_MODULE, //��Ϊ
	LEINENG_MODULE, //����
	YFY1_MODULE, //Ӣ��Դ(������)
	YFY2_MODULE, //Ӣ��Դ(������)
	YFY3_MODULE,  //Ӣ��Դ(������)
	MAX_MODULE,
}_MODULE_BRAND;


typedef enum
{
	MK_20kW750V =0, //20kW750V �㹦��ģ��
	MK_20kW500V , //20kW500V ģ��
	MK_10kW150V, //10kW150V ģ��
	MK_B75025T, //15kW ˫��ģ��
	MK1_20kW750V, //20kW750V �㹦��ģ��(������)
	MK2_20kW750V, //20kW750V �㹦��ģ��(������)
	MK3_20kW750V, //20kW750V �㹦��ģ��(������)
	MK4_20kW750V, //20kW750V �㹦��ģ��(������)
	MK5_20kW750V, //20kW750V �㹦��ģ��(������)
	MK6_20kW750V, //20kW750V �㹦��ģ��(������)
	MAX_TYPE
}_MODULE_TYPE;


#define POWER_MODULE_YKR           0x01              	 //Ӣ������ģ��
#define POWER_MODULE_YFY           0x02              	 //Ӣ��Դ�Ĺ���ģ��
#define POWER_MODULE_GW            0x03              	 //����ģ��
#define POWER_MODULE_SH            0x05              	 //����ģ��   

#define MODULENUM_MAX_USED 			8					//ģ�����Ϊ8��
#define MODULENUM_SIGNAL_USED 		4					//��ǹΪ���Ϊ4��


//#define POWER_MODULE_TYPE_CUR      POWER_MODULE_YFY  		//ѡ��Ĺ���ģ�鳧��
//#define POWER_MODULE_TYPE          YFY75050_MODULE     		//ģ���������
//#define POWER_MODULE_TYPE          YL75030_MODULE     		//ģ���������

//#define POWER_MODULE_TYPE_CUR      POWER_MODULE_SH  		//ѡ��Ĺ���ģ�鳧��
//#define POWER_MODULE_TYPE          SH75015K3B_MODULE     		//ģ���������

#define GUN_CHARGE_MODE			   MODE_CHARGE_QZ			//���ģʽ
 

/* Private typedef----------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/

#endif	//__APPCONFIG_H_
/************************(C)COPYRIGHT 2018 ���ݻ���*****END OF FILE****************************/
