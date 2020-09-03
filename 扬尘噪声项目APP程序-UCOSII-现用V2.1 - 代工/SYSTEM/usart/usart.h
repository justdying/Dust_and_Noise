#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.csom
//修改日期:2011/6/14
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
/* RS485芯片发送使能GPIO, PB2 */

#define	USART1_FIFO_EN                     	  1


//extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
/* 类型定义 ------------------------------------------------------------------*/
/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1 */
	COM2 = 1,	/* USART1 */
	COM3 = 2,	/* USART1 */
	COM4 = 3,	/* USART1 */
	COM5 = 4,	/* USART1 */
}COM_PORT_E;

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *usart;              /* STM32内部串口设备指针 */
	uint8_t *pTxBuf;                   /* 发送缓冲区 */
	uint8_t *pRxBuf;                   /* 接收缓冲区 */
	uint16_t usTxBufSize;              /* 发送缓冲区大小 */
	uint16_t usRxBufSize;              /* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;           /* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;            /* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;           /* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;           /* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;            /* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;           /* 还未读取的新数据个数 */

	void (*SendBefor)(void);           /* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void);            /* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(uint8_t _byte);  /* 串口收到数据的回调函数指针 */
}USART_FIFO_Typedef;
/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
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
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
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


