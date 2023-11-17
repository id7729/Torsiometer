/**********************************************************************
include 
***********************************************************************/
#include "DMG80480C043_01WTC.h"
#include "ac78xx_uart.h"                // AutoChips::Device:ATC Drivers:UART
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO

/**********************************************************************
varible
***********************************************************************/
PT_SCREEN_WRITE_VAR_STR pSCREEN_WRITE_SEND;     //写系统变量发送一帧指令
PT_SCREEN_READ_VAR_STR  pSCREEN_READ_SEND;      //读系统变量发送一帧指令
PT_SCREEN_PRO_STR       pSCREEN_PRO;            //串口屏处理结构体指针变量
unsigned char KeyState=1;           //串口屏上的按键工作状态，1接收，0忽略
unsigned short ScreenRevLen;
unsigned short ScreenRevAdr;
unsigned char ScreenRevBuf[200];
unsigned char ScreenSendIndex;
unsigned char ScreenSendBuf[200];
/**********************************************************************
fun
***********************************************************************/
/**********************************************************
 * @brief   
 * @note    
 * @param   
 * @param   
 * @return  无
 *********************************************************/
int32_t Uart6_IRQHandler(uint8_t port, uint32_t LSR0, uint32_t LSR1)
{
	static uint8_t revtemp;
	
	uint16_t ch;
	/* Tx */
	if(UART6->UARTn_IER.ETXE == 1 && (LSR0 & LSR0_THRE))
	{
		
	}
	/* Rx */
	if(UART6->UARTn_IER.ERXNE == 1 && (LSR0 & LSR0_DR))
	{
		if(pSCREEN_PRO->RevLen < 1024)   //一次性接收长度小于4096
		{
			*pSCREEN_PRO->InDataWritePtr++ = UART6->UARTn_RBR_THR;//这里应该注意。原来是数据寄存器的位置
			if(pSCREEN_PRO->InDataWritePtr >= pSCREEN_PRO->InDataTailPtr)
				pSCREEN_PRO->InDataWritePtr = pSCREEN_PRO->InDataBuf;
			pSCREEN_PRO->RevLen++;
			
			if(pSCREEN_PRO->TimActiveFlag == 0)
				pSCREEN_PRO->TimActiveFlag = 1;
			pSCREEN_PRO->CMDRevDelayParam = 0;
		}
		else
		{
			revtemp = UART6->UARTn_RBR_THR;//这里应该注意。原来是数据寄存器的位置
		} 
	}
	return 0;
}
/**********************************************************
 * @brief   
 * @note    
 * @param   
 * @param   
 * @return  无
 *********************************************************/
void SCREENInit(void)
{
//    uart_init(SCREEN_UART_PORT,SCREEN_UART_BAUD); 
	UART_SettingType uartSetting = {0};
	GPIO_SetFunc(GPIO_PA13, 3);//PA13复用功能3 --- UART6_Tx
	GPIO_SetFunc(GPIO_PA14, 3);//PA14复用功能3 --- UART6_Rx

	uartSetting.baudrate = 57600;
	uartSetting.dataBits = 8;
	uartSetting.stopBits = 1;
	uartSetting.parity = 0;
	uartSetting.fifoByte = 1;
	uartSetting.dmaEn = 0;
	uartSetting.rateStep = 0;
	UART_Init(SCREEN_UART_PORT, &uartSetting);

	UART_SetEventCallback(UART6, Uart6_IRQHandler);  /* Set UART6 Interrupt Callback */	
//    uart_rx_irq_en(SCREEN_UART_PORT);
	UART_SetRxIntEn(SCREEN_UART_PORT, 1);

	NVIC_ClearPendingIRQ(UART6_IRQn);//uartIndex
    NVIC_EnableIRQ(UART6_IRQn);	
}

/**********************************************************   
 * @brief   
 * @note    
 * @param   无
 * @return  无
 *********************************************************/
unsigned char SCREENVarMalloc(void)
{
	unsigned int MemUse;
    unsigned int l;
    
    MemUse = 0;
   
	//pSCREEN_WRITE_SEND - 发送写指令指针
    l=sizeof(SCREEN_WRITE_VAR_STR);
	pSCREEN_WRITE_SEND = (PT_SCREEN_WRITE_VAR_STR)malloc(l);
	if(pSCREEN_WRITE_SEND == NULL)return 1;
	memset(pSCREEN_WRITE_SEND,0,l);
	MemUse += l;
    
    //pSCREEN_READ_SEND - 发送读指令指针
    l=sizeof(SCREEN_READ_VAR_STR);
	pSCREEN_READ_SEND = (PT_SCREEN_READ_VAR_STR)malloc(l);
	if(pSCREEN_READ_SEND == NULL)return 2;
	memset(pSCREEN_READ_SEND,0,l);
	MemUse += l;
    
    //pSCREEN_PRO - 串口屏处理结构体指针
    l=sizeof(SCREEN_PRO_STR);
	pSCREEN_PRO = (PT_SCREEN_PRO_STR)malloc(l);
	if(pSCREEN_PRO == NULL)return 3;
	memset(pSCREEN_PRO,0,l);
	MemUse += l;
    
    //pSCREEN_PRO->InDataBuf - 接收缓冲区指针
    l=1024;
	pSCREEN_PRO->InDataBuf = (unsigned char*)malloc(l);
	if(pSCREEN_PRO->InDataBuf == NULL)return 4;
	memset(pSCREEN_PRO->InDataBuf,0,l);
	pSCREEN_PRO->InDataReadPtr = pSCREEN_PRO->InDataBuf;
	pSCREEN_PRO->InDataWritePtr = pSCREEN_PRO->InDataBuf;
	pSCREEN_PRO->InDataTailPtr = (unsigned char*)((unsigned int)pSCREEN_PRO->InDataBuf + l);
	MemUse += l;
    
    //pSCREEN_PRO->InDataTempBuf - 解析缓冲区结构体指针
    l=sizeof(SCREEN_FRAME_STR);
	pSCREEN_PRO->InDataTempBuf = (PT_SCREEN_FRAME_STR)malloc(l);
	if(pSCREEN_PRO->InDataTempBuf == NULL)return 5;
	memset(pSCREEN_PRO->InDataTempBuf,0,l);
	MemUse += l;
    
    #ifdef PC_PRINTF
    printf("\r\nSCREEN mlloc size is %d.",MemUse);
    #endif
    return 0;
}

/**********************************************************
 * @fun     SCREENHandle
 * @brief   串口屏报文处理函数
 * @note    用于解析串口发送过来的报文
 * @param   none 
 * @return  无
 *********************************************************/
signed char SCREENHandle(void)
{
	unsigned int tmp1;
	unsigned int DataPiece1,DataPiece2;
	unsigned int count1;
    unsigned short adr;
    unsigned short dl;
    signed char ret;
    
    //判断长度
	if( (pSCREEN_PRO->RevLen < SCREEN_BACK_LEN_MIN) //如果一帧长度小于最短长度
      ||(pSCREEN_PRO->RevLen > SCREEN_BACK_LEN_MAX))
	{
        printf("\r\n");
		while(pSCREEN_PRO->RevLen!=0)
		{
            #ifdef PC_PRINTF
            printf("%x ",*pSCREEN_PRO->InDataReadPtr);
            #endif
			pSCREEN_PRO->InDataReadPtr++;
			if(pSCREEN_PRO->InDataReadPtr >= pSCREEN_PRO->InDataTailPtr)
				pSCREEN_PRO->InDataReadPtr = pSCREEN_PRO->InDataBuf;
			pSCREEN_PRO->RevLen --;
		}
		ret = (-1);   //长度不够
        #ifdef PC_PRINTF
        printf("\r\n接收串口屏命令出错%d,长度有误",ret);
        #endif
	}
	else //长度足够
	{
        //将接收缓冲区内的整帧内容拷贝到待处理缓存区内
		tmp1 = (unsigned int)pSCREEN_PRO->InDataReadPtr + pSCREEN_PRO->RevLen;
		if(tmp1 >= (unsigned int)pSCREEN_PRO->InDataTailPtr)
		{
			DataPiece1 = ((unsigned int)pSCREEN_PRO->InDataTailPtr) - ((unsigned int)pSCREEN_PRO->InDataReadPtr);
			memcpy((unsigned char *)pSCREEN_PRO->InDataTempBuf, pSCREEN_PRO->InDataReadPtr, DataPiece1);
			DataPiece2 = pSCREEN_PRO->RevLen - DataPiece1;
			memcpy(((unsigned char *)((unsigned int)pSCREEN_PRO->InDataTempBuf + DataPiece1)), pSCREEN_PRO->InDataBuf, DataPiece2);
			pSCREEN_PRO->InDataReadPtr = (unsigned char *)((unsigned int)pSCREEN_PRO->InDataBuf + DataPiece2);
		}
		else
		{
			memcpy(pSCREEN_PRO->InDataTempBuf, pSCREEN_PRO->InDataReadPtr, pSCREEN_PRO->RevLen);
			pSCREEN_PRO->InDataReadPtr += pSCREEN_PRO->RevLen;
		}
        
        pSCREEN_PRO->RevLen = 0;
        
		//判断首部
		if(pSCREEN_PRO->InDataTempBuf->Head == 0xA55A) //帧首部相同
		{
            if(pSCREEN_PRO->InDataTempBuf->Cmd == CMD_WRITE_VAR) //写数据到变量空间的反馈
            {
                if(  (pSCREEN_PRO->InDataTempBuf->Data[0] == 0X4F)
                   &&(pSCREEN_PRO->InDataTempBuf->Data[1] == 0X4B))
                {
                    ret = 4;   //写数据到变量空间的反馈数据正确
                }
                else
                {
                    ret = 5;   //写数据到变量空间的反馈数据错误
                    #ifdef PC_PRINTF
                    printf("\r\n接收串口屏帧出错%d,数据出错",ret);
                    #endif
                }
            }
            else if(pSCREEN_PRO->InDataTempBuf->Cmd == CMD_READ_VAR) //从变量空间读数据的反馈
            {
                if(KeyState) //按键使能，获取按键值和数据
                {
                    //变量地址
                    adr=pSCREEN_PRO->InDataTempBuf->Data[0];
                    adr<<=8;
                    adr+=pSCREEN_PRO->InDataTempBuf->Data[1];
                    ScreenRevAdr = adr;
                    
                    //数据长度
                    dl=pSCREEN_PRO->InDataTempBuf->Data[2]*2;
                    
                    //数据长度和数据内容
                    ScreenRevLen = 0;
                    for(count1=0;count1<dl;count1++)
                    {
                        if(pSCREEN_PRO->InDataTempBuf->Data[3+count1]==0xFF)
                        {
                            break;
                        }
                        ScreenRevLen ++;
                        ScreenRevBuf[count1] = pSCREEN_PRO->InDataTempBuf->Data[3+count1];
                    }
                    
                    ret = 6;   //接收变量数据并处理
                    #ifdef PC_PRINTF
                    printf("\r\n接收串口屏变量地址%x,数据长度%d",ScreenRevAdr,ScreenRevLen);
                    #endif
                }
                else //按键禁用
                {
                    ret = 7;
                    #ifdef PC_PRINTF
                    printf("\r\n禁止接收串口屏数据");
                    #endif
                }
            }
            else //命令出错
            {
                ret = (-3);
                #ifdef PC_PRINTF
                printf("\r\n接收串口屏帧出错%d,命令出错",ret);
                #endif
            }
		}
		else //帧首部出错
		{
			ret = (-2); 
            #ifdef PC_PRINTF
            printf("\r\n接收串口屏帧出错%d,首部出错",ret);
            #endif
		}
	}
    return ret;
}

/**********************************************************
 * @fun     SCREENReadVarCmd      
 * @brief   串口屏<读系统变量>指令
 * @note    
 * @param   无
 * @return  无
 *********************************************************/
unsigned char SCREENReadVarCmd(unsigned short adr, unsigned char len)
{
    unsigned char *pt;
    unsigned char i;
    
    //组帧发送
    pSCREEN_READ_SEND->Head = 0xA55A;
    pSCREEN_READ_SEND->Len = 4;  
    pSCREEN_READ_SEND->Cmd = 0x83;
    pSCREEN_READ_SEND->Adr = ((adr<<8)|(adr>>8));
    pSCREEN_READ_SEND->Rlen = len;
    pt = (unsigned char *)(unsigned int)pSCREEN_READ_SEND;
    for(i=0;i<7;i++)
    {
        UART_SendData(SCREEN_UART_PORT, *pt ++);
    }
    
    //等待反馈
    pSCREEN_PRO->CMDWaitDelayParm = 0; //计时定时器清零
    pSCREEN_PRO->WaitTimeActiveFlag = 1; //开始计时
    while(pSCREEN_PRO->CMDWaitDelayParm < 10) //10ms
    {
        if(pSCREEN_PRO->CMDHandleFlag) //在给定时间内得到反馈
        {
            pSCREEN_PRO->WaitTimeActiveFlag=0; //停止计时
            break;
        }
    }
  
    //超时检测
    if(pSCREEN_PRO->CMDWaitDelayParm >= 10) //等待超过10ms
    {
        #ifdef PC_PRINTF
        printf("\r\n串口屏读系统变量超时");
        #endif
        return 1;
    }
    else //在给定时间内反馈
    {
        if(pSCREEN_PRO->CMDHandleFlag)
        {
            pSCREEN_PRO->CMDHandleFlag=0;
            SCREENHandle();
        }
        return 0;
    }
}

/**********************************************************
 * @fun     SCREENWriteVarCmd      
 * @brief   串口屏<写系统变量>指令
 * @note    
 * @param   无
 * @return  无
 *********************************************************/
unsigned char SCREENWriteVarCmd(unsigned short adr, unsigned char *pdata, unsigned char len)
{
    unsigned char *ptv;
    unsigned char t;
    
    //组帧发送
    pSCREEN_WRITE_SEND->Head = 0xA55A;
    pSCREEN_WRITE_SEND->Len = 3+len;
    pSCREEN_WRITE_SEND->Cmd = 0x82;
    pSCREEN_WRITE_SEND->Adr = ((adr<<8)|(adr>>8));
    for(t=0;t<len;t++)
    {
        pSCREEN_WRITE_SEND->Data[t]=pdata[t];
    }
    ptv = (unsigned char *)(unsigned int)pSCREEN_WRITE_SEND;
    for(t=0;t<(6+len);t++)
    {
        UART_SendData(SCREEN_UART_PORT, *ptv ++);
    }
    
    //等待反馈
    pSCREEN_PRO->CMDWaitDelayParm = 0; //计时定时器清零
    pSCREEN_PRO->WaitTimeActiveFlag = 1; //开始计时
    while(pSCREEN_PRO->CMDWaitDelayParm < 10) //10ms
    {
        if(pSCREEN_PRO->CMDHandleFlag) //在给定时间内得到反馈
        {
            pSCREEN_PRO->WaitTimeActiveFlag=0; //停止计时
            break;
        }
    }
  
    //超时检测
    if(pSCREEN_PRO->CMDWaitDelayParm >= 10) //等待超过10ms
    {
        #ifdef PC_PRINTF
        printf("\r\n串口屏写系统变量超时");
        #endif
        return 1;
    }
    else //在给定时间内反馈
    {
        if(pSCREEN_PRO->CMDHandleFlag)
        {
            pSCREEN_PRO->CMDHandleFlag=0;
            SCREENHandle();
        }
        return 0;
    }
}

/**********************************************************
 * @fun     SCREEN_CLEAR_INF      
 * @brief   清空状态提示栏的内容
 * @note    
 * @param   无
 * @return  无
 *********************************************************/
void SCREEN_CLEAR_INF(void) 
{
    SCREENWriteVarCmd(  ADR_RETURN_STATUS_BAR, 
                        (unsigned char *)"                                ", 
                        strlen("                                "));
}

/******************************************************
    整形数据转字符串函数
    char *itoa(int value, char *string, int radix)
    radix=10 标示是10进制	非十进制，转换结果为0;						   			  
**********************************************************/
char *itoa(int value, char *string, int radix)
{
    int i, d;
    int flag = 0;
    char *ptr = string;
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
    if (value < 0)
    {
        *ptr++ = '-';
        value *= -1;
    }
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
    *ptr = 0;
    return string;
} 

/**********************************************************
 * @fun     SCREENprintf      
 * @brief   串口屏打印函数,模拟printf
 * @note    
 * @param   Adr:    串口屏地址
            Data:   发送字符串
			...:    发送不定参数
 * @return  无
 *********************************************************/
void SCREENprintf(uint32_t Adr,char* Data,...)
{
	const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap,Data);
    ScreenSendIndex=0;
	while(*Data!='\0')//判断是否到达字符串结束符
    {
		if(*Data==0x0d)//换行符
        {
            Data++;
            ScreenSendIndex=0;
        }
        else if(*Data==0x0a)//换行符
        {   
            Data++;
            ScreenSendIndex=0;
        }
		else if(*Data=='%')
        {
			switch (*++Data)
            {
				case 's':   //字符串
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) 
                    {
                    	ScreenSendBuf[ScreenSendIndex ++]=*s;
                	}
					Data++;
                	break;
                    
               case 'c':    //字符
                    d = va_arg(ap, int);
                    ScreenSendBuf[ScreenSendIndex ++]=d;
					Data++;
                	break;
                    
            	case 'd':   //十进制
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) 
                    {
                    	ScreenSendBuf[ScreenSendIndex ++]=*s;
                	}
					Data++;
                	break;
                    
				default:    //其他情况
                    ScreenSendBuf[ScreenSendIndex ++]='%';
                    ScreenSendBuf[ScreenSendIndex ++]=*Data++;
				    break;
			}		
		}
		else
        {
            ScreenSendBuf[ScreenSendIndex ++]=*Data++;
        }
	}
    
    switch(Adr)
    {
        case ADR_RETURN_STATUS_BAR://主界面的状态返回栏
            for(;ScreenSendIndex<TEXT_DIS_TIP_LEN;ScreenSendIndex++)
                ScreenSendBuf[ScreenSendIndex]=0xFF;
            SCREENWriteVarCmd(Adr,ScreenSendBuf,TEXT_DIS_TIP_LEN);
            break;
        
        case ADR_RETURN_TORSIONAL_VALUE_BAR://扭力值界面的扭力值大小栏
            for(;ScreenSendIndex < TEXT_DIS_VALUE_LEN;ScreenSendIndex++)
                ScreenSendBuf[ScreenSendIndex]=0xFF;
            SCREENWriteVarCmd(Adr,ScreenSendBuf,TEXT_DIS_VALUE_LEN);
            break;
        
    }
}

/*************************************************************************
 * @fun     DispASCII     
 * @brief   显示ASCII
 * @note    在SCREENWriteVarCmd函数前调用,用来填满ScreenSendBuf发送缓冲
 * @param   align,0居左,1居中
 * @return  无
 *************************************************************************/
void DispASCII(uint8_t *ptv,uint8_t lenmax,uint8_t align)
{
    uint8_t t,len,len1;
    
    //求该ASCII数组长度
    for(t=0;t<lenmax;t++)
    {
        if(ptv[t]==0xFF)break;
    }
    len=t;
    
    if(align==0) //居左显示
    {
        for(t=0;t<len;t++)ScreenSendBuf[t]=*ptv ++;
        for(;t<lenmax;t++)ScreenSendBuf[t]=0xFF;
    }
    else //居中显示
    {
        len1=lenmax-len;
        if(len1%2)
        {
            if(len1==1)
                len1=0;
            else 
                len1=(len1-1)/2;
        }
        else 
        {
            len1=len1/2;
        }
        
        for(t=0;t<len1;t++)ScreenSendBuf[t]=0x20;
        for(;t<len1+len;t++)ScreenSendBuf[t]=*ptv ++;
        for(;t<lenmax;t++)ScreenSendBuf[t]=0xFF;
    }
}

/**********************************************************
 * @fun     SCREENPageSwitch      
 * @brief   串口屏发送页面切换指令
 * @note    
 * @param   pageid 页面ID
 * @return  无
 *********************************************************/
void SCREENPageSwitch(unsigned short pageid)
{
    ScreenSendBuf[0]=0x5a;
    ScreenSendBuf[1]=1;
    ScreenSendBuf[2]=(unsigned char)(pageid>>8);
    ScreenSendBuf[3]=(unsigned char)pageid;
    SCREENWriteVarCmd(ADR_PIC_SET,ScreenSendBuf,4);
}

/**************************************************************************
 * @fun     GUI_UpdateKeyStatus
 * @brief   none
 * @note    none
 * @param   param:  0断开
                    1关闭
 * @return  none
 *************************************************************************/
//void GUI_UpdateKeyStatus(unsigned char param)
//{
//    switch(param)
//    {
//        case 1: 
//            ScreenSendBuf[0]=0;ScreenSendBuf[1]=1;
//            break;
//        
//        case 2: 
//            ScreenSendBuf[0]=0;ScreenSendBuf[1]=2;
//            break;
//    }
//    SCREENWriteVarCmd(ADR_ICON_DIS_KEY,ScreenSendBuf,2);
//}
