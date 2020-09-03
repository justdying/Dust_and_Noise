#ifndef __MOSBUS_HOST_H__
#define __MOSBUS_HOST_H__

#include <stm32f10x.h>
#include "usart.h"

#define SlaveAddr_MelogiEle		0x03   //气象多要素传感器
//#define SlaveAddr_TemHum			0x04   //温湿度传感器
//#define SlaveAddr_Noise				0x03   //噪音传感器
#define SlaveAddr_PM2_5PM10		0x10   //PM传感器      //0x0f   05
#define SlaveAddr_Atmo				0x06   //大气压传感器
#define SlaveAddr_Light				0x07   //光照传感器
#define SlaveAddr_Fan					0x01   //风速传感器    //08   01 
#define SlaveAddr_Wind_Dir		0x02   //风向传感器		//09   02
#define HBAUD485		USART1_BAUD

/* 01H 读强制单线圈(继电器) */
/* 05H 写强制单线圈(继电器) */
#define REG_D01		0x0101
#define REG_D02		0x0102
#define REG_D03		0x0103
#define REG_D04		0x0104
#define REG_DXX 	REG_D04

/* 02H 读取输入状态(控制按钮) */
#define REG_T01		0x0201
#define REG_T02		0x0202
#define REG_TXX		REG_T02

/* 03H 读保持寄存器(内部寄存器) */
/* 06H 写保持寄存器(内部寄存器) */
/* 10H 写多个保存寄存器(内部寄存器) */
#define REG_P01		0x0301		
#define REG_P02		0x0302	

/* 04H 读取输入寄存器(模拟量信号) */
#define REG_A01		0x0401
#define REG_AXX		REG_A01

/* RTU 应答代码 */
#define RSP_OK				0		/* 成功 */
#define RSP_ERR_CMD			0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR	0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		0x04	/* 写入失败 */

#define H_RX_BUF_SIZE		64
#define H_TX_BUF_SIZE      	128

typedef struct
{
	uint8_t RxBuf[H_RX_BUF_SIZE];
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;

	uint8_t RspCode;

	uint8_t TxBuf[H_TX_BUF_SIZE];
	uint8_t TxCount;
	
	uint16_t Reg01H;		/* 保存主机发送的寄存器首地址 */
	uint16_t Reg02H;
	uint16_t Reg03H;		
	uint16_t Reg04H;

	uint8_t RegNum;			/* 寄存器个数 */

	uint8_t fAck01H;		/* 应答命令标志 0 表示执行失败 1表示执行成功 */
	uint8_t fAck02H;
	uint8_t fAck03H;
	uint8_t fAck04H;
	uint8_t fAck05H;		
	uint8_t fAck06H;		
	uint8_t fAck10H;
	
}MODH_T;

typedef struct
{
	/* 03H 06H 读写保持寄存器 */
	uint16_t P01;
	uint16_t P02;
	uint16_t FanSpeed;    //风速值
	int16_t  Tem;         //温度值
	uint16_t Hum;         //湿度值
	uint16_t Noise; 			//噪音值
	uint16_t PM2_5;				//PM2.5
	uint16_t PM10;				//PM10
	uint16_t Atmo;				//气压
	uint16_t WinDir;			//风向
	uint8_t  Fack_FanSpeed;
	uint8_t  Fack_MelogiEle;
	uint8_t  Fack_PM;
	uint8_t  Fack_WinDir;	
}VAR_T;

void MODH_Poll(void);
uint8_t MODH_ReadParam_01H(uint8_t slaveaddr,uint16_t _reg, uint16_t _num);
uint8_t MODH_ReadParam_02H(uint8_t slaveaddr,uint16_t _reg, uint16_t _num);
uint8_t MODH_ReadParam_03H(uint8_t slaveaddr,uint16_t _reg, uint16_t _num);
uint8_t MODH_ReadParam_04H(uint8_t slaveaddr,uint16_t _reg, uint16_t _num);
uint8_t MODH_WriteParam_05H(uint8_t slaveaddr,uint16_t _reg, uint16_t _value);
uint8_t MODH_WriteParam_06H(uint8_t slaveaddr,uint16_t _reg, uint16_t _value);
uint8_t MODH_WriteParam_10H(uint8_t slaveaddr,uint16_t _reg, uint8_t _num, uint8_t *_buf);
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void * _pCallBack);
extern MODH_T g_tModH;
extern VAR_T g_tVar;
#endif

/***************************** (END OF FILE) *********************************/
