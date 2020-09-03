#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"
#include "rtc.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.7
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	 

//0,��֧��ucos
//1,֧��ucos
#define SYSTEM_SUPPORT_OS		1		//����ϵͳ�ļ����Ƿ�֧��UCOS
#define	DEBUG		       			1   //֧�ֵ���											    
	 
	
//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

//ϵͳָ��
#define SYSTEM_ORDER_GETCONFIG  	  0XF0
//#define SYSTEM_ORDER_COLLE_SPACE  0XF1
#define SYSTEM_ORDER_COMPANY_NAME 0XF2
#define SYSTEM_ORDER_EQUIP_NUMBER 0XF3
#define SYSTEM_ORDER_TIME_SET 		0XF4
#define SYSTEM_ORDER_REALTIME_DATA 		0XF5
#define SYSTEM_ORDER_DUST_GATE 		0XF6
#define SYSTEM_ORDER_DTU_INIT 				0XF7
#define SYSTEM_ORDER_DUST_FACTOR 	0XF8
#define SYSTEM_ORDER_SHOW_TYPE 	  0XF9
#define SYSTEM_ORDER_RESET			 		0XFA
#define SYSTEM_ORDER_APP_UPDATE 	0X77
#define SYSTEM_ORDER_DTU_AIP_PR 	0XD1
#define SYSTEM_ORDER_DTU_BIP_PR 	0XD2
//�̵������Ƶĸ���
#define SYSTEM_FOG_GUN_ON 				0X0B
#define SYSTEM_FOG_GUN_OFF 				0X0C

typedef struct config_sock
{
	uint8_t ip0;
	uint8_t ip1;
	uint8_t ip2;
	uint8_t ip3;
	uint16_t pr;
	uint8_t enable;
}Config_Sock;
//�޸�����ĸ��� �ǵ��޸�BOOTLEADER��������FLASH�洢��λ��
typedef struct system_config_table
{	
	uint8_t  Calendar[26];									//����
	uint8_t  Project_Company_name[40];			//��˾��������
	uint8_t  Equipment_number[40];					//�豸���
	Config_Sock Socka;											//SOCKA
	Config_Sock Sockb;											//SOCKB						
	uint16_t PM10_low_threshold; 					//PM2.5��������Ũ�ȷ�ֵ
	uint16_t PM10_high_threshold;					//PM2.5��������Ũ�ȷ�ֵ
	float 	 PM2_5_k_factor; 								//PM2.5������k
	float    PM10_k_factor;
	uint8_t  PM2_5_b_factor;								//PM2.5������b
	uint8_t  PM10_b_factor;
	uint8_t  Led_Show_Type;
	uint32_t Table_address;									//�洢�ĵ�ַ
}System_Config_Table;

extern System_Config_Table System_Config_Table1;		//ϵͳ���ñ�ȫ��
extern void Init_System_Config_Table(void);					//��ʼ��ϵͳ���ñ�
extern void Print_System_Config_Table(void);				//��ӡϵͳ���ñ�
//����Ϊ��ຯ��
void WFI_SET(void);								//ִ��WFIָ��
void INTX_DISABLE(void);					//�ر������ж�
void INTX_ENABLE(void);						//���������ж�
void MSR_MSP(u32 addr);						//���ö�ջ��ַ
void SYSTEMRESET(void);  	//��λ
#endif
