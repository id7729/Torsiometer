#ifndef __DEMO_H__
#define __DEMO_H__

//联合体类型声明，该联合体中，n与c共用一块内存空间，一个n等价于四个c，只不过表现形式不一样
typedef union 
{
	int n;	
	unsigned char c[4];		
}Data_Processing_U;

extern float Num;

void SendRequst(void);

void Led_Control_Task(void);

void Task_Screen(void);

void Show_Torsional_Value_Task(void);
void Show_Ceiling_Floor_Value_Task(void);

void Led_Control_Test_Task(void);

void RGY_Light_Task(void);

void Speaker_Task(void);

extern uint8_t ReadMeasuredValue[];
extern Data_Processing_U Torque_value;//变量声明
#endif
