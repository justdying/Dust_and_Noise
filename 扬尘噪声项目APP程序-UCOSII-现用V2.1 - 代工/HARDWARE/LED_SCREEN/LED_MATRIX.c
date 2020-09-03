/**
�����ļ���LED_MATRIX.c
�������ƣ�ȫ��LED����������
�������ݣ�ȫ��LED��������ʾ��������
�����д��7426����
��д���ڣ�2017��07��
�޸ļ�¼��NA
**/	
#include "sys.h"
#include "LED_MATRIX.h"
#include "LED_CODE.h"
#include "Hw_platform.h" 
#include "GT21L16S2W.h"
#include "string.h"
#include "delay.h"
#include "modbus_host.h"
#include "includes.h"					//ucos ʹ��	
#define CHINESE_REC_NUM	64

u8 DispTemp[Height*(Width>>3)]={0};//��ʾ��������
u8 RollDispTem[Height*2+2];//�������ҹ�������
unsigned char ChineseRecordData[CHINESE_REC_NUM][32];//��ʾ�ַ���ģ���ݻ���
unsigned char DisplayRecordIdex[CHINESE_REC_NUM*2 + 2];//��ʾ�ַ���������,���Ȳ��һ�������ַ�
unsigned char DisplayRecordCurrentIdex = 0;
//I/O�ڳ�ʼ��
void LED_Gpio_Int(void)
{
  GPIO_InitTypeDef GPIO_Output_diy;//����I/O�����ýṹ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE,ENABLE);//��GPIO��ʱ��

  GPIO_Output_diy.GPIO_Speed=GPIO_Speed_50MHz;	//IO������������
  GPIO_Output_diy.GPIO_Mode = GPIO_Mode_Out_PP;		    // ����Ϊ�������

  GPIO_Output_diy.GPIO_Pin = SA_PIN|SB_PIN;
  GPIO_Init(SA_PORT, &GPIO_Output_diy);	
	
	GPIO_Output_diy.GPIO_Pin = SB_PIN;
  GPIO_Init(SB_PORT, &GPIO_Output_diy);
  
  GPIO_Output_diy.GPIO_Pin = R1_PIN;
  GPIO_Init(R1_PORT, &GPIO_Output_diy);
	
	GPIO_Output_diy.GPIO_Pin = R2_PIN;
  GPIO_Init(R2_PORT, &GPIO_Output_diy);
	
	GPIO_Output_diy.GPIO_Pin = R3_PIN;
  GPIO_Init(R3_PORT, &GPIO_Output_diy);
	
	GPIO_Output_diy.GPIO_Pin = R4_PIN;
  GPIO_Init(R4_PORT, &GPIO_Output_diy);

  GPIO_Output_diy.GPIO_Pin = OE_PIN;
  GPIO_Init(OE_PORT, &GPIO_Output_diy);	
  
  GPIO_Output_diy.GPIO_Pin = ST_PIN;
  GPIO_Init(ST_PORT, &GPIO_Output_diy);

  GPIO_Output_diy.GPIO_Pin = SH_PIN;
  GPIO_Init(SH_PORT, &GPIO_Output_diy);	
}
//����˵����0����������������������>Width
//			|
//			|
//		   Hight
/**
�������ƣ�void scan(u8 i)
�������ã�LED������ɨ��
����������i:Ҫɨ�������
�������أ���
�޸�˵����NA
**/
static void scan(u8 i)	//��ɨ�躯��
{
	 switch(i)
	 {
			//��ɨ�躯�� 4ɨ
	    case 0: {SA_STATE=0;SB_STATE=0;};break;
			case 1: {SA_STATE=1;SB_STATE=0;};break;
			case 2: {SA_STATE=0;SB_STATE=1;};break;
			case 3: {SA_STATE=1;SB_STATE=1;};break;
	 		//��ɨ�躯�� 8ɨ
//	        case 0: {SA_STATE=0;SB_STATE=0;CLR_SC;};break;
//			case 1: {SA_STATE=1;SB_STATE=0;CLR_SC;};break;
//			case 2: {SA_STATE=0;SB_STATE=1;CLR_SC;};break;
//			case 3: {SA_STATE=1;SB_STATE=1;CLR_SC;};break;
//			case 4: {SA_STATE=0;SB_STATE=0;SET_SC;};break;
//			case 5: {SA_STATE=1;SB_STATE=0;SET_SC;};break;
//			case 6: {SA_STATE=0;SB_STATE=1;SET_SC;};break;
//			case 7: {SA_STATE=1;SB_STATE=1;SET_SC;};break;
			//��ɨ�躯�� 16ɨ
//	        case 0: {SA_STATE=0;SB_STATE=0;CLR_SC;SET_SD;};break;
//			case 1: {SA_STATE=1;SB_STATE=0;CLR_SC;SET_SD;};break;
//			case 2: {SA_STATE=0;SB_STATE=1;CLR_SC;SET_SD;};break;
//			case 3: {SA_STATE=1;SB_STATE=1;CLR_SC;SET_SD;};break;
//			case 4: {SA_STATE=0;SB_STATE=0;SET_SC;SET_SD;};break;
//			case 5: {SA_STATE=1;SB_STATE=0;SET_SC;SET_SD;};break;
//			case 6: {SA_STATE=0;SB_STATE=1;SET_SC;SET_SD;};break;
//			case 7: {SA_STATE=1;SB_STATE=1;SET_SC;SET_SD;};break;
//			case 8: {SA_STATE=0;SB_STATE=0;CLR_SC;CLR_SD;};break;
//			case 9: {SA_STATE=1;SB_STATE=0;CLR_SC;CLR_SD;};break;
//			case 10:{SA_STATE=0;SB_STATE=1;CLR_SC;CLR_SD;};break;
//			case 11:{SA_STATE=1;SB_STATE=1;CLR_SC;CLR_SD;};break;
//			case 12:{SA_STATE=0;SB_STATE=0;SET_SC;CLR_SD;};break;
//			case 13:{SA_STATE=1;SB_STATE=0;SET_SC;CLR_SD;};break;
//			case 14:{SA_STATE=0;SB_STATE=1;SET_SC;CLR_SD;};break;
//			case 15:{SA_STATE=1;SB_STATE=1;SET_SC;CLR_SD;};break;
	   default: break;
		  
	 }
}
/**
�������ƣ�void delay_ms(u16 ms)
�������ã����ӳ�
����������ms:�ӳ�ѭ���ĸ���
�������أ���
�޸�˵����NA
**/
//static void delay_ms(u16 ms)//��ʱ����
//{
// u8 j;
// while(ms--)
// for(j=0;j<100;j++); 
//}
/**
�������ƣ�static void WriteByte(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
�������ã����ӳ�
����������dat1:��һ�е�����	 dat2:�ڶ��е�����	dat3:�����е�����  dat4:�����е�����
�������أ���
�޸�˵����NA
**/
static void WriteByte(u8 dat1,u8 dat2,u8 dat3)  //д��һ����
{ 
 u8 k;
 for(k=0;k<8;k++)						   
 {
  if(dat1&LED_BIT_MASK)						   //�������λ
  {
		R1_STATE= !System_Config_Table1.Led_Show_Type;   //�¿��
  }
  else
  {
		R1_STATE= System_Config_Table1.Led_Show_Type;
  }
  if(dat2&LED_BIT_MASK)
  {
		R2_STATE=!System_Config_Table1.Led_Show_Type;
  }
  else
  {
		R2_STATE = System_Config_Table1.Led_Show_Type;
  }
  if(dat3&LED_BIT_MASK)
  {
   R3_STATE = !System_Config_Table1.Led_Show_Type;
  }
  else
  {
   R3_STATE = System_Config_Table1.Led_Show_Type;
  }
#if CLK_POLARITY_LEVEL == CLK_POLARITY_POS_EDGE
  SH_STATE=1;									//CLK�������������ݵ�������
  __nop();
  __nop();
  __nop();
  SH_STATE=0;
#else
  SH_STATE=0;									//CLK�½����������ݵ�������
  __nop();
	__nop();
	__nop();
  SH_STATE=1;
#endif
  dat1=dat1 LED_SHIFT_OP 1;
  dat2=dat2 LED_SHIFT_OP 1;
  dat3=dat3 LED_SHIFT_OP 1;
 }
}

extern OS_EVENT *SEM_LEDCALLBACK;
void Led_Wait_Callback(void)
{
	OSSemPost(SEM_LEDCALLBACK);//�����ź���
}
/**
�������ƣ�void WriteDisplayTemp(void)
�������ã��ѻ������������ʾ��LED��������
������������
�������أ���
�޸�˵����NA
**/
void WriteDisplayTemp(void)			        //д��������
{
	u8 i;
	uint8_t err;
	for(i = 0 ; i < 4; i++)     //��Ļ 4*4
	{
#if MODULE_CONNECT_TYPE==MODULE_CONNECT_TYPE3
		  	//��һ���� ��һ��595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
				//																		[0.12][16.12][32.12][64.12]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(3 + 2*4)]);	  //��16��595���Ʋ���
		 	//����С��																[0.8]	[16.8] [32.8][64.8]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(2 + 2*4)]);	  //��15��595���Ʋ���
		 	//�ڶ�С��																[0.4]	[16.4] [32.4][64.4]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(1 + 2*4)]);	  //��14��595���Ʋ���
		 	//��һС��																[0.0]	[16.0] [32.0][64.0]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(0 + 2*4)]);	  //��13��595���Ʋ���
					 
			//��һ���� �ڶ���595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1	
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(3 + 2*4)]);	  //��12��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(2 + 2*4)]);	  //��11��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(1 + 2*4)]);	  //��10��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(0 + 2*4)]);	  //��9��595���Ʋ���
					 
			//��һ���� ������595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(3 + 2*4)]);	  //��8��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(2 + 2*4)]);	  //��7��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(1 + 2*4)]);	  //��6��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(0 + 2*4)]);	  //��5��595���Ʋ���
					 
			//��һ���� ���ĸ�595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(3 + 2*4)]);	  //��4��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(2 + 2*4)]);	  //��3��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(1 + 2*4)]);	  //��2��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(0 + 2*4)]);	  //��1��595���Ʋ���	
	
		 /********************************************/
		 	//�ڶ����� ��һ��595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(3 + 2*4)]);	  //��16��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(2 + 2*4)]);	  //��15��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(1 + 2*4)]);	  //��14��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(0 + 2*4)]);	  //��13��595���Ʋ���
			//�ڶ����� �ڶ���595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(3 + 2*4)]);	  //��12��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(2 + 2*4)]);	  //��11��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(1 + 2*4)]);	  //��10��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(0 + 2*4)]);	  //��9��595���Ʋ���
			//�ڶ����� ������595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(3 + 2*4)]);	  //��8��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(2 + 2*4)]);	  //��7��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(1 + 2*4)]);	  //��6��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(0 + 2*4)]);	  //��5��595���Ʋ���
			//�ڶ����� ���ĸ�595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(3 + 2*4)]);	  //��4��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(2 + 2*4)]);	  //��3��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(1 + 2*4)]);	  //��2��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(0 + 2*4)]);	  //��1��595���Ʋ���
		/********************************************/
		
				 //�������� ��һ��595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(3 + 2*4)]);	  //��16��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(2 + 2*4)]);	  //��15��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(1 + 2*4)]);	  //��14��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(0 + 2*4)]);	  //��13��595���Ʋ���
			//�������� �ڶ���595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(3 + 2*4)]);	  //��12��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(2 + 2*4)]);	  //��11��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(1 + 2*4)]);	  //��10��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(0 + 2*4)]);	  //��9��595���Ʋ���
			//�������� ������595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(3 + 2*4)]);	  //��8��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(2 + 2*4)]);	  //��7��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(1 + 2*4)]);	  //��6��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(0 + 2*4)]);	  //��5��595���Ʋ���
			//�������� ���ĸ�595 ����С��    ��һ����|�ڶ�����|��������|���Ĵ���		//������Ŵ��㿪ʼ����Ҫ��1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(3 + 2*4)]);	  //��4��595���Ʋ���
		 	//����С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(2 + 2*4)]);	  //��3��595���Ʋ���
		 	//�ڶ�С��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(1 + 2*4)]);	  //��2��595���Ʋ���
		 	//��һС��                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(0 + 2*4)]);	  //��1��595���Ʋ���

#endif
#if LT_POLARITY_LEVEL == LT_POLARITY_POS_EDGE
		 ST_STATE=0;									//ST_CP������������������
		 __nop();
		 __nop();
	   __nop();
		 ST_STATE=1;
#else
		 ST_STATE=1;									//ST_CP�½���������������
		 __nop();
		 __nop();
	   __nop();
		 ST_STATE=0;
#endif
		 scan(i);//��ɨ�� 
#if EN_POLARITY_LEVEL == EN_POLARITY_POSITIVE
		 OE_STATE=1;
		 bsp_StartHardTimer(1, 1000, (void *)Led_Wait_Callback);
		 OSSemPend(SEM_LEDCALLBACK, 0, &err);
		 //����OS���ӳٺ����ӳ���ʾ��ͬʱ����ִ�б������ע�⣬��ʾ�������ȼ�������
		 //�ӳٲ���С��ϵͳ�����л�ʱ��,�����Ҫ��С���ӳ٣�Ҫ�ö�ʱ����������ȴ��ź�������ʱ��post�ź���
//		 OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,NULL); //
		 //delay_ms(5);
		 OE_STATE=0;
#else
		 OE_STATE=0;
			//����OS���ӳٺ����ӳ���ʾ��ͬʱ����ִ�б������ע�⣬��ʾ�������ȼ�������
		 //�ӳٲ���С��ϵͳ�����л�ʱ��,�����Ҫ��С���ӳ٣�Ҫ�ö�ʱ����������ȴ��ź�������ʱ��post�ź���
//		 OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,NULL); //
		 bsp_StartHardTimer(1, 1000, (void *)Led_Wait_Callback);
		 OSSemPend(SEM_LEDCALLBACK, 0, &err);
		 //delay_ms(5);
		 OE_STATE=1;
#endif
ST_STATE=0;//�е�оƬʱ��涨���������Ҫ�ָ�����ŵ�ƽ
	}
}
/**
�������ƣ�void DisplayPoint(u8 x,u8 y)
�������ã�����������ʾһ����
����������x:X����;y:Y����
�������أ���
�޸�˵����NA
**/
void DisplayPoint(u8 x,u8 y)	  //����λ����ʾһ����
{
  	unsigned char data1,data2;  //data1��ǰ�������
	  //�����߶�λ�ô�С����x���ܹ����ڵ���Width��y���ܹ����ڵ���Height 
		data2 = DispTemp[y * (Width >>3 ) + (x>>3)]; 
		data1 = (~(0x80 >> (x & 7))) & data2; //����������㻺��λ��	
		DispTemp[y * (Width >>3 ) + (x>>3)]=data1;			  
}
/**
�������ƣ�void DisplayPointNow(u8 x,u8 y)
�������ã���������������ʾһ����
����������x:X����;y:Y����;
�������أ���
�޸�˵����NA
**/
void DisplayPointNow(u8 x,u8 y)	  //����λ��������ʾһ����
{
  	unsigned char data1,data2;  //data1��ǰ�������
		//�����߶�λ�ô�С����x���ܹ����ڵ���Width��y���ܹ����ڵ���Height 
		data2 = DispTemp[y * (Width >>3 ) + (x>>3)]; 
		data1 = (~(0x80 >> (x & 7))) & data2; //����������㻺��λ��	
		DispTemp[y * (Width >>3 ) + (x>>3)]=data1;
	
		WriteDisplayTemp();			  
}
/**
�������ƣ�void ClearDisplay(void)
�������ã������ʾ���沢����Ļ
������������
�������أ���
�޸�˵����NA
**/
void ClearDisplay(uint8_t mode)
{
	u32 i;
	for(i = 0 ; i < sizeof(DispTemp) ; i++)
	{
	 	 DispTemp[i] = 0xff;
	}
	if(mode)
	WriteDisplayTemp();

}

/**
�������ƣ�void ClearDisplayLine(u8 line)
�������ã������ʾ���沢����Ļ
����������line Ҫ�������
�������أ���
�޸�˵����NA
**/                                          
void ClearDisplayLine(u8 line)
{
	u32 i;
	for(i = 0 ; i < (Width>>3)*16 ; i++)
	{
	 	 DispTemp[line*(Width>>3) + i] = 0xff;
	}
	WriteDisplayTemp();
}


/**
�������ƣ�void FullDisplay(void)
�������ã�������ʾ���沢ȫ��Ļ��ʾ
������������
�������أ���
�޸�˵����NA
**/
void FullDisplay(void)
{
	u32 i;
	for(i = 0 ; i < sizeof(DispTemp) ; i++)
	{
	 	 DispTemp[i] = 0x00;
	}
	WriteDisplayTemp();

}
/**
�������ƣ�void StaticChineseDisplayTest(u8 *ch,u8 rows)
�������ã���̬��ʾAscii�ַ�
����������ch:�����ַ���;rows:�ڼ�����ʾ;
�������أ���
�޸�˵����NA
**/
void StaticAscllDisplay(u8 *ch,u8 rows)
{
	  u8 dat;
	  u16 i;
	  u16  count = 0;
	  u8 row_count = rows;
	  if(row_count < 4)
	  {
	  	while((*ch != '\0'))
		{		
			dat =*ch - 32;
			for(i = 0;i < 16;i++)               
			{
				DispTemp[i*(Width>>3) + count + row_count * 16 * (Width>>3)]=Ascll[16*dat+i];;
			}
			count++;
			if(count >= (Width>>3))
			{
			   count = 0;
			   row_count++;
			   if(row_count >= (Height >> 4))
			   {
			   		break;
			   }
			}
			ch++;
		}
		WriteDisplayTemp();
	  }
}



/////////////////////////////////////�����ǽӿں���///////////////////////////////////////////////////////////

/**
�������ƣ�void StaticStringDisplayInPos(u8 *ch, u8 x, u8 y)
�������ã���̬��ʾ����ַ�
����������ch:�ַ���;x:��ʼ��ʾ��x����;y:��ʼ��ʾ��y����;mode:�Ƿ�������ʾ 
�������أ���
�޸�˵����NA
**/
void StaticStringDisplayInPos(uint8_t *ch, uint8_t x, uint8_t y,uint8_t mode)
{
  u16 i;
	u16  count = x >> 3;
	u8 row_count = y;
	u8 hex[2];
	u8 dat;
	u8 idex;
	u8 count_max;
	if((row_count <= Line4) && (count <= Width >> 3))
	{
		while((*ch != '\0'))		  
		{
				if(*ch >= 0x80)
				{
					idex = 0;
					for(idex = 0 ; idex < CHINESE_REC_NUM ; idex++ )	 //ͨ�������ַ����������ҵ���Ҫ��ʾ���ֵ�λ��
					{
						if(DisplayRecordIdex[idex*2] == *ch)
						{
							if(DisplayRecordIdex[idex*2 + 1] == *(ch+1))
							{
								break;
							}
						}		
					}
					if(idex >= CHINESE_REC_NUM)
					{
						hex[0] = *ch;
						hex[1] = *(ch + 1);
						DisplayRecordIdex[DisplayRecordCurrentIdex * 2 + 0] = hex[0];
						DisplayRecordIdex[DisplayRecordCurrentIdex * 2 + 1] = hex[1];
						GT_FrontIC_ReadBytes(ChineseRecordData[DisplayRecordCurrentIdex] , GB2312_GetAddr(hex,FONT_TYPE_16) ,32 );
						idex = DisplayRecordCurrentIdex;
						DisplayRecordCurrentIdex++;
						if(DisplayRecordCurrentIdex >= CHINESE_REC_NUM)
						{
							DisplayRecordCurrentIdex = 0;
						}
					}
					for(i = 0; i < 16; i++)               
					{
						DispTemp[i*(Width>>3) + 0 + count + row_count * (Width>>3)] = ~ChineseRecordData[idex][i*2+0];
					} 
					for(i = 0; i < 16; i++)               
					{
						DispTemp[i*(Width>>3) + 1 + count + row_count * (Width>>3)] = ~ChineseRecordData[idex][i*2+1];
					} 
					count+=2;
					if(*(ch + 2) >= 0x80)
					{
						count_max = (Width>>3) - 1;
					}
					else
					{
						count_max = (Width>>3);
					}
					if(count >= count_max)
					{
						 count = 0;
						 row_count+=16;
						 if(row_count >= Height)
						 {
								break;
						 }
					}
					ch+= 2;
				}
				else//��ʾӢ��
				{
						dat =*ch - 32;
						for(i = 0;i < 16;i++)               
						{
							DispTemp[i*(Width>>3) + count + row_count * (Width>>3)]=Ascll[16*dat+i];
						}
						count++;
						if(*(ch + 2) >= 0x80)
						{
							count_max = (Width>>3) - 1;
						}
						else
						{
							count_max = (Width>>3);
						}
						if(count >= count_max)
						{
							 count = 0;
							 row_count+=16;
							 if(row_count >= Height)
							 {
									break;
							 }
						}
						ch++;
				}
		} 	
		if(mode)
		WriteDisplayTemp();
	}
}

/**
�������ƣ�void RollingStringDisplayInLine(u8 *ch,u8 y ,u8 dir,u16 speed)
�������ã�������ʾ����ַ���
����������ch:�ַ���;y:�ڼ��л�������ʾ;dir:��������0,��,1,��,2,��,3,��;speed:�����ٶ�,Խ��Խ��;
�������أ���
�޸�˵����NA
**/
void RollingStringDisplayInLine(u8 *ch,u8 y ,u8 dir,u16 speed)
{
  u16 i , j , n;
	u8 line = y;
	u8 hex[2];
	u8 dat;
	u8 idex;
	while((*ch != '\0'))		  
	{
			if(*ch >= 0x80)
			{
				idex = 0;
				for(idex = 0 ; idex < CHINESE_REC_NUM ; idex++ )	 //ͨ�������ַ����������ҵ���Ҫ��ʾ���ֵ�λ��
				{
					if(DisplayRecordIdex[idex*2] == *ch)
					{
						if(DisplayRecordIdex[idex*2 + 1] == *(ch+1))
						{
							break;
						}
					}		
				}
				if(idex >= CHINESE_REC_NUM)
				{
					hex[0] = *ch;
					hex[1] = *(ch + 1);
					DisplayRecordIdex[DisplayRecordCurrentIdex * 2 + 0] = hex[0];
					DisplayRecordIdex[DisplayRecordCurrentIdex * 2 + 1] = hex[1];
					GT_FrontIC_ReadBytes(ChineseRecordData[DisplayRecordCurrentIdex] , GB2312_GetAddr(hex,FONT_TYPE_16) ,32 );
					idex = DisplayRecordCurrentIdex;
					DisplayRecordCurrentIdex++;
					if(DisplayRecordCurrentIdex >= CHINESE_REC_NUM)
					{
						DisplayRecordCurrentIdex = 0;
					}
				}
				if(dir < 2)
				{
					for(i = 0;i < 16;i++)               
					{
						RollDispTem[line+i] = ~ChineseRecordData[idex][i*2+dir];//ͨ��dirȥ�ж�����˭���Ƴ�
					}
					for(j = 0 ; j < 8 ; j ++)
					{
							for(i = 0;i < 16;i++)               
							{
								if(dir == 0)
								{
									for(n = 0;n < (Width>>3) - 1;n++)
									{
										DispTemp[i*(Width>>3)+ n + line*(Width>>3)] = ((DispTemp[i*(Width>>3)+ n +line*(Width>>3)]<<1)
																																				|((DispTemp[i*(Width>>3)+ n+1 +line*(Width>>3)]>>7)& 0x01));
									}
									DispTemp[i*(Width>>3)+((Width>>3)-1)+line*(Width>>3)] = ((DispTemp[i*(Width>>3)+((Width>>3)-1)+line*(Width>>3)]<<1)
																																		|(((RollDispTem[line+i])>>(7 - j))& 0x01));
								}
								else if(dir == 1)
								{
									DispTemp[i*(Width>>3)+0+line*(Width>>3)] = ((DispTemp[i*(Width>>3)+0+line*(Width>>3)]>>1)
																																		|(((RollDispTem[line+i])<<(7-j))& 0x80));
									for(n = 1;n < (Width>>3);n++)
									{
										DispTemp[i*(Width>>3)+ n + line*(Width>>3)] = ((DispTemp[i*(Width>>3)+ n +line*(Width>>3)]>>1)
																																		|((DispTemp[i*(Width>>3)+ n-1 +line*(Width>>3)]<<7)& 0x80));
									}
								}
							}
							for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
							{
									WriteDisplayTemp();
							}
					}
					for(i = 0;i < 16;i++)               
					{
						RollDispTem[line+i] = ~ChineseRecordData[idex][i*2+(1-dir)];
					}
					for(j = 0 ; j < 8 ; j ++)
					{
							for(i = 0;i < 16;i++)               
							{
								if(dir == 0)
								{
									for(n = 0;n < (Width>>3) - 1;n++)
									{
										DispTemp[i*(Width>>3)+ n + line*(Width>>3)] = ((DispTemp[i*(Width>>3)+ n +line*(Width>>3)]<<1)
																																				|((DispTemp[i*(Width>>3)+ n+1 +line*(Width>>3)]>>7)& 0x01));
									}
									DispTemp[i*(Width>>3)+((Width>>3)-1)+line*(Width>>3)] = ((DispTemp[i*(Width>>3)+((Width>>3)-1)+line*(Width>>3)]<<1)
																																		|(((RollDispTem[line+i])>>(7 - j))& 0x01));
								}
								else if(dir == 1)
								{
									DispTemp[i*(Width>>3)+0+line*(Width>>3)] = ((DispTemp[i*(Width>>3)+0+line*(Width>>3)]>>1)
																																		|(((RollDispTem[line+i])<<(7-j))& 0x80));
									for(n = 1;n < (Width>>3);n++)
									{
										DispTemp[i*(Width>>3)+ n + line*(Width>>3)] = ((DispTemp[i*(Width>>3)+ n +line*(Width>>3)]>>1)
																																		|((DispTemp[i*(Width>>3)+ n-1 +line*(Width>>3)]<<7)& 0x80));
									}
								}
							}
							for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
							{
									WriteDisplayTemp();
							}
					}
				}
				else
				{
						for(i = 0;i < 16;i++)               
						{
							RollDispTem[((line+i)<<1)] = ~ChineseRecordData[idex][i*2+0];
							RollDispTem[((line+i)<<1)+1] = ~ChineseRecordData[idex][i*2+1];
						}
						if(dir == 2)
						{
								for(i = 0;i < 16; i++)
								{		
										for(n = Height - 1;n >= 1;n--)
										{
											DispTemp[(line>>3) +  n*(Width>>3)] = DispTemp[(line>>3) +  (n-1)*(Width>>3)];
											DispTemp[(line>>3) +  n*(Width>>3) + 1] = DispTemp[(line>>3) +  (n-1)*(Width>>3) + 1];
										}
										DispTemp[(line>>3) + 0*(Width>>3)] = RollDispTem[((line+(15 - i))<<1)];
										DispTemp[(line>>3) + 0*(Width>>3) + 1] = RollDispTem[((line+(15 - i))<<1)+1];
										RollDispTem[((line+(15 - i))<<1)] = RollDispTem[((line+(15 - i + 1))<<1)];
										RollDispTem[((line+(15 - i))<<1)+1] = RollDispTem[((line+(15 - i + 1))<<1)+1];
										for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
										{
												WriteDisplayTemp();
										}
								}
						}
						else
						{
							for(i = 0;i < 16; i++)
							{		
									for(n = 0;n < Height - 1;n++)
									{
										DispTemp[(line>>3) +  n*(Width>>3)] = DispTemp[(line>>3) +  (n+1)*(Width>>3)];
										DispTemp[(line>>3) +  n*(Width>>3) + 1] = DispTemp[(line>>3) +  (n+1)*(Width>>3) + 1];
									}
									DispTemp[(line>>3) + (Height - 1)*(Width>>3)] = RollDispTem[((line+(i))<<1)];
									DispTemp[(line>>3) + (Height - 1)*(Width>>3) + 1] = RollDispTem[((line+(i))<<1)+1];
									RollDispTem[((line+(i))<<1)] = RollDispTem[((line+(i + 1))<<1)];
									RollDispTem[((line+(i))<<1)+1] = RollDispTem[((line+(i + 1))<<1)+1];
									for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
									{
											WriteDisplayTemp();
									}
							}
						}	
				}
				ch+= 2;
			}
			else//��ʾӢ��
			{
					dat =*ch - 32;
					if(dir < 2)
					{
						for(i = 0;i < 16;i++)               
						{
							RollDispTem[line+i] = Ascll[16*dat+i];
						}
						for(j = 0 ; j < 8 ; j ++)
						{
								for(i = 0;i < 16;i++)               
								{
									if(dir == 0)
									{
										for(n = 0;n < (Width>>3) - 1;n++)
										{
											DispTemp[i*(Width>>3)+ n + line*(Width>>3)] = ((DispTemp[i*(Width>>3)+ n +line*(Width>>3)]<<1)
																																					|((DispTemp[i*(Width>>3)+ n+1 +line*(Width>>3)]>>7)& 0x01));
										}
										DispTemp[i*(Width>>3)+((Width>>3)-1)+line*(Width>>3)] = ((DispTemp[i*(Width>>3)+((Width>>3)-1)+line*(Width>>3)]<<1)
																																			|(((RollDispTem[line+i])>>(7 - j))& 0x01));
									}
									else if(dir == 1)
									{
										DispTemp[i*(Width>>3)+0+line*(Width>>3)] = ((DispTemp[i*(Width>>3)+0+line*(Width>>3)]>>1)
																																			|(((RollDispTem[line+i])<<(7-j))& 0x80));
										for(n = 1;n < (Width>>3);n++)
										{
											DispTemp[i*(Width>>3)+ n + line*(Width>>3)] = ((DispTemp[i*(Width>>3)+ n +line*(Width>>3)]>>1)
																																			|((DispTemp[i*(Width>>3)+ n-1 +line*(Width>>3)]<<7)& 0x80));
										}
									}
								}
								for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
								{
										WriteDisplayTemp();
								}
						}
					}
					else
					{
// 						for(i = 0;i < 16;i++)               
// 						{
// 							RollDispTem[((line+i)<<1)] = Ascll[16*dat+i];
// 							RollDispTem[((line+i)<<1)+1] = ~0x00;
// 						}
						for(i = 0;i < 16;i++)//������Ϊ��Ӣ���ܹ���ʾ�������м䣬���ǻ�����һ��������              
						{
							RollDispTem[((line+i)<<1)] = 0xf0|(Ascll[16*dat+i] >> 4);
							RollDispTem[((line+i)<<1)+1] = 0x0f | (Ascll[16*dat+i] << 4);
						}
						if(dir == 2)
						{
								for(i = 0;i < 16; i++)
								{		
										for(n = Height - 1;n >= 1;n--)
										{
											DispTemp[(line>>3) +  n*(Width>>3)] = DispTemp[(line>>3) +  (n-1)*(Width>>3)];
											DispTemp[(line>>3) +  n*(Width>>3) + 1] = DispTemp[(line>>3) +  (n-1)*(Width>>3) + 1];
										}
										DispTemp[(line>>3) + 0*(Width>>3)] = RollDispTem[((line+(15 - i))<<1)];
										DispTemp[(line>>3) + 0*(Width>>3) + 1] = RollDispTem[((line+(15 - i))<<1)+1];
										RollDispTem[((line+(15 - i))<<1)] = RollDispTem[((line+(15 - i + 1))<<1)];
										RollDispTem[((line+(15 - i))<<1)+1] = RollDispTem[((line+(15 - i + 1))<<1)+1];
										for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
										{
												WriteDisplayTemp();
										}
								}
						}
						else
						{
							for(i = 0;i < 16; i++)
							{		
									for(n = 0;n < Height - 1;n++)
									{
										DispTemp[(line>>3) +  n*(Width>>3)] = DispTemp[(line>>3) +  (n+1)*(Width>>3)];
										DispTemp[(line>>3) +  n*(Width>>3) + 1] = DispTemp[(line>>3) +  (n+1)*(Width>>3) + 1];
									}
									DispTemp[(line>>3) + (Height - 1)*(Width>>3)] = RollDispTem[((line+(i))<<1)];
									DispTemp[(line>>3) + (Height - 1)*(Width>>3) + 1] = RollDispTem[((line+(i))<<1)+1];
									RollDispTem[((line+(i))<<1)] = RollDispTem[((line+(i + 1))<<1)];
									RollDispTem[((line+(i))<<1)+1] = RollDispTem[((line+(i + 1))<<1)+1];
									for(n = 0;n < speed; n++)//��ʾ���ݴ�����ֵԽС�ƶ��ٶ�Խ��
									{
											WriteDisplayTemp();
									}
							}
						}
					}
					ch++;
			} 	
		}
}

/**
�������ƣ�void DisplayIntValue(u16 val , u8 x , u8 y , u8 type)
�������ã���ʾһ��16λ��С������
����������val:Ҫ��ʾ��ֵ;x:��ʼ��ʾ��x����;y:��ʼ��ʾ��y���� type:��ʾ��λ��
�������أ���
�޸�˵����NA
**/
//void DisplayIntValue(u16 val , u8 x , u8 y , u8 type)
//{
//	u8 data_tem[6];
//	u8 len = type&0x0f;
//	u8 zero_disp = type&0x80;
//	u8 i;
//	//u8 zero_count = 0;//�������λ����Ҫ�Ļ������òο����д����������������λ����������ÿ�
//	if(len < 6)
//	{
//		data_tem[5] = '\0';
//		data_tem[0] = val / 10000 + 48;
//		data_tem[1] = val / 1000 % 10 + 48;
//		data_tem[2] = val / 100 % 10 + 48;
//		data_tem[3] = val / 10 % 10 + 48;
//		data_tem[4] = val % 10 + 48;
//		if(!zero_disp)
//		{
//			for(i = (5 - len) ; i < 6 ; i++)
//			{
//					if(data_tem[i] == '0')
//					{
//						data_tem[i] = ' ';
//						//zero_count++;
//					}
//					else
//					{
//							break;
//					}
//			}
//		}
//		StaticStringDisplayInPos((data_tem + (5 - len)) , x , y);
//		//StaticStringDisplayInPos((data_tem + (5 - len + zero_count)) , x , y);
//	}
//}
/**
//�������ƣ�void DisplayFloatValue(float dat , u8 x , u8 y , u8 type)
//�������ã���ʾһ��С��ֻ����һλС��
//����������val:Ҫ��ʾ��ֵ;x:��ʼ��ʾ��x����;y:��ʼ��ʾ��y���� type:��ʾ��λ������С����
//�������أ���
//�޸�˵����NA
**/
//void DisplayFloatValue(float dat , u8 x , u8 y , u8 type)//ֻ����һλС��
//{
//  u8 data_tem[6];
//	u8 len = type&0x0f;
//	u8 zero_disp = type&0x80;
//	u8 i;
//	if(len < 6)
//	{
//		data_tem[5] = '\0';
//		dat*=10;

//		data_tem[4]=(short int)dat%10 + 48;
//		data_tem[3]= '.';
//		data_tem[2]=(short int)dat/10%10 + 48;
//		data_tem[1]=(short int)dat/100%10 + 48;
//		data_tem[0]=(short int)dat/1000%10 + 48;
//		
//		if(!zero_disp)
//		{
//			for(i = (5 - len) ; i < 6 ; i++)
//			{
//					if((data_tem[i] == '0')&&(data_tem[i+1] != '.'))//�����ǰ��ʾ��Ϊ�������һ����ʾ�Ĳ���С���㣬����ʾ��
//					{
//						data_tem[i] = ' ';
//					}
//					else
//					{
//							break;
//					}
//			}
//		}
//		
//		StaticStringDisplayInPos((data_tem + (5 - type)) , x , y);
//	}
//}

///**
//�������ƣ�void DisplayStringAndIntValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
//�������ã���ʾ�����Ĳ���
//����������str1:��������val:Ҫ��ʾ��ֵ;col:��ʼ��ʾ��x����;row:��ʼ��ʾ��y���� type:��ʾ��λ��;str2:��λ�ַ���
//�������أ���
//�޸�˵����NA
//**/
//void DisplayStringAndIntValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
//{
//	u8 len = 0;
//	u16 posx, posy;
//	posx = col;
//	posy = row;
//	while(*(str1+(++len)) != '\0');//�����ַ����ĳ���
//	StaticStringDisplayInPos(str1, posx , posy);
//	posx += len*8;
//	if(posx >= Width)
//	{
//		posx = 0;
//		posy++;
//	}
//	DisplayIntValue(val,posx,posy,type);
//	posx += type*8;
//	if(posx >= Width)
//	{
//		posx = 0;
//		posy++;
//	}
//	StaticStringDisplayInPos(str2, posx , posy);
//}

/**
�������ƣ�void DisplayStringAndFloatValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
�������ã���ʾ�����С���Ĳ���
����������str1:��������val:Ҫ��ʾ��ֵ;col:��ʼ��ʾ��x����;row:��ʼ��ʾ��y���� type:��ʾ��λ��;str2:��λ�ַ���
�������أ���
�޸�˵����NA
**/
//void DisplayStringAndFloatValue(u8 *str1,float val , u8 col , u8 row , u8 type , u8 *str2)
//{
//	u8 len = 0;
//	u16 posx, posy;
//	posx = col;
//	posy = row;
//	while(*(str1+(++len)) != '\0');//�����ַ����ĳ���
//	StaticStringDisplayInPos(str1, posx , posy);
//	posx += len*8;
//	if(posx >= Width)
//	{
//		posx = 0;
//		posy++;
//	}
//	DisplayFloatValue(val,posx,posy,type);
//	posx += type*8;
//	if(posx >= Width)
//	{
//		posx = 0;
//		posy++;
//	}
//	StaticStringDisplayInPos(str2, posx , posy);
//}

/**
�������ƣ�void DisplayStringAndFloatValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
�������ã���ʾ�����С���Ĳ���
����������str1:��������val:Ҫ��ʾ��ֵ;col:��ʼ��ʾ��x����;row:��ʼ��ʾ��y���� type:��ʾ��λ��;str2:��λ�ַ���
�������أ���
�޸�˵����NA
**/
//void DisplayStringAndTime(u16 year,u8 month , u8 date , u8 hour , u8 min , u8 sec , u8 weak ,u8 y)
//{
//	u16 years = year;
//	u8 posy = y;
//	if(years < 200)
//	{
//		years += 2000;
//	}
//	StaticStringDisplayInPos("-", 40, posy);
//	StaticStringDisplayInPos("-", 64, posy);
//	//StaticStringDisplayInPos("������", 24, posy + 16);
//	StaticStringDisplayInPos(":", 32, posy + 32);
//	StaticStringDisplayInPos(":", 56, posy + 32);
//	DisplayIntValue(years,8,posy,4|DisplayZero);
//	DisplayIntValue(month,48,posy,2|DisplayZero);
//	DisplayIntValue(date,72,posy,2|DisplayZero);
//	
//	DisplayIntValue(hour,16,posy + 32,2|DisplayZero);
//	DisplayIntValue(min,40,posy + 32,2|DisplayZero);
//	DisplayIntValue(sec,64,posy + 32,2|DisplayZero);
//	switch(weak)
//	{
//			case 0:
//			case 7:	StaticStringDisplayInPos("������", 24, posy + 16); break;
//			case 1: StaticStringDisplayInPos("����һ", 24, posy + 16); break;
//			case 2: StaticStringDisplayInPos("���ڶ�", 24, posy + 16); break;
//			case 3: StaticStringDisplayInPos("������", 24, posy + 16); break;
//			case 4: StaticStringDisplayInPos("������", 24, posy + 16); break;
//			case 5: StaticStringDisplayInPos("������", 24, posy + 16); break;
//			case 6: StaticStringDisplayInPos("������", 24, posy + 16); break;
//			default: StaticStringDisplayInPos("���ڡ�", 24, posy + 16); break;
//	}
//}
//����������ֵweek
//����ַ���buff
void Get_Week(uint8_t week,uint8_t* buff)
{
	switch(week)
	{
			case 0:
			case 7:	 memcpy(buff,"������",sizeof("������")); break;
			case 1:  memcpy(buff,"����һ",sizeof("����һ")); break;
			case 2:  memcpy(buff,"���ڶ�",sizeof("���ڶ�")); break;
			case 3:  memcpy(buff,"������",sizeof("������")); break;
			case 4:  memcpy(buff,"������",sizeof("������")); break;
			case 5:  memcpy(buff,"������",sizeof("������")); break;
			case 6:  memcpy(buff,"������",sizeof("������")); break;
			default: break;
	}
}
