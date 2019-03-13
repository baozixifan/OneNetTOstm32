#ifndef _ONENET_H_
#define _ONENET_H_


#define SEND_TYPE_HEART			2	//ĞÄÌøÇëÇó


_Bool OneNet_DevLink(void);

void OneNet_SendData(void);

void OneNet_RevPro(unsigned char *cmd);

unsigned char OneNET_SendData_Heart(void);

_Bool OneNet_Check_Heart(void);

#endif
