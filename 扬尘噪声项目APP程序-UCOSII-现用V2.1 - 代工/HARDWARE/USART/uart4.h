#ifndef __UART4_H
#define __UART4_H
#include "sys.h"

#define USART4_MAX_RECV_LEN		256					//最大接收缓存字节数
#define USART4_MAX_SEND_LEN		256					//最大发送缓存字节数
extern u8  USART4_RX_BUF[USART4_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_5_MAX_RECV_LEN字节
extern u8  USART4_TX_BUF[USART4_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_5_MAX_RECV_LEN字节
extern vu16  Usart4_Rec_Cnt;             //本帧数据长度	
extern vu16  Usart4_Tx_Cnt;
extern vu16 Uart4_Rx_Sta;
extern void uart4_init(u32 bound);
extern void u4_printf(uint8_t DMA,char* fmt,...);
#endif

