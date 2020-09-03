#include "uart4.h"
#include "sys.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "delay.h"
#include "includes.h"					//ucos ʹ��	
//LORA
vu16  Usart4_Rec_Cnt = 0;             //��֡���ݳ���	
vu16  Usart4_Tx_Cnt = 0;
vu16 Uart4_Rx_Sta = 0;
uint8_t USART4_RX_BUF[USART4_MAX_RECV_LEN]={0}; 				//���ջ���,���USART2_5_MAX_RECV_LEN���ֽ�.
uint8_t USART4_TX_BUF[USART4_MAX_SEND_LEN]={0}; 			//���ͻ���,���USART2_5_MAX_RECV_LEN�ֽ�

//����ʹ��UART2_RX��DMA����
void UART4_RX_DMA_Enable(void)
{ 
	DMA_Cmd(DMA2_Channel3, DISABLE ); //��ֹͣDMA����ͣ���� 
 	DMA_SetCurrDataCounter(DMA2_Channel3, USART4_MAX_RECV_LEN); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA2_Channel3, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	
//����ʹ��UART2_TX��DMA����
void UART4_TX_DMA_Enable(uint16_t  lenth)
{ 
	DMA_Cmd(DMA2_Channel5, DISABLE ); //��ֹͣDMA
 	DMA_SetCurrDataCounter(DMA2_Channel5, lenth); //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA2_Channel5, ENABLE); //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 
}	
//UART2_RX��DMA��ʼ��
void UART4_DMA_RX_Init(void)
{
	DMA_InitTypeDef DMA_UART4_RX;
	
	//��Ӧ��DMA����
	DMA_DeInit(DMA2_Channel3);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����2��Ӧ����DMAͨ��6
	DMA_UART4_RX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA����ADC����ַ
	DMA_UART4_RX.DMA_MemoryBaseAddr = (u32)USART4_RX_BUF;  //DMA�ڴ����ַ
	DMA_UART4_RX.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴������ȡ���͵��ڴ�
	DMA_UART4_RX.DMA_BufferSize = USART4_MAX_RECV_LEN;  //DMAͨ����DMA����Ĵ�С
	DMA_UART4_RX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART4_RX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART4_RX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART4_RX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART4_RX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART4_RX.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART4_RX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA2_Channel3, &DMA_UART4_RX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
		
	DMA_Cmd(DMA2_Channel3, ENABLE);  //��ʽ����DMA����
}
//UART1_TX��DMA��ʼ��
void UART4_DMA_TX_Init(void)
{
	DMA_InitTypeDef DMA_UART4_TX;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	//ʹ��DMA����
	//��Ӧ��DMA����
	DMA_DeInit(DMA2_Channel5);   //��DMA��ͨ��5�Ĵ�������Ϊȱʡֵ  ����1��Ӧ����DMAͨ��5
	DMA_UART4_TX.DMA_PeripheralBaseAddr = (u32)&UART4->DR;  //DMA����ADC����ַ
	DMA_UART4_TX.DMA_MemoryBaseAddr = (u32)USART4_TX_BUF;  //DMA�ڴ����ַ
	DMA_UART4_TX.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_UART4_TX.DMA_BufferSize = 0;  //DMAͨ����DMA����Ĵ�С
	DMA_UART4_TX.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_UART4_TX.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_UART4_TX.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_UART4_TX.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_UART4_TX.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_UART4_TX.DMA_Priority = DMA_Priority_VeryHigh; //DMAͨ�� xӵ�������ȼ� 
	DMA_UART4_TX.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA2_Channel5, &DMA_UART4_TX);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
		
	DMA_Cmd(DMA2_Channel5, ENABLE);  //��ʽ����DMA����
	
}

void uart4_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
	USART_DeInit(UART4);  //��λ����4

	//UART4_TX   PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC.10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PC10

	//UART4_RX	  PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��PB11

	//Uart4 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART4, &USART_InitStructure); //��ʼ������
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�ж�
	
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); //���������ж�
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE); //ʹ�ܴ���3��DMA����
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); //ʹ�ܴ���3��DMA����
	
	USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ��� 
	UART4_DMA_TX_Init(); //UART3_TX��EDMA���ܳ�ʼ��						//������Ҫ
	UART4_DMA_RX_Init(); //UART3_RX��EDMA���ܳ�ʼ��						//������Ҫ
}
//����4,printf ����
//ȷ��һ�η������ݲ�����USART2_5_MAX_RECV_LEN�ֽ�
void u4_printf(uint8_t DMA,char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART4_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART4_TX_BUF);		//�˴η������ݵĳ���
	if(DMA==0)
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(UART4,USART4_TX_BUF[j]); 
	} 
	else
	UART4_TX_DMA_Enable(i); //ʹ��DMA���ڷ���
}
void UART4_IRQHandler(void)
{     
//	static OS_ERR err; 
	//����3�����ж�

	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)  
	{	
		/* 1.�����־ */
		USART_ClearITPendingBit(UART4, USART_IT_IDLE); //����жϱ�־
		
		/* 2.��ȡDMA */
		USART_ReceiveData(UART4); //��ȡ����
		Usart4_Rec_Cnt = USART4_MAX_RECV_LEN - DMA_GetCurrDataCounter(DMA2_Channel3); //���ո������ڽ��ջ������ܴ�С���Ѿ����յĸ���
		
		/* 3.�������ݽ����������� */
//		memcpy(USART2_TX_BUF, USART2_RX_BUF, Usart2_Rec_Cnt); //������ת��ͨ������1��DMA��ʽ���ͳ�ȥ����
//		u2_printf("%s",USART2_TX_BUF);
//		UART3_TX_DMA_Enable(Usart2_5_Rec_Cnt); //����һ��DMA���ͣ�ʵ��ת��
		/* 4.�����µ�һ��DMA���� */
		Uart4_Rx_Sta|=0x8000;
		UART4_RX_DMA_Enable(); //����ʹ��DMA���ȴ���һ�εĽ���
   }  	
}


