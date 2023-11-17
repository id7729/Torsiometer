#include "ac78xx.h"                     // Device header
#include "LED.h"
#include "demo.h"
#include "RS485_driver_public.h"
#define Task_Max	(6)

//任务结构体
typedef struct _Task_Components
{
	uint8_t Run;			//程序运行标记：0-不运行，1-运行
	uint16_t Timer;			//计时器
	uint8_t ItvTime;		//任务运行间隔时间
	void (*TaskHook)(void);	//要运行的任务函数
	
}Task_Components;			//任务定义

static Task_Components TaskComps[] = 
{
	{0, 50, 100, SendRequst},						//发送扭力值获取请求报文
	{0, 100, 100, Led_Control_Task},				//LED状态更新任务(更新data[100]数组)
	{0, 10, 100, Task_Screen},						//
	{0, 500, 50, Show_Torsional_Value_Task},
	{0, 500, 20, Show_Ceiling_Floor_Value_Task},
	{0, 100, 20, RGY_Light_Task}
};

/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : 任务标志处理,数中断次数，达到对应次数的时候就置任务标志位
* EntryParameter : None
* ReturnValue    : None
* attention      : ***定时在中断调用此函数即可***
**************************************************************************************/

void TaskRemarks(void)
{
	uint8_t i;
	
	for(i=0; i<Task_Max; i++)			//逐个任务时间处理
	{
		if(TaskComps[i].Timer)			//时间不为0
		{
			TaskComps[i].Timer--;		//减去一个节拍
			if(TaskComps[i].Timer == 0)	//时间减完了
			{
				TaskComps[i].Timer = TaskComps[i].ItvTime;	//恢复计时器值，重新下一次
				TaskComps[i].Run = 1;		//任务标志位至1，任务可以运行
			}
		}
	}		
}
/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : 任务处理|判断该执行哪一个函数
* EntryParameter : None
* ReturnValue    : None
* attention      : *** 放在main函数里面的while(1)中即可 ***
**************************************************************************************/
void TaskProcess(void)
{
	uint8_t i;
	for(i=0; i<Task_Max; i++)			//逐个任务时间处理
	{
		if(TaskComps[i].Run)			//时间不为0
		{
			TaskComps[i].TaskHook();	//运行任务
			TaskComps[i].Run = 0;		//标志清0
		}
	}	
}


