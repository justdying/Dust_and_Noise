#ifndef __USART5_H
#define __USART5_H	
#include "sys.h" 

/* 串口设备结构体 */
typedef struct
{
	__IO uint16_t usTxWrite;           /* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;            /* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;           /* 等待发送的数据个数 */
	__IO uint16_t usRxWrite;           /* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;            /* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;           /* 还未读取的新数据个数 */
}USART_Typedef;
void uart5_init(u32 bound);   			//串口5初始化
void u5_printf(char* fmt,...);
void U5_SendString(uint8_t *_buf, uint8_t _len);
#endif

