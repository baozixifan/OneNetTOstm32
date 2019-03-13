#ifndef _BC35_H_
#define _BC35_H_





#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void BC35_Init(void);

void BC35_Clear(void);

//void BC35_SendData(unsigned char *data, unsigned short len);

//unsigned char *BC35_GetIPD(unsigned short timeOut);

unsigned char *BC35_GetDATA(unsigned short timeOut);

unsigned char BC35_GetNSONMI(unsigned short timeOut);

//_Bool BC35_SendRevCmd(char *cmd, char *res);

_Bool BC35_SendREVCMD(char *cmd,unsigned short len,char *res);

_Bool BC35_SendCmd(char *cmd, char *res);

void BC35_SENDDATA(unsigned char *data, unsigned short len);


#endif
