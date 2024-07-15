﻿/****************************************Copyright (c)****************************************************
**                               HangzhouKuaidianXinnengyuan Co.,LTD.
**                                				EFC 
**
**--------------File Info---------------------------------------------------------------------------------
** File name:          
** Last modified Date:  
** Last Version:       
** Descriptions:        
**--------------------------------------------------------------------------------------------------------
** Created by:          
** Created date:        
** Version:             
** Descriptions:       
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
** Rechecked by:
*********************************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "version.h"
#include "app_conf.h"
/***********************************************************************************************
* Function		: SysSoftwareVersionGet
* Description	: 
* Input			: 
* Output		: 
* Note(s)		: 
* Contributor	:
***********************************************************************************************/
void SysSoftwareVersionGet(char* str_ver)
{
    volatile char *tmp_ver_info = SYS_APP_VER;
    
    char buf[SYS_VER_INFO_LEN]={0};

    memcpy(buf,(char *)&tmp_ver_info[0],strlen(SYS_APP_VER));
    
    if(NULL != str_ver)
    {
        memcpy(str_ver,buf,strlen(SYS_APP_VER));
    }
}

/*********************************************************************************************************
  END OF FILE
*********************************************************************************************************/ 
