#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//�ڴ���� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/1/20
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
#ifndef NULL
#define NULL 0
#endif

//���������ڴ��
#define SRAMIN	 0		//�ڲ��ڴ��
#define SRAMEX   1		//�ⲿ�ڴ�� 

#define SRAMBANK 	2	//����֧�ֵ�SRAM����.	


//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			18*1024  						//�������ڴ� 40K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С


		 
 
//�ڴ���������
struct _m_mallco_dev
{
	void (*init)(void);					//��ʼ��
	u8 (*perused)(void);		  	    	//�ڴ�ʹ����
	u8 	*membase;				//�ڴ�� ����SRAMBANK��������ڴ�
	u16 *memmap; 				//�ڴ����״̬��
	u8  memrdy; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,u8 c,u32 count);	//�����ڴ�
void mymemcpy(void *des,void *src,u32 n);//�����ڴ�     
void my_mem_init(void);				//�ڴ�����ʼ������(��/�ڲ�����)
u32 my_mem_malloc(u32 size);	//�ڴ����(�ڲ�����)
u8 my_mem_free(u32 offset);		//�ڴ��ͷ�(�ڲ�����)
u8 my_mem_perused(void);				//����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(u32 size);			//�ڴ����(�ⲿ����)
void *myrealloc(void *ptr,u32 size);//���·����ڴ�(�ⲿ����)
#endif













