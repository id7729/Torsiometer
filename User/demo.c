#include "ac78xx.h"                     // Device header
#include "demo.h"
#include "LED.h"
#include "math.h"
#include "RS485_driver_private.h"
#include "DMG80480C043_01WTC.h"
#include "WorkMode.h"
uint8_t ReadMeasuredValue[8] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x02, 0xA4, 0x0D};//��ȡ����ֵ������
Data_Processing_U Torque_value;
	
float Num=0;

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
	Num = ((Torque_value.n) + 68) / 10;	//ÿ��0.1N*m
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
	pdata[1] = Num;
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
	pdata[1] = Ceiling_Value;
	SCREENWriteVarCmd(ADR_CEILING_BAR, pdata, 2);
	pdata[1] = Floor_Value;
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
	if(Num > CurrentIndex1 || Num == CurrentIndex1)	//Numֵ��������ֵ��Ť��ֵ����״̬
	{
		unsigned char pdata[2] = {0, 2};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);
	}
	if(Num > CurrentIndex2 && Num < CurrentIndex1) //Num�������ޣ�С�����ޣ�����״̬
	{
		unsigned char pdata[2] = {0, 1};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);				
	}
	if(Num < CurrentIndex2 || Num == CurrentIndex2)	//NumС�����ޣ�Ť��ֵ��С״̬
	{
		unsigned char pdata[2] = {0, 0};
		SCREENWriteVarCmd(ADR_VAR_ICON, pdata, 2);
	}	
}
