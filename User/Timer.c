#include "ac78xx.h"                     // Device header
#include "ac78xx_timer.h"               // AutoChips::Device:ATC Drivers:TIMER
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO
#include "LED.h"
#include "Round_Robin_Sche.h"
#include "DMG80480C043_01WTC.h"
//#define PAout(n)   BIT_ADDR(GPIOA_ODR_ADDR, n)
//#define LED1 PAout(13)
//uint8_t counter = 10;
//uint16_t i = 0;

//串口屏接收一帧数据判断2ms
void Task_Wait_One_Frame(void)
{
	if(pSCREEN_PRO->TimActiveFlag)
	{
		pSCREEN_PRO->CMDRevDelayParam ++;
		if(pSCREEN_PRO->CMDRevDelayParam>=2)
		{
			pSCREEN_PRO->CMDRevDelayParam=0;
			pSCREEN_PRO->TimActiveFlag=0;
			pSCREEN_PRO->CMDHandleFlag=1;
		}
	}	
}

//串口屏等待反馈超时判断100ms
void Task_Wait_Respond_Timeout(void)
{
	if(pSCREEN_PRO->WaitTimeActiveFlag)
	{
		pSCREEN_PRO->CMDWaitDelayParm ++;
		if(pSCREEN_PRO->CMDWaitDelayParm>=100)
			pSCREEN_PRO->CMDWaitDelayParm=100;
	}	
}


void TIM1_Task(uint8_t timerIndex)
{
    if (TIMER_GetIntFlag(TIMER1))
    {
		Task_Wait_One_Frame();
		Task_Wait_Respond_Timeout();
//		counter--;
//		if(counter == 0)
//		{
		LED_TASK();	
//			counter = 10;
//		}
		TaskRemarks();
		TIMER_ClrIntFlag(TIMER1);
	}
}

void Timer_Init(void)
{
    TIMER_ConfigType  timerConfig1 = {0};
    /* configure TIMER channel 1, enable timer */
    timerConfig1.loadValue      = 48000;   //系统时钟96M
    timerConfig1.interruptEn      = 1;
    timerConfig1.linkMode        = 0;
    timerConfig1.timerEn          = 1;
	
    TIMER_SetCallback(TIMER1, TIM1_Task);
    TIMER_Init(TIMER1, &timerConfig1);
}


