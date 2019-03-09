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
static char Value2Hex(const int value);
static int Str2Hex(char *str,char *hex);
#endif

