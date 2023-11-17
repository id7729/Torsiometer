#include "ac78xx.h"                     // Device header
#include "demo.h"
#include "LED.h"
#include "math.h"
#include "RS485_driver_private.h"
#include "DMG80480C043_01WTC.h"
#include "WorkMode.h"
uint8_t ReadMeasuredValue[8] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x02, 0xA4, 0x0D};//读取测量值请求报文
Data_Processing_U Torque_value;
	
float Num=0;

/**************************************************************************************
* FunctionName   : SendRequst()
* Description    : 传感器测量值获取请求发送
* EntryParameter : None
* ReturnValue    : None
* attention      : *** 每次只能发送固定的报文,在任务中100ms执行一次 ***
**************************************************************************************/
void SendRequst(void)
{
	UART5_SendString(ReadMeasuredValue, 8);	
}

/**************************************************************************************
* FunctionName   : Led_Control_Task
* Description    : Led灯光状态控制，对扭力传感器回传的数据进行简单处理
* EntryParameter : void
* ReturnValue    : void
* attention      : *** 作为一个任务等待被调用 ***
**************************************************************************************/
void Led_Control_Task(void)
{
	Led_Control(100, 0, 1);				//关闭所有灯光
	Led_Control(CurrentIndex1, 1, 0);	//标定上限显示
	Led_Control(CurrentIndex2, 1, 0);	//标定下限显示
	Num = ((Torque_value.n) + 68) / 10;	//每格0.1N*m
	Num = fabs(Num);
	Num -= offset;
	if(Num < 0)
	{
		Num = 0;
	}
	Led_Control(Num, 1, 1);		
}

/**************************************************************************************
* FunctionName   : Task_Screen
* Description    : 串口屏处理函数，里面分成两个子任务，一个是触摸屏按键处理函数，另一个是串口屏接受处理函数
* EntryParameter : none
* ReturnValue    : none
* attention      : ***    ***
**************************************************************************************/
void Task_Screen(void)
{
	/*****************************************************/
	//工作模式处理
	Menu_Pro();
	/*****************************************************/
	//串口屏接收处理	
	if(pSCREEN_PRO->CMDHandleFlag)
	{
		pSCREEN_PRO->CMDHandleFlag=0;
		SCREENHandle();
	}		
}

/**************************************************************************
 * @fun     Show_Torsional_Value()      
 * @brief   在扭力值界面显示扭力值
 * @note    Num 是一个全局变量，表示点灯的数量，也表示扭力值的大小
 * @param          
 * @return  无
 *************************************************************************/
void Show_Torsional_Value_Task(void)
{
	unsigned char pdata[2] = {0};
	pdata[0] = 0x00;
	pdata[1] = Num;
	SCREENWriteVarCmd(ADR_DATA_VARIABLES_BAR, pdata, 2);
}


/**************************************************************************
 * @fun     Show_Ceiling_Floor_Value_Task()      
 * @brief   在标定界面显示上限和下限值
 * @note    Num 是一个全局变量，表示点灯的数量，也表示扭力值的大小
 * @param          
 * @return  无
 *************************************************************************/
void Show_Ceiling_Floor_Value_Task(void)
{
	unsigned char pdata[2] = {0};
	pdata[0] = 0x00;
	pdata[1] = Ceiling_Value;
	SCREENWriteVarCmd(ADR_CEILING_BAR, pdata, 2);
	pdata[1] = Floor_Value;
	SCREENWriteVarCmd(ADR_FLOOR_BAR, pdata, 2);
}

/**************************************************************************************
* FunctionName   : RGY_Light_Task
* Description    : 用于比较扭力值与上下限值的大小，并且使用指示灯将其三种状态显示出来
* EntryParameter : 
* ReturnValue    : 
* attention      : ***    ***
**************************************************************************************/
void RGY_Light_Task(void)
{
	if(Num > CurrentIndex1 || Num == CurrentIndex1)	//Num值大于上限值，扭力值过大状态
	{
		unsigned char pdata[2] = {0, 2};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);
	}
	if(Num > CurrentIndex2 && Num < CurrentIndex1) //Num大于下限，小于上限，正常状态
	{
		unsigned char pdata[2] = {0, 1};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);				
	}
	if(Num < CurrentIndex2 || Num == CurrentIndex2)	//Num小于下限，扭力值过小状态
	{
		unsigned char pdata[2] = {0, 0};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);
	}	
}
