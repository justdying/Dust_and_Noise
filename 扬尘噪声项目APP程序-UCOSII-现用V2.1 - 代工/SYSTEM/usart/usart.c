#include "sys.h"
#include "usart.h"	
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os ʹ��	  
#endif
//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    

_ttywrch(int ch) 
{ 
ch = ch; 
} 

_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
/* ˽�б��� ------------------------------------------------------------------*/
/* ����ÿ�����ڽṹ����� */
#if USART1_FIFO_EN == 1
	static USART_FIFO_Typedef g_tUsart1;
	static uint8_t g_TxBuf1[USART1_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf1[USART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
void RS485_SendOver(void);
void RS485_SendBefor(void);
void RS485_ReciveNew(uint8_t _byte);

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ��ʼ��������صı���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static void UsartVarInit(void)
{
#if USART1_FIFO_EN == 1
	g_tUsart1.usart = USART1;						        /* STM32 �����豸 */
	g_tUsart1.pTxBuf = g_TxBuf1;					        /* ���ͻ�����ָ�� */
	g_tUsart1.pRxBuf = g_RxBuf1;					        /* ���ջ�����ָ�� */
	g_tUsart1.usTxBufSize = USART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUsart1.usRxBufSize = USART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUsart1.usTxWrite = 0;			          			/* ����FIFOд���� */
	g_tUsart1.usTxRead = 0;						          /* ����FIFO������ */
	g_tUsart1.usRxWrite = 0;						          /* ����FIFOд���� */
	g_tUsart1.usRxRead = 0;						          /* ����FIFO������ */
	g_tUsart1.usRxCount = 0;						          /* ���յ��������ݸ��� */
	g_tUsart1.usTxCount = 0;						          /* �����͵����ݸ��� */
	g_tUsart1.SendBefor = RS485_SendBefor;						          /* ��������ǰ�Ļص����� */
	g_tUsart1.SendOver = RS485_SendOver;						          /* ������Ϻ�Ļص����� */
	g_tUsart1.ReciveNew = RS485_ReciveNew;						          /* ���յ������ݺ�Ļص����� */
#endif

}


//��ʼ��IO ����1 
//bound:������
void uart_init()
{
	#if USART1_FIFO_EN == 1		/* ����1 TX = PA9   RX = PA10 */
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	
 	USART_DeInit(USART1);  //��λ����1
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
 
	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = USART1_BAUD;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
	#endif

}
/*
*********************************************************************************************************
*	�� �� ��: RS485_InitTXE
*	����˵��: ����RS485����ʹ�ܿ��� TXE
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void RS485_InitTXEN(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RS485_TXEN_RCC_CLOCKGPIO, ENABLE);	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* �������ģʽ */
	GPIO_InitStructure.GPIO_Pin = RS485_TXEN_GPIO_PIN;
	GPIO_Init(RS485_TXEN_GPIO, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBefor
*	����˵��: ��������ǰ��׼������������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendBefor = RS485_SendBefor
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
	RS485_TX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}


/*
*********************************************************************************************************
*	�� �� ��: RS485_SendOver
*	����˵��: ����һ�����ݽ�������ƺ�������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendOver = RS485_SendOver
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
	RS485_RX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}


/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBuf
*	����˵��: ͨ��RS485оƬ����һ�����ݡ�ע�⣬���������ȴ�������ϡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	comSendBuf(COM1, _ucaBuf, _usLen);
}
/*
*********************************************************************************************************
*	�� �� ��: RS485_ReciveNew
*	����˵��: ���յ��µ�����
*	��    ��: _byte ���յ���������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern void MODH_ReciveNew(uint8_t _data);
void RS485_ReciveNew(uint8_t _byte)
{
	MODH_ReciveNew(_byte);
}

/**
  * ��������: ���ڲ�������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void Usart_FIFO_Init(void)
{  
  /* ��ʼ��������صı��� */
  UsartVarInit();  
  RS485_InitTXEN();
  /* ����NVIC���趨USART�����ж����ȼ� */ 
  /* ��ʼ��USART��ӦGPIO��USART���� */
  uart_init();  
}
/*
*********************************************************************************************************
*	�� �� ��: ComToUsart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: uartָ��
*********************************************************************************************************
*/
USART_FIFO_Typedef *ComToUsart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
#if USART1_FIFO_EN == 1
			return &g_tUsart1;
#else
			return 0;
#endif
	}
	if (_ucPort == COM2)
	{
#if USART2_FIFO_EN == 1
			return &g_tUsart2;
#else
			return 0;
#endif
	}
	if (_ucPort == COM3)
	{
#if USART3_FIFO_EN == 1
			return &g_tUsart3;
#else
			return 0;
#endif
	}
	if (_ucPort == COM5)
	{
#if UART5_FIFO_EN == 1
			return &g_tUart5;
#else
			return 0;
#endif
	}
	else
	{
		/* �����κδ��� */
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: UsartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
*	��    ��: _pUsart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
static uint8_t UsartGetChar(USART_FIFO_Typedef *_pUsart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	DISABLE_INT();
	usCount = _pUsart->usRxCount;
	ENABLE_INT();

	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUsart->pRxBuf[_pUsart->usRxRead];		/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
		DISABLE_INT();
		if (++_pUsart->usRxRead >= _pUsart->usRxBufSize)
		{
			_pUsart->usRxRead = 0;
		}
		_pUsart->usRxCount--;
		ENABLE_INT();
		return 1;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UsartSend(USART_FIFO_Typedef *_pUsart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
		/* �� _pUsart->usTxBufSize == 1 ʱ, ����ĺ���������(������) */
		while (1)
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = _pUsart->usTxCount;
			ENABLE_INT();

			if (usCount < _pUsart->usTxBufSize)
			{
				break;
			}
		}
		/* �����������뷢�ͻ����� */
		_pUsart->pTxBuf[_pUsart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUsart->usTxWrite >= _pUsart->usTxBufSize)
		{
			_pUsart->usTxWrite = 0;
		}
		_pUsart->usTxCount++;
		ENABLE_INT();
	}
	USART_ITConfig(_pUsart->usart, USART_IT_TXE, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: comSendBuf
*	����˵��: �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM3)
*			  _ucaBuf: �����͵����ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	USART_FIFO_Typedef *pUsart;

	pUsart = ComToUsart(_ucPort);
	if (pUsart == 0)
	{
		return;
	}
	if (pUsart->SendBefor != 0)
	{
		pUsart->SendBefor();
	}
	UsartSend(pUsart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM3)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �Ӵ��ڻ�������ȡ1�ֽڣ��������������������ݾ���������
*	��    ��: _ucPort: �˿ں�(COM1 - COM3)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	USART_FIFO_Typedef *pUsart;

	pUsart = ComToUsart(_ucPort);
	if (pUsart == 0)
	{
		return 0;
	}
	return UsartGetChar(pUsart, _pByte);
}

/*
*********************************************************************************************************
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM3)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
	USART_FIFO_Typedef *pUsart;

	pUsart = ComToUsart(_ucPort);
	if (pUsart == 0)
	{
		return;
	}
	pUsart->usTxWrite = 0;
	pUsart->usTxRead = 0;
	pUsart->usTxCount = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM3)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
	USART_FIFO_Typedef *pUsart;

	pUsart = ComToUsart(_ucPort);
	if (pUsart == 0)
	{
		return;
	}
	pUsart->usRxWrite = 0;
	pUsart->usRxRead = 0;
	pUsart->usRxCount = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SetUart1Baud
*	����˵��: �޸�UART1������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SetUsart1Baud(uint32_t _baud)
{
	USART_InitTypeDef USART_InitStructure;

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = _baud;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
}
/*
*********************************************************************************************************
*	�� �� ��: UsartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUsart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UsartIRQ(USART_FIFO_Typedef *_pUsart)
{
	/* ��������ж�  */
	if (USART_GetITStatus(_pUsart->usart, USART_IT_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;

		ch = USART_ReceiveData(_pUsart->usart);
		_pUsart->pRxBuf[_pUsart->usRxWrite] = ch;
//		USART_SendData(USART3, ch);
		if (++_pUsart->usRxWrite >= _pUsart->usRxBufSize)
		{
			_pUsart->usRxWrite = 0;
		}
		if (_pUsart->usRxCount < _pUsart->usRxBufSize)
		{
			_pUsart->usRxCount++;
		}
		/* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
    if (_pUsart->ReciveNew)
    {
      _pUsart->ReciveNew(ch);
    }
	}

	/* �����ͻ��������ж� */
	if (USART_GetITStatus(_pUsart->usart, USART_IT_TXE) != RESET)
	{
		if (_pUsart->usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			USART_ITConfig(_pUsart->usart, USART_IT_TXE, DISABLE);

			/* ʹ�����ݷ�������ж� */
			USART_ITConfig(_pUsart->usart, USART_IT_TC, ENABLE);
		}
		else
		{
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			USART_SendData(_pUsart->usart, _pUsart->pTxBuf[_pUsart->usTxRead]);
			
//#if DEBUG  
//			delay_ms(1);
//			USART_SendData(USART3,_pUsart->pTxBuf[_pUsart->usTxRead]);
//#endif
			if (++_pUsart->usTxRead >= _pUsart->usTxBufSize)
			{
				_pUsart->usTxRead = 0;
			}
			_pUsart->usTxCount--;
		}

	}
	/* ����bitλȫ��������ϵ��ж� */
	else if (USART_GetITStatus(_pUsart->usart, USART_IT_TC) != RESET)
	{
		if (_pUsart->usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			USART_ITConfig(_pUsart->usart, USART_IT_TC, DISABLE);

			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
			if (_pUsart->SendOver)
			{
				_pUsart->SendOver();
			}
		}
		else
		{
			/* ��������£��������˷�֧ */
			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			USART_SendData(_pUsart->usart, _pUsart->pTxBuf[_pUsart->usTxRead]);
			if (++_pUsart->usTxRead >= _pUsart->usTxBufSize)
			{
				_pUsart->usTxRead = 0;
			}
			_pUsart->usTxCount--;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if USART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	UsartIRQ(&g_tUsart1);
}
#endif






