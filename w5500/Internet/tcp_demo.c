/**
******************************************************************************
* @file    			tcp_demo.c                                               
* @version 			V1.2										  		
* @date    			2017-11-01								 
* @brief 			  TCP ��ʾ����     					        
*
* @company  		��������Ƽ����޹�˾
* @information  WIZnet W5500�ٷ������̣�ȫ�̼���֧�֣��۸�������ƣ�
* @website  		www.wisioe.com																							 	 				
* @forum        www.w5500.cn																								 		
* @qqGroup      383035001																										 
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include "tcp_demo.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"

//uint8 buff[2048];				                              	        /*����һ��2KB�Ļ���*/

///**
//*@brief		TCP Server�ػ���ʾ������
//*@param		��
//*@return	��
//*/
//void do_tcp_server(void)
//{	
//	uint16 len=0;  
//	switch(getSn_SR(SOCK_TCPS))											            	/*��ȡsocket��״̬*/
//	{
//		case SOCK_CLOSED:													                  /*socket���ڹر�״̬*/
//			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*��socket*/
//			break;     
//		
//		case SOCK_INIT:														                  /*socket�ѳ�ʼ��״̬*/
//			listen(SOCK_TCPS);												                /*socket��������*/
//			break;
//		
//		case SOCK_ESTABLISHED:												              /*socket�������ӽ���״̬*/
//		
//			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
//			{
//				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*��������жϱ�־λ*/
//			}
//			len=getSn_RX_RSR(SOCK_TCPS);									            /*����lenΪ�ѽ������ݵĳ���*/
//			if(len>0)
//			{
//				recv(SOCK_TCPS,buff,len);								              	/*��������Client������*/
//				buff[len]=0x00; 											                  /*�����ַ���������*/
//				printf("%s\r\n",buff);
//				send(SOCK_TCPS,buff,len);									              /*��Client��������*/
//			}
//			break;
//		
//		case SOCK_CLOSE_WAIT:												                /*socket���ڵȴ��ر�״̬*/
//			close(SOCK_TCPS);
//			break;
//	}
//}

/**
*@brief		TCP Client�ػ���ʾ������
*@param		��
*@return	��
*/
uint8 do_tcp_client(void)
{		
	uint8 status = SOCK_CLOSED;

	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
		  printf("��ʼ��\r\n");
			socket(SOCK_TCPC,Sn_MR_TCP,local_port,Sn_MR_ND);
			status = SOCK_CLOSED;
			break;

		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
		  printf("����\r\n");
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
			status = SOCK_INIT;
			break;

	case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
		  if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
		  {
		  	setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
		  }
//      
//		  len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
//		  if(len>0)
//		  {
//		  	recv(SOCK_TCPC,buff,len); 							   		         /*��������Server������*/
//		  	buff[len]=0x00;  											                 /*�����ַ���������*/
//		  	printf("%s\r\n",buff);
//		  	send(SOCK_TCPC,buff,len);								     	         /*��Server��������*/
//		  }		
		status = SOCK_ESTABLISHED;
		//printf("���ӳɹ�\r\n");	
		 break;
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																										
	case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
		  printf("�ر�\r\n");
		  close(SOCK_TCPC);
		status = SOCK_CLOSE_WAIT;
		  break;
	}
	return status;
}