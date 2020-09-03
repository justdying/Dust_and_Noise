#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "includes.h"
//串口接收缓存区 	
vu16  Usart2_5_Rec_Cnt=0;             //本帧数据长度	
vu16  Usart2_5_Txd_Cnt=0;             //本帧数据长度	
u8 USART2_5_RX_BUF[USART2_5_MAX_RECV_LEN]={0}; 				//接收缓冲,最大USART2_5_MAX_RECV_LEN个字节.
u8 USART2_5_TX_BUF[USART2_5_MAX_TX_LEN]={0}; 			//发送缓冲,最大USART2_5_MAX_RECV_LEN字节
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目

vu16 USART2_5_RX_STA=0;   	

//重新使能UART2_TX的DMA功能
void UART2_TX_DMA_Enable(uint16_t  lenth)
{ 
	DMA_Cmd(DMA1_Channel7, DISABLE ); //先停止DMA
 	DMA_SetCurrDataCounter(DMA1_Channel7, lenth); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA1_Channel7, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}	
//重新使能UART2_RX的DMA功能
void UART2_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel6, DISABLE ); //先停止DMA，暂停接收 
 	DMA_SetCurrDataCounter(DMA1_Channel6, USART2_5_MAX_RECV_LEN); //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA1_Channel6, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}	

void USART2_IRQHandler(void)
{     
//	static OS_ERR err; 
	//串口3空闲中断
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
	{	
		/* 1.清除标志 */
		USART_ClearITPendingBit(USART2, USART_IT_IDLE); //清除中断标志
		
		/* 2.读取DMA */
		USART_ReceiveData(USART2); //读取数据
		Usart2_5_Rec_Cnt = USART2_5_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel6); //接收个数等于接收缓冲区总大小减已经接收的个数
		
		/* 3.搬移数据进行其他处理 */
//		memcpy(USART2_5_TX_BUF, USART2_5_RX_BUF, Usart2_5_Rec_Cnt); //将接收转移通过串口1的DMA方式发送出去测试
//		UART3_TX_DMA_Enable(Usart2_5_Rec_Cnt); //开启一次DMA发送，实现转发
		USART2_5_RX_STA|=0x8000;
		/* 4.开启新的一次DMA接收 */
		UART2_RX_DMA_Enable(); //重新使能DMA，等待下一次的接收
//		if(USART2_5_RX_BUF[0]=='#' && USART2_5_RX_BUF[1]=='#')
//			OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量
   }  	
	
}

//UART2_RX的DMA初始化
void UART2_DMA_RX_Init(void)
{
	DMA_InitTypeDef DMA_UART2_RX;
	
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel6);   //将DMA的通道5寄存器重设为缺省值  串口2对应的是DMA通道6
	DMA_UART2_RX.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  //DMA外设ADC基地址
	DMA_UART2_RX.DMA_MemoryBaseAddr = (u32)USART2_5_RX_BUF;  //DMA内存基地址
	DMA_UART2_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设读取发送到内存
	DMA_UART2_RX.DMA_BufferSize = USART2_5_MAX_RECV_LEN;  //DMA通道的DMA缓存的大小
	DMA_UART2_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART2_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART2_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART2_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART2_RX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART2_RX.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有中优先级 
	DMA_UART2_RX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel6, &DMA_UART2_RX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
		
	DMA_Cmd(DMA1_Channel6, ENABLE);  //正式驱动DMA传输
}
//UART1_TX的DMA初始化
void UART2_DMA_TX_Init(void)
{
	DMA_InitTypeDef DMA_UART2_TX;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	//相应的DMA配置
	DMA_DeInit(DMA1_Channel7);   //将DMA的通道5寄存器重设为缺省值  串口1对应的是DMA通道5
	DMA_UART2_TX.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  //DMA外设ADC基地址
	DMA_UART2_TX.DMA_MemoryBaseAddr = (u32)USART2_5_TX_BUF;  //DMA内存基地址
	DMA_UART2_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_UART2_TX.DMA_BufferSize = 0;  //DMA通道的DMA缓存的大小
	DMA_UART2_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_UART2_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_UART2_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_UART2_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_UART2_TX.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_UART2_TX.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_UART2_TX.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel7, &DMA_UART2_TX);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
		
	DMA_Cmd(DMA1_Channel7, ENABLE);  //正式驱动DMA传输
	
}

void usart2_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
		/* 第1步：打开GPIO和USART部件的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  第3步已经做了，因此这步可以不做
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* 第4步： 配置串口硬件参数 */
	USART_InitStructure.USART_BaudRate = bound;	/* 波特率 */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		/* 仅选择接收模式 */
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	
		/* 使能串口2中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); //开启空闲中断
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); //使能串口3的DMA接收
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //使能串口3的DMA发送
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	UART2_DMA_TX_Init(); //UART3_TX的EDMA功能初始化
	UART2_DMA_RX_Init(); //UART3_RX的EDMA功能初始化
//	TIM7_Int_Init(99,7199);		//10ms中断
	USART2_5_RX_STA=0;		//清零
}

 //串口3,printf 函数
//确保一次发送数据不超过USART2_5_MAX_RECV_LEN字节
//DMA: 1,DMA方式发送 0，普通方式发送
void u2_printf(uint8_t DMA,char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_5_TX_BUF);		//此次发送数据的长度
	if(DMA==0)
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART2,USART2_5_TX_BUF[j]); 
	} 
	else
	UART2_TX_DMA_Enable(i); //使能DMA串口发送
}



















