#include "ac78xx.h"                     // Device header
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO

#define PIN_DATA_2A		GPIO_PD0	//数据线
#define PIN_CLK_2A		GPIO_PD1	//时钟线

/**************************************************************************************
* FunctionName   : Init_WTN6040()
* Description    : WTN6040初始化函数，用于初始化芯片的控制引脚以及初始化延时函数
* EntryParameter : none
* ReturnValue    : none
* attention      : ***    ***
**************************************************************************************/
void Init_WTN6040(void)
{
	GPIO_SetDir(PIN_DATA_2A, 1);	//将数据线引脚设为输出
	GPIO_SetDir(PIN_CLK_2A, 1);		//将时钟线引脚设为输出
	InitDelay();
}


/**************************************************************************************
* FunctionName   : Line_2A_WTN6(uint8_t DDATA)
* Description    : 实现二线串口通信函数
* EntryParameter : DDATA为要发送的数据
* ReturnValue    : 空
* attention      : ***    ***
**************************************************************************************/
void Line_2A_WTN6(uint8_t DDATA)
{
	uint8_t S_DATA,j,B_DATA;
	GPIO_SetPinBits(PIN_CLK_2A);	//时钟线
	GPIO_SetPinBits(PIN_DATA_2A);	//数据线
	S_DATA = DDATA;
	GPIO_ResetPinBits(PIN_CLK_2A);
	mdelay(10);						//延时5ms
	B_DATA = S_DATA&0x01;
	
	for(j=0;j<8;j++)
	{
		GPIO_ResetPinBits(PIN_CLK_2A);
		GPIO_SetPinValue(PIN_DATA_2A, B_DATA);
		udelay(600);							//延时300us
		GPIO_SetPinBits(PIN_CLK_2A);
		udelay(600);							//延时300us
		
		S_DATA = S_DATA>>1;
		B_DATA = S_DATA&0x01;
	}
	GPIO_SetPinBits(PIN_DATA_2A);
	GPIO_SetPinBits(PIN_CLK_2A);
	mdelay(4);
}
