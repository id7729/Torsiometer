#include "ac78xx.h"                     // Device header
#include "LED.h"
#include "demo.h"
#include "RS485_driver_public.h"
#define Task_Max	(6)

//����ṹ��
typedef struct _Task_Components
{
	uint8_t Run;			//�������б�ǣ�0-�����У�1-����
	uint16_t Timer;			//��ʱ��
	uint8_t ItvTime;		//�������м��ʱ��
	void (*TaskHook)(void);	//Ҫ���е�������
	
}Task_Components;			//������

static Task_Components TaskComps[] = 
{
	{0, 50, 100, SendRequst},						//����Ť��ֵ��ȡ������
	{0, 100, 100, Led_Control_Task},				//LED״̬��������(����data[100]����)
	{0, 10, 100, Task_Screen},						//
	{0, 500, 50, Show_Torsional_Value_Task},
	{0, 500, 20, Show_Ceiling_Floor_Value_Task},
	{0, 100, 20, RGY_Light_Task}
};

/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : �����־����,���жϴ������ﵽ��Ӧ������ʱ����������־λ
* EntryParameter : None
* ReturnValue    : None
* attention      : ***��ʱ���жϵ��ô˺�������***
**************************************************************************************/

void TaskRemarks(void)
{
	uint8_t i;
	
	for(i=0; i<Task_Max; i++)			//�������ʱ�䴦��
	{
		if(TaskComps[i].Timer)			//ʱ�䲻Ϊ0
		{
			TaskComps[i].Timer--;		//��ȥһ������
			if(TaskComps[i].Timer == 0)	//ʱ�������
			{
				TaskComps[i].Timer = TaskComps[i].ItvTime;	//�ָ���ʱ��ֵ��������һ��
				TaskComps[i].Run = 1;		//�����־λ��1�������������
			}
		}
	}		
}
/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : ������|�жϸ�ִ����һ������
* EntryParameter : None
* ReturnValue    : None
* attention      : *** ����main���������while(1)�м��� ***
**************************************************************************************/
void TaskProcess(void)
{
	uint8_t i;
	for(i=0; i<Task_Max; i++)			//�������ʱ�䴦��
	{
		if(TaskComps[i].Run)			//ʱ�䲻Ϊ0
		{
			TaskComps[i].TaskHook();	//��������
			TaskComps[i].Run = 0;		//��־��0
		}
	}	
}


