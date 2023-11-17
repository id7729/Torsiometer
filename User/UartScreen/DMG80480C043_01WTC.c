/**********************************************************************
include 
***********************************************************************/
#include "DMG80480C043_01WTC.h"
#include "ac78xx_uart.h"                // AutoChips::Device:ATC Drivers:UART
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO

/**********************************************************************
varible
***********************************************************************/
PT_SCREEN_WRITE_VAR_STR pSCREEN_WRITE_SEND;     //дϵͳ��������һָ֡��
PT_SCREEN_READ_VAR_STR  pSCREEN_READ_SEND;      //��ϵͳ��������һָ֡��
PT_SCREEN_PRO_STR       pSCREEN_PRO;            //����������ṹ��ָ�����
unsigned char KeyState=1;           //�������ϵİ�������״̬��1���գ�0����
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
 * @return  ��
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
		if(pSCREEN_PRO->RevLen < 1024)   //һ���Խ��ճ���С��4096
		{
			*pSCREEN_PRO->InDataWritePtr++ = UART6->UARTn_RBR_THR;//����Ӧ��ע�⡣ԭ�������ݼĴ�����λ��
			if(pSCREEN_PRO->InDataWritePtr >= pSCREEN_PRO->InDataTailPtr)
				pSCREEN_PRO->InDataWritePtr = pSCREEN_PRO->InDataBuf;
			pSCREEN_PRO->RevLen++;
			
			if(pSCREEN_PRO->TimActiveFlag == 0)
				pSCREEN_PRO->TimActiveFlag = 1;
			pSCREEN_PRO->CMDRevDelayParam = 0;
		}
		else
		{
			revtemp = UART6->UARTn_RBR_THR;//����Ӧ��ע�⡣ԭ�������ݼĴ�����λ��
		} 
	}
	return 0;
}
/**********************************************************
 * @brief   
 * @note    
 * @param   
 * @param   
 * @return  ��
 *********************************************************/
void SCREENInit(void)
{
//    uart_init(SCREEN_UART_PORT,SCREEN_UART_BAUD); 
	UART_SettingType uartSetting = {0};
	GPIO_SetFunc(GPIO_PA13, 3);//PA13���ù���3 --- UART6_Tx
	GPIO_SetFunc(GPIO_PA14, 3);//PA14���ù���3 --- UART6_Rx

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
 * @param   ��
 * @return  ��
 *********************************************************/
unsigned char SCREENVarMalloc(void)
{
	unsigned int MemUse;
    unsigned int l;
    
    MemUse = 0;
   
	//pSCREEN_WRITE_SEND - ����дָ��ָ��
    l=sizeof(SCREEN_WRITE_VAR_STR);
	pSCREEN_WRITE_SEND = (PT_SCREEN_WRITE_VAR_STR)malloc(l);
	if(pSCREEN_WRITE_SEND == NULL)return 1;
	memset(pSCREEN_WRITE_SEND,0,l);
	MemUse += l;
    
    //pSCREEN_READ_SEND - ���Ͷ�ָ��ָ��
    l=sizeof(SCREEN_READ_VAR_STR);
	pSCREEN_READ_SEND = (PT_SCREEN_READ_VAR_STR)malloc(l);
	if(pSCREEN_READ_SEND == NULL)return 2;
	memset(pSCREEN_READ_SEND,0,l);
	MemUse += l;
    
    //pSCREEN_PRO - ����������ṹ��ָ��
    l=sizeof(SCREEN_PRO_STR);
	pSCREEN_PRO = (PT_SCREEN_PRO_STR)malloc(l);
	if(pSCREEN_PRO == NULL)return 3;
	memset(pSCREEN_PRO,0,l);
	MemUse += l;
    
    //pSCREEN_PRO->InDataBuf - ���ջ�����ָ��
    l=1024;
	pSCREEN_PRO->InDataBuf = (unsigned char*)malloc(l);
	if(pSCREEN_PRO->InDataBuf == NULL)return 4;
	memset(pSCREEN_PRO->InDataBuf,0,l);
	pSCREEN_PRO->InDataReadPtr = pSCREEN_PRO->InDataBuf;
	pSCREEN_PRO->InDataWritePtr = pSCREEN_PRO->InDataBuf;
	pSCREEN_PRO->InDataTailPtr = (unsigned char*)((unsigned int)pSCREEN_PRO->InDataBuf + l);
	MemUse += l;
    
    //pSCREEN_PRO->InDataTempBuf - �����������ṹ��ָ��
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
 * @brief   ���������Ĵ�����
 * @note    ���ڽ������ڷ��͹����ı���
 * @param   none 
 * @return  ��
 *********************************************************/
signed char SCREENHandle(void)
{
	unsigned int tmp1;
	unsigned int DataPiece1,DataPiece2;
	unsigned int count1;
    unsigned short adr;
    unsigned short dl;
    signed char ret;
    
    //�жϳ���
	if( (pSCREEN_PRO->RevLen < SCREEN_BACK_LEN_MIN) //���һ֡����С����̳���
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
		ret = (-1);   //���Ȳ���
        #ifdef PC_PRINTF
        printf("\r\n���մ������������%d,��������",ret);
        #endif
	}
	else //�����㹻
	{
        //�����ջ������ڵ���֡���ݿ�����������������
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
        
		//�ж��ײ�
		if(pSCREEN_PRO->InDataTempBuf->Head == 0xA55A) //֡�ײ���ͬ
		{
            if(pSCREEN_PRO->InDataTempBuf->Cmd == CMD_WRITE_VAR) //д���ݵ������ռ�ķ���
            {
                if(  (pSCREEN_PRO->InDataTempBuf->Data[0] == 0X4F)
                   &&(pSCREEN_PRO->InDataTempBuf->Data[1] == 0X4B))
                {
                    ret = 4;   //д���ݵ������ռ�ķ���������ȷ
                }
                else
                {
                    ret = 5;   //д���ݵ������ռ�ķ������ݴ���
                    #ifdef PC_PRINTF
                    printf("\r\n���մ�����֡����%d,���ݳ���",ret);
                    #endif
                }
            }
            else if(pSCREEN_PRO->InDataTempBuf->Cmd == CMD_READ_VAR) //�ӱ����ռ�����ݵķ���
            {
                if(KeyState) //����ʹ�ܣ���ȡ����ֵ������
                {
                    //������ַ
                    adr=pSCREEN_PRO->InDataTempBuf->Data[0];
                    adr<<=8;
                    adr+=pSCREEN_PRO->InDataTempBuf->Data[1];
                    ScreenRevAdr = adr;
                    
                    //���ݳ���
                    dl=pSCREEN_PRO->InDataTempBuf->Data[2]*2;
                    
                    //���ݳ��Ⱥ���������
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
                    
                    ret = 6;   //���ձ������ݲ�����
                    #ifdef PC_PRINTF
                    printf("\r\n���մ�����������ַ%x,���ݳ���%d",ScreenRevAdr,ScreenRevLen);
                    #endif
                }
                else //��������
                {
                    ret = 7;
                    #ifdef PC_PRINTF
                    printf("\r\n��ֹ���մ���������");
                    #endif
                }
            }
            else //�������
            {
                ret = (-3);
                #ifdef PC_PRINTF
                printf("\r\n���մ�����֡����%d,�������",ret);
                #endif
            }
		}
		else //֡�ײ�����
		{
			ret = (-2); 
            #ifdef PC_PRINTF
            printf("\r\n���մ�����֡����%d,�ײ�����",ret);
            #endif
		}
	}
    return ret;
}

/**********************************************************
 * @fun     SCREENReadVarCmd      
 * @brief   ������<��ϵͳ����>ָ��
 * @note    
 * @param   ��
 * @return  ��
 *********************************************************/
unsigned char SCREENReadVarCmd(unsigned short adr, unsigned char len)
{
    unsigned char *pt;
    unsigned char i;
    
    //��֡����
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
    
    //�ȴ�����
    pSCREEN_PRO->CMDWaitDelayParm = 0; //��ʱ��ʱ������
    pSCREEN_PRO->WaitTimeActiveFlag = 1; //��ʼ��ʱ
    while(pSCREEN_PRO->CMDWaitDelayParm < 10) //10ms
    {
        if(pSCREEN_PRO->CMDHandleFlag) //�ڸ���ʱ���ڵõ�����
        {
            pSCREEN_PRO->WaitTimeActiveFlag=0; //ֹͣ��ʱ
            break;
        }
    }
  
    //��ʱ���
    if(pSCREEN_PRO->CMDWaitDelayParm >= 10) //�ȴ�����10ms
    {
        #ifdef PC_PRINTF
        printf("\r\n��������ϵͳ������ʱ");
        #endif
        return 1;
    }
    else //�ڸ���ʱ���ڷ���
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
 * @brief   ������<дϵͳ����>ָ��
 * @note    
 * @param   ��
 * @return  ��
 *********************************************************/
unsigned char SCREENWriteVarCmd(unsigned short adr, unsigned char *pdata, unsigned char len)
{
    unsigned char *ptv;
    unsigned char t;
    
    //��֡����
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
    
    //�ȴ�����
    pSCREEN_PRO->CMDWaitDelayParm = 0; //��ʱ��ʱ������
    pSCREEN_PRO->WaitTimeActiveFlag = 1; //��ʼ��ʱ
    while(pSCREEN_PRO->CMDWaitDelayParm < 10) //10ms
    {
        if(pSCREEN_PRO->CMDHandleFlag) //�ڸ���ʱ���ڵõ�����
        {
            pSCREEN_PRO->WaitTimeActiveFlag=0; //ֹͣ��ʱ
            break;
        }
    }
  
    //��ʱ���
    if(pSCREEN_PRO->CMDWaitDelayParm >= 10) //�ȴ�����10ms
    {
        #ifdef PC_PRINTF
        printf("\r\n������дϵͳ������ʱ");
        #endif
        return 1;
    }
    else //�ڸ���ʱ���ڷ���
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
 * @brief   ���״̬��ʾ��������
 * @note    
 * @param   ��
 * @return  ��
 *********************************************************/
void SCREEN_CLEAR_INF(void) 
{
    SCREENWriteVarCmd(  ADR_RETURN_STATUS_BAR, 
                        (unsigned char *)"                                ", 
                        strlen("                                "));
}

/******************************************************
    ��������ת�ַ�������
    char *itoa(int value, char *string, int radix)
    radix=10 ��ʾ��10����	��ʮ���ƣ�ת�����Ϊ0;						   			  
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
 * @brief   ��������ӡ����,ģ��printf
 * @note    
 * @param   Adr:    ��������ַ
            Data:   �����ַ���
			...:    ���Ͳ�������
 * @return  ��
 *********************************************************/
void SCREENprintf(uint32_t Adr,char* Data,...)
{
	const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap,Data);
    ScreenSendIndex=0;
	while(*Data!='\0')//�ж��Ƿ񵽴��ַ���������
    {
		if(*Data==0x0d)//���з�
        {
            Data++;
            ScreenSendIndex=0;
        }
        else if(*Data==0x0a)//���з�
        {   
            Data++;
            ScreenSendIndex=0;
        }
		else if(*Data=='%')
        {
			switch (*++Data)
            {
				case 's':   //�ַ���
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) 
                    {
                    	ScreenSendBuf[ScreenSendIndex ++]=*s;
                	}
					Data++;
                	break;
                    
               case 'c':    //�ַ�
                    d = va_arg(ap, int);
                    ScreenSendBuf[ScreenSendIndex ++]=d;
					Data++;
                	break;
                    
            	case 'd':   //ʮ����
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) 
                    {
                    	ScreenSendBuf[ScreenSendIndex ++]=*s;
                	}
					Data++;
                	break;
                    
				default:    //�������
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
        case ADR_RETURN_STATUS_BAR://�������״̬������
            for(;ScreenSendIndex<TEXT_DIS_TIP_LEN;ScreenSendIndex++)
                ScreenSendBuf[ScreenSendIndex]=0xFF;
            SCREENWriteVarCmd(Adr,ScreenSendBuf,TEXT_DIS_TIP_LEN);
            break;
        
        case ADR_RETURN_TORSIONAL_VALUE_BAR://Ť��ֵ�����Ť��ֵ��С��
            for(;ScreenSendIndex < TEXT_DIS_VALUE_LEN;ScreenSendIndex++)
                ScreenSendBuf[ScreenSendIndex]=0xFF;
            SCREENWriteVarCmd(Adr,ScreenSendBuf,TEXT_DIS_VALUE_LEN);
            break;
        
    }
}

/*************************************************************************
 * @fun     DispASCII     
 * @brief   ��ʾASCII
 * @note    ��SCREENWriteVarCmd����ǰ����,��������ScreenSendBuf���ͻ���
 * @param   align,0����,1����
 * @return  ��
 *************************************************************************/
void DispASCII(uint8_t *ptv,uint8_t lenmax,uint8_t align)
{
    uint8_t t,len,len1;
    
    //���ASCII���鳤��
    for(t=0;t<lenmax;t++)
    {
        if(ptv[t]==0xFF)break;
    }
    len=t;
    
    if(align==0) //������ʾ
    {
        for(t=0;t<len;t++)ScreenSendBuf[t]=*ptv ++;
        for(;t<lenmax;t++)ScreenSendBuf[t]=0xFF;
    }
    else //������ʾ
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
 * @brief   ����������ҳ���л�ָ��
 * @note    
 * @param   pageid ҳ��ID
 * @return  ��
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
 * @param   param:  0�Ͽ�
                    1�ر�
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
