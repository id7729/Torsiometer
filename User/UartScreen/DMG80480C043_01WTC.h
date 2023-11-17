#ifndef _SCREEN_H_
#define _SCREEN_H_
/**********************************************************************
include
***********************************************************************/
//#include "headfile.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "VarMalloc.h"
/**********************************************************************
define
***********************************************************************/
//---------------------------         串口号       ------------------------------//
#define SCREEN_UART_PORT                UART6
#define SCREEN_UART_BAUD                115200

//---------------------------         命令         ------------------------------//
#define CMD_WRITE_VAR                   0X82    //写系统变量
#define CMD_READ_VAR                    0X83    //读系统变量
#define SCREEN_BACK_LEN_MIN             6       //返回一帧的最短长度
#define SCREEN_BACK_LEN_MAX             32      //返回一帧的最短长度

//---------------------------         页面ID       ------------------------------//
#define PAGE_ID_POWER_ON                0       //上电界面
#define PAGE_ID_VOL                     2       //电压环境界面

//---------------------------         图标ID       ------------------------------//
#define ICON_ID_WATER_0                 1
#define ICON_ID_WATER_25                2
#define ICON_ID_WATER_50                3
#define ICON_ID_WATER_75                4
#define ICON_ID_WATER_100               5

#define ICON_ID_DUSTBIN_0               11
#define ICON_ID_DUSTBIN_25              12
#define ICON_ID_DUSTBIN_50              13
#define ICON_ID_DUSTBIN_75              14
#define ICON_ID_DUSTBIN_100             15

//---------------------------     系统变量地址    ------------------------------//
#define ADR_DEVICE_ID                   0X0000  //T5 CPU ID
#define ADR_SYSTEM_RESET                0X0004  //系统复位
#define ADR_OS_UPDATE_CMD               0X0006  //
#define ADR_NOR_FLASH_RW_CMD            0X0008  //
#define ADR_VER                         0X000F  //
#define ADR_RTC_READ                    0X0010  //读取RTC
#define ADR_PIC_NOW                     0X0014  //读取当前显示页面ID
#define ADR_GUI_STATUS                  0X0015  //
#define ADR_TP_STATUS                   0X0016  //
#define ADR_VCC_NOW                     0X0030  //
#define ADR_LED_NOW                     0X0031  //
#define ADR_AD0_AD1                     0X0032  //
#define ADR_SD_LOAD                     0X007C  //
#define ADR_SYSTEM_CONFIG               0X0080  //
#define ADR_LED_CONFIG                  0X0082  //设置触摸屏背光待机时间
#define ADR_PIC_SET                     0X0084  //切换页面
#define ADR_RTC_SET                     0X009C  //设置RTC
#define ADR_MUSIC_PLAY_SET              0X00A0  //
#define ADR_BMP_DOWNLOAD                0X00A2  //
#define ADR_JPEG_DOWNLOAD               0X00A6  //
#define ADR_NAND_FLASH_RW_CMD           0X00AA  //
#define ADR_DCS_BUS_DATA                0X0100  //

//---------------------------     自定义变量地址    ------------------------------//
//首页
#define ADR_KEY_CLEAR_TARE              0X5000  //去皮按键
#define ADR_KEY_CALIB					0x5001	//标定按钮
#define ADR_KEY_TORSIONAL_VALUE			0x5002	//扭力值显示按键
#define ADR_RETURN_STATUS_BAR			0x5003	//状态返回文本框
#define ADR_KEY_RGY_LIGHT				0x5004	//红绿黄灯显示按键
#define TEXT_DIS_TIP_LEN				92		//文本框显示的长度


//标定界面
#define ADR_KEY_CALIB_BACK              0X5010  //返回
#define ADR_CEILING_BAR               	0X5011  //上限数据框
#define ADR_FLOOR_BAR					0x5012	//下限数据框
#define ADR_KEY_CALIB_CEILING_SUB       0X5013  //上限减
#define ADR_KEY_CALIB_CEILING_PLUS      0X5014  //上限加
#define ADR_KEY_CALIB_FLOOR_SUB         0X5015  //下限减
#define ADR_KEY_CALIB_FLOOR_PLUS        0X5016  //下限加

//扭力值界面
#define ADR_KEY_TORSIONAL_BACK	             	0X5020  //扭力值界面返回主菜单
#define ADR_RETURN_TORSIONAL_VALUE_BAR          0X5021  //扭力值显示文本框
#define ADR_DATA_VARIABLES_BAR					0x5022	//扭力值数据变量框
#define TEXT_DIS_VALUE_LEN						92

//红绿黄灯显示界面
#define ADR_KEY_RGY_LIGHT_BACK					0x5030
#define ADR_VAR_ICON							0x5031

/**********************************************************************
typedef
***********************************************************************/
/*******************************************************************
    串口屏一帧结构体(无校验)(发送一帧写系统变量命令)
    (5A A5 长度 82 系统变量地址 数据)
*******************************************************************/
typedef struct {
    unsigned short Head;           //头    0x5AA5
    unsigned char  Len;            //长度(包括指令、数据)
    unsigned char  Cmd;            //指令 0X82
    unsigned short Adr;            //系统变量地址
    unsigned char  Data[1018];     //数据
}SCREEN_WRITE_VAR_STR, *PT_SCREEN_WRITE_VAR_STR;

/*******************************************************************
    串口屏一帧结构体(无校验)(发送一帧读系统变量命令)
    (5A A5 04 83 变量地址 读取长度)
*******************************************************************/
typedef struct {
    unsigned short Head;           //头    0x5AA5
    unsigned char  Len;            //长度(包括指令、数据)
    unsigned char  Cmd;            //指令 0X83
    unsigned short Adr;            //系统变量地址
    unsigned char  Rlen;           //读取数据长度
}SCREEN_READ_VAR_STR, *PT_SCREEN_READ_VAR_STR;

/*******************************************************************
    串口屏一帧结构体(无校验)(发送一帧写/读系统变量命令(反馈))
    (5A A5 03   82 4F 4B)
    (5A A5 长度 83 系统变量地址 读取长度 数据)
*******************************************************************/
typedef struct {
    unsigned short Head;           //头    0x5AA5
    unsigned char  Len;            //长度(包括指令、数据)
    unsigned char  Cmd;            //指令 0X82 0X83
    unsigned char  Data[32];       //(0X4F 0X4B) / (系统变量地址 读取数据长度 数据)
}SCREEN_FRAME_STR, *PT_SCREEN_FRAME_STR;

/*******************************************************************
    串口屏处理相关结构体
*******************************************************************/
typedef	struct {
	unsigned short RevLen;		        //接收长度
	unsigned char  TimActiveFlag;	    //打开1/关闭0
	unsigned char  CMDRevDelayParam;	//命令延时值10us一次，用来判断完整一条命令
	unsigned char  CMDHandleFlag;		//新命令接收到，处理标志位
	unsigned char  Filter1;		        //
    unsigned char  WaitTimeActiveFlag;  //超时等待计时打开
    unsigned char  CMDWaitDelayParm;	//指令返回等待超时计时
	
	unsigned char* InDataBuf;	        //接收缓存区(在串口接收中断函数里使用)
	unsigned char* InDataWritePtr;	    //接收写指针
	unsigned char* InDataReadPtr;	    //接收读指针
	unsigned char* InDataTailPtr;	    //接收尾指针
    PT_SCREEN_FRAME_STR InDataTempBuf;  //解析缓冲区(报文解析过程用)
}SCREEN_PRO_STR, *PT_SCREEN_PRO_STR;
                    
/**********************************************************************
variable extern
***********************************************************************/
extern PT_SCREEN_WRITE_VAR_STR  pSCREEN_WRITE_SEND;
extern PT_SCREEN_READ_VAR_STR   pSCREEN_READ_SEND;
extern PT_SCREEN_PRO_STR        pSCREEN_PRO;
extern unsigned char KeyState;
extern unsigned short ScreenRevLen;
extern unsigned short ScreenRevAdr; 
extern unsigned char ScreenRevBuf[];
extern unsigned char ScreenSendIndex;
extern unsigned char ScreenSendBuf[];
/**********************************************************************
fun extern
***********************************************************************/
//屏幕通讯口初始化
void SCREENInit(void);

unsigned char SCREENVarMalloc(void);
signed char SCREENHandle(void);

//读写系统变量
unsigned char SCREENReadVarCmd(unsigned short, unsigned char);
unsigned char SCREENWriteVarCmd(unsigned short, unsigned char *, unsigned char);

//显示字符串
void SCREEN_CLEAR_INF(void);
void SCREENprintf(uint32_t,char*,...);

//页面切换
void SCREENPageSwitch(unsigned short);
void GUI_UpdateKeyStatus(unsigned char);
#endif
