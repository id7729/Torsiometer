#ifndef _WORKMODE_H_
#define _WORKMODE_H_
/**********************************************************************
include
***********************************************************************/
//#include "common.h"
#include "stdint.h"
/**********************************************************************
define
***********************************************************************/
#define ALIGN_LEFT                      0   //¾Ó×ó
#define ALIGN_MIDDLE                    1   //¾ÓÖÐ

void Menu_Pro(void);


/**********************************************************************
typedef
***********************************************************************/

/**********************************************************************
variable extern
***********************************************************************/
extern uint8_t CurrentIndex1;
extern uint8_t LastIndex1;
extern uint8_t CurrentIndex2;
extern uint8_t LastIndex2;
extern uint8_t Ceiling_Value;
extern uint8_t Floor_Value;
extern float offset;
/**********************************************************************
fun extern
***********************************************************************/
void Show_Torsional_Value(void);
void Show_Ceiling_Floor_Value_Task(void);

#endif
