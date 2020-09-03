/**
代码文件：LED_MATRIX.c
程序名称：全彩LED点阵屏驱动
程序内容：全彩LED点阵屏显示驱动程序
程序编写：7426笨蛋
编写日期：2017年07月
修改记录：NA
**/	
#include "sys.h"
#include "LED_MATRIX.h"
#include "LED_CODE.h"
#include "Hw_platform.h" 
#include "GT21L16S2W.h"
#include "string.h"
#include "delay.h"
#include "modbus_host.h"
#include "includes.h"					//ucos 使用	
#define CHINESE_REC_NUM	64

u8 DispTemp[Height*(Width>>3)]={0};//显示缓存数组
u8 RollDispTem[Height*2+2];//用作左右滚动缓存
unsigned char ChineseRecordData[CHINESE_REC_NUM][32];//显示字符字模数据缓存
unsigned char DisplayRecordIdex[CHINESE_REC_NUM*2 + 2];//显示字符索引缓存,优先查找缓存里的字符
unsigned char DisplayRecordCurrentIdex = 0;
//I/O口初始化
void LED_Gpio_Int(void)
{
  GPIO_InitTypeDef GPIO_Output_diy;//定义I/O口设置结构体
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE,ENABLE);//开GPIO的时钟

  GPIO_Output_diy.GPIO_Speed=GPIO_Speed_50MHz;	//IO口输出最大速率
  GPIO_Output_diy.GPIO_Mode = GPIO_Mode_Out_PP;		    // 设置为推挽输出

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
//坐标说明：0———————————>Width
//			|
//			|
//		   Hight
/**
函数名称：void scan(u8 i)
函数作用：LED点阵行扫描
函数参数：i:要扫描的行数
函数返回：无
修改说明：NA
**/
static void scan(u8 i)	//行扫描函数
{
	 switch(i)
	 {
			//行扫描函数 4扫
	    case 0: {SA_STATE=0;SB_STATE=0;};break;
			case 1: {SA_STATE=1;SB_STATE=0;};break;
			case 2: {SA_STATE=0;SB_STATE=1;};break;
			case 3: {SA_STATE=1;SB_STATE=1;};break;
	 		//行扫描函数 8扫
//	        case 0: {SA_STATE=0;SB_STATE=0;CLR_SC;};break;
//			case 1: {SA_STATE=1;SB_STATE=0;CLR_SC;};break;
//			case 2: {SA_STATE=0;SB_STATE=1;CLR_SC;};break;
//			case 3: {SA_STATE=1;SB_STATE=1;CLR_SC;};break;
//			case 4: {SA_STATE=0;SB_STATE=0;SET_SC;};break;
//			case 5: {SA_STATE=1;SB_STATE=0;SET_SC;};break;
//			case 6: {SA_STATE=0;SB_STATE=1;SET_SC;};break;
//			case 7: {SA_STATE=1;SB_STATE=1;SET_SC;};break;
			//行扫描函数 16扫
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
函数名称：void delay_ms(u16 ms)
函数作用：简单延迟
函数参数：ms:延迟循环的个数
函数返回：无
修改说明：NA
**/
//static void delay_ms(u16 ms)//延时函数
//{
// u8 j;
// while(ms--)
// for(j=0;j<100;j++); 
//}
/**
函数名称：static void WriteByte(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
函数作用：简单延迟
函数参数：dat1:第一行的数据	 dat2:第二行的数据	dat3:第三行的数据  dat4:第四行的数据
函数返回：无
修改说明：NA
**/
static void WriteByte(u8 dat1,u8 dat2,u8 dat3)  //写入一数据
{ 
 u8 k;
 for(k=0;k<8;k++)						   
 {
  if(dat1&LED_BIT_MASK)						   //先送最高位
  {
		R1_STATE= !System_Config_Table1.Led_Show_Type;   //新款的
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
  SH_STATE=1;									//CLK上升沿送入数据到锁存器
  __nop();
  __nop();
  __nop();
  SH_STATE=0;
#else
  SH_STATE=0;									//CLK下降沿送入数据到锁存器
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
	OSSemPost(SEM_LEDCALLBACK);//发送信号量
}
/**
函数名称：void WriteDisplayTemp(void)
函数作用：把缓存里的数据显示到LED点阵屏上
函数参数：无
函数返回：无
修改说明：NA
**/
void WriteDisplayTemp(void)			        //写缓存命令
{
	u8 i;
	uint8_t err;
	for(i = 0 ; i < 4; i++)     //屏幕 4*4
	{
#if MODULE_CONNECT_TYPE==MODULE_CONNECT_TYPE3
		  	//第一块屏 第一个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
				//																		[0.12][16.12][32.12][64.12]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(3 + 2*4)]);	  //第16个595控制部分
		 	//第三小行																[0.8]	[16.8] [32.8][64.8]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(2 + 2*4)]);	  //第15个595控制部分
		 	//第二小行																[0.4]	[16.4] [32.4][64.4]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(1 + 2*4)]);	  //第14个595控制部分
		 	//第一小行																[0.0]	[16.0] [32.0][64.0]
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 0 + 48*(0 + 2*4)]);	  //第13个595控制部分
					 
			//第一块屏 第二个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1	
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(3 + 2*4)]);	  //第12个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(2 + 2*4)]);	  //第11个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(1 + 2*4)]);	  //第10个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 1 + 48*(0 + 2*4)]);	  //第9个595控制部分
					 
			//第一块屏 第三个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(3 + 2*4)]);	  //第8个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(2 + 2*4)]);	  //第7个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(1 + 2*4)]);	  //第6个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 2 + 48*(0 + 2*4)]);	  //第5个595控制部分
					 
			//第一块屏 第四个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(3 + 2*4)]);	  //第4个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(2 + 2*4)]);	  //第3个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(1 + 2*4)]);	  //第2个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 0*4 + 3 + 48*(0 + 2*4)]);	  //第1个595控制部分	
	
		 /********************************************/
		 	//第二块屏 第一个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(3 + 2*4)]);	  //第16个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(2 + 2*4)]);	  //第15个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(1 + 2*4)]);	  //第14个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 0 + 48*(0 + 2*4)]);	  //第13个595控制部分
			//第二块屏 第二个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(3 + 2*4)]);	  //第12个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(2 + 2*4)]);	  //第11个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(1 + 2*4)]);	  //第10个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 1 + 48*(0 + 2*4)]);	  //第9个595控制部分
			//第二块屏 第三个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(3 + 2*4)]);	  //第8个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(2 + 2*4)]);	  //第7个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(1 + 2*4)]);	  //第6个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 2 + 48*(0 + 2*4)]);	  //第5个595控制部分
			//第二块屏 第四个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(3 + 2*4)]);	  //第4个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(2 + 2*4)]);	  //第3个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(1 + 2*4)]);	  //第2个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 1*4 + 3 + 48*(0 + 2*4)]);	  //第1个595控制部分
		/********************************************/
		
				 //第三块屏 第一个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(3 + 2*4)]);	  //第16个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(2 + 2*4)]);	  //第15个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(1 + 2*4)]);	  //第14个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 0 + 48*(0 + 2*4)]);	  //第13个595控制部分
			//第三块屏 第二个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(3 + 2*4)]);	  //第12个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(2 + 2*4)]);	  //第11个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(1 + 2*4)]);	  //第10个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 1 + 48*(0 + 2*4)]);	  //第9个595控制部分
			//第三块屏 第三个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(3 + 2*4)]);	  //第8个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(2 + 2*4)]);	  //第7个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(1 + 2*4)]);	  //第6个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 2 + 48*(0 + 2*4)]);	  //第5个595控制部分
			//第三块屏 第四个595 第四小行    第一大行|第二大行|第三大行|第四大行		//所有序号从零开始，需要减1
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(3 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(3 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(3 + 2*4)]);	  //第4个595控制部分
		 	//第三小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(2 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(2 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(2 + 2*4)]);	  //第3个595控制部分
		 	//第二小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(1 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(1 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(1 + 2*4)]);	  //第2个595控制部分
		 	//第一小行                                                                           
		 WriteByte(DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(0 + 0*4)],DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(0 + 1*4)],
		 		   DispTemp[(Width>>3)*i + 2*4 + 3 + 48*(0 + 2*4)]);	  //第1个595控制部分

#endif
#if LT_POLARITY_LEVEL == LT_POLARITY_POS_EDGE
		 ST_STATE=0;									//ST_CP上升沿锁存的数据输出
		 __nop();
		 __nop();
	   __nop();
		 ST_STATE=1;
#else
		 ST_STATE=1;									//ST_CP下降沿锁存的数据输出
		 __nop();
		 __nop();
	   __nop();
		 ST_STATE=0;
#endif
		 scan(i);//行扫描 
#if EN_POLARITY_LEVEL == EN_POLARITY_POSITIVE
		 OE_STATE=1;
		 bsp_StartHardTimer(1, 1000, (void *)Led_Wait_Callback);
		 OSSemPend(SEM_LEDCALLBACK, 0, &err);
		 //调用OS的延迟函数延迟显示，同时可以执行别的任务，注意，显示任务优先级最好设高
		 //延迟不能小于系统任务切换时间,如果需要更小的延迟，要用定时器，在这里等待信号量，定时器post信号量
//		 OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,NULL); //
		 //delay_ms(5);
		 OE_STATE=0;
#else
		 OE_STATE=0;
			//调用OS的延迟函数延迟显示，同时可以执行别的任务，注意，显示任务优先级最好设高
		 //延迟不能小于系统任务切换时间,如果需要更小的延迟，要用定时器，在这里等待信号量，定时器post信号量
//		 OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,NULL); //
		 bsp_StartHardTimer(1, 1000, (void *)Led_Wait_Callback);
		 OSSemPend(SEM_LEDCALLBACK, 0, &err);
		 //delay_ms(5);
		 OE_STATE=1;
#endif
ST_STATE=0;//有的芯片时序规定数据输出后要恢复锁存脚电平
	}
}
/**
函数名称：void DisplayPoint(u8 x,u8 y)
函数作用：根据坐标显示一个点
函数参数：x:X坐标;y:Y坐标
函数返回：无
修改说明：NA
**/
void DisplayPoint(u8 x,u8 y)	  //任意位置显示一个点
{
  	unsigned char data1,data2;  //data1当前点的数据
	  //调用者对位置大小负责，x不能够大于等于Width，y不能够大于等于Height 
		data2 = DispTemp[y * (Width >>3 ) + (x>>3)]; 
		data1 = (~(0x80 >> (x & 7))) & data2; //根据坐标计算缓存位置	
		DispTemp[y * (Width >>3 ) + (x>>3)]=data1;			  
}
/**
函数名称：void DisplayPointNow(u8 x,u8 y)
函数作用：根据坐标马上显示一个点
函数参数：x:X坐标;y:Y坐标;
函数返回：无
修改说明：NA
**/
void DisplayPointNow(u8 x,u8 y)	  //任意位置马上显示一个点
{
  	unsigned char data1,data2;  //data1当前点的数据
		//调用者对位置大小负责，x不能够大于等于Width，y不能够大于等于Height 
		data2 = DispTemp[y * (Width >>3 ) + (x>>3)]; 
		data1 = (~(0x80 >> (x & 7))) & data2; //根据坐标计算缓存位置	
		DispTemp[y * (Width >>3 ) + (x>>3)]=data1;
	
		WriteDisplayTemp();			  
}
/**
函数名称：void ClearDisplay(void)
函数作用：清除显示缓存并清屏幕
函数参数：无
函数返回：无
修改说明：NA
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
函数名称：void ClearDisplayLine(u8 line)
函数作用：清除显示缓存并清屏幕
函数参数：line 要清除的行
函数返回：无
修改说明：NA
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
函数名称：void FullDisplay(void)
函数作用：填满显示缓存并全屏幕显示
函数参数：无
函数返回：无
修改说明：NA
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
函数名称：void StaticChineseDisplayTest(u8 *ch,u8 rows)
函数作用：静态显示Ascii字符
函数参数：ch:中文字符串;rows:第几行显示;
函数返回：无
修改说明：NA
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



/////////////////////////////////////以下是接口函数///////////////////////////////////////////////////////////

/**
函数名称：void StaticStringDisplayInPos(u8 *ch, u8 x, u8 y)
函数作用：静态显示混合字符
函数参数：ch:字符串;x:起始显示的x坐标;y:起始显示的y坐标;mode:是否立即显示 
函数返回：无
修改说明：NA
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
					for(idex = 0 ; idex < CHINESE_REC_NUM ; idex++ )	 //通过索引字符串数组来找到需要显示汉字的位置
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
				else//显示英文
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
函数名称：void RollingStringDisplayInLine(u8 *ch,u8 y ,u8 dir,u16 speed)
函数作用：滚动显示混合字符串
函数参数：ch:字符串;y:第几行或者列显示;dir:滚动方向0,左,1,右,2,上,3,下;speed:滚动速度,越大越慢;
函数返回：无
修改说明：NA
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
				for(idex = 0 ; idex < CHINESE_REC_NUM ; idex++ )	 //通过索引字符串数组来找到需要显示汉字的位置
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
						RollDispTem[line+i] = ~ChineseRecordData[idex][i*2+dir];//通过dir去判断左右谁先移出
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
							for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
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
							for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
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
										for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
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
									for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
									{
											WriteDisplayTemp();
									}
							}
						}	
				}
				ch+= 2;
			}
			else//显示英文
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
								for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
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
						for(i = 0;i < 16;i++)//这里是为了英文能够显示在中文中间，但是会增加一点运算量              
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
										for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
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
									for(n = 0;n < speed; n++)//显示数据次数，值越小移动速度越快
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
函数名称：void DisplayIntValue(u16 val , u8 x , u8 y , u8 type)
函数作用：显示一个16位大小的整数
函数参数：val:要显示的值;x:起始显示的x坐标;y:起始显示的y坐标 type:显示的位数
函数返回：无
修改说明：NA
**/
//void DisplayIntValue(u16 val , u8 x , u8 y , u8 type)
//{
//	u8 data_tem[6];
//	u8 len = type&0x0f;
//	u8 zero_disp = type&0x80;
//	u8 i;
//	//u8 zero_count = 0;//如果连空位都不要的话可以用参考这个写法，不过建议留空位，这样对齐好看
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
//函数名称：void DisplayFloatValue(float dat , u8 x , u8 y , u8 type)
//函数作用：显示一个小数只保留一位小数
//函数参数：val:要显示的值;x:起始显示的x坐标;y:起始显示的y坐标 type:显示的位数包括小数点
//函数返回：无
//修改说明：NA
**/
//void DisplayFloatValue(float dat , u8 x , u8 y , u8 type)//只保留一位小数
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
//					if((data_tem[i] == '0')&&(data_tem[i+1] != '.'))//如果当前显示的为零而且下一个显示的不是小数点，就显示空
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
//函数名称：void DisplayStringAndIntValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
//函数作用：显示整数的参数
//函数参数：str1:参数名称val:要显示的值;col:起始显示的x坐标;row:起始显示的y坐标 type:显示的位数;str2:单位字符串
//函数返回：无
//修改说明：NA
//**/
//void DisplayStringAndIntValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
//{
//	u8 len = 0;
//	u16 posx, posy;
//	posx = col;
//	posy = row;
//	while(*(str1+(++len)) != '\0');//计算字符串的长度
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
函数名称：void DisplayStringAndFloatValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
函数作用：显示浮点带小数的参数
函数参数：str1:参数名称val:要显示的值;col:起始显示的x坐标;row:起始显示的y坐标 type:显示的位数;str2:单位字符串
函数返回：无
修改说明：NA
**/
//void DisplayStringAndFloatValue(u8 *str1,float val , u8 col , u8 row , u8 type , u8 *str2)
//{
//	u8 len = 0;
//	u16 posx, posy;
//	posx = col;
//	posy = row;
//	while(*(str1+(++len)) != '\0');//计算字符串的长度
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
函数名称：void DisplayStringAndFloatValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2)
函数作用：显示浮点带小数的参数
函数参数：str1:参数名称val:要显示的值;col:起始显示的x坐标;row:起始显示的y坐标 type:显示的位数;str2:单位字符串
函数返回：无
修改说明：NA
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
//	//StaticStringDisplayInPos("星期日", 24, posy + 16);
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
//			case 7:	StaticStringDisplayInPos("星期日", 24, posy + 16); break;
//			case 1: StaticStringDisplayInPos("星期一", 24, posy + 16); break;
//			case 2: StaticStringDisplayInPos("星期二", 24, posy + 16); break;
//			case 3: StaticStringDisplayInPos("星期三", 24, posy + 16); break;
//			case 4: StaticStringDisplayInPos("星期四", 24, posy + 16); break;
//			case 5: StaticStringDisplayInPos("星期五", 24, posy + 16); break;
//			case 6: StaticStringDisplayInPos("星期六", 24, posy + 16); break;
//			default: StaticStringDisplayInPos("星期×", 24, posy + 16); break;
//	}
//}
//输入星期数值week
//输出字符串buff
void Get_Week(uint8_t week,uint8_t* buff)
{
	switch(week)
	{
			case 0:
			case 7:	 memcpy(buff,"星期日",sizeof("星期日")); break;
			case 1:  memcpy(buff,"星期一",sizeof("星期一")); break;
			case 2:  memcpy(buff,"星期二",sizeof("星期二")); break;
			case 3:  memcpy(buff,"星期三",sizeof("星期三")); break;
			case 4:  memcpy(buff,"星期四",sizeof("星期四")); break;
			case 5:  memcpy(buff,"星期五",sizeof("星期五")); break;
			case 6:  memcpy(buff,"星期六",sizeof("星期六")); break;
			default: break;
	}
}
