#ifndef __DELAY_H
#define __DELAY_H 			   
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 

#define DISABLE_INT()                         { __set_PRIMASK(1); }	/* ���ж� */
#define ENABLE_INT()                          { __set_PRIMASK(0); }	/* ���ж� */ 
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);
int32_t bsp_CheckRunTime(int32_t _LastTime);
int32_t bsp_GetRunTime(void);


#endif





























