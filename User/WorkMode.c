/**********************************************************************
include
***********************************************************************/
#include "WorkMode.h"
#include "DMG80480C043_01WTC.h"
#include "RS485_driver_private.h"
#include "demo.h"
#include "LED.h"
uint8_t ZeroClearing[11] = {0x01, 0x10, 0x00, 0x5E, 0x00, 0x01, 0x02, 0x00, 0x01, 0x6A, 0xFE};
uint8_t CurrentIndex1 = 0;			//标定上限当前值，用于点灯
uint8_t LastIndex1 = 0;				//上一次标定上限值，主要用于关灯的作用
uint8_t CurrentIndex2 = 0;			//标定下限当前值，用于点灯
uint8_t LastIndex2 = 0;             //上一次标定下限值，主要用于关灯的作用
uint8_t Ceiling_Value = 0;			//上限值具体值，是加/减的操作对象	
uint8_t Floor_Value = 0;			//下限值具体值，是加/减的操作对象
int offset = 0;

/**********************************************************************
fun
***********************************************************************/


/**********************************************************
 * @fun		Sustain_CeilingLed_ON()
 * @brief   设置LED灯光的常亮状态(表示上限和下限的LED)
 * @note   	 
 * @param   uint8_t ScreenRevBuf[1] - 传入上限/下限值
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
 * @brief   设置LED灯光的常亮状态(表示上限和下限的LED)
 * @note   	 
 * @param   uint8_t ScreenRevBuf[1] - 传入上限/下限值
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
 * @brief   首页菜单界面,处理
 * @note    
 * @param   无
 * @return  无
 ******************************************************************/
void Menu_Pro(void)
{
    
    /******************************************************************/
    //按键处理
    if(ScreenRevAdr)
    {
        switch(ScreenRevAdr)
        {
            //去皮
            case ADR_KEY_CLEAR_TARE:				
                offset = Num;
				SCREENprintf(ADR_RETURN_STATUS_BAR,"Request sent successfully");
                break;
            
            //上限加
            case ADR_KEY_CALIB_CEILING_PLUS:
				Ceiling_Value++;
				Sustain_CeilingLed_ON();			
                break;

            //上限减
            case ADR_KEY_CALIB_CEILING_SUB:
				if((Ceiling_Value - 1 > Floor_Value) && (Ceiling_Value > 1))
				{
					Ceiling_Value--;					
				}
				Sustain_CeilingLed_ON();			
                break;									
			
			//下限加
			case ADR_KEY_CALIB_FLOOR_PLUS:
				if(Ceiling_Value - 1 > Floor_Value)
				{
					Floor_Value++;					
				}
				Sustain_FloorLed_ON();
				break;

			//下限减
			case ADR_KEY_CALIB_FLOOR_SUB:
				if(Floor_Value > 1)
				{
					Floor_Value--;
				}
				Sustain_FloorLed_ON();
				break;
			
            //扭力值
            case ADR_KEY_TORSIONAL_VALUE:
                break;
			
			//标定按键
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

