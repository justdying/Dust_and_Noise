#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2011/6/14
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
/* RS485оƬ����ʹ��GPIO, PB2 */

#define	USART1_FIFO_EN                     	  1


//extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
/* ���Ͷ��� ------------------------------------------------------------------*/
/* ����˿ں� */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART1 */
	COM3 = 2,	/* USART1 */
	COM4 = 3,	/* USART1 */
	COM5 = 4,	/* USART1 */
}COM_PORT_E;

/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *usart;              /* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;                   /* ���ͻ����� */
	uint8_t *pRxBuf;                   /* ���ջ����� */
	uint16_t usTxBufSize;              /* ���ͻ�������С */
	uint16_t usRxBufSize;              /* ���ջ�������С */
	__IO uint16_t usTxWrite;           /* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;            /* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;           /* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;           /* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;            /* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;           /* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void);           /* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void);            /* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);  /* �����յ����ݵĻص�����ָ�� */
}USART_FIFO_Typedef;
/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
#if USART1_FIFO_EN == 1
	#define USART1_BAUD			                    9600
	#define USART1_TX_BUF_SIZE	                1*1024
	#define USART1_RX_BUF_SIZE	                1*1024
	#define RS485_TXEN_RCC_CLOCKGPIO 	          RCC_APB2Periph_GPIOA
	#define RS485_TXEN_GPIO                     GPIOA
	#define RS485_TXEN_GPIO_PIN	                GPIO_Pin_11
	#define RS485_RX_EN()	                      RS485_TXEN_GPIO->BRR = RS485_TXEN_GPIO_PIN
	#define RS485_TX_EN()	                      RS485_TXEN_GPIO->BSRR = RS485_TXEN_GPIO_PIN	
#endif
#if UART5_FIFO_EN == 1
	#define UART5_BAUD			                    9600
	#define UART5_TX_BUF_SIZE	               	  1*1024
	#define UART5_RX_BUF_SIZE	               		1*1024
	#define RS485_2_TXEN_RCC_CLOCKGPIO 	        RCC_APB2Periph_GPIOB
	#define RS485_2_TXEN_GPIO                   GPIOB
	#define RS485_2_TXEN_GPIO_PIN	              GPIO_Pin_3
	#define RS485_2_RX_EN()	                    RS485_2_TXEN_GPIO->BRR = RS485_2_TXEN_GPIO_PIN
	#define RS485_2_TX_EN()	                    RS485_2_TXEN_GPIO->BSRR = RS485_2_TXEN_GPIO_PIN	
#endif
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void Usart_FIFO_Init(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);

void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);

void bsp_SetUsart1Baud(uint32_t _baud);
void bsp_SetUsart2Baud(uint32_t _baud);
void bsp_SetUsart3Baud(uint32_t _baud);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);

#endif


