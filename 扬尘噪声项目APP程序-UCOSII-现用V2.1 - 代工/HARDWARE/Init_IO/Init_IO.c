
#include "Init_IO.h"
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PC端口时钟
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;							//LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 			//推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 				//根据设定参数初始化GPIOC.13
 GPIO_ResetBits(GPIOA,GPIO_Pin_15);						 						//PC.13 输出高
 GPIO_SetBits(GPIOA,GPIO_Pin_15);						 						//PC.13 输出高
}
////初始化继电器端口  PC0\PC1\PC2
//void RELAY_Init(void)
//{
// 
// GPIO_InitTypeDef  GPIO_InitStructure;
// 	
// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 						//使能PC端口时钟
//	
// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;		//
// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 									//推挽输出
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;									//IO口速度为50MHz
// GPIO_Init(GPIOC, &GPIO_InitStructure);					 										//根据设定参数初始化GPIOC.0
// GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);						//PC.0  1  2 输出低
//}
 
