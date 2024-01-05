#include "ac78xx.h"                     // Device header
#include "demo.h"
#include "LED.h"
#include "math.h"
#include "RS485_driver_private.h"
#include "DMG80480C043_01WTC.h"
#include "WorkMode.h"
#include "WTN6040.h"
uint8_t ReadMeasuredValue[8] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x02, 0xA4, 0x0D};//��ȡ����ֵ������
Data_Processing_U Torque_value;
int show_num = 0;	
int Num=0;
uint8_t flag_speaker_yet = 0;
uint8_t flag_speaker_yet_counter = 0;

/**************************************************************************************
* FunctionName   : SendRequst()
* Description    : ����������ֵ��ȡ������
* EntryParameter : None
* ReturnValue    : None
* attention      : *** ÿ��ֻ�ܷ��͹̶��ı���,��������100msִ��һ�� ***
**************************************************************************************/
void SendRequst(void)
{
	UART5_SendString(ReadMeasuredValue, 8);	
}

/**************************************************************************************
* FunctionName   : Led_Control_Task
* Description    : Led�ƹ�״̬���ƣ���Ť���������ش������ݽ��м򵥴���
* EntryParameter : void
* ReturnValue    : void
* attention      : *** ��Ϊһ������ȴ������� ***
**************************************************************************************/
void Led_Control_Task(void)
{

	Led_Control(100, 0, 1);				//�ر����еƹ�
	Led_Control(CurrentIndex1, 1, 0);	//�궨������ʾ
	Led_Control(CurrentIndex2, 1, 0);	//�궨������ʾ
	Num = ((Torque_value.n) + 68)/20;	//��λ0.2N*m
	show_num = (Num + 50) + offset;
//	Num = fabs(Num);
	if(show_num < 0)
	{
		show_num = 0;
	}
	Led_Control(show_num, 1, 1);		
}

/**************************************************************************************
* FunctionName   : Task_Screen
* Description    : ������������������ֳ�����������һ���Ǵ�������������������һ���Ǵ��������ܴ�����
* EntryParameter : none
* ReturnValue    : none
* attention      : ***    ***
**************************************************************************************/
void Task_Screen(void)
{
	/*****************************************************/
	//����ģʽ����
	Menu_Pro();
	/*****************************************************/
	//���������մ���	
	if(pSCREEN_PRO->CMDHandleFlag)
	{
		pSCREEN_PRO->CMDHandleFlag=0;
		SCREENHandle();
	}		
}

/**************************************************************************
 * @fun     Show_Torsional_Value()      
 * @brief   ��Ť��ֵ������ʾŤ��ֵ
 * @note    Num ��һ��ȫ�ֱ�������ʾ��Ƶ�������Ҳ��ʾŤ��ֵ�Ĵ�С
 * @param          
 * @return  ��
 *************************************************************************/
void Show_Torsional_Value_Task(void)
{
	unsigned char pdata[2] = {0};
	pdata[0] = 0x00;
	if(show_num < 50)
	{
		pdata[1] = 2 * (50 - show_num);
	}
	else
	{
		pdata[1] = 2 * (show_num - 50);
	}
	SCREENWriteVarCmd(ADR_DATA_VARIABLES_BAR, pdata, 2);
}


/**************************************************************************
 * @fun     Show_Ceiling_Floor_Value_Task()      
 * @brief   �ڱ궨������ʾ���޺�����ֵ
 * @note    Num ��һ��ȫ�ֱ�������ʾ��Ƶ�������Ҳ��ʾŤ��ֵ�Ĵ�С
 * @param          
 * @return  ��
 *************************************************************************/
void Show_Ceiling_Floor_Value_Task(void)
{
	unsigned char pdata[2] = {0};
	pdata[0] = 0x00;
	pdata[1] = (2 * Ceiling_Value);
	SCREENWriteVarCmd(ADR_CEILING_BAR, pdata, 2);
	pdata[1] = (2 * Floor_Value);
	SCREENWriteVarCmd(ADR_FLOOR_BAR, pdata, 2);
}

/**************************************************************************************
* FunctionName   : RGY_Light_Task
* Description    : ���ڱȽ�Ť��ֵ��������ֵ�Ĵ�С������ʹ��ָʾ�ƽ�������״̬��ʾ����
* EntryParameter : 
* ReturnValue    : 
* attention      : ***    ***
**************************************************************************************/
void RGY_Light_Task(void)
{
	if(show_num > CurrentIndex1)	//Numֵ��������ֵ��Ť��ֵ����״̬
	{
		unsigned char pdata[2] = {0, 2};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);
	}
	if(show_num > CurrentIndex2 && show_num < CurrentIndex1) //Num�������ޣ�С�����ޣ�����״̬
	{
		unsigned char pdata[2] = {0, 1};
		
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);				
	}
	if(show_num < CurrentIndex2)	//NumС�����ޣ�Ť��ֵ��С״̬
	{
		unsigned char pdata[2] = {0, 0};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);
	}	
}

/**************************************************************************************
* FunctionName   : Speaker_Task(void)
* Description    : ������ʾ������
* EntryParameter : none
* ReturnValue    : none
* attention      : ***    ***
**************************************************************************************/
void Speaker_Task(void)
{
	if(show_num > CurrentIndex1 && flag_speaker_yet == 0)	//Numֵ��������ֵ��Ť��ֵ����״̬
	{
		Line_2A_WTN6(OVER_CEILING);
		flag_speaker_yet = 1;
	}
	else if(show_num < CurrentIndex2)
	{
		flag_speaker_yet_counter++;
		if(flag_speaker_yet_counter > 3)
		{
			flag_speaker_yet = 0;
		}
	}
	
	
//	if(Num < CurrentIndex2)	//NumС�����ޣ�Ť��ֵ��С״̬
//	{
//		Line_2A_WTN6(UNDER_FLOOR);	
//	}
}
