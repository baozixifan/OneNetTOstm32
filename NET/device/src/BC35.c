/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	BC35.c
	*
        *	���ߣ� 		leigong
	*
	*	���ڣ� 		2017-10-20
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		BC35����
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸����
#include "BC35.h"
#include "oled.h"
//Ӳ������
#include "delay.h"
#include "usart.h"

//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "HEXSTR.h"


#define BC35_ONENET_INFO "AT+NSOCO=1,183.230.40.39,6002\r\n"  //"AT+IPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"


unsigned char BC35_buf[128];        //����ģ��ķ�����Ϣ
unsigned short BC35_cnt = 0, BC35_cntPre = 0;


//==========================================================
//	�������ƣ�	BC35_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void BC35_Clear(void)
{

    memset(BC35_buf, 0, sizeof(BC35_buf));
    BC35_cnt = 0;

}

//==========================================================
//	�������ƣ�	BC35_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool BC35_WaitRecive(void)
{

    if(BC35_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
        return REV_WAIT;

    if(BC35_cnt == BC35_cntPre)                                             //�����һ�ε�ֵ�������ͬ����˵���������
    {
        BC35_cnt = 0;							//��0���ռ���

        return REV_OK;							//���ؽ�����ɱ�־
    }

    BC35_cntPre = BC35_cnt;                                                 //��Ϊ��ͬ

    return REV_WAIT;							//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	BC35_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool BC35_SendCmd(char *cmd, char *res)
{

    unsigned char timeOut = 200;

    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
    DelayXms(200);
    while(timeOut--)
    {
        if(BC35_WaitRecive() == REV_OK)					//����յ�����
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            if(strstr((const char *)BC35_buf, res) != NULL)		//����������ؼ���
            {
                BC35_Clear();                                           //��ջ��� �ɹ�����0
                return 0;
            }
        }

        DelayXms(10);
    }

    return 1;

}




//==========================================================
//	�������ƣ�	BC35_SendREVCMD
//
//	�������ܣ�	�������ݺ����ݳ��ȣ�����鷵����Ϣ
//
//	��ڲ�����	cmd������
//              len: �����
//				      res����Ҫ���Ĳ��ַ�������
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool BC35_SendREVCMD(char *cmd,unsigned short len,char *res)
{

    unsigned char timeOut = 200;
	  char flag = 0;

    Usart_SendString(USART2, (unsigned char *)cmd, len);
    DelayXms(200);

	    while(timeOut--)
    {
        if(BC35_WaitRecive() == REV_OK)					//����յ�����
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            if(strstr((const char *)BC35_buf, res) != NULL)		//����������ؼ���
            {
                BC35_Clear();                                           //��ջ��� �ɹ�����0
                return 0;
            }
						else if((flag == 0) && (strstr((const char *)BC35_buf, "ERROR") != NULL))                                            //�������������ؼ��ʣ���800ms���ط�һ��
						{
							flag = 1;
							UsartPrintf(USART_DEBUG,"flag = 1");
							BC35_Clear();
							DelayXms(800);
							Usart_SendString(USART2, (unsigned char *)cmd, len);
						}

        }

        DelayXms(10);
    }

    return 1;

}



//==========================================================
//	�������ƣ�	BC35_SendRevCmd
//
//	�������ܣ�	���������ȡ��������
//
//	��ڲ�����	cmd������
//				      res����Ҫ���Ĳ��ַ�������
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool BC35_SendRevCmd(char *cmd, char *res)
{

    BC35_Clear();

    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
    DelayXms(200);

    return 0;
}

 //==========================================================
//	�������ƣ�	BC35_SENDDATA
//
//	�������ܣ�	�����ַ���������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
// sprintf((char*)(SendBuf),"%s,%d,%s\r\n","AT+NSOSD=1",len,(char *)data);
//	˵����		
//==========================================================
void BC35_SENDDATA(unsigned char *data, unsigned short len)
{
    char SendBuf[500];
		int cmd_len = 0;//BC35�������ݰ�ǰ�������
		int n = 0;//���ݿ���������
		int i = 0;//���������������ݰ�������
		int total_len = 0;//�����������ݰ��ĳ���
	
    memset(SendBuf, 0, sizeof(SendBuf));
    sprintf((char*)(SendBuf),"%s,%d,","AT+NSOSD=1",len/2);
		cmd_len = strlen(SendBuf);
	    
		  for(i = cmd_len;i < cmd_len+len; i++)
		{		     
	         SendBuf[i] = data[n++];//���ַ������е��ַ�һ��һ����ת�뻺��
		}
		SendBuf[i++] = '\r';
		SendBuf[i++] = '\n';
		SendBuf[i++] = '\0';

		total_len = cmd_len + len + 2;
		
				for (int j = 0; j < total_len; j++)
		{
			UsartPrintf(USART_DEBUG,"SendBuf[%d] = %c\r\n", j, SendBuf[j]);//���ת������ַ������ݰ�
		}


    if(!BC35_SendREVCMD(SendBuf,total_len, "OK"))				//�յ���OK��ʱ���Է�������
    {
			  UsartPrintf(USART_DEBUG,"*********BC35_SendREVCMD**********\r\n");
        DelayXms(500);
			
//		  BC35_SendREVCMD("AT+NSORF=1,100\r\n",16,"OK");
 //       Usart_SendString(USART2,"AT+NSORF=1,100\r\n",16);	//�����豸������������

 //       DelayXms(500);

        UsartPrintf(USART_DEBUG,"Send OK\r\n");


    }

}  


//==========================================================
//	�������ƣ�	BC35_SendData
//
//	�������ܣ�	�����ַ�������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
//void BC35_SendData(unsigned char *data, unsigned short len)
//{
//    char SendBuf[500];
//    memset(SendBuf, 0, sizeof(SendBuf));
//    sprintf((char*)(SendBuf),"%s,%d,%s\r\n","AT+NSOSD=1",len,(char *)data);
//    UsartPrintf(USART_DEBUG,(char *)SendBuf);
//    if(!BC35_SendCmd(SendBuf, "OK"))				//�յ���>��ʱ���Է�������
//    {
//        DelayXms(500);
//        Usart_SendString(USART2,"AT+NSORF=1,100\r\n", 16);	//�����豸������������
//        if(BC35_WaitRecive() == REV_OK)							//����յ�����
//        {
//            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
//        }
//        DelayXms(500);

//        UsartPrintf(USART_DEBUG,"Send OK\r\n");

//	
//    }

//}

//==========================================================
//	�������ƣ�	BC35_GetNSONMI
//
//	�������ܣ�	�ȴ���ȡ�����Socket��Ϣ��
//              �յ�+NSONMI����ȡ�����յ���������������AT+NSORF,��ȡ���ݡ�
//
//	��ڲ�����	timeOut���ȴ���ʱʱ��
//				��������*10ms
//
//	���ز�����	��ȡ�ɹ�����1��ʧ�ܷ���0.
//
//	˵����		
//==========================================================

unsigned char BC35_GetNSONMI(unsigned short timeOut)
{
	 char *req_length = NULL;
	 char BC35_SendAT[100] = "AT+NSORF=1,";//��Ƭ����BC35�����Ѵ�onenet���յ�������
	    do
    {
        if(BC35_WaitRecive() == REV_OK)								//����������
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            req_length = strstr((char *)BC35_buf, ",");
            if(req_length == NULL)											//���û�ҵ���������IPDATAͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
            {
                UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
            }
            else
            {
                UsartPrintf(USART_DEBUG, "�յ���������\r\n");

                req_length++;
                UsartPrintf(USART_DEBUG, "���յ�������%s\r\n",req_length);
							  strcat(BC35_SendAT,req_length);
							  
                UsartPrintf(USART_DEBUG,BC35_SendAT);
								BC35_SendRevCmd(BC35_SendAT,"1"); 
							  return 1;
                

            }
        }

        DelayXms(10);													//��ʱ�ȴ�
//				UsartPrintf(USART_DEBUG, "=====================\r\n");
    } while(timeOut--);
	
		BC35_Clear();
		
	return 0;
	
}




//==========================================================
//	�������ƣ�	BC35_GetDATA
//
//	�������ܣ�	ʹ��AT+NSORF��BC35_GetNSONMI����ʹ�ú󣩣���ȡģ�鷵�ص����ݡ�
//
//	��ڲ�����	timeOut�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//==========================================================
unsigned char *BC35_GetDATA(unsigned short timeOut)
{
    char *ptrIPD = NULL;   //IPͷָ��
	  char *token = NULL;    //�ָ�ָ��
	  char i = 0;            //�ָ������
    do
    {
        if(BC35_WaitRecive() == REV_OK)								//����������
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            ptrIPD = strstr((char *)BC35_buf, "1,183.230.40.39,6002,");
            if(ptrIPD == NULL)											//���û�ҵ���������IPDATAͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
            {
                UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
            }
            else
            {
                UsartPrintf(USART_DEBUG, "�յ�����\r\n");
                token = strtok(BC35_buf,","); //��BC35_buf���ݶ��ŷֽ⡣
							
							while(token != NULL&&i < 4)
							{
								i++;
								UsartPrintf(USART_DEBUG,(char *)token);
								token = strtok(NULL,",");																
							}
							
							if(i == 4)
							{
							      UsartPrintf(USART_DEBUG, "�������\r\n");
                    UsartPrintf(USART_DEBUG,(char *)token);
                    return (unsigned char *)(token);
							}
							
              else
                    return NULL;
            }
        }

        DelayXms(10);													//��ʱ�ȴ�
//				UsartPrintf(USART_DEBUG, "=====================\r\n");
    } while(timeOut--);

    return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}









//==========================================================
//	�������ƣ�	BC35_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�IP��ͷ����
//
//	��ڲ�����	timeOut�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//==========================================================
//unsigned char *BC35_GetIPD(unsigned short timeOut)
//{
//    char *ptrIPD = NULL;
//    do
//    {
//        if(BC35_WaitRecive() == REV_OK)								//����������
//        {
//            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
//            ptrIPD = strstr((char *)BC35_buf, "1,183.230.40.39,6002,");
//            if(ptrIPD == NULL)											//���û�ҵ���������IPDATAͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
//            {
//                UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
//            }
//            else
//            {
//                UsartPrintf(USART_DEBUG, "�յ�����\r\n");
//                ptrIPD = strchr(ptrIPD, '\n');							//�ҵ�'\n'
//                if(ptrIPD != NULL)
//                {
//                    ptrIPD++;
//                    UsartPrintf(USART_DEBUG, "�������\r\n");
//                    UsartPrintf(USART_DEBUG,(char *)ptrIPD);
//                    return (unsigned char *)(ptrIPD);
//                }
//                else
//                    return NULL;
//            }
//        }

//        DelayXms(10);													//��ʱ�ȴ�
////				UsartPrintf(USART_DEBUG, "=====================\r\n");
//    } while(timeOut--);

//    return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

//}

//==========================================================
//	�������ƣ�	BC35_Init
//
//	�������ܣ�	��ʼ��BC35
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void BC35_Init(void)
{

    BC35_Clear();
    DelayXms(1000);
    UsartPrintf(USART_DEBUG, "1.ATE0\r\n");     //�رջ���
    while(BC35_SendCmd("ATE0\r\n", "OK"))
        DelayXms(500);

    while(BC35_SendCmd("AT+CMEE=1\r\n", "OK"))  //��SIM��PIN�����������READY����ʾ�����ɹ�
        DelayXms(500);

    UsartPrintf(USART_DEBUG, "2.AT\r\n");
    while(BC35_SendCmd("AT\r\n", "OK"))	//���¿�״̬�Ƿ�OK �ܷ��ȡ������
        DelayXms(500);

    UsartPrintf(USART_DEBUG, "3.AT+CGSN=1\r\n");//��ȡIMEI
    while(BC35_SendCmd("AT+CGSN=1\r\n", "OK"))
        DelayXms(500);

    BC35_Clear();
    if(!BC35_SendCmd("AT+NBAND?\r\n","+NBAND:5"))//�ж��Ƿ�Ϊ+NBAND:5
    {
        OLED_ShowString(4,4,"BAND:5 REG[..]");   //�Ƿ��ǵ��ŵ�
    }
    else
    {
        OLED_ShowString(4,4,"BAND:8 REG[..]");    //��������ƶ���ͨ
    }
    //	while(BC35_SendCmd("AT+NBAND?\r\n","OK"))
    //		DelayXms(500);
    //	while(BC35_SendCmd("AT+NBAND?\r\n","+NBAND:7"))
    DelayXms(5000);

    BC35_Clear();
    UsartPrintf(USART_DEBUG, "4.AT+CIMI\r\n");
    if(!BC35_SendCmd("AT+CIMI\r\n","OK"))
    {
        OLED_ShowString(4,4,"NBSIMCARD [OK]");
    }
    else
    {
        OLED_ShowString(4,4,"NBSIMCARD [NO]");
    }
    DelayXms(500);

    BC35_Clear();
    UsartPrintf(USART_DEBUG, "5.AT+CSQ\r\n");
    while(!BC35_SendCmd("AT+CSQ\r\n","CSQ:99,99"))                     //����
        DelayXms(500);



    //	UsartPrintf(USART_DEBUG, "8. AT+QIACT?\r\n");
    //	while(BC35_SendCmd("AT+QIACT?\r\n","OK"))
    //		DelayXms(500);
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "6.AT+CGATT=1\r\n");
    BC35_SendCmd("AT+CGATT=1\r\n","OK");                                 //����״̬
    DelayXms(500);

    UsartPrintf(USART_DEBUG, "7.AT+CGATT?\r\n");
    while(BC35_SendCmd("AT+CGATT?\r\n","CGATT:1"))                                 //����״̬
    {
        UsartPrintf(USART_DEBUG, "Regedit PDP[..]\r\n");
        DelayXms(500);
    }

    UsartPrintf(USART_DEBUG, "8.AT+CGPADDR\r\n");
    BC35_SendCmd("AT+CGPADDR\r\n","OK");                                 //����״̬
    DelayXms(500);

    UsartPrintf(USART_DEBUG, "9.AT+NSOCR=STREAM,6,56000,1\r\n");
    while(BC35_SendCmd("AT+NSOCR=STREAM,6,56000,1\r\n","OK"))           //����Ϊ�����ӣ���Ȼƽ̨IP��������
    {
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        DelayXms(500);
    }

    UsartPrintf(USART_DEBUG, "-----------------\r\n");
    UsartPrintf(USART_DEBUG, BC35_ONENET_INFO);
    while(BC35_SendCmd(BC35_ONENET_INFO,"OK"))				//���ӵ�������
    {
        UsartPrintf(USART_DEBUG, "AT+NSOCL=1\r\n");
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        DelayXms(500);
        UsartPrintf(USART_DEBUG, "AT+NSOCR=STREAM,6,56000,1\r\n");
        while(BC35_SendCmd("AT+NSOCR=STREAM,6,56000,1\r\n","OK"))	//����Ϊ�����ӣ���Ȼƽ̨IP��������
        {
            UsartPrintf(USART_DEBUG, "AT+NSOCL=1\r\n");
            BC35_SendCmd("AT+NSOCL=1\r\n","OK");
            DelayXms(500);
        }
        DelayXms(500);
    }
    UsartPrintf(USART_DEBUG, "=====BC35 Init OK=====\r\n");

}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	����2�շ��ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void USART2_IRQHandler(void)
{

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
    {
        if(BC35_cnt >= sizeof(BC35_buf))	BC35_cnt = 0; //��ֹ���ڱ�ˢ��
        BC35_buf[BC35_cnt++] = USART2->DR;

        USART_ClearFlag(USART2, USART_FLAG_RXNE);
    }

}
