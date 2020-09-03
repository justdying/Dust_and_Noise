#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define MINUTEFLAG 0x01
#define TENMINUTEFLAG 0x02
#define HOURFLAG 0x04
#define DAYFLAG 0x08
#define TENSECOND 0x10
#define MINUTEFLAG_SEN 0x20
#define TENMINUTEFLAG_SEN 0x40
#define HOURFLAG_SEN 0x80
#define DAYFLAG_SEN 0x0100

extern void TIM2_Int_Init(void);
#endif
