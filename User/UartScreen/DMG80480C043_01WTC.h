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
//---------------------------         ���ں�       ------------------------------//
#define SCREEN_UART_PORT                UART6
#define SCREEN_UART_BAUD                115200

//---------------------------         ����         ------------------------------//
#define CMD_WRITE_VAR                   0X82    //дϵͳ����
#define CMD_READ_VAR                    0X83    //��ϵͳ����
#define SCREEN_BACK_LEN_MIN             6       //����һ֡����̳���
#define SCREEN_BACK_LEN_MAX             32      //����һ֡����̳���

//---------------------------         ҳ��ID       ------------------------------//
#define PAGE_ID_POWER_ON                0       //�ϵ����
#define PAGE_ID_VOL                     2       //��ѹ��������

//---------------------------         ͼ��ID       ------------------------------//
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

//---------------------------     ϵͳ������ַ    ------------------------------//
#define ADR_DEVICE_ID                   0X0000  //T5 CPU ID
#define ADR_SYSTEM_RESET                0X0004  //ϵͳ��λ
#define ADR_OS_UPDATE_CMD               0X0006  //
#define ADR_NOR_FLASH_RW_CMD            0X0008  //
#define ADR_VER                         0X000F  //
#define ADR_RTC_READ                    0X0010  //��ȡRTC
#define ADR_PIC_NOW                     0X0014  //��ȡ��ǰ��ʾҳ��ID
#define ADR_GUI_STATUS                  0X0015  //
#define ADR_TP_STATUS                   0X0016  //
#define ADR_VCC_NOW                     0X0030  //
#define ADR_LED_NOW                     0X0031  //
#define ADR_AD0_AD1                     0X0032  //
#define ADR_SD_LOAD                     0X007C  //
#define ADR_SYSTEM_CONFIG               0X0080  //
#define ADR_LED_CONFIG                  0X0082  //���ô������������ʱ��
#define ADR_PIC_SET                     0X0084  //�л�ҳ��
#define ADR_RTC_SET                     0X009C  //����RTC
#define ADR_MUSIC_PLAY_SET              0X00A0  //
#define ADR_BMP_DOWNLOAD                0X00A2  //
#define ADR_JPEG_DOWNLOAD               0X00A6  //
#define ADR_NAND_FLASH_RW_CMD           0X00AA  //
#define ADR_DCS_BUS_DATA                0X0100  //

//---------------------------     �Զ��������ַ    ------------------------------//
//��ҳ
#define ADR_KEY_CLEAR_TARE              0X5000  //ȥƤ����
#define ADR_KEY_CALIB					0x5001	//�궨��ť
#define ADR_KEY_TORSIONAL_VALUE			0x5002	//Ť��ֵ��ʾ����
#define ADR_RETURN_STATUS_BAR			0x5003	//״̬�����ı���
#define ADR_KEY_RGY_LIGHT				0x5004	//���̻Ƶ���ʾ����
#define TEXT_DIS_TIP_LEN				92		//�ı�����ʾ�ĳ���


//�궨����
#define ADR_KEY_CALIB_BACK              0X5010  //����
#define ADR_CEILING_BAR               	0X5011  //�������ݿ�
#define ADR_FLOOR_BAR					0x5012	//�������ݿ�
#define ADR_KEY_CALIB_CEILING_SUB       0X5013  //���޼�
#define ADR_KEY_CALIB_CEILING_PLUS      0X5014  //���޼�
#define ADR_KEY_CALIB_FLOOR_SUB         0X5015  //���޼�
#define ADR_KEY_CALIB_FLOOR_PLUS        0X5016  //���޼�

//Ť��ֵ����
#define ADR_KEY_TORSIONAL_BACK	             	0X5020  //Ť��ֵ���淵�����˵�
#define ADR_RETURN_TORSIONAL_VALUE_BAR          0X5021  //Ť��ֵ��ʾ�ı���
#define ADR_DATA_VARIABLES_BAR					0x5022	//Ť��ֵ���ݱ�����
#define TEXT_DIS_VALUE_LEN						92

//���̻Ƶ���ʾ����
#define ADR_KEY_RGY_LIGHT_BACK					0x5030
#define ADR_VAR_ICON							0x5031

/**********************************************************************
typedef
***********************************************************************/
/*******************************************************************
    ������һ֡�ṹ��(��У��)(����һ֡дϵͳ��������)
    (5A A5 ���� 82 ϵͳ������ַ ����)
*******************************************************************/
typedef struct {
    unsigned short Head;           //ͷ    0x5AA5
    unsigned char  Len;            //����(����ָ�����)
    unsigned char  Cmd;            //ָ�� 0X82
    unsigned short Adr;            //ϵͳ������ַ
    unsigned char  Data[1018];     //����
}SCREEN_WRITE_VAR_STR, *PT_SCREEN_WRITE_VAR_STR;

/*******************************************************************
    ������һ֡�ṹ��(��У��)(����һ֡��ϵͳ��������)
    (5A A5 04 83 ������ַ ��ȡ����)
*******************************************************************/
typedef struct {
    unsigned short Head;           //ͷ    0x5AA5
    unsigned char  Len;            //����(����ָ�����)
    unsigned char  Cmd;            //ָ�� 0X83
    unsigned short Adr;            //ϵͳ������ַ
    unsigned char  Rlen;           //��ȡ���ݳ���
}SCREEN_READ_VAR_STR, *PT_SCREEN_READ_VAR_STR;

/*******************************************************************
    ������һ֡�ṹ��(��У��)(����һ֡д/��ϵͳ��������(����))
    (5A A5 03   82 4F 4B)
    (5A A5 ���� 83 ϵͳ������ַ ��ȡ���� ����)
*******************************************************************/
typedef struct {
    unsigned short Head;           //ͷ    0x5AA5
    unsigned char  Len;            //����(����ָ�����)
    unsigned char  Cmd;            //ָ�� 0X82 0X83
    unsigned char  Data[32];       //(0X4F 0X4B) / (ϵͳ������ַ ��ȡ���ݳ��� ����)
}SCREEN_FRAME_STR, *PT_SCREEN_FRAME_STR;

/*******************************************************************
    ������������ؽṹ��
*******************************************************************/
typedef	struct {
	unsigned short RevLen;		        //���ճ���
	unsigned char  TimActiveFlag;	    //��1/�ر�0
	unsigned char  CMDRevDelayParam;	//������ʱֵ10usһ�Σ������ж�����һ������
	unsigned char  CMDHandleFlag;		//��������յ��������־λ
	unsigned char  Filter1;		        //
    unsigned char  WaitTimeActiveFlag;  //��ʱ�ȴ���ʱ��
    unsigned char  CMDWaitDelayParm;	//ָ��صȴ���ʱ��ʱ
	
	unsigned char* InDataBuf;	        //���ջ�����(�ڴ��ڽ����жϺ�����ʹ��)
	unsigned char* InDataWritePtr;	    //����дָ��
	unsigned char* InDataReadPtr;	    //���ն�ָ��
	unsigned char* InDataTailPtr;	    //����βָ��
    PT_SCREEN_FRAME_STR InDataTempBuf;  //����������(���Ľ���������)
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
//��ĻͨѶ�ڳ�ʼ��
void SCREENInit(void);

unsigned char SCREENVarMalloc(void);
signed char SCREENHandle(void);

//��дϵͳ����
unsigned char SCREENReadVarCmd(unsigned short, unsigned char);
unsigned char SCREENWriteVarCmd(unsigned short, unsigned char *, unsigned char);

//��ʾ�ַ���
void SCREEN_CLEAR_INF(void);
void SCREENprintf(uint32_t,char*,...);

//ҳ���л�
void SCREENPageSwitch(unsigned short);
void GUI_UpdateKeyStatus(unsigned char);
#endif
