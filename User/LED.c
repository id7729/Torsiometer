#include "ac78xx.h"                     // Device header
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO


uint8_t count = 0;

uint8_t data[100] = {0};//用于记录点灯数据的小本子，也就是TASK执行的依据


/**
	*@brief		LED灯的初始化
	*@param		无
	*@retval	无
*/
void LED_Init(void)
{
	//设置LED引脚为输出
	GPIOA->GPIO_CR.GPIO_CR_Byte |= 0x000003FF;
	GPIOB->GPIO_CR.GPIO_CR_Byte |= 0x000003FF;
	
	//将LED引脚初始化置0
	GPIOA->GPIO_ODR.Byte &= 0xFFFFFC00;
	GPIOB->GPIO_ODR.Byte &= 0xFFFFFC00;
	
}

/**
	*@brief		用于控制需要点灯的数量
	*@param		index	用于点灯到的位置
	*@param		status	控制灯的状态		1 --- 表示亮		0 --- 表示灭
	*@param		flag	用于选择是点亮单个（index）的灯还是0-index所有的灯
	*@retval	none
*/
void Led_Control(uint8_t index, uint8_t status, uint8_t flag)
{
	if(flag == 1)	//点亮0-index所有的灯
	{
		uint8_t i = 0;
		for(i = 0; i < index; i++)
		{
			data[i] = status;
		}
	}
	else	//单独点亮某个灯
	{
		data[index] = status;
	}
}


/**
	*@brief		用于LED灯的刷新
	*@param		无
	*@retval	无
*/
//void LED_TASK(void)
//{
//	static uint8_t counter = 0;		
////	udelay(10);	
//	counter++;
//	if(counter == 1)
//	{
//		GPIOA->GPIO_ODR.Byte = 0xFC00;//关闭所有片选端
//		GPIOB->GPIO_ODR.Byte = 0xFC00;//将位选段全部清0
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
		GPIOB->GPIO_ODR.Byte &= 0xFC00;//将位选段全部清0
		
//		GPIOA->GPIO_ODR.Byte &= (~(0x0001) << count);//关闭片选段，每次TASK就关闭上一个片选段	
//		GPIOA->GPIO_ODR.Byte = 0xFC00;//关闭所有片选端
//		GPIOB->GPIO_ODR.Byte = 0xFC00;//将位选段全部清0
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
