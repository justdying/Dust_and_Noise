#ifndef __UART4_H
#define __UART4_H
#include "sys.h"

#define USART4_MAX_RECV_LEN		256					//�����ջ����ֽ���
#define USART4_MAX_SEND_LEN		256					//����ͻ����ֽ���
extern u8  USART4_RX_BUF[USART4_MAX_RECV_LEN]; 		//���ջ���,���USART2_5_MAX_RECV_LEN�ֽ�
extern u8  USART4_TX_BUF[USART4_MAX_SEND_LEN]; 		//���ͻ���,���USART2_5_MAX_RECV_LEN�ֽ�
extern vu16  Usart4_Rec_Cnt;             //��֡���ݳ���	
extern vu16  Usart4_Tx_Cnt;
extern vu16 Uart4_Rx_Sta;
extern void uart4_init(u32 bound);
extern void u4_printf(uint8_t DMA,char* fmt,...);
#endif

