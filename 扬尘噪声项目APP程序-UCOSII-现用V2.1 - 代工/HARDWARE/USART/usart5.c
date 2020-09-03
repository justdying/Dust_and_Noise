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
/* ��1���� ����GPIO��UARTʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	USART_DeInit(UART5);  //��λ����5
	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	/* ��4���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = bound;	/* ������ */
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
	
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������*/
	USART_Cmd(UART5, ENABLE);		/* ʹ�ܴ��� */
	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(UART5, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
}	
void U5_SendString(uint8_t *_buf, uint8_t _len)
{
	uint8_t j;
	for(j=0;j<_len;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
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
	i=strlen((const char*)USART2_5_TX_BUF);		//�˴η������ݵĳ���
	uart_5.usTxCount = i;
//	for(j=0;j<i;j++)							//ѭ����������
//	{
//	  while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
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
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
			Res = USART_ReceiveData(UART5);//(UART5->DR);	//��ȡ���յ�������
			bsp_StartHardTimer(4, timeout, (void *)uart5_RxTimeOut);   //������ʱ��
			USART2_5_RX_BUF[USART2_5_RX_STA++] = Res;	//��¼���յ���ֵ	
			Usart2_5_Rec_Cnt++;			
	}
		/* �����ͻ��������ж� */
	if (USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{
		if (uart_5.usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
			/* ʹ�����ݷ�������ж� */
			USART_ITConfig(UART5, USART_IT_TC, ENABLE);
		}
		else
		{
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			USART_SendData(UART5, USART2_5_TX_BUF[uart_5.usTxRead]);
			if (++uart_5.usTxRead >= USART2_5_MAX_TX_LEN)
			{
				uart_5.usTxRead = 0;
			}
			uart_5.usTxCount--;
		}
	}
	/* ����bitλȫ��������ϵ��ж� */
	else if (USART_GetITStatus(UART5, USART_IT_TC) != RESET)
	{
		uart_5.usTxRead = 0;   //��շ�������
		if (uart_5.usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			USART_ITConfig(UART5, USART_IT_TC, DISABLE);		
		}
		else
		{
			/* ��������£��������˷�֧ */
			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			USART_SendData(UART5, USART2_5_TX_BUF[uart_5.usTxRead]);
			if (++uart_5.usTxRead >= USART2_5_MAX_TX_LEN)
			{
				uart_5.usTxRead = 0;
			}
			uart_5.usTxCount--;
		}
	}
}







