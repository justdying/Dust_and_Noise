#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
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
