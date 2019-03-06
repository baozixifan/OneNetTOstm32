#include "HEXSTR.h"

const unsigned char Num2CharTable[] = "0123456789ABCDEF";

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
