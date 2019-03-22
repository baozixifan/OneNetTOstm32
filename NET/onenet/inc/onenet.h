#ifndef _ONENET_H_
#define _ONENET_H_


#define SEND_TYPE_HEART			2	//心跳请求


_Bool OneNet_DevLink(void);

void OneNet_SendData(void);

void OneNet_RevPro(unsigned char *cmd);

unsigned char OneNET_SendData_Heart(void);

_Bool OneNet_Check_Heart(void);

unsigned char OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);

#endif
