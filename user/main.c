/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
        *	作者： 		leigong
	*
        *	日期： 		2018-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		接入onenet，上传数据和命令控制
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "BC35.h"
#include "oled.h"
//硬件驱动
#include "delay.h"
#include "usart.h"
#include "beep.h"
#include "adxl362.h"
#include "DHT11.h"
//C库
#include <string.h>
#include "HEXSTR.h"
#include "MQTTData.h"



/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

    Delay_Init();					//systick初始化

    Usart1_Init(9600);                                  //串口1，打印信息用

    Usart2_Init(9600);                                  //串口2，驱动BC35用

    //ADXL362_Init();					//三轴加速计初始化 NO USE
    DHT11_Init ();
    Beep_Init();					//蜂鸣器初始化

    UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");


    OLED_Init();		//INIT OLED
    OLED_Clear();
    OLED_ShowCHinese(30,0,0);
    OLED_ShowCHinese(48,0,1);
    OLED_ShowCHinese(66,0,2);
    OLED_ShowCHinese(84,0,3);

}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{

    unsigned short timeCount = 0;	//发送间隔变量

    unsigned char *dataPtr = NULL;
	
//	  unsigned char mqttPacketdataHEX[200];//转为16进制数组后，数组的缓存
	
	  unsigned char mqttRevdatahex[200];//接受的字符串转化为16进制数组

    Hardware_Init();				//初始化外围硬件

    BC35_Init();				//初始化bc35

    while(OneNet_DevLink())			//接入OneNET
        DelayXms(500);

    Beep_Set(BEEP_ON);				//鸣叫提示接入成功
    DelayXms(250);
    Beep_Set(BEEP_OFF);

    while(1)
    {

//        if(++timeCount >= 500)									//发送间隔5s
//        {
//            ADXL362_GetValue();
//            DHT11_Read_TempAndHumidity();
//            UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
//            OLED_ShowString(4,4,"SEND DATA [OK]");
//            OneNet_SendData();									//发送数据
//            timeCount = 0;
//            BC35_Clear();
//        }		
			if(BC35_GetNSONMI(250))
			{
	

				
				UsartPrintf(USART_DEBUG, "+++++++++++++++++++++++++++*******");
        dataPtr = BC35_GetDATA(250);
        if(dataPtr != NULL)
				{			
	      UsartPrintf(USART_DEBUG, "********************\r\n");		
				StringToHex(mqttRevdatahex, dataPtr);								
				}
            OneNet_RevPro(mqttRevdatahex);				
				    ClearRAM((u8*)mqttRevdatahex,200);         //删包
				
			}
        DelayXms(10);
	
    }

}
