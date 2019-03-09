/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
        *	���ߣ� 		leigong
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"
#include "MQTTData.h"
//�����豸
#include "BC35.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"
#include "HEXSTR.h"
#include "MQTTData.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "adxl362.h"
#include "DHT11.h"
//C��
#include <string.h>
#include <stdio.h>


#define PROID		"183382"

#define AUTH_INFO	"csibc35"

#define DEVID		"508860545"


extern unsigned char BC35_buf[128];

//u8 http_len=0;
//void hextostr(uint8 *pkt,uint8 *data,uint32 len)
//{
//    u8 i;
//    *pkt = 0;
//    for(i=0;i<len;i++)
//    {
//        if(data[i]/16<10)
//            pkt[2*i]=data[i]/16+0x30;
//        else
//            pkt[2*i]=data[i]/16+0x37;
//        if(data[i]%16<10)
//            pkt[2*i+1]=data[i]%16+0x30;
//        else
//            pkt[2*i+1]=data[i]%16+0x37;
//    }
//}
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-ʧ��	0-�ɹ�
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���
		
		unsigned char mqttSenddataString[200];//תΪASCII���ַ������ַ����Ļ���
		
		unsigned char mqttRevdatahex[200];//���ܵ��ַ���ת��Ϊ16��������

    unsigned char *dataPtr;

    _Bool status = 1;

    UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                "PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                , PROID, AUTH_INFO, DEVID);

    if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
		
					
//					for (int i = 0; i < mqttPacket._len; i++)
//					{				
//					 UsartPrintf(USART_DEBUG,"mqttPacket._data[%d] = %02x\r\n", i,mqttPacket._data[i]);
//					}
	
				
        HexArrayToString(mqttPacket._data, mqttPacket._len, mqttSenddataString);					
				BC35_SENDDATA(mqttSenddataString, (mqttPacket._len)*2);
			
				if(BC35_GetNSONMI(250))
				{
					 dataPtr = BC35_GetDATA(250);								//�ȴ�ƽ̨��Ӧ
				}				

        UsartPrintf(USART_DEBUG, (char *)dataPtr);				
				StringToHex(mqttRevdatahex, dataPtr);
				
        if(mqttRevdatahex!= NULL)
        {
          			
						if(MQTT_UnPacketRecv(mqttRevdatahex) == MQTT_PKT_CONNACK)
						{
							switch(MQTT_UnPacketConnectAck(mqttRevdatahex))
							{
								case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
								
								case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
								case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
								case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
								case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
								case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
								
								default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
							}
						}
        }

        MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
				ClearRAM((u8*)mqttSenddataString,200);         //ɾ��
    }
    else
        UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");

    return status;

}



unsigned char OneNet_FillBuf(char *buf)
{

    char text[200];

    memset(text, 0, sizeof(text));

    strcpy(buf, "{\"datastreams\":[");

    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"Xg\",\"datapoints\":[{\"value\":%0.2f}]},", adxl362Info.x);
    //	strcat(buf, text);
    //
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"Yg\",\"datapoints\":[{\"value\":%0.2f}]},", adxl362Info.y);
    //	strcat(buf, text);

    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"Zg\",\"datapoints\":[{\"value\":%0.2f}]}", adxl362Info.z);
    //	strcat(buf, text);

    memset(text, 0, sizeof(text));
    sprintf(text, "{\"id\":\"Humi\",\"datapoints\":[{\"value\":%d}]},", DHT11_Data.humi_int);
    strcat(buf, text);

    memset(text, 0, sizeof(text));
    sprintf(text, "{\"id\":\"Temp\",\"datapoints\":[{\"value\":%d}]},",DHT11_Data.temp_int);
    strcat(buf, text);
    //		//LED�ƿ���
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"LED\",\"datapoints\":[{\"value\":%d}]},",1);
    //	strcat(buf, text);
    //GPS������
    memset(text, 0, sizeof(text));
    sprintf(text, "{\"id\":\"GPS\",\"datapoints\":[{\"value\":{\"lon\":%s,\"lat\":%s}}]}","116.3972282","39.909604");
    strcat(buf, text);

    strcat(buf, "]}");

    return strlen(buf);

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���

    char buf[500];
    char bufhex[500];
    short body_len = 0, i = 0;

    UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");

    memset(buf, 0, sizeof(buf));
    memset(bufhex, 0, sizeof(bufhex));
    body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
    UsartPrintf(USART_DEBUG, buf);
    DelayXms(500);
    if(body_len)
    {
        if(MQTT_PacketSaveData(DEVID, body_len, NULL, 1, &mqttPacket) == 0)							//���
        {
            for(;i < body_len; i++)
                mqttPacket._data[mqttPacket._len++] = buf[i];
            mqtt_SendData_message(bufhex,buf,body_len);
            UsartPrintf(USART_DEBUG, "bufhex:%s\r\n", bufhex);
            DelayXms(500);
           // BC35_SendData((u8 *)bufhex,strlen((const char*)bufhex)/2);										//�ϴ����ݵ�ƽ̨
            UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
            MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
        }
        else
            UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
    }

}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���

    char *req_payload = NULL;
    char *cmdid_topic = NULL;
		
		unsigned char mqttSenddataString[200];//תΪASCII���ַ������ַ����Ļ���
		
    unsigned short req_len = 0;

    unsigned char type = 0;

    short result = 0;

    char *dataPtr = NULL;
    char numBuf[10];
    int num = 0;

    type = MQTT_UnPacketRecv(cmd);
		
		
    switch(type)
    {
    case MQTT_PKT_CMD:															//�����·�

        result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
        if(result == 0)
        {
            UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

            if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
            {

							  UsartPrintf(USART_DEBUG, "Yuanshi_mqttPacket._len=%d\r\n",mqttPacket._len);
							
                UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
							
//						for (int i = 0; i < mqttPacket._len; i++)
//					{						
//					 UsartPrintf(USART_DEBUG,"mqttPacket._data[%d] = %x\r\n", i,mqttPacket._data[i]);
//					}
					
							  HexArrayToString(mqttPacket._data, mqttPacket._len, mqttSenddataString);
							  UsartPrintf(USART_DEBUG, "mqttPacket._len=%d\r\n",(mqttPacket._len)*2);
//						for (int i = 0; i < (mqttPacket._len)*2; i++)
//					{						
//					 UsartPrintf(USART_DEBUG,"mqttPacketdataHEX[%d] = %c\r\n", i,mqttPacketdataHEX[i]);
//					}
					
                BC35_SENDDATA(mqttSenddataString, (mqttPacket._len)*2);				//�ظ�����
							  ClearRAM((u8*)mqttSenddataString,200);//ɾ��
                MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
            }
        }

        break;

		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack

                    if(MQTT_UnPacketPublishAck(cmd) == 0)
                        UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");

                    break;

		default:
                    result = -1;
                    break;
                }

    BC35_Clear();										//��ջ���
								
	

    if(result == -1)
        return;


		
    dataPtr = strchr(req_payload, '{');					//����'}'

    if(dataPtr != NULL && result != -1)					//����ҵ���
    {
        dataPtr++;

        while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
        {
            numBuf[num++] = *dataPtr++;
        }

        num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
				
				
    }
		
		
		
		
		
		
		if(strstr((char *)req_payload, "redled"))		//����"redled"
		{
			if(num == 1)								//�����������Ϊ1������
			{
//				Led4_Set(LED_ON);
				UsartPrintf(USART_DEBUG, "Led4_Set(LED_ON)");
			}
			else if(num == 0)							//�����������Ϊ0�������
			{
//				Led4_Set(LED_OFF);
				UsartPrintf(USART_DEBUG, "Led4_Set(LED_OFF)");
			}
		}
														//��ͬ
		else if(strstr((char *)req_payload, "greenled"))
		{
			if(num == 1)
			{
//				Led5_Set(LED_ON);
				UsartPrintf(USART_DEBUG, "Led5_Set(LED_ON)");
			}
			else if(num == 0)
			{
	//			Led5_Set(LED_OFF);
				UsartPrintf(USART_DEBUG, "Led5_Set(LED_OFF)");
			}
		}
		else if(strstr((char *)req_payload, "yellowled"))
		{
			if(num == 1)
			{
		//		Led6_Set(LED_ON);
			}
			else if(num == 0)
			{
			//	Led6_Set(LED_OFF);
			}
		}
		else if(strstr((char *)req_payload, "blueled"))
		{
			if(num == 1)
			{
		//		Led7_Set(LED_ON);
			}
			else if(num == 0)
			{
		//		Led7_Set(LED_OFF);
			}
		}	
		

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }

}
