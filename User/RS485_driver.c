#include "ac78xx.h"                     // Device header
#include "ac78xx_uart.h"                // AutoChips::Device:ATC Drivers:UART
#include "ac78xx_gpio.h"                // AutoChips::Device:ATC Drivers:GPIO
#include "LED.h"
#include "demo.h"
#include "RS485_driver_private.h"

uint8_t rx5Buf = 0;


int32_t UART_IRQHandler(uint8_t port, uint32_t LSR0, uint32_t LSR1);

/**************************************************************************************
* FunctionName   : RS485_Init()
* Description    : 485���ڳ�ʼ��������ʹ��UART5������ʼ����485_IO
* EntryParameter : None
* ReturnValue    : None
* attention      : *** �������main������ ***
**************************************************************************************/
void RS485_Init(void)
{
	//1.��ʼ��UART5
	UART_SettingType uartSetting = {0};

	
	GPIO_SetFunc(GPIO_PB13, 3);//PB13���ù���3 --- UART5_Tx
    GPIO_SetFunc(GPIO_PB14, 3);//PB14���ù���3 --- UART5_Rx	

    uartSetting.baudrate = 4800;
    uartSetting.dataBits = 8;
    uartSetting.stopBits = 1;
    uartSetting.parity = 0;
    uartSetting.fifoByte = 1;
    uartSetting.dmaEn = 0;
	uartSetting.rateStep = 0;
    UART_Init(UART5, &uartSetting);

    UART_SetEventCallback(UART5, UART_IRQHandler);  /* Set UART5 Interrupt Callback */
    UART_SetRxIntEn(UART5, 1);

    NVIC_ClearPendingIRQ(UART5_IRQn);//uartIndex
    NVIC_EnableIRQ(UART5_IRQn);		
	
	//2.��ʼ��485_IO	---output
	GPIO_SetDir(GPIO_PB12, 1);
	
//	//��ʼ�����Ե�
//	GPIO_SetDir(GPIO_PA12,1);
//	GPIO_ResetPinBits(GPIO_PA12);
}

/**************************************************************************************
* FunctionName   : UART5_SendString(uint8_t *pDataBuf, uint8_t dataLen)
* Description    : 485�ַ������ͺ���
* EntryParameter : *pDataBuf��Ҫ���͵�����ָ�룬�������д洢Ҫ���͵����ݣ�
					dataLen��Ҫ�������ݵĳ���
* ReturnValue    : None
* attention      : ***   ***
**************************************************************************************/
void UART5_SendString(uint8_t *pDataBuf, uint8_t dataLen)
{
	GPIO_SetPinBits(GPIO_PB12);
	for (uint8_t ii = 0; ii < dataLen; ii++)
	{
//		UART_SetTxIntEn(UART5, 1);//ע��,ÿ�η���֮ǰ����TX�ж�
		UART_SendData(UART5, pDataBuf[ii]);
		while(!UART5->UARTn_LSR0.TC);
	}
	GPIO_ResetPinBits(GPIO_PB12);
}


/**************************************************************************************
* FunctionName   : UART_IRQHandler()
* Description    : �����жϺ��������ڴ����ͻ�����жϣ����滹�����Ť��ֵ���ݴ�����
* EntryParameter : port�����ں�
				   LSR0����·״̬�Ĵ���0
				   LSR1����·״̬�Ĵ���1
* ReturnValue    : 0
* attention      : *** �����ֶ����ݲ��� ***
**************************************************************************************/
int32_t UART_IRQHandler(uint8_t port, uint32_t LSR0, uint32_t LSR1)
{		
		/*rx interrupt*/
		static uint8_t RxState = 0;//����һ������״̬����״̬��־
		static uint8_t pRxpacket = 0;//����һ�����ռ����ı���
		if (UART5->UARTn_IER.ERXNE == 1 && (LSR0 & LSR0_DR))
		{	
			
			rx5Buf = UART5->UARTn_RBR_THR;
			if(RxState == 0)//�ȴ�ģ���ַ״̬
			{
				if(rx5Buf == 0x01)//����Ϊ�棬�յ�ģ���ַ
				{
					RxState = 1;//��״̬����״̬λ��1��׼������ȴ�������״̬
				}				
			}
			else if(RxState == 1)//�ȴ�������״̬
			{
				if(rx5Buf == 0x03)//�յ�������0x03
				{
					RxState = 2;//��״̬����״̬λ��2��׼���������ݳ���λ					
				}				
			}
			else if(RxState == 2)//�������ݳ���λ״̬
			{
				pRxpacket = rx5Buf;
				RxState = 3;//��״̬����״̬λ��3,׼����������״̬				
			}
			else if(RxState == 3)
			{
				Torque_value.c[pRxpacket - 1] = rx5Buf;
				pRxpacket --;
				if(pRxpacket <= 0)
				{
					RxState = 4;//��״̬����״̬λ��4,׼����������У��λ
				}
			}
			else if(RxState == 4)
			{
				RxState = 0;//��״̬��־��0�����»ص��ȴ�ģ���ַ��״̬
				//���㣺1.��У��λֱ����ȥ����֪���᲻�����������
				//		2.��������֡�Ľ����ܴ��壬ֻ�ܽ����̶�������֡
				//		����2�����ø��Ӹ��ӵķ�֧��ʵ�ֶ���֡�Ľ�������֪���ɲ����У�
			}

		}

		/*tx interrupt*/
		if (UART5->UARTn_IER.ETXE == 1 && (LSR0 & LSR0_THRE))
		{
			
		}		
    return 0;
}

//void UART5_IRQHandler(void)
//{		
//		/*rx interrupt*/
//		static uint8_t RxState = 0;//����һ������״̬����״̬��־
//		static uint8_t pRxpacket = 0;//����һ�����ռ����ı���
//		if (UART5->UARTn_IER.ERXNE == 1 && (UART5->UARTn_LSR0.DR))
//		{	
//			
//			rx5Buf = UART5->UARTn_RBR_THR;
//							if(RxState == 0)//�ȴ�ģ���ַ״̬
//			{
//				if(rx5Buf == 0x01)//����Ϊ�棬�յ�ģ���ַ
//				{
//					RxState = 1;//��״̬����״̬λ��1��׼������ȴ�������״̬
//				}				
//			}
//			else if(RxState == 1)//�ȴ�������״̬
//			{
//				if(rx5Buf == 0x03)//�յ�������0x03
//				{
//					RxState = 2;//��״̬����״̬λ��2��׼���������ݳ���λ					
//				}				
//			}
//			else if(RxState == 2)//�������ݳ���λ״̬
//			{
//				pRxpacket = rx5Buf;
//				RxState = 3;//��״̬����״̬λ��3,׼����������״̬				
//			}
//			else if(RxState == 3)
//			{
//				Torque_value.c[pRxpacket - 1] = rx5Buf;
//				pRxpacket --;
//				if(pRxpacket <= 0)
//				{
//					RxState = 4;//��״̬����״̬λ��4,׼����������У��λ
//				}
//			}
//			else if(RxState == 4)
//			{
//				RxState = 0;//��״̬��־��0�����»ص��ȴ�ģ���ַ��״̬
//				//���㣺1.��У��λֱ����ȥ����֪���᲻�����������
//				//		2.��������֡�Ľ����ܴ��壬ֻ�ܽ����̶�������֡
//				//		����2�����ø��Ӹ��ӵķ�֧��ʵ�ֶ���֡�Ľ�������֪���ɲ����У�
//			}

//		}

//		/*tx interrupt*/
//		if (UART5->UARTn_IER.ETXE == 1 && (UART5->UARTn_LSR0.THRE))
//		{
//			
//		}		
//}

