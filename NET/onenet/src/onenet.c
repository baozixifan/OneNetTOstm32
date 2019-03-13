/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
        *	作者： 		leigong
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"
#include "MQTTData.h"
//网络设备
#include "BC35.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"
#include "HEXSTR.h"
#include "MQTTData.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "adxl362.h"
#include "DHT11.h"
//C库
#include <string.h>
#include <stdio.h>


#define PROID		"183382"

#define AUTH_INFO	"csibc35"

#define DEVID		"508860545"

unsigned char heart_beat = 0; //心跳
unsigned char errCount = 0;   //错误计数
//unsigned char sendData = 0;   //发送数据类型
extern unsigned char BC35_buf[200];

//==========================================================
//	函数名称：	OneNET_SendData_Heart
//
//	函数功能：	心跳检测
//
//	入口参数：	无
//
//	返回参数：	0-发送成功	；SEND_TYPE_HEART-需要重送
//
//	说明：		
//==========================================================
unsigned char OneNET_SendData_Heart(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包
	unsigned char mqttSenddataString[200];//转为ASCII码字符串后，字符串的缓存
	
	
	if(MQTT_PacketPing(&mqttPacket))
		return SEND_TYPE_HEART;
	
	heart_beat = 0;

  HexArrayToString(mqttPacket._data, mqttPacket._len, mqttSenddataString);	
	BC35_SENDDATA(mqttSenddataString, (mqttPacket._len)*2);			//向平台上传心跳请求
//	NET_DEVICE_AddDataSendList(mqttPacket._data, mqttPacket._len, 0);	//加入链表
	
	MQTT_DeleteBuffer(&mqttPacket);										//删包
	ClearRAM((u8*)mqttSenddataString,200);         //删包
	
	return 0;

}

//==========================================================
//	函数名称：	OneNet_HeartBeat_Check
//
//	函数功能：	发送心跳后的心跳检测
//
//	入口参数：	无
//
//	返回参数：	0-成功	1-等待
//
//	说明：		基于调用时基，runCount每隔此函数调用一次的时间自增
//				达到设定上限检测心跳标志位是否就绪
//				上限时间可以不用太精确
//==========================================================
_Bool OneNet_Check_Heart(void)
{
	
	static unsigned char runCount = 0;
	

	if(heart_beat == 1)
	{
		runCount = 0;
		errCount = 0;
		
		return 0;
	}
	
	if(++runCount >= 40)           //心跳停止累计40个周期
	{
		runCount = 0;
		
		UsartPrintf(USART_DEBUG, "HeartBeat TimeOut: %d\r\n", errCount);
		OneNET_SendData_Heart();		//再次发送心跳请求
		
		if(++errCount >= 3)         //心跳停止重发累计3次
		{
			unsigned char errType = 0;
			
			errCount = 0;
			
			UsartPrintf(USART_DEBUG, "NET_DEVICE_Check_needed\r\n");
			
//			errType = NET_DEVICE_Check();											//网络设备状态检查
//			if(errType == CHECK_CONNECTED || errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
//				faultTypeReport = faultType = FAULT_PRO;								//标记为协议错误
//			else if(errType == CHECK_NO_DEVICE)
//				faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
//			else
//				faultTypeReport = faultType = FAULT_NONE;								//无错误
		}
	}
	
	return 1;

}

//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-失败	0-成功
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包
		
		unsigned char mqttSenddataString[200];//转为ASCII码字符串后，字符串的缓存
		
		unsigned char mqttRevdatahex[200];//接受的字符串转化为16进制数组

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
					 dataPtr = BC35_GetDATA(250);								//等待平台响应
				}				

        UsartPrintf(USART_DEBUG, (char *)dataPtr);				
				StringToHex(mqttRevdatahex, dataPtr);
				
        if(mqttRevdatahex!= NULL)
        {
          			
						if(MQTT_UnPacketRecv(mqttRevdatahex) == MQTT_PKT_CONNACK)
						{
							switch(MQTT_UnPacketConnectAck(mqttRevdatahex))
							{
								case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
								
								case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
								case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
								case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
								case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
								case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
								
								default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
							}
						}
        }

        MQTT_DeleteBuffer(&mqttPacket);								//删包
				ClearRAM((u8*)mqttSenddataString,200);         //删包
				ClearRAM((u8*)mqttRevdatahex,200);         //删包
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
    //		//LED灯控制
    //	memset(text, 0, sizeof(text));
    //	sprintf(text, "{\"id\":\"LED\",\"datapoints\":[{\"value\":%d}]},",1);
    //	strcat(buf, text);
    //GPS的数据
    memset(text, 0, sizeof(text));
    sprintf(text, "{\"id\":\"GPS\",\"datapoints\":[{\"value\":{\"lon\":%s,\"lat\":%s}}]}","116.3972282","39.909604");
    strcat(buf, text);

    strcat(buf, "]}");

    return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包

    char buf[500];
    char bufhex[500];
    short body_len = 0, i = 0;

    UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");

    memset(buf, 0, sizeof(buf));
    memset(bufhex, 0, sizeof(bufhex));
    body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
    UsartPrintf(USART_DEBUG, buf);
    DelayXms(500);
    if(body_len)
    {
        if(MQTT_PacketSaveData(DEVID, body_len, NULL, 1, &mqttPacket) == 0)							//封包
        {
            for(;i < body_len; i++)
                mqttPacket._data[mqttPacket._len++] = buf[i];
            mqtt_SendData_message(bufhex,buf,body_len);
            UsartPrintf(USART_DEBUG, "bufhex:%s\r\n", bufhex);
            DelayXms(500);
           // BC35_SendData((u8 *)bufhex,strlen((const char*)bufhex)/2);										//上传数据到平台
            UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
            MQTT_DeleteBuffer(&mqttPacket);															//删包
        }
        else
            UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
    }

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包

    char *req_payload = NULL;
    char *cmdid_topic = NULL;
		
		unsigned char mqttSenddataString[200];//转为ASCII码字符串后，字符串的缓存
		
    unsigned short req_len = 0;

    unsigned char type = 0;

    short result = 0;

    char *dataPtr = NULL;
    char numBuf[10];
    int num = 0;

    type = MQTT_UnPacketRecv(cmd);
		
		
    switch(type)
    {
			
		case MQTT_PKT_PINGRESP:
	
				UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
				heart_beat = 1;
		
				break;
		
    case MQTT_PKT_CMD:															//命令下发

        result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
        if(result == 0)
        {
            UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

            if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
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
					
                BC35_SENDDATA(mqttSenddataString, (mqttPacket._len)*2);				//回复命令
							  ClearRAM((u8*)mqttSenddataString,200);//删包
                MQTT_DeleteBuffer(&mqttPacket);									//删包
            }
        }

        break;

		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack

                    if(MQTT_UnPacketPublishAck(cmd) == 0)
                        UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");

                    break;

		default:
                    result = -1;
                    break;
                }

    BC35_Clear();										//清空缓存
								
	

    if(result == -1)
        return;


		
    dataPtr = strchr(req_payload, '{');					//搜索'}'

    if(dataPtr != NULL && result != -1)					//如果找到了
    {
        dataPtr++;

        while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
        {
            numBuf[num++] = *dataPtr++;
        }

        num = atoi((const char *)numBuf);				//转为数值形式
				
				
    }
		
		
		
		
		
		
		if(strstr((char *)req_payload, "redled"))		//搜索"redled"
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
//				Led4_Set(LED_ON);
				UsartPrintf(USART_DEBUG, "Led4_Set(LED_ON)");
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
//				Led4_Set(LED_OFF);
				UsartPrintf(USART_DEBUG, "Led4_Set(LED_OFF)");
			}
		}
														//下同
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
