#include "HEXSTR.h"

const unsigned char Num2CharTable[] = "0123456789ABCDEF";


//将16进制数组转换为对应的ASCII码字符串并显示出来
void HexArrayToString(unsigned char *hexarray, int length, unsigned char *string)
{
	int i = 0;
	while (i < length)
	{
		*(string++) = Num2CharTable[((hexarray[i] >> 4) & 0x0f) ];
		*(string++) = Num2CharTable[(hexarray[i] & 0x0f) ];
		i++;
	}
	*string = 0x0;
}


//==========================================================
//	函数名称：	tringToHex
//
//	函数功能：	16进制字符串转为16进制数组
//
//	入口参数：	hex:转化后的16进制数组；str:转化前的16进制字符串
//
//	返回参数：	无
//
//	说明：		效率较低
//==========================================================
void StringToHex(unsigned char* hex, char* str)
{
	int len = strlen(str);
	int tmp;
	for (int i = 0; i < len / 2; i++)
	{
		if (str[2 * i] >= '0' && str[2 * i] <= '9')
		{
			tmp = ((str[2 * i] - '0') << 4);
		}
		else if (str[2 * i] >= 'A' && str[2 * i] <= 'F')
		{
			tmp = (((str[2 * i] - 'A') + 10) << 4);
		}
		else if (str[2 * i] >= 'a' && str[2 * i] <= 'f')
		{
			tmp = (((str[2 * i] - 'a') + 10) << 4);
		}

		if (str[2 * i + 1] >= '0' && str[2 * i + 1] <= '9')
		{
			hex[i] = tmp + (str[2 * i + 1] - '0');
		}
		else if (str[2 * i + 1] >= 'A' && str[2 * i + 1] <= 'F')
		{
			hex[i] = tmp + (str[2 * i + 1] - 'A') + 10;
		}
		else if (str[2 * i + 1] >= 'a' && str[2 * i + 1] <= 'f')
		{
			hex[i] = tmp + (str[2 * i + 1] - 'a') + 10;
		}
	}
}
