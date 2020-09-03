#ifndef __USART5_H
#define __USART5_H	
#include "sys.h" 

/* �����豸�ṹ�� */
typedef struct
{
	__IO uint16_t usTxWrite;           /* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;            /* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;           /* �ȴ����͵����ݸ��� */
	__IO uint16_t usRxWrite;           /* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;            /* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;           /* ��δ��ȡ�������ݸ��� */
}USART_Typedef;
void uart5_init(u32 bound);   			//����5��ʼ��
void u5_printf(char* fmt,...);
void U5_SendString(uint8_t *_buf, uint8_t _len);
#endif

