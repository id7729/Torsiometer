#ifndef _VAR_MALLOC_H_
#define _VAR_MALLOC_H_
/**********************************************************************
include
***********************************************************************/
#include "stdint.h"
/**********************************************************************
define
***********************************************************************/
/********************************************************/
//工作模式宏定义
#define WORK_MODE_POWERON   1   //开机界面工作模式
#define WORK_MODE_VOLTEST   2   //电压环境工作模式


/**********************************************************************
typedef
***********************************************************************/
/*****************************************************************
//4字节空间联合体
*****************************************************************/
typedef union {
    float       tempf;
    int32_t     tempI32;
    uint32_t    tempU32;
    int16_t     tempI16[2];
    uint16_t    tempU16[2];
    int8_t      tempI8[4];
    uint8_t     tempU8[4];
}TEMP_UNI;

/*****************************************************************
//系统相关结构体
*****************************************************************/
typedef struct {
    uint8_t WorkMode;                           //工作模式
    
    uint16_t _200MS_MAIN_TIMER;                 //主函数定时器
    uint16_t _500MS_MAIN_TIMER;
    uint16_t _1S_MAIN_TIMER;
    uint16_t _2S_MAIN_TIMER;
    uint16_t _3S_MAIN_TIMER;
    uint16_t _5S_MAIN_TIMER;
    
    uint16_t _100MS_WORK_TIMER;                 //工作定时器
    uint16_t _200MS_WORK_TIMER;
    uint16_t _300MS_WORK_TIMER;                 
    uint16_t _500MS_WORK_TIMER;  
    uint16_t _1S_WORK_TIMER;  
    uint16_t _30S_WORK_TIMER;  
}SYSTEM_STR, *PT_SYSTEM_STR;

/**********************************************************************
variable extern
***********************************************************************/
extern TEMP_UNI             temp_uni;
extern PT_SYSTEM_STR        pSYSTEM;
/**********************************************************************
extern fun
***********************************************************************/
signed char VarMalloc(void);

#endif
