#ifndef __MQTTData_H
#define __MQTTData_H	 
//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define u8 unsigned char 
#define u16 unsigned int 
void ClearRAM(u8* ram,u32 n);
u16 mqtt_connect_message(char *mqtt_message,char *PROID,char *DEVID,char *AUTH_INFO);
u16 mqtt_SendData_message(char *mqtt_messageHEX,char *SendData,short length);
u16 mqtt_subscribe_message(u8 *mqtt_message,char *topic,u8 qos,u8 whether);
u8 mqtt_ping_message(u8 *mqtt_message);
u8 mqtt_disconnect_message(u8 *mqtt_message);
u8 mqtt_disconnect_message(u8 *mqtt_message);
u16 mqtt_publish_message(u8 *mqtt_message, char * topic, char * message, u8 qos);
u16 mqtt_onenetpublish_message(u8 *mqtt_message, char * topic, char * message,u16 len, u8 qos);
#endif

