#include "uart4.h"
#include "sys.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "delay.h"
#include "includes.h"					//ucos 使用	
//LORA
vu16  Usart4_Rec_Cnt = 0;             //本帧数据长度	
vu16  Usart4_Tx_Cnt = 0;
vu16 Uart4_Rx_Sta = 0;
uint8_t USART4_RX_BUF[USART4_MAX_RECV_LEN]={0}; 				//接收缓冲,最大USART2_5_MAX_RECV_LEN个字节.
uint8_t USART4_TX_BUF[USART4_MAX_SEND_LEN]={0}; 			//发送缓冲,最大USART2_5_MAX_RECV_LEN字节

//重新使能UART2_RX的DMA功能
void UART4_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA2_Channel3, DISABLE ); //先停止DMA，暂停接收 
 	DMA_SetCurrDataCounter(DMA2_Channel3, USART4_MAX_RECV_LEN); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA2_Channel3, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}	
//重新使能UART2_TX的DMA功能
void UART4_TX_DMA_Enable(uint16_t  lenth)
{ 
	DMA_Cmd(DMA2_Channel5, DISABLE ); //先停止DMA
 	DMA_SetCurrDataCounter(DMA2_Channel5, lenth); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA2_Channel5, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}	
//UART2_RX的DMA初始化
void UART4_DMA_RX_Init(void)
{
	DMA_InitTypeDef DMA_UART4_RX;
	
	//相应的DMA配置
	DMA_DeInit(DMA2_Channel3);   //将DMA的通道5寄存器重设为缺省值  串口2对应的是DMA通道6
	DMA_UART4_RX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA外设ADC基地址
	DMA_UART4_RX.DMA_MemoryBaseAddr = (u32)USART4_RX_BUF;  //DMA内存基地址
	DMA_UART4_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_UART4_RX.DMA_BufferSize = USART4_MAX_RECV_LEN;  //DMA通道的DMA缓存的大小
	DMA_UART4_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART4_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART4_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART4_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART4_RX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART4_RX.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有中优先级 
	DMA_UART4_RX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA2_Channel3, &DMA_UART4_RX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
		
	DMA_Cmd(DMA2_Channel3, ENABLE);  //正式驱动DMA传输
}
//UART1_TX的DMA初始化
void UART4_DMA_TX_Init(void)
{
	DMA_InitTypeDef DMA_UART4_TX;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//使能DMA传输
	//相应的DMA配置
	DMA_DeInit(DMA2_Channel5);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_UART4_TX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA外设ADC基地址
	DMA_UART4_TX.DMA_MemoryBaseAddr = (u32)USART4_TX_BUF;  //DMA内存基地址
	DMA_UART4_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_UART4_TX.DMA_BufferSize = 0;  //DMA通道的DMA缓存的大小
	DMA_UART4_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART4_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART4_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART4_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART4_TX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART4_TX.DMA_Priority = DMA_Priority_VeryHigh; //DMA通道 x拥有中优先级 
	DMA_UART4_TX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA2_Channel5, &DMA_UART4_TX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
		
	DMA_Cmd(DMA2_Channel5, ENABLE);  //正式驱动DMA传输
	
}

void uart4_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	USART_DeInit(UART4);  //复位串口4

	//UART4_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC10

	//UART4_RX	  PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PB11

	//Uart4 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART4, &USART_InitStructure); //初始化串口
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//中断
	
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); //开启空闲中断
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); //使能串口3的DMA接收
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); //使能串口3的DMA发送
	
	USART_Cmd(UART4, ENABLE);                    //使能串口 
	UART4_DMA_TX_Init(); //UART3_TX的EDMA功能初始化						//两个都要
	UART4_DMA_RX_Init(); //UART3_RX的EDMA功能初始化						//两个都要
}
//串口4,printf 函数
//确保一次发送数据不超过USART2_5_MAX_RECV_LEN字节
void u4_printf(uint8_t DMA,char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART4_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART4_TX_BUF);		//此次发送数据的长度
	if(DMA==0)
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(UART4,USART4_TX_BUF[j]); 
	} 
	else
	UART4_TX_DMA_Enable(i); //使能DMA串口发送
}
void UART4_IRQHandler(void)
{     
//	static OS_ERR err; 
	//串口3空闲中断

	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  
	{	
		/* 1.清除标志 */
		USART_ClearITPendingBit(UART4, USART_IT_IDLE); //清除中断标志
		
		/* 2.读取DMA */
		USART_ReceiveData(UART4); //读取数据
		Usart4_Rec_Cnt = USART4_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA2_Channel3); //接收个数等于接收缓冲区总大小减已经接收的个数
		
		/* 3.搬移数据进行其他处理 */
//		memcpy(USART2_TX_BUF, USART2_RX_BUF, Usart2_Rec_Cnt); //将接收转移通过串口1的DMA方式发送出去测试
//		u2_printf("%s",USART2_TX_BUF);
//		UART3_TX_DMA_Enable(Usart2_5_Rec_Cnt); //开启一次DMA发送，实现转发
		/* 4.开启新的一次DMA接收 */
		Uart4_Rx_Sta|=0x8000;
		UART4_RX_DMA_Enable(); //重新使能DMA，等待下一次的接收
   }  	
}


