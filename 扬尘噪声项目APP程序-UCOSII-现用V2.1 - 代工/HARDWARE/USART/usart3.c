#include "delay.h"
#include "stdarg.h"	 
#include "usart2.h"
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "includes.h"

//����ʹ��UART3_TX��DMA����
void UART3_TX_DMA_Enable(uint16_t  lenth)
{ 
	DMA_Cmd(DMA1_Channel2, DISABLE ); //��ֹͣDMA
 	DMA_SetCurrDataCounter(DMA1_Channel2, lenth); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA1_Channel2, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	
//����ʹ��UART3_RX��DMA����
void UART3_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel3, DISABLE ); //��ֹͣDMA����ͣ���� 
 	DMA_SetCurrDataCounter(DMA1_Channel3, USART2_5_MAX_RECV_LEN); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA1_Channel3, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	



void USART3_IRQHandler(void)
{   
//	static OS_ERR err;   
	//����3�����ж�
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  
	{	
		/* 1.�����־ */
		USART_ClearITPendingBit(USART3, USART_IT_IDLE); //����жϱ�־
		
		/* 2.��ȡDMA */
		USART_ReceiveData(USART3); //��ȡ����
		Usart2_5_Rec_Cnt = USART2_5_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel3); //���ո������ڽ��ջ������ܴ�С���Ѿ����յĸ���
		/* 3.�������ݽ����������� */
//		memcpy(USART2_5_TX_BUF, USART2_5_RX_BUF, Usart2_5_Rec_Cnt); //������ת��ͨ������1��DMA��ʽ���ͳ�ȥ����
//		UART3_TX_DMA_Enable(Usart2_5_Rec_Cnt); //����һ��DMA���ͣ�ʵ��ת��
		USART2_5_RX_STA|=0x8000;
		/* 4.�����µ�һ��DMA���� */
		UART3_RX_DMA_Enable(); //����ʹ��DMA���ȴ���һ�εĽ���
//		if(USART2_5_RX_BUF[0]=='#' && USART2_5_RX_BUF[1]=='#')
//			OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���
   }  	
	
	
}   

   


//UART1_RX��DMA��ʼ��
void UART3_DMA_RX_Init(void)
{
	DMA_InitTypeDef DMA_UART3_RX;
	
	//��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel3);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����3��Ӧ����DMAͨ��5
	DMA_UART3_RX.DMA_PeripheralBaseAddr = (u32)&USART3->DR;  //DMA����ADC����ַ
	DMA_UART3_RX.DMA_MemoryBaseAddr = (u32)USART2_5_RX_BUF;  //DMA�ڴ����ַ
	DMA_UART3_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_UART3_RX.DMA_BufferSize = USART2_5_MAX_RECV_LEN;  //DMAͨ����DMA����Ĵ�С
	DMA_UART3_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART3_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART3_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART3_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART3_RX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART3_RX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART3_RX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel3, &DMA_UART3_RX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
		
	DMA_Cmd(DMA1_Channel3, ENABLE);  //��ʽ����DMA����
}
//UART1_TX��DMA��ʼ��
void UART3_DMA_TX_Init(void)
{
	DMA_InitTypeDef DMA_UART3_TX;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	//��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel2);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����1��Ӧ����DMAͨ��5
	DMA_UART3_TX.DMA_PeripheralBaseAddr = (u32)&USART3->DR;  //DMA����ADC����ַ
	DMA_UART3_TX.DMA_MemoryBaseAddr = (u32)USART2_5_TX_BUF;  //DMA�ڴ����ַ
	DMA_UART3_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_UART3_TX.DMA_BufferSize = 0;  //DMAͨ����DMA����Ĵ�С
	DMA_UART3_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART3_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART3_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART3_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART3_TX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART3_TX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART3_TX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel2, &DMA_UART3_TX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
		
	DMA_Cmd(DMA1_Channel2, ENABLE);  //��ʽ����DMA����
	
}


//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart3_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //����3ʱ��ʹ��

 	USART_DeInit(USART3);  //��λ����3
		 //USART3_TX   PB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ��PB10
   
    //USART3_RX	  PB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //��ʼ��PB11
	
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART3, &USART_InitStructure); //��ʼ������	3
  
	//ʹ�ܽ����ж�
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); //���������ж�
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE); //ʹ�ܴ���3��DMA����
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���3��DMA����
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
	UART3_DMA_TX_Init(); //UART3_TX��EDMA���ܳ�ʼ��
	UART3_DMA_RX_Init(); //UART3_RX��EDMA���ܳ�ʼ��
//	TIM7_Int_Init(99,7199);		//10ms�ж�
	USART2_5_RX_STA=0;		//����
//	TIM_Cmd(TIM7,DISABLE);			//�رն�ʱ��7

}
//����3,printf ����
//ȷ��һ�η������ݲ�����USART2_5_MAX_RECV_LEN�ֽ�
void u3_printf(char* fmt,...)  
{  
	u16 i; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_5_TX_BUF);		//�˴η������ݵĳ���
//	for(j=0;j<i;j++)							//ѭ����������
//	{
//	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
//		USART_SendData(USART3,USART2_5_TX_BUF[j]); 
//	} 
	UART3_TX_DMA_Enable(i); //ʹ��DMA���ڷ���
}


















