/*****************************************Copyright(C)******************************************
*******************************************杭州汇誉*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			:ModuleYouren.c
* Author			: 
* Date First Issued	: 
* Version			: 
* Description		: 
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* //2010		    : V
* Description		: 
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "sysconfig.h"
#include "ModuleSIM7600.h"
#include "bsp_uart.h"
#include "4GMain.h"
#include "DataChangeInterface.h"
#include "bsp_debug_usart.h"
#include "DisplayMain.h"
#define DIM7600_RECV_TABLE_LEN        (24u)
/* Private define-----------------------------------------------------------------------------*/
__packed typedef struct
{
    _4G_STATE SIM_ExistState;                      	//模块是否存在
	_4G_STATE SIM_ConectState[10];					//是否连接到服务器(最大可连接10路)
	_4G_STATE SIM_Regiset[10];						//是否注册成功(最大可连接10路)
	  _4G_MODE  Mode;									//4G模式还是HTTP模式
    _4G_STATE HTTP_Start;							//HTTP开始返回
    _4G_STATE HTTP_ConectState;						//HTTP连接状态
    _4G_STATE HTTP_PasswordState;					//HTTP密码登录状态
    _4G_STATE HTTP_Get;								//HTTP获取
    _4G_STATE HTTP_Read;							//HTTP读取
    _4G_STATE HTTP_ReadDone;						//读取完成
	
	_4G_STATE FTP_Start;							//FTP开始返回
    _4G_STATE FTP_Login;							//FTP密码登录状态
    _4G_STATE FTP_SetType;							//FTP设置类型
    _4G_STATE FTP_Read;								//FTP读取

	__packed union
    {
        INT8U OneByteBits;
        __packed struct
        {
			INT8U ATCMD_CIMI   	  : 1;			 //读取卡号返回
			INT8U ATCMD_CSQ  : 1;			 	 //读取信号强度返回
			
			INT8U ATCMD_CGDCONT   : 1;			 //配置PDP返回
			INT8U ATCMD_CIPMODE   : 1;			 //数据透传返回
			INT8U ATCMD_NETOPEN   : 1;			 //激活PDP返回
			INT8U ATCMD_SETCIPRXGET  : 1;         //接收手动获取
            INT8U ATCMD_OK        : 1;           //切换倒指令模式
			INT8U res        : 1;           //预留
        }OneByte;
	}State0;
	INT8U ATCMD_CIPOPEN[10];				//连接服务器返回状态
	INT8U ATCMD_SENDACK[10];				//发送数据确认
}_SIM7600_CMD_STATE;


static INT8U Send_AT_EnterATCmd(void);
static INT8U Send_AT_CIPCLOSE(INT8U num);
static INT8U Send_AT_NETCLOSE(void);

/* Private typedef----------------------------------------------------------------------------*/
/*static */_SIM7600_CMD_STATE   SIM7600CmdState;		
INT8U CSQNum = 0;				//信号强度数值

/*****************************************************************************
* Function     : APP_GetCSQNum
* Description  : 获取型号强度值
* Input        : 
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/	
INT8U APP_GetCSQNum(void)
{
	return CSQNum;
}

/*****************************************************************************
* Function     : APP_GetSIM7600Mode
* Description  : 设置4g当前模式
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
uint8_t APP_SetSIM7600Mode(_4G_MODE mode)
{
    SIM7600CmdState.Mode = mode;
    return TRUE;
}


/*****************************************************************************
* Function     : APP_GetSIM7600Mode
* Description  : 获取4g当前模式
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
_4G_MODE APP_GetSIM7600Mode(void)
{
    return SIM7600CmdState.Mode;
}

//以下是AT指令发送处理
/*****************************************************************************
* Function     : SIM7600Reset
* Description  : 模块复位
* Input        : 
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/	
INT8U SIM7600Reset()
{
	printf("4G_SIM7600_Reset\r\n");    //4g模块断电复位
	static INT8U count  = 10;
	INT8U i;
	memset(&SIM7600CmdState,0,sizeof(SIM7600CmdState));	
	for(i = 0;i < NetConfigInfo[SYSSet.NetYXSelct].NetNum;i++)
	{
		Send_AT_CIPCLOSE(i);
	}
	Send_AT_NETCLOSE();
	
	if(++count > 10)
	{
	//电源上电
		OSTimeDly(SYS_DELAY_100ms);
		BSP_IOClose(IO_4G_PWR);
		OSTimeDly(SYS_DELAY_5s);
		BSP_IOOpen(IO_4G_PWR);
		OSTimeDly(SYS_DELAY_100ms);
		
			//开机
		BSP_IOOpen(IO_4G_ONOFF);
		OSTimeDly(SYS_DELAY_500ms);
		BSP_IOClose(IO_4G_ONOFF);
		OSTimeDly(SYS_DELAY_500ms);
		OSTimeDly(SYS_DELAY_10s);
		OSTimeDly(SYS_DELAY_5s);
		count = 0;
	}
	else
	{
		//复位
		BSP_IOOpen(IO_4G_RES);
		OSTimeDly(SYS_DELAY_3s);
		BSP_IOClose(IO_4G_RES);
		OSTimeDly(SYS_DELAY_10s);
		OSTimeDly(SYS_DELAY_5s);
	}
	return FALSE;
}

/*****************************************************************************
* Function     : SIM7600CloseNet
* Description  : 关闭网络
* Input        : 
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/	
INT8U SIM7600CloseNet(INT8U num)
{
	if(num > 10)
	{
		return FALSE;
	}		
	
	Send_AT_CIPCLOSE(num);
	return TRUE;
}

/*****************************************************************************
* Function     : APP_GetSIM7600Status
* Description  : 获取模块是否存在
* Input        : 
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/	
_4G_STATE APP_GetSIM7600Status(void)
{
	return SIM7600CmdState.SIM_ExistState;
}



/*****************************************************************************
* Function     : APP_GetModuleConnectState
* Description  : 连接服务器状态,最大可连接10路
* Input        :     
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
_4G_STATE APP_GetModuleConnectState(INT8U num)
{
	if(num > 10)
	{
		return STATE_4G_ERR;
	}
	return SIM7600CmdState.SIM_ConectState[num];
}

/*****************************************************************************
* Function     : APP_GetAppRegisterState
* Description  : 注册是否成功,最大可连接10路
* Input        :     
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
_4G_STATE APP_GetAppRegisterState(INT8U num)
{
	if(num > 10)
	{
		return STATE_4G_ERR;
	}
	return SIM7600CmdState.SIM_Regiset[num];
}

/*****************************************************************************
* Function     : APP_GetAppRegisterState
* Description  : 注册是否成功,最大可连接10路
* Input        :     
* Output       : 
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U  APP_SetAppRegisterState(INT8U num,_4G_STATE state)
{
	if(num > 10)
	{
		return FALSE;
	}
	 SIM7600CmdState.SIM_Regiset[num] = state;
	return TRUE;
}


/*****************************************************************************
* Function     : Send_AT_CIPCLOSE
* Description  : 关闭服务器
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_CIPCLOSE(INT8U num)
{
	INT8U buf[60];
	INT32S length;
	 if(num> 10)
	 {
		 return FALSE;
	 }

	length = snprintf((char *)buf, sizeof(buf), "AT+CIPCLOSE=%d\r\n",num);
	if (length == -1)
	{
        printf("snprintf error, the len is %d", length);
		return FALSE;
	}
	if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {	
		return TRUE;
    }
	
    return FALSE;
}	

/*****************************************************************************
* Function     :Send_AT_READ
* Description  :读取数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_READ(uint32_t len)
{
    uint8_t buf[60];
    int length;


    length = snprintf((char *)buf, sizeof(buf), "AT+HTTPREAD=0,%d\r\n",len);
    if (length == -1)
    {
        printf("snprintf error, the len is %d", length);
        return FALSE;
    }
    if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
* Function     : Send_AT_NETCLOSE
* Description  : 去除pdp
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_NETCLOSE(void)
{
	if (UART_4GWrite((INT8U*)"AT+NETCLOSE\r\n", strlen("AT+NETCLOSE\r\n") ) == strlen("AT+NETCLOSE\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     :Send_AT_HTTPTERM
* Description  :结束HTTP
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_HTTPTERM(void)
{

    if (UART_4GWrite((uint8_t*)"AT+HTTPTERM\r\n", strlen("AT+HTTPTERM\r\n") ) == strlen("AT+HTTPTERM\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     :Send_AT_CFTPSSTART
* Description  :开始FTP
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_CFTPSSTART(void)
{

    if (UART_4GWrite((uint8_t*)"AT+CFTPSSTART\r\n", strlen("AT+CFTPSSTART\r\n") ) == strlen("AT+CFTPSSTART\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
* Function     :Send_AT_HTTPPAR
* Description  :连接HTTP服务器
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_HTTPPAR(char *http_add)
{
    uint8_t buf[100] = {0};
    int length;
    length = snprintf((char *)buf, sizeof(buf),"AT+HTTPPARA=\"URL\",\"%s\"\r\n",http_add);
    if (length == -1)
    {
        printf("snprintf error, the len is %d", length);
        return FALSE;
    }
    if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
* Function     :Send_AT_CFTPSLOGIN
* Description  :连接FTP服务器
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_CFTPSLOGIN(char *ftp_add,uint16_t port,char* username,char* password)
{
    uint8_t buf[200] = {0};
    int length;
    length = snprintf((char *)buf, sizeof(buf),"AT+CFTPSLOGIN=\"%s\",%d,\"%s\",\"%s\",0\r\n",ftp_add,port,username,password);
    if (length == -1)
    {
        printf("snprintf error, the len is %d", length);
        return FALSE;
    }
    if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
* Function     :Send_AT_CFTPSLOGIN
* Description  :连接FTP服务器
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_CFTPSGET(char* path)
{
    uint8_t buf[100] = {0};
    int length;
    length = snprintf((char *)buf, sizeof(buf),"AT+CFTPSGET=\"%s\"\r\n",path);
    if (length == -1)
    {
        printf("snprintf error, the len is %d", length);
        return FALSE;
    }
    if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     :Send_AT_CFTPSTYPE
* Description  :设置传输类型为BIN
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_CFTPSTYPE(void)
{
	if (UART_4GWrite((INT8U*)"AT+CFTPSTYPE=I\r\n", strlen("AT+CFTPSTYPE=I\r\n") ) == strlen("AT+CFTPSTYPE=I\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}


/*****************************************************************************
* Function     :Send_AT_GET
* Description  :获取数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_GET(void)
{
    if (UART_4GWrite((uint8_t*)"AT+HTTPACTION=0\r\n", strlen("AT+HTTPACTION=0\r\n") ) == strlen("AT+HTTPACTION=1\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}



/*****************************************************************************
* Function     :Send_AT_HTTPINIT
* Description  :开始HTTP
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Send_AT_HTTPINIT(void)
{
    if (UART_4GWrite((uint8_t*)"AT+HTTPINIT\r\n", strlen("AT+HTTPINIT\r\n") ) == strlen("AT+HTTPINIT\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_EnterATCmd
* Description  : 进入AT配置指令1
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U Send_AT_EnterATCmd(void)
{
	if (UART_4GWrite((INT8U*)"+++", strlen("+++") ) == strlen("+++") )
    {	
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_CSQ
* Description  : 读取信号强度
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U Send_AT_CSQ(void)
{
	if (UART_4GWrite((INT8U*)"AT+CSQ\r\n", strlen("AT+CSQ\r\n") ) == strlen("AT+CSQ\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}	

/*****************************************************************************
* Function     : Send_AT_CIMI
* Description  : 读取卡号
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_CIMI(void)
{
	if (UART_4GWrite((INT8U*)"AT+CIMI\r\n", strlen("AT+CIMI\r\n") ) == strlen("AT+CIMI\r\n") )
	//if(BSP_UARTWrite(GPRS_UART,(INT8U*)"AT+CIMI\r\n",strlen("AT+CIMI\r\n")))
    {
        return TRUE;
    }
    return FALSE;
}	
/*****************************************************************************
* Function     : Send_AT_CGDCONT
* Description  : 配置PDP
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_CGDCONT(void)
{
	if (UART_4GWrite((INT8U*)"AT+CGDCONT=1,\"IP\",\"cmnet\"\r\n", strlen("AT+CGDCONT=1,\"IP\",\"cmnet\"\r\n") ) == strlen("AT+CGDCONT=1,\"IP\",\"cmnet\"\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}	
/*****************************************************************************
* Function     : Send_AT_CIPMODE
* Description  : 数据透传
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U Send_AT_CIPMODE(void)
{
	if (UART_4GWrite((INT8U*)"AT+CIPMODE=1\r\n", strlen("AT+CIPMODE=1\r\n") ) == strlen("AT+CIPMODE=1\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_NOTCIPMODE
* Description  : 非数据透传
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_NOTCIPMODE(void)
{
	if (UART_4GWrite((INT8U*)"AT+CIPMODE=0\r\n", strlen("AT+CIPMODE=0\r\n") ) == strlen("AT+CIPMODE=0\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_CIPRXGET
* Description  : 接收手动获取
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_SetCIPRXGET(void)
{
	if (UART_4GWrite((INT8U*)"AT+CIPRXGET=1\r\n", strlen("AT+CIPRXGET=1\r\n") ) == strlen("AT+CIPMODE=0\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_NETOPEN
* Description  : 激活PDP
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_NETOPEN(void)
{
	if (UART_4GWrite((INT8U*)"AT+NETOPEN\r\n", strlen("AT+NETOPEN\r\n") ) == strlen("AT+NETOPEN\r\n") )
    {
        return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_CIPSEND
* Description  : 发送数据请求
* Input        :num  哪个socket 
				len   数据长度
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Send_AT_CIPSEND(INT8U num,INT8U len)
{
	INT8U buf[60];
	INT32S length;

	length = snprintf((char *)buf, sizeof(buf), "AT+CIPSEND=%d,%d\r\n",num,len);
	if (length == -1)
	{
        printf("snprintf error, the len is %d", length);
		return FALSE;
	}
	if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
		return TRUE;
    }
	return FALSE;
}

/*****************************************************************************
* Function     : Send_AT_CIPRXGET
* Description  : 读取数据请求
* Input        :num  哪个socket 
				len   数据长度
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U Send_AT_CIPRXGET(INT8U num)
{
	INT8U buf[60];
	INT32S length;

	length = snprintf((char *)buf, sizeof(buf), "AT+CIPRXGET=2,%d\r\n",num);
	if (length == -1)
	{
        printf("snprintf error, the len is %d", length);
		return FALSE;
	}
	if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
		return TRUE;
    }
	return FALSE;
}

/*****************************************************************************
* Function     :Module_ConnectServer
* Description  :连接服务器
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Module_ConnectServer(INT8U num,INT8U* pIP,INT16U port)
{
	INT8U buf[60];
     INT32S length;
	 if((pIP == NULL) || (num> 10))
	 {
		 return FALSE;
	 }

	length = snprintf((char *)buf, sizeof(buf), "AT+CIPOPEN=%d,\"TCP\",\"%s\",%d\r\n",num,pIP,port);
	if (length == -1)
	{
        printf("snprintf error, the len is %d", length);
		return FALSE;
	}
	if (UART_4GWrite(buf, strlen((char*)buf) ) == strlen((char*)buf) )
    {
		return TRUE;	
    }
	return FALSE;
}	



//以下是AT指令接收数据处理
/*****************************************************************************
* Function     : Recv_AT_EnterATCmd_Ack
* Description  : 进入AT配置指令返回
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_EnterATCmd_Ack(INT8U *pdata, INT16U len)
{
	if (UART_4GWrite((INT8U*)"+++", strlen("+++") ) == strlen("+++") )
    {	
        return TRUE;
    }
    return FALSE;
}	


/*****************************************************************************
* Function     : Recv_AT_CSQ_Ack
* Description  : 读取信号强度返回
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_CSQ_Ack(INT8U *pdata, INT16U len)
{
	
	if ( (pdata == NULL) || len < 16)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if (strstr((char *)pdata, "OK") == NULL)
    {
        return FALSE;
	} 
	
	CSQNum = atoi((const char *)&pdata[14]);
	if((CSQNum == 99) || (CSQNum == 0) )
	{
		//无信号
		return FALSE;
	}
	OSSchedLock();
	SIM7600CmdState.State0.OneByte.ATCMD_CSQ = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}	

/*****************************************************************************
* Function     : Recv_AT_CIMI_Ack
* Description  : 读取卡号返回
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_CIMI_Ack(INT8U *pdata, INT16U len)
{
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if (strstr((char *)pdata, "OK") == NULL)
    {
        return FALSE;
    } 
	OSSchedLock();
	SIM7600CmdState.State0.OneByte.ATCMD_CIMI = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}	
/*****************************************************************************
* Function     : Recv_AT_CGDCONT_Ack
* Description  : 配置PDP返回
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_CGDCONT_Ack(INT8U *pdata, INT16U len)
{
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
//	if (strstr((char *)pdata, "OK") == NULL)
//    {
//        return FALSE;
//    } 
	OSSchedLock();
	SIM7600CmdState.State0.OneByte.ATCMD_CGDCONT = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}	
/*****************************************************************************
* Function     : Recv_AT_CIPMODE_Ack
* Description  : 数据透传返回
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_CIPMODE_Ack(INT8U *pdata, INT16U len)
{
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
//	if (strstr((char *)pdata, "OK") == NULL)
//    {
//        return FALSE;
//    } 
	OSSchedLock();
	SIM7600CmdState.State0.OneByte.ATCMD_CIPMODE = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}

/*****************************************************************************
* Function     : Recv_AT_NETOPEN_Ack
* Description  : 激活PDP返回
* Input        : INT8U *pdata  
                 INT16U len    
* Output       : None
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_NETOPEN_Ack(INT8U *pdata, INT16U len)
{
	
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
//	if (strstr((char *)pdata, "OK") == NULL)
//    {
//        return FALSE;
//    } 
	OSSchedLock();
	SIM7600CmdState.State0.OneByte.ATCMD_NETOPEN = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}


/*****************************************************************************
* Function     :Recv_AT_ConnectServer_Ack
* Description  :连接服务器返回
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_ConnectServer0_Ack(INT8U *pdata, INT16U len)
{
//	AT+CIPOPEN=0,"TCP","122.114.122.174",33870
//	OK

//	+CIPOPEN: 0,0

	INT8U res;
	char *poffset;
	
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if (strstr((char *)pdata, "OK") != NULL)
    {
        OSSchedLock();
		SIM7600CmdState.ATCMD_CIPOPEN[0] = _4G_RESPOND_OK;
		OSSchedUnlock();
		return TRUE;
    } 
	
	poffset = strtok((char *)pdata, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	res = atoi(poffset);
	//0表示新连接上，4表示意见连接上了
	if((res != 0) || (res != 4))
	{
		return FALSE;
	}
	OSSchedLock();
	SIM7600CmdState.ATCMD_CIPOPEN[0] = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}	

/*****************************************************************************
* Function     :Recv_AT_ConnectServer_Ack
* Description  :连接服务器返回
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_ConnectServer1_Ack(INT8U *pdata, INT16U len)
{
	//	AT+CIPOPEN=0,"TCP","122.114.122.174",33870
//	OK

//	+CIPOPEN: 0,0

	INT8U res;
	char *poffset;
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if (strstr((char *)pdata, "OK") != NULL)
    {
        OSSchedLock();
		SIM7600CmdState.ATCMD_CIPOPEN[1] = _4G_RESPOND_OK;
		OSSchedUnlock();
		return TRUE;
    } 
	
	poffset = strtok((char *)pdata, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	res = atoi(poffset);
	//0表示新连接上，4表示意见连接上了
	if((res != 0) || (res != 4))
	{
		return FALSE;
	}
	OSSchedLock();
	SIM7600CmdState.ATCMD_CIPOPEN[1] = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}	


/*****************************************************************************
* Function     :Recv_AT_CmdConnectServer0_Ack
* Description  :连接服务器指令返回
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_ConnectServer0Cmd_Ack(INT8U *pdata, INT16U len)
{
	//	AT+CIPOPEN=0,"TCP","122.114.122.174",33870
//	OK

//	+CIPOPEN: 0,0

	INT8U res;
	char *poffset;
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}

	pdata[len] = '\0';  //结束符
	if(NetConfigInfo[SYSSet.NetYXSelct].NetNum == 1)
	{
		{
			if (strstr((char *)pdata, "ERROR") == NULL)  //没有错误就返回注册成功
			{
				OSSchedLock();
				SIM7600CmdState.ATCMD_CIPOPEN[0] = _4G_RESPOND_OK;
				OSSchedUnlock();
				return TRUE;
			} 
			return FALSE;
		}
	}
	else
	{
		if (strstr((char *)pdata, "ERROR") != NULL)
		{
			return FALSE;
		} 
		
		if (strstr((char *)pdata, "OK") != NULL)
		{
			OSSchedLock();
			SIM7600CmdState.ATCMD_CIPOPEN[0] = _4G_RESPOND_OK;
			OSSchedUnlock();
			return TRUE;
		} 
		
		poffset = strtok((char *)pdata, ",");
		if(poffset == NULL)
		{
			return FALSE;
		}
		
		poffset = strtok(NULL, ",");
		if(poffset == NULL)
		{
			return FALSE;
		}
		
		poffset = strtok(NULL, ",");
		if(poffset == NULL)
		{
			return FALSE;
		}
		
		poffset = strtok(NULL, ",");
		if(poffset == NULL)
		{
			return FALSE;
		}
		res = atoi(poffset);
		//0表示新连接上，4表示意见连接上了
		if((res != 0) || (res != 4))
		{
			return FALSE;
		}
		OSSchedLock();
		SIM7600CmdState.ATCMD_CIPOPEN[0] = _4G_RESPOND_OK;
		OSSchedUnlock();
	}
	return TRUE;
}

/*****************************************************************************
* Function     :Recv_AT_CmdConnectServer0_Ack
* Description  :连接服务器指令返回
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_ConnectServer1Cmd_Ack(INT8U *pdata, INT16U len)
{
		//	AT+CIPOPEN=0,"TCP","122.114.122.174",33870
//	OK

//	+CIPOPEN: 0,0


	INT8U res;
	char *poffset;
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if (strstr((char *)pdata, "OK") != NULL)
    {
        OSSchedLock();
		SIM7600CmdState.ATCMD_CIPOPEN[1] = _4G_RESPOND_OK;
		OSSchedUnlock();
		return TRUE;
    } 
	
	if (strstr((char *)pdata, "ERROR") != NULL)
    {
		return FALSE;
    } 
	
	poffset = strtok((char *)pdata, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	res = atoi(poffset);
	//0表示新连接上，4表示意见连接上了
	if((res != 0) || (res != 4))
	{
		return FALSE;
	}
	OSSchedLock();
	SIM7600CmdState.ATCMD_CIPOPEN[1] = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}
/*****************************************************************************
* Function     :Recv_AT_SetReAct_Ack
* Description  :设置接收主动获取
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_SetReAct_Ack(INT8U *pdata, INT16U len)
{
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
//	if (strstr((char *)pdata, "OK") == NULL)
//    {
//        return FALSE;
//    } 
	OSSchedLock();
	SIM7600CmdState.State0.OneByte.ATCMD_SETCIPRXGET = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}


/*****************************************************************************
* Function     :Recv_AT_SendAck0Cmd_Ack
* Description  :发送数据确认
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_SendAck0Cmd_Ack(INT8U *pdata, INT16U len)
{
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	//
	pdata[len] = '\0';  //结束符
//	if ((strstr((char *)pdata, ">") == NULL) )
//    {
//		OSSchedLock();
//		SIM7600CmdState.ATCMD_SENDACK[0] = _4G_RESPOND_AGAIN;
//		OSSchedUnlock();
//        return FALSE;
//    } 
	if ((strstr((char *)pdata, "ERROR") != NULL) )
    {
		OSSchedLock();
		SIM7600CmdState.ATCMD_SENDACK[0] = _4G_RESPOND_AGAIN;
		OSSchedUnlock();
        return FALSE;
    } 
	OSSchedLock();
	SIM7600CmdState.ATCMD_SENDACK[0] = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}

/*****************************************************************************
* Function     :Recv_AT_SendAck0Cmd_Ack
* Description  :发送数据确认
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_SendAck1Cmd_Ack(INT8U *pdata, INT16U len)
{
	if ( (pdata == NULL) || !len)
	{
		return FALSE;
	}
	//
	pdata[len] = '\0';  //结束符
	//	if ((strstr((char *)pdata, ">") == NULL) )
//    {
//		OSSchedLock();
//		SIM7600CmdState.ATCMD_SENDACK[1] = _4G_RESPOND_AGAIN;
//		OSSchedUnlock();
//        return FALSE;
//    } 
	if ((strstr((char *)pdata, "ERROR") != NULL) )
    {
		OSSchedLock();
		SIM7600CmdState.ATCMD_SENDACK[1] = _4G_RESPOND_AGAIN;
		OSSchedUnlock();
        return FALSE;
    } 
	OSSchedLock();
	SIM7600CmdState.ATCMD_SENDACK[1] = _4G_RESPOND_OK;
	OSSchedUnlock();
	return TRUE;
}



/*****************************************************************************
* Function     :Recv_AT_SendAck0Cmd_Ack
* Description  :发送数据确认
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_ReRecv0Cmd_Ack(INT8U *pdata, INT16U len)
{

	_RECV_DATA_CONTROL* pcontrol = APP_RecvDataControl(0);
	if(pcontrol == NULL)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if ((strstr((char *)pdata, "+IP ERROR: No data") != NULL) )
    {
		//表示无数据
        return FALSE;
    } 
//	AT+CIPRXGET=2,0
//	+CIPRXGET: 2,0,6,0
//	123456
	
//	OK
//	if ((strstr((char *)pdata, "OK") == NULL) )
//   {
//		//表示无数据
//       return FALSE;
//   } 
	char *poffset = strtok((char *)pdata + 18, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");  //为数据长度
	if(poffset == NULL)
	{
		return FALSE;
	}
	pcontrol->len = atoi(poffset);
	if(pcontrol->len > 500)
	{
		return FALSE;
	}
	//数据拷贝
	if(pcontrol->len < 10)
	{
		memcpy(pcontrol->DataBuf,poffset+5,pcontrol->len);
	}
	else if(pcontrol->len < 100)
	{
		memcpy(pcontrol->DataBuf,poffset+6,pcontrol->len);
	}
	else
	{
		memcpy(pcontrol->DataBuf,poffset+7,pcontrol->len);
	}
	pcontrol->RecvStatus = RECV_FOUND_DATA;  //有数据了，因为这个变量再同一个任务，故不要加锁
	return TRUE;
}

/*****************************************************************************
* Function     :Recv_AT_SendAck0Cmd_Ack
* Description  :发送数据确认
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_AT_ReRecv1Cmd_Ack(INT8U *pdata, INT16U len)
{
	_RECV_DATA_CONTROL* pcontrol = APP_RecvDataControl(1);
	if(pcontrol == NULL)
	{
		return FALSE;
	}
	pdata[len] = '\0';  //结束符
	if ((strstr((char *)pdata, "+IP ERROR: No data") != NULL) )
    {
		//表示无数据
        return FALSE;
    } 
//	AT+CIPRXGET=2,0
//	+CIPRXGET: 2,0,6,0
//	123456
	
//	OK
//	if ((strstr((char *)pdata, "OK") == NULL) )
//   {
//		//表示无数据
//       return FALSE;
//   } 
	char *poffset = strtok((char *)pdata + 18, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");
	if(poffset == NULL)
	{
		return FALSE;
	}
	poffset = strtok(NULL, ",");  //为数据长度
	if(poffset == NULL)
	{
		return FALSE;
	}
	pcontrol->len = atoi(poffset);
	if(pcontrol->len > 500)
	{
		return FALSE;
	}
	//数据拷贝
	if(pcontrol->len < 10)
	{
		memcpy(pcontrol->DataBuf,poffset+5,pcontrol->len);
	}
	else if(pcontrol->len < 100)
	{
		memcpy(pcontrol->DataBuf,poffset+6,pcontrol->len);
	}
	else
	{
		memcpy(pcontrol->DataBuf,poffset+7,pcontrol->len);
	}
	pcontrol->RecvStatus = RECV_FOUND_DATA;  //有数据了，因为这个变量再同一个任务，故不要加锁
	return TRUE;
}



/*****************************************************************************
* Function     :Recv_ActRecv0_Ack
* Description  ：有数据接收
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_ActRecv0_Ack(INT8U *pdata, INT16U len)
{
	pdata = pdata;
	len = len;
	
	Send_AT_CIPRXGET(0);		//去读取数据
	return TRUE;
}

/*****************************************************************************
* Function     :Recv_ActRecv1_Ack
* Description  ：有数据接收
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
static INT8U Recv_ActRecv1_Ack(INT8U *pdata, INT16U len)
{
	pdata = pdata;
	len = len;
	
	Send_AT_CIPRXGET(1);		//去读取数据
	return TRUE;
}

/*****************************************************************************
* Function     :Recv_ActRecv1_Ack
* Description  ：有数据接收
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_HttpStart_Ack(uint8_t *pdata, uint16_t len)
{

//[09:13:46.037]发→◇AT+HTTPINIT
//□
//[09:13:46.048]收←◆AT+HTTPINIT
//OK

    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符
    if ((strstr((char *)pdata, "OK") == NULL) )
    {
        //表示不成功
        return FALSE;
    }

    OSSchedLock();
    SIM7600CmdState.HTTP_Start = STATE_OK;
    OSSchedUnlock();

    return TRUE;
}

/*****************************************************************************
* Function     :Recv_FTPStart_Ack
* Description  ：
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_FTPStart_Ack(uint8_t *pdata, uint16_t len)
{

    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符
    if ((strstr((char *)pdata, "OK") == NULL) )
    {
        //表示不成功
        return FALSE;
    }

    OSSchedLock();
    SIM7600CmdState.FTP_Start = STATE_OK;
    OSSchedUnlock();

    return TRUE;
}

/*****************************************************************************
* Function     :Recv_FTPStart_Ack
* Description  ：
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_FTPLOGIN_Ack(uint8_t *pdata, uint16_t len)
{

    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符
    if ((strstr((char *)pdata, "OK") == NULL) )
    {
        //表示不成功
        return FALSE;
    }

    OSSchedLock();
    SIM7600CmdState.FTP_Login = STATE_OK;
    OSSchedUnlock();

    return TRUE;
}

/*****************************************************************************
* Function     :Recv_FTPStart_Ack
* Description  ：
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_CFTPSTYPE_Ack(uint8_t *pdata, uint16_t len)
{

    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符
    if ((strstr((char *)pdata, "OK") == NULL) )
    {
        //表示不成功
        return FALSE;
    }

    OSSchedLock();
    SIM7600CmdState.FTP_SetType = STATE_OK;
    OSSchedUnlock();

    return TRUE;
}

/*****************************************************************************
* Function     :Recv_FTPStart_Ack
* Description  ：
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_CFTPSGET_Ack(uint8_t *pdata, uint16_t len)
{

    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
#warning "FTP 需要处理"
    OSSchedLock();
    SIM7600CmdState.FTP_Read = STATE_OK;
    OSSchedUnlock();

    return TRUE;
}

/*****************************************************************************
* Function     :Recv_HttpConect_Ack
* Description  ：连接返回
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_HttpConect_Ack(uint8_t *pdata, uint16_t len)
{

//[09:14:48.781]发→◇AT+HTTPPARA="URL","http://14.205.92.144/dowload/project.hex"
//□
//[09:14:48.798]收←◆AT+HTTPPARA="URL","http://14.205.92.144/dowload/project.hex"
//OK
    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符
    if ((strstr((char *)pdata, "OK") == NULL) )
    {
        //表示不成功
        return FALSE;
    }
    OSSchedLock();
    SIM7600CmdState.HTTP_ConectState = STATE_OK;
    OSSchedUnlock();
    return TRUE;
}

/*****************************************************************************
* Function     :Recv_HttpConect_Ack
* Description  ：连接返回
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_HttpGet_Ack(uint8_t *pdata, uint16_t len)
{
//[09:15:18.284]发→◇AT+HTTPACTION=0
//□
//[09:15:18.291]收←◆AT+HTTPACTION=0
//OK

//[09:15:18.654]收←◆
//+HTTPACTION: 0,200,440836
    _RECV_DATA_CONTROL* pcontrol = APP_RecvDataControl(0);
    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符

//	if ((strstr((char *)pdata, "OK") == NULL) )
//	{
//		//表示不成功
//		return FALSE;
//	}

    char *poffset = strtok((char *)pdata, ",");
    if(poffset == NULL)
    {
        return FALSE;
    }
    poffset = strtok(NULL, ",");
    if(poffset == NULL)
    {
        return FALSE;
    }
    poffset = strtok(NULL, ",");
    if(poffset == NULL)
    {
        return FALSE;
    }
    pcontrol->AllLen = atoi((char*)poffset);
    if(pcontrol->AllLen == 0)
    {
        return FALSE;
    }
    OSSchedLock();
    SIM7600CmdState.HTTP_Get = STATE_OK;
    OSSchedUnlock();
    return TRUE;
}

/*****************************************************************************
* Function     :Recv_ReadData_Ack
* Description  ：读取数据
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_ReadData_Ack(uint8_t *pdata, uint16_t len)
{
//AT+HTTPREAD=0,1000
//OK

//+HTTPREAD: 1000
    char * pchar  = "+HTTPREAD:";
    uint16_t datalen;
	uint16_t num = 0;
    _RECV_DATA_CONTROL* pcontrol = APP_RecvDataControl(0);
    if ( (pdata == NULL) || !len)
    {
        return FALSE;
    }
    //
    pdata[len] = '\0';  //结束符

    if ((strstr((char *)pdata, "OK") == NULL) )
    {
        //表示不成功
        return FALSE;
    }
    char *poffset;


    //38
    poffset = pdata + 14;


    datalen = atoi((char*)poffset);
    if((datalen == 0) || (datalen > 1024) )
    {
        return FALSE;
    }
    if (datalen >= 1000)
    {
        poffset += 14;
    }
    else if (datalen >= 100)
    {
        poffset += 13;
    }
    else if (datalen >= 10)
    {
        poffset += 12;
    }
    else
    {
        poffset += 11;
    }







    datalen = 0;

	while(1)
	{
		if((poffset[num] >= '0') && (poffset[num] <= '9'))   //找到数字
		{
			break;
		}
		num++;
		if(num > 60)
		{
			 return FALSE;
		}
	}
    poffset += num;

    datalen= atoi((char*)poffset);
    if((datalen == 0) || (datalen > 1024) )
    {
        return FALSE;
    }
    pcontrol->len = datalen;
    if (pcontrol->len >= 1000)
    {
        poffset += 6;
    }
    else if (pcontrol->len >= 100)
    {
        poffset += 5;
    }
    else if (pcontrol->len >= 10)
    {
        poffset += 4;
    }
    else
    {
        poffset += 3;
    }

    if (((uint32_t)poffset - (uint32_t)pdata + pcontrol->len + 16) != len)
    {
        return FALSE;
    }
    memcpy(pcontrol->DataBuf,poffset,pcontrol->len);   //拷贝数据
    pcontrol->DownPackLen = pcontrol->len;           //单包的长度
    OSSchedLock();
    SIM7600CmdState.HTTP_Read = STATE_OK;
    OSSchedUnlock();
    return TRUE;
}

/*****************************************************************************
* Function     :Recv_HttpConect_Ack
* Description  ：连接返回
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
static uint8_t Recv_ReadDone_Ack(uint8_t *pdata, uint16_t len)
{
//[15:51:43.864]收←◆
//+HTTPREAD: 0
    OSSchedLock();
    SIM7600CmdState.HTTP_ReadDone = STATE_OK;
    OSSchedUnlock();
    return TRUE;
}
//GPRS接收表
static const _4G_AT_FRAME 	TIM7600_RecvFrameTable[DIM7600_RECV_TABLE_LEN] = 
{

	{"AT+CSQ"						,      Recv_AT_CSQ_Ack							},		//读取信号强度返回
	
	{"AT+CIMI"         			    ,  	   Recv_AT_CIMI_Ack							},		//读取卡号返回
	
	{"AT+CGDCONT"                   ,      Recv_AT_CGDCONT_Ack                    	},		//配置PDP返回
	
	{"AT+CIPMODE"                 	,      Recv_AT_CIPMODE_Ack                       	},		//数据透传返回
	
	{"AT+NETOPEN"                 	,      Recv_AT_NETOPEN_Ack                       	},		//激活PDP
	
	{"AT+CIPOPEN=0"                 ,      Recv_AT_ConnectServer0Cmd_Ack               },		//连接服务器指令返回
	
	{"AT+CIPOPEN=1"                 ,      Recv_AT_ConnectServer1Cmd_Ack               },		//连接服务器指令返回
	
	
	{"+CIPOPEN: 0"                 ,      Recv_AT_ConnectServer0_Ack               },		//连接服务器返回
	
	{"+CIPOPEN: 1"                 ,      Recv_AT_ConnectServer1_Ack               },		//连接服务器返回
	
	{"OK"							,      Recv_AT_EnterATCmd_Ack					},		//进入AT配置指令返回
	
	{"AT+CIPSEND=0"					,      Recv_AT_SendAck0Cmd_Ack					},		//发送数据确认
	
	{"AT+CIPSEND=1"					,      Recv_AT_SendAck1Cmd_Ack					},		//发送数据确认
	
	{"AT+CIPRXGET=2,0"				,      Recv_AT_ReRecv0Cmd_Ack					},		//接收返回
	
	{"AT+CIPRXGET=2,1"				,      Recv_AT_ReRecv1Cmd_Ack					},		//接收返回
	
	{"AT+CIPRXGET=1"				,      Recv_AT_SetReAct_Ack						},		//设置接收主动获取
	
	{"\r\n+CIPRXGET: 1,0"			,	   Recv_ActRecv0_Ack						},		//0号主动接收数据
	
	{"\r\n+CIPRXGET: 1,1"			,	   Recv_ActRecv1_Ack						},		//1号主动接收数据
	
	{"AT+HTTPINIT"					,		Recv_HttpStart_Ack						},		//开始返回

    {"AT+HTTPPARA=\"URL\""			,		Recv_HttpConect_Ack						},		//注册返回

    {"\r\n+HTTPACTION:"				,		Recv_HttpGet_Ack						},		//读取数据返回长度

    {"\r\n+HTTPREAD: 0"				,		Recv_ReadDone_Ack							},		//读取完成

    {"AT+HTTPREAD=0"				,		Recv_ReadData_Ack							},		//读取数据
//	
	{"AT+CFTPSSTART"				,		Recv_FTPStart_Ack						},			//开始FTP应答

	{"AT+CFTPSLOGIN"				,		Recv_FTPLOGIN_Ack						},			//FTP登录应答
//	
//	{"AT+CFTPSTYPE"					,		Recv_CFTPSTYPE_Ack						},			//FTP 传输类型应答
//	
//	{"\r\n+CFTPSGET:"				,		Recv_CFTPSGET_Ack						},			//获取数据返回
};



/*****************************************************************************
* Function     :Module_SIM7600Test
* Description  :模块是否存在
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U Module_SIM7600Test(void)
{
    static INT8U i = 0;
//	INT8U err;
//	static INT8U count = 1;
    
	//读取卡号
	Send_AT_CIMI();
	//等待2s回复
	i = 0;
	while (i < SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		OSTimeDly(SYS_DELAY_100ms);
		//等待AT指令被回复
		if (SIM7600CmdState.State0.OneByte.ATCMD_CIMI == _4G_RESPOND_OK)
		{
			SIM7600CmdState.State0.OneByte.ATCMD_CIMI = _4G_RESPOND_ERROR;
			break;
		}
		i++;
	}
	  //判断是否回复超时
	if (i == SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		printf("respond cmd:ATCMD_CIMI timeout");
		return FALSE;
	}
	
	
	//读取信号强度
	Send_AT_CSQ();
	//等待2s回复
	i = 0;
	while (i < SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		OSTimeDly(SYS_DELAY_100ms);
		//等待AT指令被回复
		if (SIM7600CmdState.State0.OneByte.ATCMD_CSQ == _4G_RESPOND_OK)
		{
			SIM7600CmdState.State0.OneByte.ATCMD_CSQ = _4G_RESPOND_ERROR;
			break;
		}
		i++;
	}
	//判断是否回复超时
	if (i == SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		printf("respond cmd:ATCMD_CSQ timeout");
		return FALSE;
	}
	
	//配置pdp
	Send_AT_CGDCONT();
		//等待2s回复
	i = 0;
	while (i < SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		OSTimeDly(SYS_DELAY_100ms);
		//等待AT指令被回复
		if (SIM7600CmdState.State0.OneByte.ATCMD_CGDCONT == _4G_RESPOND_OK)
		{
			SIM7600CmdState.State0.OneByte.ATCMD_CGDCONT = _4G_RESPOND_ERROR;
			break;
		}
		i++;
	}
	  //判断是否回复超时
	if (i == SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		printf("respond cmd:ATCMD_CGDCONT timeout");
		return FALSE;
	}
	
	//设置为透传模式
	if(NetConfigInfo[SYSSet.NetYXSelct].NetNum == 1)
	{
	
		Send_AT_CIPMODE();
	}
	else
	{
		//设置数据为非透传
		Send_AT_NOTCIPMODE();
	}

	i = 0;
	while (i < SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		OSTimeDly(SYS_DELAY_100ms);
		//等待AT指令被回复
		if (SIM7600CmdState.State0.OneByte.ATCMD_CIPMODE == _4G_RESPOND_OK)
		{
			SIM7600CmdState.State0.OneByte.ATCMD_CIPMODE = _4G_RESPOND_ERROR;
			break;
		}
		i++;
	}
	  //判断是否回复超时
	if (i == SYS_DELAY_2s / SYS_DELAY_100ms)
	{
		printf("respond cmd:ATCMD_CIPMODE timeout");
		return FALSE;
	}

	if(NetConfigInfo[SYSSet.NetYXSelct].NetNum > 1)
	{
		Send_AT_SetCIPRXGET(); //接收手动获取
		i = 0;
		while (i < SYS_DELAY_2s / SYS_DELAY_100ms)
		{
			OSTimeDly(SYS_DELAY_100ms);
			//等待AT指令被回复
			if (SIM7600CmdState.State0.OneByte.ATCMD_SETCIPRXGET == _4G_RESPOND_OK)
			{
				SIM7600CmdState.State0.OneByte.ATCMD_SETCIPRXGET = _4G_RESPOND_ERROR;
				break;
			}
			i++;
		}
		  //判断是否回复超时
		if (i == SYS_DELAY_2s / SYS_DELAY_100ms)
		{
			printf("respond cmd:ATCMD_CIPMODE timeout");
			return FALSE;
		}
	}
	
	SIM7600CmdState.SIM_ExistState = STATE_OK;		//模块存在

	return TRUE;
}

/*****************************************************************************
* Function     :ModuleSIM7600_ConnectServer
* Description  :连接服务器
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U ModuleSIM7600_ConnectServer(INT8U num,INT8U* pIP,INT16U port)
{
	static INT8U i = 0;
	//激活PDP
	Send_AT_NETOPEN();		
	i = 0;
	while (i < (SYS_DELAY_2s / SYS_DELAY_100ms) )
	{
		OSTimeDly(SYS_DELAY_100ms);
		//等待AT指令被回复
		if (SIM7600CmdState.State0.OneByte.ATCMD_NETOPEN == _4G_RESPOND_OK)
		{
			SIM7600CmdState.State0.OneByte.ATCMD_NETOPEN = _4G_RESPOND_ERROR;
			break;
		}
		i++;
	}
	  //判断是否回复超时
	if (i >= (SYS_DELAY_2s / SYS_DELAY_100ms) )
	{
		printf("respond cmd:ATCMD_NETOPEN timeout\r\n");
		return FALSE;
	}
	
	
	Module_ConnectServer(num,pIP,port);		
	i = 0;
	while (i < (SYS_DELAY_5s / SYS_DELAY_100ms) )
	{
		OSTimeDly(SYS_DELAY_100ms);
		//等待AT指令被回复
		if (SIM7600CmdState.ATCMD_CIPOPEN[num] == _4G_RESPOND_OK)
		{
			SIM7600CmdState.ATCMD_CIPOPEN[num] = _4G_RESPOND_ERROR;
			break;
		}
		i++;
	}
	  //判断是否回复超时
	if (i >= (SYS_DELAY_5s / SYS_DELAY_100ms))
	{
		printf("Connect_to_server_timeout\r\n");
		return FALSE;
	}
	
	SIM7600CmdState.SIM_ConectState[num] = STATE_OK;		//已经连接上了服务器
	printf("lianjiePT%d \r\n",num); //连接第几平台
	return TRUE;
}	


/*****************************************************************************
* Function     :ModuleSIM7600_SendData
* Description  :发送数据
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U ModuleSIM7600_SendData(INT8U num,INT8U* pdata,INT16U len)
{
	INT8U i = 0;
	
	num = num;
	i = 0;
	
	if(NetConfigInfo[SYSSet.NetYXSelct].NetNum > 1)
	{
		if((num > 10) || (pdata == NULL) )
		{
			return FALSE;
		}
		Send_AT_CIPSEND(num,len);
		while (i < SYS_DELAY_10s / SYS_DELAY_50ms)
		{
			OSTimeDly(SYS_DELAY_50ms);
			//等待AT指令被回复
			if (SIM7600CmdState.ATCMD_SENDACK[num] == _4G_RESPOND_OK)
			{
				SIM7600CmdState.ATCMD_SENDACK[num] = _4G_RESPOND_ERROR;
				break;
			}
			i++;
			if((i % 20) == 0) //1s钟没回复 就继续发送
			{
				Send_AT_CIPSEND(num,len);       //多发比发不出去好！
			}
			
		}
		if (i == SYS_DELAY_10s / SYS_DELAY_50ms)
		{
			APP_SetNetNotConect(num);
			//SIM7600CmdState.SIM_ConectState[num] = STATE_ERR;  //等待2s还没发送数据，则重新连接网络
			printf("ATCMD_SENDACK_timeout+Duannet_PT:%d \r\n",num);
			return FALSE;
		}
	}
	//pdata[len] = 0;
	//printf("GPRS recv [%s]", pdata);  
	#if(NET_SENDRECV_PRINTF)
	printfchar_NETsend(pdata,len); //接收打印
	#endif
	//真正发送数据
	if (UART_4GWrite(pdata,len ) != len )
    {
        return FALSE;
    }
	if((SYSSet.NetYXSelct == XY_YKC)||(SYSSet.NetYXSelct == XY_TT)|| (SYSSet.NetYXSelct == XY_YL3)|| (SYSSet.NetYXSelct == XY_YL4))   //连云快充双平台的时候，发送数据很快
	{
		OSTimeDly(SYS_DELAY_500ms);
	}
	else
	{
		OSTimeDly(SYS_DELAY_1s);
	}
	return TRUE;
	
}

/*****************************************************************************
* Function     :ModuleSIM7600_ReadData
* Description  :读取数据请求
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U ModuleSIM7600_ReadData(INT8U num)
{
	if(num > 10)
	{
		return FALSE;
	}
	Send_AT_CIPRXGET(num);
	return TRUE;
}

/*****************************************************************************
* Function     :SIM7600_RecvDesposeCmd
* Description  :命令模式下模块接收处理
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U SIM7600_RecvDesposeCmd(INT8U *pdata,INT16U len)
{
	INT8U i;
	
	if((pdata == NULL) || (!len))
	{
		return FALSE;
	}
	for(i = 0;i < DIM7600_RECV_TABLE_LEN;i++)
	{
		
		if (!strncmp( (char *)pdata, TIM7600_RecvFrameTable[i].ATCode, MIN(strlen(TIM7600_RecvFrameTable[i].ATCode),len) ) )     
		{
			TIM7600_RecvFrameTable[i].Fun(pdata,len);
			return TRUE;
		}
	}
	return FALSE;
	
}

/*****************************************************************************
* Function     :APP_SetNetNotConect
* Description  :设置网络未连接
* Input        :
* Output       :
* Return       : 
* Note(s)      : 
* Contributor  : 2018年7月11日
*****************************************************************************/
INT8U APP_SetNetNotConect(INT8U num)
{
	if(num >= NetConfigInfo[SYSSet.NetYXSelct].NetNum)
	{
		return FALSE;
	}
	if(num == 0)   //主平台
	{
		SIM7600CmdState.SIM_ExistState = STATE_4G_ERR;   //只有一个模块的时候需要重新启动
		CSQNum = 0;			//信号强度为0
	}
	SIM7600CmdState.SIM_ConectState[num] = STATE_4G_ERR;
	SIM7600CmdState.SIM_Regiset[num] = STATE_4G_ERR;
	SIM7600CmdState.ATCMD_CIPOPEN[num] = STATE_4G_ERR;
	return TRUE;
}

/*****************************************************************************
* Function     :Module_HTTPDownload
* Description  :HTTP下载
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
uint8_t Module_HTTPDownload(_HTTP_INFO *info)
{
    _RECV_DATA_CONTROL* pcontrol = APP_RecvDataControl(0);
    static uint32_t i,a= 0;
    static uint32_t downtime = 0,lastdowntime;
    uint32_t  down_len = 0;    //下载的长度
    uint8_t  down_state = 0x55;		//表示升级成功

    if(info == NULL)
    {
        return FALSE;
    }
    if(info->ServerAdd[0] == 0)
    {
        return FALSE;
    }

	OSTimeDly(SYS_DELAY_2s); 
    Send_AT_EnterATCmd();
	OSTimeDly(SYS_DELAY_2s); 
    Send_AT_HTTPTERM();
    OSTimeDly(SYS_DELAY_1s); 

    //开始http
    Send_AT_HTTPINIT();
    //等待2s回复
    i = 0;
    while (i < SYS_DELAY_5s / SYS_DELAY_100ms)
    {
		OSTimeDly(SYS_DELAY_100ms); 
        //等待AT指令被回复
        if (SIM7600CmdState.HTTP_Start== STATE_OK)
        {
            SIM7600CmdState.HTTP_Start = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_5s / SYS_DELAY_100ms)
    {
        printf("respond cmd:HTTPINIT timeout");
        return FALSE;
    }


    //连接http服务器
    Send_AT_HTTPPAR(info->ServerAdd);
    //等待2s回复
    i = 0;
    while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
    {
       OSTimeDly(SYS_DELAY_50ms); 
        //等待AT指令被回复
        if (SIM7600CmdState.HTTP_ConectState== STATE_OK)
        {
            SIM7600CmdState.HTTP_ConectState = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_2s / SYS_DELAY_50ms)
    {
        printf("respond cmd:Send_AT_HTTPPAR timeout");
        return FALSE;
    }


    //密码登录
//    if(info->ServerPassword[0] != 0)   //说明有密码
//    {
//        //密码验证
//        Send_AT_HTTPPAR_PASSWORD(info->ServerPassword);
//        i = 0;
//        while (i < CM_TIME_2_SEC / CM_TIME_50_MSEC)
//        {
//            OSTimeDly(CM_TIME_50_MSEC, OS_OPT_TIME_PERIODIC, &timeerr);
//            //等待AT指令被回复
//            if (SIM7600CmdState.HTTP_PasswordState== STATE_OK)
//            {
//                SIM7600CmdState.HTTP_PasswordState = STATE_ERR;
//                break;
//            }
//            i++;
//        }
//        //判断是否回复超时
//        if (i == CM_TIME_2_SEC / CM_TIME_50_MSEC)
//        {
//            printf("respond cmd:mima timeout");
//            return FALSE;
//        }
//    }


    //Get
    Send_AT_GET();
    //等待2s回复
    i = 0;
    while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
    {
       OSTimeDly(SYS_DELAY_50ms); 
        //等待AT指令被回复
        if (SIM7600CmdState.HTTP_Get== STATE_OK)
        {
            SIM7600CmdState.HTTP_Get = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_2s / SYS_DELAY_50ms)
    {
        printf("respond cmd:Send_AT_GET timeout");
        return FALSE;
    }

    if( pcontrol->AllLen > (1024*500) )   //数据总长度不能操作200k
    {
        printf("http data too len");
        return FALSE;
    }



    downtime =OSTimeGet();
    lastdowntime = downtime;
    //read
    while(!SIM7600CmdState.HTTP_ReadDone)
    {
        if(down_len > pcontrol->AllLen)
        {
            return FALSE;
        }
        if(down_len == pcontrol->AllLen)
        {

			BootLoadWrite((INT8U*)&down_len,1,sizeof(down_len));
            SIM7600CmdState.HTTP_ReadDone = STATE_OK;    //说明下载成功
			BootLoadWrite(&down_state,0,sizeof(down_state));
            break;
        }
        if((pcontrol->AllLen - down_len) > 1000)
        {
            Send_AT_READ(1000);   //一次一次读取
        }
        else
        {
            Send_AT_READ(pcontrol->AllLen - down_len);   //一次一次读取
        }
        downtime =OSTimeGet();

        if(SIM7600CmdState.HTTP_ReadDone== STATE_OK)
        {
            //校验，返回下载成功
            if(down_len == pcontrol->AllLen)
            {
				BootLoadWrite((INT8U*)&down_len,1,sizeof(down_len));
				SIM7600CmdState.HTTP_ReadDone = STATE_OK;    //说明下载成功
				BootLoadWrite(&down_state,0,sizeof(down_state));
            }
            break;
        }
        if((downtime - lastdowntime)  > (SYS_DELAY_2M * 2))   //2分钟没又下载完成说明超时了
        {
            printf("respond cmd:HTTP READ timeout");
            return FALSE;
        }


        //等待2s回复
        //i = 0;
        i = 0;
		while (i < SYS_DELAY_2s / SYS_DELAY_10ms)
        {
            OSTimeDly(SYS_DELAY_10ms); 
            //等待AT指令被回复
            if (SIM7600CmdState.HTTP_Read== STATE_OK)
            {
                SIM7600CmdState.HTTP_Read = STATE_4G_ERR;

                //数据存储
				BootLoadWrite(pcontrol->DataBuf,down_len + 5,pcontrol->DownPackLen);
                down_len += pcontrol->DownPackLen;
                break;
            }
            i++;
        }
		printf("udata down_len %d",down_len);
        //判断是否回复超时
		while (i >= (SYS_DELAY_2s / SYS_DELAY_10ms))
        {
            printf("respond cmd:HTTP READ timeout");
            return FALSE;
        }
    }
    Send_AT_HTTPTERM();  //关闭http服务
	OSTimeDly(SYS_DELAY_1s); 
    return TRUE;
}

uint8_t test1 = 0,test2 = 0,test3 = 0,test4 = 0,test5 = 0,test6 = 0,test7 = 0,test8 = 0,test9 = 0,test10 = 0,test11 = 0;
void ftp_test(void)
{
	static uint8_t reset = 1;
	if(reset)
	{
		reset = 0;
		SIM7600Reset();
	}
	if(test1)
	{
		test1 = 0;
		 Send_AT_CFTPSSTART();
	}
	if(test2)
	{
		test2 = 0;
		UART_4GWrite((uint8_t*)"AT+CFTPSLOGIN=\"139.198.183.65\",21,\"ftptest\",\"lcye3437563\",0\r\n", strlen("AT+CFTPSLOGIN=\"139.198.183.65\",21,\"ftptest\",\"lcye3437563\",0\r\n")  );
    
	}
	if(test3)
	{	
		test3 = 0;
		UART_4GWrite((uint8_t*)"AT+CFTPSLIST=\"/\"\r\n", strlen("AT+CFTPSLIST=\"/\"\r\n")  );
	}
	if(test4)
	{	
		test4 = 0;
		Send_AT_CSQ();
	}
	if(test5)
	{	
		test5 = 0;
		UART_4GWrite((uint8_t*)"AT+CGACT=1,1\r\n", strlen("AT+CGACT=1,1\r\n")  );  //激活
	}
	if(test6)
	{	
		test6 = 0;
		Send_AT_CFTPSTYPE();
	}
	
	if(test7)
	{	
		test7 = 0;
		UART_4GWrite((uint8_t*)"AT+CFTPSGET=?\r\n", strlen("AT+CFTPSGET=?\r\n")  );  //激活
	}
	
	
	if(test8)
	{	
		test8 = 0;
		UART_4GWrite((uint8_t*)"AT+CFTPSPWD\r\n", strlen("AT+CFTPSPWD\r\n")  );  //获取当前
	}
	if(test9)
	{	
		test9 = 0;
		UART_4GWrite((uint8_t*)"AT+CFTPSGET=\"/download/rtthread.rbl\",500\r\n", strlen("AT+CFTPSGET=\"/download/rtthread.rbl\",500\r\n")  );  //获取当前
	}

}
/*****************************************************************************
* Function     :Module_FatDownload
* Description  :FTP下载
* Input        :
* Output       :
* Return       :
* Note(s)      :
* Contributor  : 2018年7月11日
*****************************************************************************/
uint8_t Module_FTPDownload(_FTP_INFO *info)
{
    _RECV_DATA_CONTROL* pcontrol = APP_RecvDataControl(0);
    static uint32_t i,a= 0;
    static uint32_t downtime = 0,lastdowntime;
    uint32_t  down_len = 0;    //下载的长度
    uint8_t  down_state = 0x55;		//表示升级成功

    if(info == NULL)
    {
        return FALSE;
    }
    if(info->SerAdd[0] == 0)
    {
        return FALSE;
    }

	OSTimeDly(SYS_DELAY_2s); 
    Send_AT_EnterATCmd();
	OSTimeDly(SYS_DELAY_2s); 
//    Send_AT_HTTPTERM();
//    OSTimeDly(SYS_DELAY_1s); 

    //开始http
    Send_AT_CFTPSSTART();
    //等待2s回复
    i = 0;
    while (i < SYS_DELAY_2s / SYS_DELAY_100ms)
    {
		OSTimeDly(SYS_DELAY_2s); 
        //等待AT指令被回复
        if (SIM7600CmdState.FTP_Start== STATE_OK)
        {
            SIM7600CmdState.FTP_Start = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_2s / SYS_DELAY_100ms)
    {
        printf("respond cmd:HTTPINIT timeout");
        return FALSE;
    }
	OSTimeDly(SYS_DELAY_2s); 

    //连接http服务器
    Send_AT_CFTPSLOGIN((char*)info->SerAdd,info->Port,(char*)info->usename,(char*)info->password);
    //等待2s回复
    i = 0;
    while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
    {
       OSTimeDly(SYS_DELAY_50ms); 
        //等待AT指令被回复
        if (SIM7600CmdState.FTP_Login== STATE_OK)
        {
            SIM7600CmdState.FTP_Login = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_2s / SYS_DELAY_50ms)
    {
        printf("respond cmd:Send_AT_HTTPPAR timeout");
        return FALSE;
    }

	//配置为BIN传输
	Send_AT_CFTPSTYPE();
	//等待2s回复
    i = 0;
    while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
    {
       OSTimeDly(SYS_DELAY_50ms); 
        //等待AT指令被回复
        if (SIM7600CmdState.HTTP_ConectState== STATE_OK)
        {
            SIM7600CmdState.HTTP_ConectState = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_2s / SYS_DELAY_50ms)
    {
        printf("respond cmd:Send_AT_HTTPPAR timeout");
        return FALSE;
    }
   
    //Get
    Send_AT_CFTPSGET((char*)info->FilePath);
    //等待2s回复
    i = 0;
    while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
    {
       OSTimeDly(SYS_DELAY_50ms); 
        //等待AT指令被回复
        if (SIM7600CmdState.HTTP_Get== STATE_OK)
        {
            SIM7600CmdState.HTTP_Get = STATE_4G_ERR;
            break;
        }
        i++;
    }
    //判断是否回复超时
    if (i == SYS_DELAY_2s / SYS_DELAY_50ms)
    {
        printf("respond cmd:Send_AT_GET timeout");
        return FALSE;
    }

    if( pcontrol->AllLen > (1024*500) )   //数据总长度不能操作200k
    {
        printf("http data too len");
        return FALSE;
    }



    downtime =OSTimeGet();
    lastdowntime = downtime;
    //read
    while(!SIM7600CmdState.HTTP_ReadDone)
    {
        if(down_len > pcontrol->AllLen)
        {
            return FALSE;
        }
        if(down_len == pcontrol->AllLen)
        {

			BootLoadWrite((INT8U*)&down_len,1,sizeof(down_len));
            SIM7600CmdState.HTTP_ReadDone = STATE_OK;    //说明下载成功
			BootLoadWrite(&down_state,0,sizeof(down_state));
            break;
        }
        if((pcontrol->AllLen - down_len) > 500)
        {
            Send_AT_READ(500);   //一次一次读取
        }
        else
        {
            Send_AT_READ(pcontrol->AllLen - down_len);   //一次一次读取
        }
        downtime =OSTimeGet();

        if(SIM7600CmdState.HTTP_ReadDone== STATE_OK)
        {
            //校验，返回下载成功
            if(down_len == pcontrol->AllLen)
            {
				BootLoadWrite((INT8U*)&down_len,1,sizeof(down_len));
				SIM7600CmdState.HTTP_ReadDone = STATE_OK;    //说明下载成功
				BootLoadWrite(&down_state,0,sizeof(down_state));
            }
            break;
        }
        if((downtime - lastdowntime)  > (SYS_DELAY_1M * 2))   //2分钟没又下载完成说明超时了
        {
            printf("respond cmd:HTTP READ timeout");
            return FALSE;
        }


        //等待2s回复
        //i = 0;
        i = 0;
		while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
        {
            OSTimeDly(SYS_DELAY_50ms); 
            //等待AT指令被回复
            if (SIM7600CmdState.HTTP_Read== STATE_OK)
            {
                SIM7600CmdState.HTTP_Read = STATE_4G_ERR;

                //数据存储
				BootLoadWrite(pcontrol->DataBuf,down_len + 5,pcontrol->DownPackLen);
                down_len += pcontrol->DownPackLen;
                break;
            }
            i++;
        }
        //判断是否回复超时
		while (i < SYS_DELAY_2s / SYS_DELAY_50ms)
        {
            printf("respond cmd:HTTP READ timeout");
            return FALSE;
        }
    }
    Send_AT_HTTPTERM();  //关闭http服务
	OSTimeDly(SYS_DELAY_1s); 
    return TRUE;
}
/* Private macro------------------------------------------------------------------------------*/	

/* Private variables--------------------------------------------------------------------------*/
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/

/************************(C)COPYRIGHT 2022 杭州汇誉*****END OF FILE****************************/

