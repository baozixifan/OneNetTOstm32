#include "MQTTData.h"

void ClearRAM(u8* ram,u32 n)
{
    u32 i;
    for (i = 0;i < n;i++)
    {
        ram[i] = 0x00;
    }
}

char Value2Hex(const int value)
{
    char Hex = NULL;
    if (value>=0 && value<=9)
    {
        Hex = (char)(value+'0');
    }
    else if (value>9 && value<16)
    {
        Hex = (char)(value-10+'A');
    }
    return Hex;
}

int Str2Hex(char *str,char *hex)
{
    int high = 0;
    int low = 0;
    int temp = 0;

    if (NULL==str || NULL==hex){
        //printf("the str or hex is wrong\n");
        return -1;
    }

    if (0==strlen(str)){
        //printf("the input str is wrong\n");
        return -2;
    }

    while(*str)
    {
        temp = (int)(*str);
        high = temp>>4;
        low = temp & 15;
        *hex = Value2Hex(high);
        hex++;
        *hex = Value2Hex(low);
        hex++;
        str++;
    }
    *hex = '\0';
    return 0;
}
//����MQTT���Ӱ� 
u16 mqtt_connect_message(char *mqtt_message,char *PROID,char *AUTH_INFO,char *DEVID)
{
    //	char HTTP_Buf[400];     //HTTP���Ļ�����
    char str[500];
    char strHEX[500];
    u16 PROID_length = strlen(PROID);
    u16 DEVID_length = strlen(DEVID);
    u16 AUTH_INFO_length = strlen(AUTH_INFO);
    u16 MQTT_length = strlen("MQTT");
    u16 packetLen;
    ClearRAM((u8*)str,200);   //���commandAT���ݰ����棬�����������ip�˿ںͳ���
    ClearRAM((u8*)strHEX,200); //���locationHex���ݰ����棬ת��Ϊhex ��Ҫ���͵���ʵ����
    packetLen = 12;//�̶�������ͷ����
    if(PROID_length > 0)
        packetLen = packetLen + 2 + PROID_length;
    if(DEVID_length > 0)
        packetLen = packetLen+ 2 + DEVID_length;
    if(AUTH_INFO_length > 0)
        packetLen = packetLen+ 2 + AUTH_INFO_length;

    strncpy((char*)(mqtt_message+strlen((char*)mqtt_message)),"10",2);
    sprintf(strHEX,"%02x",(packetLen-2));

    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);   //���commandAT���ݰ����棬�����������ip�˿ںͳ���
    ClearRAM((u8*)strHEX,200); //���locationHex���ݰ����棬ת��Ϊhex ��Ҫ���͵���ʵ����
    strcat(mqtt_message,"00044D51545404C00100");
    //װ���豸ID�����ݰ�		
    sprintf(str, "%02x%02x",((0xff00&(DEVID_length))>>8),(0xff&DEVID_length));
    strcat(mqtt_message,str);
    ClearRAM((u8*)str,200);
    sprintf(str, "%s",DEVID);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    //װ�ز�ƷID�����ݰ�
    sprintf(str, "%02x%02x",((0xff00&(PROID_length))>>8),(0xff&PROID_length));
    strcat(mqtt_message,str);
    ClearRAM((u8*)str,200);
    sprintf(str, "%s",PROID);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    //װ�ؼ�Ȩ��Ϣ�����ݰ�
    sprintf(str, "%02x%02x",((0xff00&(AUTH_INFO_length))>>8),(0xff&AUTH_INFO_length));
    strcat(mqtt_message,str);
    ClearRAM((u8*)str,200);
    sprintf(str, "%s",AUTH_INFO);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_message,strHEX);
    strcat(mqtt_message,"\r\n");
    return packetLen;
}
//����MQTT���ݰ� 
u16 mqtt_SendData_message(char *mqtt_messageHEX,char *SendData,short length)
{
    char str[200];
    char strHEX[200];
    u16 packetLen=length+8;
    u16 SendData_length = strlen(SendData);
	
    strncpy((char*)(mqtt_messageHEX+strlen((char*)mqtt_messageHEX)),"30",2);
    sprintf(strHEX,"%02x",(packetLen));
    strcat(mqtt_messageHEX,strHEX);
	
    ClearRAM((u8*)str,200);   //���commandAT���ݰ����棬�����������ip�˿ںͳ���
    ClearRAM((u8*)strHEX,200); //���locationHex���ݰ����棬ת��Ϊhex ��Ҫ���͵���ʵ����
    
	  strcat(mqtt_messageHEX,"01000324647001");
    //װ��json���ݵ����ݰ�
    sprintf(str, "%02x%02x",((0xff00&(SendData_length))>>8),(0xff&SendData_length));
    strcat(mqtt_messageHEX,str);
	
    ClearRAM((u8*)str,200);
	
    sprintf(str, "%s",SendData);
    Str2Hex((char*)str,(char*)strHEX);
    strcat(mqtt_messageHEX,strHEX);
	
    ClearRAM((u8*)str,200);
    ClearRAM((u8*)strHEX,200);
		
    return packetLen;
}
