#ifndef __USART2_H
#define __USART2_H	 
#include "sys.h"  
#define USART2_5_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART2_5_MAX_TX_LEN		2000					//最大发送缓存字节数
extern u8  USART2_5_RX_BUF[USART2_5_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_5_MAX_RECV_LEN字节
extern u8  USART2_5_TX_BUF[USART2_5_MAX_TX_LEN]; 		//发送缓冲,最大USART2_5_MAX_RECV_LEN字节
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
extern vu16 USART2_5_RX_STA;   						//接收数据状态
extern vu16  Usart2_5_Rec_Cnt;             //本帧数据长度	
extern vu16  Usart2_5_Txd_Cnt;             //本帧数据长度	
void usart2_init(u32 bound);   			//串口2初始化 
void u2_printf(uint8_t DMA,char* fmt,...);
#endif













