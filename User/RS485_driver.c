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
* Description    : 485串口初始化函数，使用UART5，还初始化了485_IO
* EntryParameter : None
* ReturnValue    : None
* attention      : *** 将其放在main函数中 ***
**************************************************************************************/
void RS485_Init(void)
{
	//1.初始化UART5
	UART_SettingType uartSetting = {0};

	
	GPIO_SetFunc(GPIO_PB13, 3);//PB13复用功能3 --- UART5_Tx
    GPIO_SetFunc(GPIO_PB14, 3);//PB14复用功能3 --- UART5_Rx	

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
	
	//2.初始化485_IO	---output
	GPIO_SetDir(GPIO_PB12, 1);
	
//	//初始化调试灯
//	GPIO_SetDir(GPIO_PA12,1);
//	GPIO_ResetPinBits(GPIO_PA12);
}

/**************************************************************************************
* FunctionName   : UART5_SendString(uint8_t *pDataBuf, uint8_t dataLen)
* Description    : 485字符串发送函数
* EntryParameter : *pDataBuf：要发送的数组指针，在数组中存储要发送的数据；
					dataLen：要发送数据的长度
* ReturnValue    : None
* attention      : ***   ***
**************************************************************************************/
void UART5_SendString(uint8_t *pDataBuf, uint8_t dataLen)
{
	GPIO_SetPinBits(GPIO_PB12);
	for (uint8_t ii = 0; ii < dataLen; ii++)
	{
//		UART_SetTxIntEn(UART5, 1);//注意,每次发送之前开启TX中断
		UART_SendData(UART5, pDataBuf[ii]);
		while(!UART5->UARTn_LSR0.TC);
	}
	GPIO_ResetPinBits(GPIO_PB12);
}


/**************************************************************************************
* FunctionName   : UART_IRQHandler()
* Description    : 串口中断函数，用于处理发送或接收中断，里面还添加了扭力值数据处理环节
* EntryParameter : port：串口号
				   LSR0：线路状态寄存器0
				   LSR1：线路状态寄存器1
* ReturnValue    : 0
* attention      : *** 无需手动传递参数 ***
**************************************************************************************/
int32_t UART_IRQHandler(uint8_t port, uint32_t LSR0, uint32_t LSR1)
{		
		/*rx interrupt*/
		static uint8_t RxState = 0;//定义一个接收状态机的状态标志
		static uint8_t pRxpacket = 0;//定义一个接收计数的变量
		if (UART5->UARTn_IER.ERXNE == 1 && (LSR0 & LSR0_DR))
		{	
			
			rx5Buf = UART5->UARTn_RBR_THR;
			if(RxState == 0)//等待模块地址状态
			{
				if(rx5Buf == 0x01)//条件为真，收到模块地址
				{
					RxState = 1;//将状态机的状态位置1，准备进入等待功能码状态
				}				
			}
			else if(RxState == 1)//等待功能码状态
			{
				if(rx5Buf == 0x03)//收到功能码0x03
				{
					RxState = 2;//将状态机的状态位置2，准备接收数据长度位					
				}				
			}
			else if(RxState == 2)//接收数据长度位状态
			{
				pRxpacket = rx5Buf;
				RxState = 3;//将状态机的状态位置3,准备接收数据状态				
			}
			else if(RxState == 3)
			{
				Torque_value.c[pRxpacket - 1] = rx5Buf;
				pRxpacket --;
				if(pRxpacket <= 0)
				{
					RxState = 4;//将状态机的状态位置4,准备接收数据校验位
				}
			}
			else if(RxState == 4)
			{
				RxState = 0;//将状态标志置0，重新回到等待模块地址的状态
				//不足：1.将校验位直接舍去，不知道会不不会存在隐患
				//		2.对于数据帧的解析很呆板，只能解析固定的数据帧
				//		关于2是想用更加复杂的分支来实现多种帧的解析，不知道可不可行？
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
//		static uint8_t RxState = 0;//定义一个接收状态机的状态标志
//		static uint8_t pRxpacket = 0;//定义一个接收计数的变量
//		if (UART5->UARTn_IER.ERXNE == 1 && (UART5->UARTn_LSR0.DR))
//		{	
//			
//			rx5Buf = UART5->UARTn_RBR_THR;
//							if(RxState == 0)//等待模块地址状态
//			{
//				if(rx5Buf == 0x01)//条件为真，收到模块地址
//				{
//					RxState = 1;//将状态机的状态位置1，准备进入等待功能码状态
//				}				
//			}
//			else if(RxState == 1)//等待功能码状态
//			{
//				if(rx5Buf == 0x03)//收到功能码0x03
//				{
//					RxState = 2;//将状态机的状态位置2，准备接收数据长度位					
//				}				
//			}
//			else if(RxState == 2)//接收数据长度位状态
//			{
//				pRxpacket = rx5Buf;
//				RxState = 3;//将状态机的状态位置3,准备接收数据状态				
//			}
//			else if(RxState == 3)
//			{
//				Torque_value.c[pRxpacket - 1] = rx5Buf;
//				pRxpacket --;
//				if(pRxpacket <= 0)
//				{
//					RxState = 4;//将状态机的状态位置4,准备接收数据校验位
//				}
//			}
//			else if(RxState == 4)
//			{
//				RxState = 0;//将状态标志置0，重新回到等待模块地址的状态
//				//不足：1.将校验位直接舍去，不知道会不不会存在隐患
//				//		2.对于数据帧的解析很呆板，只能解析固定的数据帧
//				//		关于2是想用更加复杂的分支来实现多种帧的解析，不知道可不可行？
//			}

//		}

//		/*tx interrupt*/
//		if (UART5->UARTn_IER.ETXE == 1 && (UART5->UARTn_LSR0.THRE))
//		{
//			
//		}		
//}

