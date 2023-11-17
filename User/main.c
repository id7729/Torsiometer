/******************************************************************************
 * @version: V2.0.1  demo
 ******************************************************************************/
 
/*************<include>****************/
#include "ac78xx.h"                     // Device header
#include "LED.h"
#include "Timer.h"
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO
#include "RS485_driver_public.h"
#include "Round_Robin_Sche.h"
#include "VarMalloc.h"
#include "DMG80480C043_01WTC.h"
/*************<end>********************/

int main(void)
{
	
//	InitDelay();
	LED_Init();
	RS485_Init();
	Timer_Init();
	SCREENInit();
	VarMalloc();
	
	while(1)
	{
		TaskProcess();

	}
}


