/**********************************************************************
include
***********************************************************************/
#include "WorkMode.h"
#include "DMG80480C043_01WTC.h"
#include "RS485_driver_private.h"
#include "demo.h"
#include "LED.h"
uint8_t ZeroClearing[11] = {0x01, 0x10, 0x00, 0x5E, 0x00, 0x01, 0x02, 0x00, 0x01, 0x6A, 0xFE};
uint8_t CurrentIndex1 = 0;			//�궨���޵�ǰֵ�����ڵ��
uint8_t LastIndex1 = 0;				//��һ�α궨����ֵ����Ҫ���ڹصƵ�����
uint8_t CurrentIndex2 = 0;			//�궨���޵�ǰֵ�����ڵ��
uint8_t LastIndex2 = 0;             //��һ�α궨����ֵ����Ҫ���ڹصƵ�����
uint8_t Ceiling_Value = 0;			//����ֵ����ֵ���Ǽ�/���Ĳ�������	
uint8_t Floor_Value = 0;			//����ֵ����ֵ���Ǽ�/���Ĳ�������
int offset = 0;

/**********************************************************************
fun
***********************************************************************/


/**********************************************************
 * @fun		Sustain_CeilingLed_ON()
 * @brief   ����LED�ƹ�ĳ���״̬(��ʾ���޺����޵�LED)
 * @note   	 
 * @param   uint8_t ScreenRevBuf[1] - ��������/����ֵ
 * @return  
 *********************************************************/
void Sustain_CeilingLed_ON(void)
{
	Led_Control(LastIndex1, 0, 0);
	CurrentIndex1 = Ceiling_Value;
	Led_Control(CurrentIndex1, 1, 0);
	LastIndex1 = CurrentIndex1;
}

/**********************************************************
 * @fun		Sustain_FloorLed_ON()
 * @brief   ����LED�ƹ�ĳ���״̬(��ʾ���޺����޵�LED)
 * @note   	 
 * @param   uint8_t ScreenRevBuf[1] - ��������/����ֵ
 * @return  
 *********************************************************/
void Sustain_FloorLed_ON(void)
{
	Led_Control(LastIndex2, 0, 0);
	CurrentIndex2 = Floor_Value;
	Led_Control(CurrentIndex2, 1, 0);
	LastIndex2 = CurrentIndex2;
}

   

    
/*******************************************************************
 * @func	Menu_Pro()
 * @brief   ��ҳ�˵�����,����
 * @note    
 * @param   ��
 * @return  ��
 ******************************************************************/
void Menu_Pro(void)
{
    
    /******************************************************************/
    //��������
    if(ScreenRevAdr)
    {
        switch(ScreenRevAdr)
        {
            //ȥƤ
            case ADR_KEY_CLEAR_TARE:				
                offset = Num;
				SCREENprintf(ADR_RETURN_STATUS_BAR,"Request sent successfully");
                break;
            
            //���޼�
            case ADR_KEY_CALIB_CEILING_PLUS:
				Ceiling_Value++;
				Sustain_CeilingLed_ON();			
                break;

            //���޼�
            case ADR_KEY_CALIB_CEILING_SUB:
				if((Ceiling_Value - 1 > Floor_Value) && (Ceiling_Value > 1))
				{
					Ceiling_Value--;					
				}
				Sustain_CeilingLed_ON();			
                break;									
			
			//���޼�
			case ADR_KEY_CALIB_FLOOR_PLUS:
				if(Ceiling_Value - 1 > Floor_Value)
				{
					Floor_Value++;					
				}
				Sustain_FloorLed_ON();
				break;

			//���޼�
			case ADR_KEY_CALIB_FLOOR_SUB:
				if(Floor_Value > 1)
				{
					Floor_Value--;
				}
				Sustain_FloorLed_ON();
				break;
			
            //Ť��ֵ
            case ADR_KEY_TORSIONAL_VALUE:
                break;
			
			//�궨����
            case ADR_KEY_CALIB:
				SCREEN_CLEAR_INF();
				break;
			
			case ADR_KEY_RGY_LIGHT:
				break;
               
            default:break;
        }
        ScreenRevAdr = 0;
    }
    
}

