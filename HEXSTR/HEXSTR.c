#include "HEXSTR.h"

const unsigned char Num2CharTable[] = "0123456789ABCDEF";


//��16��������ת��Ϊ��Ӧ��ASCII���ַ�������ʾ����
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
//	�������ƣ�	tringToHex
//
//	�������ܣ�	16�����ַ���תΪ16��������
//
//	��ڲ�����	hex:ת�����16�������飻str:ת��ǰ��16�����ַ���
//
//	���ز�����	��
//
//	˵����		Ч�ʽϵ�
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
