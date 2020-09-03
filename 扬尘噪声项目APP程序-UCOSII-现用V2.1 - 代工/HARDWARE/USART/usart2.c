#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "includes.h"
//���ڽ��ջ����� 	
vu16  Usart2_5_Rec_Cnt=0;             //��֡���ݳ���	
vu16  Usart2_5_Txd_Cnt=0;             //��֡���ݳ���	
u8 USART2_5_RX_BUF[USART2_5_MAX_RECV_LEN]={0}; 				//���ջ���,���USART2_5_MAX_RECV_LEN���ֽ�.
u8 USART2_5_TX_BUF[USART2_5_MAX_TX_LEN]={0}; 			//���ͻ���,���USART2_5_MAX_RECV_LEN�ֽ�
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ

vu16 USART2_5_RX_STA=0;   	

//����ʹ��UART2_TX��DMA����
void UART2_TX_DMA_Enable(uint16_t  lenth)
{ 
	DMA_Cmd(DMA1_Channel7, DISABLE ); //��ֹͣDMA
 	DMA_SetCurrDataCounter(DMA1_Channel7, lenth); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA1_Channel7, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	
//����ʹ��UART2_RX��DMA����
void UART2_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel6, DISABLE ); //��ֹͣDMA����ͣ���� 
 	DMA_SetCurrDataCounter(DMA1_Channel6, USART2_5_MAX_RECV_LEN); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA1_Channel6, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	

void USART2_IRQHandler(void)
{     
//	static OS_ERR err; 
	//����3�����ж�
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
	{	
		/* 1.�����־ */
		USART_ClearITPendingBit(USART2, USART_IT_IDLE); //����жϱ�־
		
		/* 2.��ȡDMA */
		USART_ReceiveData(USART2); //��ȡ����
		Usart2_5_Rec_Cnt = USART2_5_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA1_Channel6); //���ո������ڽ��ջ������ܴ�С���Ѿ����յĸ���
		
		/* 3.�������ݽ����������� */
//		memcpy(USART2_5_TX_BUF, USART2_5_RX_BUF, Usart2_5_Rec_Cnt); //������ת��ͨ������1��DMA��ʽ���ͳ�ȥ����
//		UART3_TX_DMA_Enable(Usart2_5_Rec_Cnt); //����һ��DMA���ͣ�ʵ��ת��
		USART2_5_RX_STA|=0x8000;
		/* 4.�����µ�һ��DMA���� */
		UART2_RX_DMA_Enable(); //����ʹ��DMA���ȴ���һ�εĽ���
//		if(USART2_5_RX_BUF[0]=='#' && USART2_5_RX_BUF[1]=='#')
//			OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���
   }  	
	
}

//UART2_RX��DMA��ʼ��
void UART2_DMA_RX_Init(void)
{
	DMA_InitTypeDef DMA_UART2_RX;
	
	//��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel6);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����2��Ӧ����DMAͨ��6
	DMA_UART2_RX.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  //DMA����ADC����ַ
	DMA_UART2_RX.DMA_MemoryBaseAddr = (u32)USART2_5_RX_BUF;  //DMA�ڴ����ַ
	DMA_UART2_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_UART2_RX.DMA_BufferSize = USART2_5_MAX_RECV_LEN;  //DMAͨ����DMA����Ĵ�С
	DMA_UART2_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART2_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART2_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART2_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART2_RX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART2_RX.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART2_RX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel6, &DMA_UART2_RX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
		
	DMA_Cmd(DMA1_Channel6, ENABLE);  //��ʽ����DMA����
}
//UART1_TX��DMA��ʼ��
void UART2_DMA_TX_Init(void)
{
	DMA_InitTypeDef DMA_UART2_TX;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
	//��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel7);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����1��Ӧ����DMAͨ��5
	DMA_UART2_TX.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  //DMA����ADC����ַ
	DMA_UART2_TX.DMA_MemoryBaseAddr = (u32)USART2_5_TX_BUF;  //DMA�ڴ����ַ
	DMA_UART2_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_UART2_TX.DMA_BufferSize = 0;  //DMAͨ����DMA����Ĵ�С
	DMA_UART2_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART2_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART2_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART2_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART2_TX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART2_TX.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART2_TX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel7, &DMA_UART2_TX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
		
	DMA_Cmd(DMA1_Channel7, ENABLE);  //��ʽ����DMA����
	
}

void usart2_init(u32 bound)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
		/* ��1������GPIO��USART������ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��3���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = bound;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		/* ��ѡ�����ģʽ */
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	
		/* ʹ�ܴ���2�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); //���������ж�
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE); //ʹ�ܴ���3��DMA����
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���3��DMA����
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
	UART2_DMA_TX_Init(); //UART3_TX��EDMA���ܳ�ʼ��
	UART2_DMA_RX_Init(); //UART3_RX��EDMA���ܳ�ʼ��
//	TIM7_Int_Init(99,7199);		//10ms�ж�
	USART2_5_RX_STA=0;		//����
}

 //����3,printf ����
//ȷ��һ�η������ݲ�����USART2_5_MAX_RECV_LEN�ֽ�
//DMA: 1,DMA��ʽ���� 0����ͨ��ʽ����
void u2_printf(uint8_t DMA,char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_5_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_5_TX_BUF);		//�˴η������ݵĳ���
	if(DMA==0)
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART2,USART2_5_TX_BUF[j]); 
	} 
	else
	UART2_TX_DMA_Enable(i); //ʹ��DMA���ڷ���
}



















