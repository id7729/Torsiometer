#include "ac78xx.h"                     // Device header
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO


uint8_t count = 0;

uint8_t data[100] = {0};//���ڼ�¼������ݵ�С���ӣ�Ҳ����TASKִ�е�����


/**
	*@brief		LED�Ƶĳ�ʼ��
	*@param		��
	*@retval	��
*/
void LED_Init(void)
{
	//����LED����Ϊ���
	GPIOA->GPIO_CR.GPIO_CR_Byte |= 0x000003FF;
	GPIOB->GPIO_CR.GPIO_CR_Byte |= 0x000003FF;
	
	//��LED���ų�ʼ����0
	GPIOA->GPIO_ODR.Byte &= 0xFFFFFC00;
	GPIOB->GPIO_ODR.Byte &= 0xFFFFFC00;
	
}

/**
	*@brief		���ڿ�����Ҫ��Ƶ�����
	*@param		index	���ڵ�Ƶ���λ��
	*@param		status	���ƵƵ�״̬		1 --- ��ʾ��		0 --- ��ʾ��
	*@param		flag	����ѡ���ǵ���������index���ĵƻ���0-index���еĵ�
	*@retval	none
*/
void Led_Control(uint8_t index, uint8_t status, uint8_t flag)
{
	if(flag == 1)	//����0-index���еĵ�
	{
		uint8_t i = 0;
		for(i = 0; i < index; i++)
		{
			data[i] = status;
		}
	}
	else	//��������ĳ����
	{
		data[index] = status;
	}
}


/**
	*@brief		����LED�Ƶ�ˢ��
	*@param		��
	*@retval	��
*/
//void LED_TASK(void)
//{
//	static uint8_t counter = 0;		
////	udelay(10);	
//	counter++;
//	if(counter == 1)
//	{
//		GPIOA->GPIO_ODR.Byte = 0xFC00;//�ر�����Ƭѡ��
//		GPIOB->GPIO_ODR.Byte = 0xFC00;//��λѡ��ȫ����0
//	}
//	if(counter > 1)
//	{		
//		GPIOA->GPIO_ODR.Byte = (0x0001 << count);
//		uint8_t t = 0;
//		for(t = 0; t < 10; t++)
//		{
//			if(data[count*10 + t] == 1)
//			{
//				GPIO_SetPinBits(GPIO_PB0 + t);
//	//			mdelay(10);
//			}					
//		}
//		counter = 0;
//		count++;
//		if(count > 9)
//		{
//			count = 0;
//		}
//	}
//}

void LED_TASK(void)
{
	static uint8_t counter = 0;		
//	udelay(10);	
	counter++;
	if(counter == 1)
	{
		GPIOB->GPIO_ODR.Byte &= 0xFC00;//��λѡ��ȫ����0
		
//		GPIOA->GPIO_ODR.Byte &= (~(0x0001) << count);//�ر�Ƭѡ�Σ�ÿ��TASK�͹ر���һ��Ƭѡ��	
//		GPIOA->GPIO_ODR.Byte = 0xFC00;//�ر�����Ƭѡ��
//		GPIOB->GPIO_ODR.Byte = 0xFC00;//��λѡ��ȫ����0
	}
	if(counter > 1)
	{		
		GPIOA->GPIO_ODR.Byte = (0x0001 << count);
		uint8_t t = 0;
		for(t = 0; t < 10; t++)
		{
			if(data[count*10 + t] == 1)
			{
				GPIO_SetPinBits(GPIO_PB0 + t);
	//			mdelay(10);
			}					
		}
		counter = 0;
		count++;
		if(count > 9)
		{
			count = 0;
		}
	}	
}
