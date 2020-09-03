#include "delay.h"
#include "stdarg.h"	 
#include "usart2.h"
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "includes.h"

//重新使能UART3_TX的DMA功能
void UART3_TX_DMA_Enable(uint16_t  lenth)
{ 
	DMA_Cmd(DMA1_Channel2, DISABLE ); //先停止DMA
 	DMA_SetCurrDataCounter(DMA1_Channel2, lenth); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA1_Channel2, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}	
//重新使能UART3_RX的DMA功能
void UART3_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel3, DISABLE ); //先停止DMA，暂停接收 
 	DMA_SetCurrDataCounter(DMA1_Channel3, USART2_5_MAX_RECV_LEN); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA1_Channel3, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}	



void USART3_IRQHandler(void)
{   
//	static OS_ERR err;   
	//串口3空闲中断
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  
	{	
		/* 1.清除标志 */
		USART_ClearITPendingBit(USART3, USART_IT_IDLE); //清除中断标志
		
		/* 2.读取DMA */
		USART_ReceiveData(USART3); //读取数据
		Usart2_5_Rec_Cnt = USART2_5_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel3); //接收个数等于接收缓冲区总大小减已经接收的个数
		/* 3.搬移数据进行其他处理 */
//		memcpy(USART2_5_TX_BUF, USART2_5_RX_BUF, Usart2_5_Rec_Cnt); //将接收转移通过串口1的DMA方式发送出去测试
//		UART3_TX_DMA_Enable(Usart2_5_Rec_Cnt); //开启一次DMA发送，实现转发
		USART2_5_RX_STA|=0x8000;
		/* 4.开启新的一次DMA接收 */
		UART3_RX_DMA_Enable(); //重新使能DMA，等待下一次的接收
//		if(USART2_5_RX_BUF[0]=='#' && USART2_5_RX_BUF[1]=='#')
//			OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量
   }  	
	
	
}   

   


//UART1_RX的DMA初始化
void UART3_DMA_RX_Init(void)
{
	DMA_InitTypeDef DMA_UART3_RX;
	
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel3);   //将DMA的通道5寄存器重设为缺省值  串口3对应的是DMA通道5
	DMA_UART3_RX.DMA_PeripheralBaseAddr = (u32)&USART3->DR;  //DMA外设ADC基地址
	DMA_UART3_RX.DMA_MemoryBaseAddr = (u32)USART2_5_RX_BUF;  //DMA内存基地址
	DMA_UART3_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_UART3_RX.DMA_BufferSize = USART2_5_MAX_RECV_LEN;  //DMA通道的DMA缓存的大小
	DMA_UART3_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART3_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART3_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART3_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART3_RX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART3_RX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_UART3_RX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel3, &DMA_UART3_RX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
		
	DMA_Cmd(DMA1_Channel3, ENABLE);  //正式驱动DMA传输
}
//UART1_TX的DMA初始化
void UART3_DMA_TX_Init(void)
{
	DMA_InitTypeDef DMA_UART3_TX;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel2);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_UART3_TX.DMA_PeripheralBaseAddr = (u32)&USART3->DR;  //DMA外设ADC基地址
	DMA_UART3_TX.DMA_MemoryBaseAddr = (u32)USART2_5_TX_BUF;  //DMA内存基地址
	DMA_UART3_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_UART3_TX.DMA_BufferSize = 0;  //DMA通道的DMA缓存的大小
	DMA_UART3_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART3_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART3_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART3_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART3_TX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART3_TX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_UART3_TX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel2, &DMA_UART3_TX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
		
	DMA_Cmd(DMA1_Channel2, ENABLE);  //正式驱动DMA传输
	
}


//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void usart3_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

 	USART_DeInit(USART3);  //复位串口3
		 //USART3_TX   PB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
   
    //USART3_RX	  PB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART3, &USART_InitStructure); //初始化串口	3
  
	//使能接收中断
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); //开启空闲中断
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); //使能串口3的DMA接收
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); //使能串口3的DMA发送
	USART_Cmd(USART3, ENABLE);                    //使能串口 
	UART3_DMA_TX_Init(); //UART3_TX的EDMA功能初始化
	UART3_DMA_RX_Init(); //UART3_RX的EDMA功能初始化
//	TIM7_Int_Init(99,7199);		//10ms中断
	USART2_5_RX_STA=0;		//清零
//	TIM_Cmd(TIM7,DISABLE);			//关闭定时器7

}
//串口3,printf 函数
//确保一次发送数据不超过USART2_5_MAX_RECV_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_5_TX_BUF);		//此次发送数据的长度
//	for(j=0;j<i;j++)							//循环发送数据
//	{
//	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//		USART_SendData(USART3,USART2_5_TX_BUF[j]); 
//	} 
	UART3_TX_DMA_Enable(i); //使能DMA串口发送
}


















