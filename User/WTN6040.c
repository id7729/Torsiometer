#include "ac78xx.h"                     // Device header
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO

#define PIN_DATA_2A		GPIO_PD0	//������
#define PIN_CLK_2A		GPIO_PD1	//ʱ����

/**************************************************************************************
* FunctionName   : Init_WTN6040()
* Description    : WTN6040��ʼ�����������ڳ�ʼ��оƬ�Ŀ��������Լ���ʼ����ʱ����
* EntryParameter : none
* ReturnValue    : none
* attention      : ***    ***
**************************************************************************************/
void Init_WTN6040(void)
{
	GPIO_SetDir(PIN_DATA_2A, 1);	//��������������Ϊ���
	GPIO_SetDir(PIN_CLK_2A, 1);		//��ʱ����������Ϊ���
	InitDelay();
}


/**************************************************************************************
* FunctionName   : Line_2A_WTN6(uint8_t DDATA)
* Description    : ʵ�ֶ��ߴ���ͨ�ź���
* EntryParameter : DDATAΪҪ���͵�����
* ReturnValue    : ��
* attention      : ***    ***
**************************************************************************************/
void Line_2A_WTN6(uint8_t DDATA)
{
	uint8_t S_DATA,j,B_DATA;
	GPIO_SetPinBits(PIN_CLK_2A);	//ʱ����
	GPIO_SetPinBits(PIN_DATA_2A);	//������
	S_DATA = DDATA;
	GPIO_ResetPinBits(PIN_CLK_2A);
	mdelay(10);						//��ʱ5ms
	B_DATA = S_DATA&0x01;
	
	for(j=0;j<8;j++)
	{
		GPIO_ResetPinBits(PIN_CLK_2A);
		GPIO_SetPinValue(PIN_DATA_2A, B_DATA);
		udelay(600);							//��ʱ300us
		GPIO_SetPinBits(PIN_CLK_2A);
		udelay(600);							//��ʱ300us
		
		S_DATA = S_DATA>>1;
		B_DATA = S_DATA&0x01;
	}
	GPIO_SetPinBits(PIN_DATA_2A);
	GPIO_SetPinBits(PIN_CLK_2A);
	mdelay(4);
}
