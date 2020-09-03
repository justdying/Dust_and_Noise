#ifndef __MOSBUS_HOST_H__
#define __MOSBUS_HOST_H__

#include <stm32f10x.h>
#include "usart.h"

#define SlaveAddr_MelogiEle		0x03   //�����Ҫ�ش�����
//#define SlaveAddr_TemHum			0x04   //��ʪ�ȴ�����
//#define SlaveAddr_Noise				0x03   //����������
#define SlaveAddr_PM2_5PM10		0x10   //PM������      //0x0f   05
#define SlaveAddr_Atmo				0x06   //����ѹ������
#define SlaveAddr_Light				0x07   //���մ�����
#define SlaveAddr_Fan					0x01   //���ٴ�����    //08   01 
#define SlaveAddr_Wind_Dir		0x02   //���򴫸���		//09   02
#define HBAUD485		USART1_BAUD

/* 01H ��ǿ�Ƶ���Ȧ(�̵���) */
/* 05H дǿ�Ƶ���Ȧ(�̵���) */
#define REG_D01		0x0101
#define REG_D02		0x0102
#define REG_D03		0x0103
#define REG_D04		0x0104
#define REG_DXX 	REG_D04

/* 02H ��ȡ����״̬(���ư�ť) */
#define REG_T01		0x0201
#define REG_T02		0x0202
#define REG_TXX		REG_T02

/* 03H �����ּĴ���(�ڲ��Ĵ���) */
/* 06H д���ּĴ���(�ڲ��Ĵ���) */
/* 10H д�������Ĵ���(�ڲ��Ĵ���) */
#define REG_P01		0x0301		
#define REG_P02		0x0302	

/* 04H ��ȡ����Ĵ���(ģ�����ź�) */
#define REG_A01		0x0401
#define REG_AXX		REG_A01

/* RTU Ӧ����� */
#define RSP_OK				0		/* �ɹ� */
#define RSP_ERR_CMD			0x01	/* ��֧�ֵĹ����� */
#define RSP_ERR_REG_ADDR	0x02	/* �Ĵ�����ַ���� */
#define RSP_ERR_VALUE		0x03	/* ����ֵ����� */
#define RSP_ERR_WRITE		0x04	/* д��ʧ�� */

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
	
	uint16_t Reg01H;		/* �����������͵ļĴ����׵�ַ */
	uint16_t Reg02H;
	uint16_t Reg03H;		
	uint16_t Reg04H;

	uint8_t RegNum;			/* �Ĵ������� */

	uint8_t fAck01H;		/* Ӧ�������־ 0 ��ʾִ��ʧ�� 1��ʾִ�гɹ� */
	uint8_t fAck02H;
	uint8_t fAck03H;
	uint8_t fAck04H;
	uint8_t fAck05H;		
	uint8_t fAck06H;		
	uint8_t fAck10H;
	
}MODH_T;

typedef struct
{
	/* 03H 06H ��д���ּĴ��� */
	uint16_t P01;
	uint16_t P02;
	uint16_t FanSpeed;    //����ֵ
	int16_t  Tem;         //�¶�ֵ
	uint16_t Hum;         //ʪ��ֵ
	uint16_t Noise; 			//����ֵ
	uint16_t PM2_5;				//PM2.5
	uint16_t PM10;				//PM10
	uint16_t Atmo;				//��ѹ
	uint16_t WinDir;			//����
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
