/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	hwtimer.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��Ƭ����ʱ����ʼ��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Э���
#include "onenet.h"

//�����豸
#include "BC35.h"

//Ӳ������
#include "hwtimer.h"

#include "stm32f10x.h"

unsigned short timerCount = 0;	//ʱ�����--��λ��


TIM_INFO timInfo = {0};

volatile unsigned char timer3out = 0;


/*
************************************************************
*	�������ƣ�	Timer1_8_Init
*
*	�������ܣ�	Timer1��8��PWM����
*
*	��ڲ�����	TIMx��TIM1 ���� TIM8
*				arr������ֵ
*				psc��Ƶֵ
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
	
	GPIO_InitTypeDef gpioInitStruct;
	TIM_TimeBaseInitTypeDef timerInitStruct;
	TIM_OCInitTypeDef timerOCInitStruct;

	if(TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	}
	
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStruct);	
	
	timerInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	timerOCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	timerOCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	timerOCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//�������:TIM����Ƚϼ��Ե�
	timerOCInitStruct.TIM_Pulse = 0;
	TIM_OC2Init(TIMx, &timerOCInitStruct);
	TIM_OC3Init(TIMx, &timerOCInitStruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE �����ʹ��	
	
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPEʹ��
	
	TIM_Cmd(TIMx, ENABLE);										//ʹ��TIMx

}


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, //TIM2
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	
							 
}



/*
************************************************************
*	�������ƣ�	TIM6_IRQHandler
*
*	�������ܣ�	RTOS��������ʱ�ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void TIM3_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		//do something...
       timer3out = 1;
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}

}

/*
************************************************************
*	�������ƣ�	TIM7_IRQHandler
*
*	�������ܣ�	Timer7�����жϷ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
//void TIM7_IRQHandler(void)
//{

//	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
//	{
//		if(oneNetInfo.netWork == 0)											//�������Ͽ�
//		{
//			if(++timerCount >= NET_TIME) 									//�������Ͽ���ʱ
//			{	
//				checkInfo.NET_DEVICE_OK = 0;								//���豸δ����־
//				
//				NET_DEVICE_ReConfig(0);										//�豸��ʼ����������Ϊ��ʼ״̬
//				
//				oneNetInfo.netWork = 0;
//			}
//		}
//		else
//		{
//			timerCount = 0;													//�������
//		}
//		
//		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
//	}

//}
