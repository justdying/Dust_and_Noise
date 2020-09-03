#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"
#include "rtc.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.7
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	 

//0,不支持ucos
//1,支持ucos
#define SYSTEM_SUPPORT_OS		1		//定义系统文件夹是否支持UCOS
#define	DEBUG		       			1   //支持调试											    
	 
	
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
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
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

//系统指令
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
//继电器控制的负载
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
//修改这个的个数 记得修改BOOTLEADER程序里面FLASH存储的位置
typedef struct system_config_table
{	
	uint8_t  Calendar[26];									//日期
	uint8_t  Project_Company_name[40];			//公司工程名字
	uint8_t  Equipment_number[40];					//设备编号
	Config_Sock Socka;											//SOCKA
	Config_Sock Sockb;											//SOCKB						
	uint16_t PM10_low_threshold; 					//PM2.5传感器低浓度阀值
	uint16_t PM10_high_threshold;					//PM2.5传感器高浓度阀值
	float 	 PM2_5_k_factor; 								//PM2.5传感器k
	float    PM10_k_factor;
	uint8_t  PM2_5_b_factor;								//PM2.5传感器b
	uint8_t  PM10_b_factor;
	uint8_t  Led_Show_Type;
	uint32_t Table_address;									//存储的地址
}System_Config_Table;

extern System_Config_Table System_Config_Table1;		//系统配置表全局
extern void Init_System_Config_Table(void);					//初始化系统配置表
extern void Print_System_Config_Table(void);				//打印系统配置表
//以下为汇编函数
void WFI_SET(void);								//执行WFI指令
void INTX_DISABLE(void);					//关闭所有中断
void INTX_ENABLE(void);						//开启所有中断
void MSR_MSP(u32 addr);						//设置堆栈地址
void SYSTEMRESET(void);  	//复位
#endif
