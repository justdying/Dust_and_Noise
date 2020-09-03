#include "delay.h"
#include "usart2.h"
#include "usart5.h"
#include "stdarg.h"	 	 
#include "stdio.h"	
#include "string.h"	 
#include "includes.h"
#include "modbus_host.h"
USART_Typedef uart_5;
void uart5_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
/* 第1步： 开启GPIO和UART时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	USART_DeInit(UART5);  //复位串口5
	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = bound;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/*USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开*/
	USART_Cmd(UART5, ENABLE);		/* 使能串口 */
	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(UART5, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
}	
void U5_SendString(uint8_t *_buf, uint8_t _len)
{
	uint8_t j;
	for(j=0;j<_len;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(UART5,_buf[j]); 
	} 
}
void u5_printf(char* fmt,...)  
{  
	u16 i; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_5_TX_BUF);		//此次发送数据的长度
	uart_5.usTxCount = i;
//	for(j=0;j<i;j++)							//循环发送数据
//	{
//	  while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//		USART_SendData(UART5,USART2_5_TX_BUF[j]); 
//	} 
	USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
}
void uart5_RxTimeOut(void)
{
	USART2_5_RX_STA|=0x8000;
}
void UART5_IRQHandler(void)
{
	static uint32_t timeout = 10000;    //10ms
	uint8_t Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
			Res = USART_ReceiveData(UART5);//(UART5->DR);	//读取接收到的数据
			bsp_StartHardTimer(4, timeout, (void *)uart5_RxTimeOut);   //开启定时器
			USART2_5_RX_BUF[USART2_5_RX_STA++] = Res;	//记录接收到的值	
			Usart2_5_Rec_Cnt++;			
	}
		/* 处理发送缓冲区空中断 */
	if (USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{
		if (uart_5.usTxCount == 0)
		{
			/* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
			USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
			/* 使能数据发送完毕中断 */
			USART_ITConfig(UART5, USART_IT_TC, ENABLE);
		}
		else
		{
			/* 从发送FIFO取1个字节写入串口发送数据寄存器 */
			USART_SendData(UART5, USART2_5_TX_BUF[uart_5.usTxRead]);
			if (++uart_5.usTxRead >= USART2_5_MAX_TX_LEN)
			{
				uart_5.usTxRead = 0;
			}
			uart_5.usTxCount--;
		}
	}
	/* 数据bit位全部发送完毕的中断 */
	else if (USART_GetITStatus(UART5, USART_IT_TC) != RESET)
	{
		uart_5.usTxRead = 0;   //清空发送索引
		if (uart_5.usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
			USART_ITConfig(UART5, USART_IT_TC, DISABLE);		
		}
		else
		{
			/* 正常情况下，不会进入此分支 */
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
			USART_SendData(UART5, USART2_5_TX_BUF[uart_5.usTxRead]);
			if (++uart_5.usTxRead >= USART2_5_MAX_TX_LEN)
			{
				uart_5.usTxRead = 0;
			}
			uart_5.usTxCount--;
		}
	}
}







