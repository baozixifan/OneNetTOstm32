/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	BC35.c
	*
        *	作者： 		leigong
	*
	*	日期： 		2017-10-20
	*
	*	版本： 		V1.0
	*
	*	说明： 		BC35驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "BC35.h"
#include "oled.h"
//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "HEXSTR.h"


#define BC35_ONENET_INFO "AT+NSOCO=1,183.230.40.39,6002\r\n"  //"AT+IPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"


unsigned char BC35_buf[128];        //接收模块的反馈信息
unsigned short BC35_cnt = 0, BC35_cntPre = 0;


//==========================================================
//	函数名称：	BC35_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void BC35_Clear(void)
{

    memset(BC35_buf, 0, sizeof(BC35_buf));
    BC35_cnt = 0;

}

//==========================================================
//	函数名称：	BC35_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool BC35_WaitRecive(void)
{

    if(BC35_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;

    if(BC35_cnt == BC35_cntPre)                                             //如果上一次的值和这次相同，则说明接收完毕
    {
        BC35_cnt = 0;							//清0接收计数

        return REV_OK;							//返回接收完成标志
    }

    BC35_cntPre = BC35_cnt;                                                 //置为相同

    return REV_WAIT;							//返回接收未完成标志

}

//==========================================================
//	函数名称：	BC35_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool BC35_SendCmd(char *cmd, char *res)
{

    unsigned char timeOut = 200;

    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
    DelayXms(200);
    while(timeOut--)
    {
        if(BC35_WaitRecive() == REV_OK)					//如果收到数据
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            if(strstr((const char *)BC35_buf, res) != NULL)		//如果检索到关键词
            {
                BC35_Clear();                                           //清空缓存 成功反馈0
                return 0;
            }
        }

        DelayXms(10);
    }

    return 1;

}




//==========================================================
//	函数名称：	BC35_SendREVCMD
//
//	函数功能：	传入数据和数据长度，并检查返回信息
//
//	入口参数：	cmd：命令
//              len: 命令长度
//				      res：需要检查的部分返回数据
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool BC35_SendREVCMD(char *cmd,unsigned short len,char *res)
{

    unsigned char timeOut = 200;
	  char flag = 0;

    Usart_SendString(USART2, (unsigned char *)cmd, len);
    DelayXms(200);

	    while(timeOut--)
    {
        if(BC35_WaitRecive() == REV_OK)					//如果收到数据
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            if(strstr((const char *)BC35_buf, res) != NULL)		//如果检索到关键词
            {
                BC35_Clear();                                           //清空缓存 成功反馈0
                return 0;
            }
						else if((flag == 0) && (strstr((const char *)BC35_buf, "ERROR") != NULL))                                            //如果检索到错误关键词，则800ms后重发一次
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
//	函数名称：	BC35_SendRevCmd
//
//	函数功能：	发送特殊的取数据命令
//
//	入口参数：	cmd：命令
//				      res：需要检查的部分返回数据
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool BC35_SendRevCmd(char *cmd, char *res)
{

    BC35_Clear();

    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
    DelayXms(200);

    return 0;
}

 //==========================================================
//	函数名称：	BC35_SENDDATA
//
//	函数功能：	发送字符数组数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
// sprintf((char*)(SendBuf),"%s,%d,%s\r\n","AT+NSOSD=1",len,(char *)data);
//	说明：		
//==========================================================
void BC35_SENDDATA(unsigned char *data, unsigned short len)
{
    char SendBuf[500];
		int cmd_len = 0;//BC35发送数据包前部命令长度
		int n = 0;//数据拷贝计数器
		int i = 0;//定义整个发送数据包计数器
		int total_len = 0;//定义整个数据包的长度
	
    memset(SendBuf, 0, sizeof(SendBuf));
    sprintf((char*)(SendBuf),"%s,%d,","AT+NSOSD=1",len/2);
		cmd_len = strlen(SendBuf);
	    
		  for(i = cmd_len;i < cmd_len+len; i++)
		{		     
	         SendBuf[i] = data[n++];//将字符数组中的字符一个一个的转入缓存
		}
		SendBuf[i++] = '\r';
		SendBuf[i++] = '\n';
		SendBuf[i++] = '\0';

		total_len = cmd_len + len + 2;
		
				for (int j = 0; j < total_len; j++)
		{
			UsartPrintf(USART_DEBUG,"SendBuf[%d] = %c\r\n", j, SendBuf[j]);//输出转化后的字符串数据包
		}


    if(!BC35_SendREVCMD(SendBuf,total_len, "OK"))				//收到‘OK’时可以发送数据
    {
			  UsartPrintf(USART_DEBUG,"*********BC35_SendREVCMD**********\r\n");
        DelayXms(500);
			
//		  BC35_SendREVCMD("AT+NSORF=1,100\r\n",16,"OK");
 //       Usart_SendString(USART2,"AT+NSORF=1,100\r\n",16);	//发送设备连接请求数据

 //       DelayXms(500);

        UsartPrintf(USART_DEBUG,"Send OK\r\n");


    }

}  


//==========================================================
//	函数名称：	BC35_SendData
//
//	函数功能：	发送字符串数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
//void BC35_SendData(unsigned char *data, unsigned short len)
//{
//    char SendBuf[500];
//    memset(SendBuf, 0, sizeof(SendBuf));
//    sprintf((char*)(SendBuf),"%s,%d,%s\r\n","AT+NSOSD=1",len,(char *)data);
//    UsartPrintf(USART_DEBUG,(char *)SendBuf);
//    if(!BC35_SendCmd(SendBuf, "OK"))				//收到‘>’时可以发送数据
//    {
//        DelayXms(500);
//        Usart_SendString(USART2,"AT+NSORF=1,100\r\n", 16);	//发送设备连接请求数据
//        if(BC35_WaitRecive() == REV_OK)							//如果收到数据
//        {
//            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
//        }
//        DelayXms(500);

//        UsartPrintf(USART_DEBUG,"Send OK\r\n");

//	
//    }

//}

//==========================================================
//	函数名称：	BC35_GetNSONMI
//
//	函数功能：	等待获取到达的Socket消息。
//              收到+NSONMI后提取出接收到数据量，并发送AT+NSORF,获取数据。
//
//	入口参数：	timeOut：等待超时时间
//				输入数字*10ms
//
//	返回参数：	获取成功返回1，失败返回0.
//
//	说明：		
//==========================================================

unsigned char BC35_GetNSONMI(unsigned short timeOut)
{
	 char *req_length = NULL;
	 char BC35_SendAT[100] = "AT+NSORF=1,";//单片机向BC35请求已从onenet接收到的数据
	    do
    {
        if(BC35_WaitRecive() == REV_OK)								//如果接收完成
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            req_length = strstr((char *)BC35_buf, ",");
            if(req_length == NULL)											//如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
            {
                UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
            }
            else
            {
                UsartPrintf(USART_DEBUG, "收到返回数据\r\n");

                req_length++;
                UsartPrintf(USART_DEBUG, "接收的数据量%s\r\n",req_length);
							  strcat(BC35_SendAT,req_length);
							  
                UsartPrintf(USART_DEBUG,BC35_SendAT);
								BC35_SendRevCmd(BC35_SendAT,"1"); 
							  return 1;
                

            }
        }

        DelayXms(10);													//延时等待
//				UsartPrintf(USART_DEBUG, "=====================\r\n");
    } while(timeOut--);
	
		BC35_Clear();
		
	return 0;
	
}




//==========================================================
//	函数名称：	BC35_GetDATA
//
//	函数功能：	使用AT+NSORF后（BC35_GetNSONMI函数使用后），获取模块返回的数据。
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *BC35_GetDATA(unsigned short timeOut)
{
    char *ptrIPD = NULL;   //IP头指针
	  char *token = NULL;    //分割指针
	  char i = 0;            //分割计数器
    do
    {
        if(BC35_WaitRecive() == REV_OK)								//如果接收完成
        {
            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
            ptrIPD = strstr((char *)BC35_buf, "1,183.230.40.39,6002,");
            if(ptrIPD == NULL)											//如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
            {
                UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
            }
            else
            {
                UsartPrintf(USART_DEBUG, "收到数据\r\n");
                token = strtok(BC35_buf,","); //将BC35_buf根据逗号分解。
							
							while(token != NULL&&i < 4)
							{
								i++;
								UsartPrintf(USART_DEBUG,(char *)token);
								token = strtok(NULL,",");																
							}
							
							if(i == 4)
							{
							      UsartPrintf(USART_DEBUG, "接收完成\r\n");
                    UsartPrintf(USART_DEBUG,(char *)token);
                    return (unsigned char *)(token);
							}
							
              else
                    return NULL;
            }
        }

        DelayXms(10);													//延时等待
//				UsartPrintf(USART_DEBUG, "=====================\r\n");
    } while(timeOut--);

    return NULL;														//超时还未找到，返回空指针

}









//==========================================================
//	函数名称：	BC35_GetIPD
//
//	函数功能：	获取平台返回的IP报头数据
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
//unsigned char *BC35_GetIPD(unsigned short timeOut)
//{
//    char *ptrIPD = NULL;
//    do
//    {
//        if(BC35_WaitRecive() == REV_OK)								//如果接收完成
//        {
//            UsartPrintf(USART_DEBUG,(char *)BC35_buf);
//            ptrIPD = strstr((char *)BC35_buf, "1,183.230.40.39,6002,");
//            if(ptrIPD == NULL)											//如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
//            {
//                UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
//            }
//            else
//            {
//                UsartPrintf(USART_DEBUG, "收到数据\r\n");
//                ptrIPD = strchr(ptrIPD, '\n');							//找到'\n'
//                if(ptrIPD != NULL)
//                {
//                    ptrIPD++;
//                    UsartPrintf(USART_DEBUG, "接收完成\r\n");
//                    UsartPrintf(USART_DEBUG,(char *)ptrIPD);
//                    return (unsigned char *)(ptrIPD);
//                }
//                else
//                    return NULL;
//            }
//        }

//        DelayXms(10);													//延时等待
////				UsartPrintf(USART_DEBUG, "=====================\r\n");
//    } while(timeOut--);

//    return NULL;														//超时还未找到，返回空指针

//}

//==========================================================
//	函数名称：	BC35_Init
//
//	函数功能：	初始化BC35
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void BC35_Init(void)
{

    BC35_Clear();
    DelayXms(1000);
    UsartPrintf(USART_DEBUG, "1.ATE0\r\n");     //关闭回显
    while(BC35_SendCmd("ATE0\r\n", "OK"))
        DelayXms(500);

    while(BC35_SendCmd("AT+CMEE=1\r\n", "OK"))  //保SIM卡PIN码解锁，返回READY，表示解锁成功
        DelayXms(500);

    UsartPrintf(USART_DEBUG, "2.AT\r\n");
    while(BC35_SendCmd("AT\r\n", "OK"))	//看下卡状态是否OK 能否获取到卡号
        DelayXms(500);

    UsartPrintf(USART_DEBUG, "3.AT+CGSN=1\r\n");//获取IMEI
    while(BC35_SendCmd("AT+CGSN=1\r\n", "OK"))
        DelayXms(500);

    BC35_Clear();
    if(!BC35_SendCmd("AT+NBAND?\r\n","+NBAND:5"))//判断是否为+NBAND:5
    {
        OLED_ShowString(4,4,"BAND:5 REG[..]");   //是否是电信的
    }
    else
    {
        OLED_ShowString(4,4,"BAND:8 REG[..]");    //否则就是移动联通
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
    while(!BC35_SendCmd("AT+CSQ\r\n","CSQ:99,99"))                     //激活
        DelayXms(500);



    //	UsartPrintf(USART_DEBUG, "8. AT+QIACT?\r\n");
    //	while(BC35_SendCmd("AT+QIACT?\r\n","OK"))
    //		DelayXms(500);
    BC35_Clear();
    UsartPrintf(USART_DEBUG, "6.AT+CGATT=1\r\n");
    BC35_SendCmd("AT+CGATT=1\r\n","OK");                                 //激活状态
    DelayXms(500);

    UsartPrintf(USART_DEBUG, "7.AT+CGATT?\r\n");
    while(BC35_SendCmd("AT+CGATT?\r\n","CGATT:1"))                                 //激活状态
    {
        UsartPrintf(USART_DEBUG, "Regedit PDP[..]\r\n");
        DelayXms(500);
    }

    UsartPrintf(USART_DEBUG, "8.AT+CGPADDR\r\n");
    BC35_SendCmd("AT+CGPADDR\r\n","OK");                                 //激活状态
    DelayXms(500);

    UsartPrintf(USART_DEBUG, "9.AT+NSOCR=STREAM,6,56000,1\r\n");
    while(BC35_SendCmd("AT+NSOCR=STREAM,6,56000,1\r\n","OK"))           //必须为单连接，不然平台IP都连不上
    {
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        DelayXms(500);
    }

    UsartPrintf(USART_DEBUG, "-----------------\r\n");
    UsartPrintf(USART_DEBUG, BC35_ONENET_INFO);
    while(BC35_SendCmd(BC35_ONENET_INFO,"OK"))				//连接到服务器
    {
        UsartPrintf(USART_DEBUG, "AT+NSOCL=1\r\n");
        BC35_SendCmd("AT+NSOCL=1\r\n","OK");
        DelayXms(500);
        UsartPrintf(USART_DEBUG, "AT+NSOCR=STREAM,6,56000,1\r\n");
        while(BC35_SendCmd("AT+NSOCR=STREAM,6,56000,1\r\n","OK"))	//必须为单连接，不然平台IP都连不上
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
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHandler(void)
{

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
    {
        if(BC35_cnt >= sizeof(BC35_buf))	BC35_cnt = 0; //防止串口被刷爆
        BC35_buf[BC35_cnt++] = USART2->DR;

        USART_ClearFlag(USART2, USART_FLAG_RXNE);
    }

}
