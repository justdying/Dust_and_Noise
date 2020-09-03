#ifndef __USART2_H
#define __USART2_H	 
#include "sys.h"  
#define USART2_5_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define USART2_5_MAX_TX_LEN		2000					//����ͻ����ֽ���
extern u8  USART2_5_RX_BUF[USART2_5_MAX_RECV_LEN]; 		//���ջ���,���USART2_5_MAX_RECV_LEN�ֽ�
extern u8  USART2_5_TX_BUF[USART2_5_MAX_TX_LEN]; 		//���ͻ���,���USART2_5_MAX_RECV_LEN�ֽ�
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
extern vu16 USART2_5_RX_STA;   						//��������״̬
extern vu16  Usart2_5_Rec_Cnt;             //��֡���ݳ���	
extern vu16  Usart2_5_Txd_Cnt;             //��֡���ݳ���	
void usart2_init(u32 bound);   			//����2��ʼ�� 
void u2_printf(uint8_t DMA,char* fmt,...);
#endif













