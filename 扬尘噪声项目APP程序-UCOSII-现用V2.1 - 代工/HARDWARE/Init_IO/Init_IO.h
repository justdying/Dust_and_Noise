#ifndef __Init_IO_H
#define __Init_IO_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PAout(15)// PB5
//#define RELAY_PHOTO_BUTTON PCout(0)// PC0
//#define RELAY1 PCout(1)// PB5
//#define RELAY2 PCout(2)// PB5
void LED_Init(void);//��ʼ��
//void RELAY_Init(void);//��ʼ���̵���
		 				    
#endif
