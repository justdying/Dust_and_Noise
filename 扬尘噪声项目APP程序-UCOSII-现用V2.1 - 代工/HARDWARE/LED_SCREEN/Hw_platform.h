#ifndef _HW_PLATFORM_
#define _HW_PLATFORM_

#define SCAN_4LINE 1
#define SCAN_8LINE 2
#define SCAN_16LINE 3

#define LED_DATA_LSB 0
#define LED_DATA_MSB 1
#define LED_DATA_TYPE LED_DATA_LSB	

#if LED_DATA_TYPE == LED_DATA_LSB
#define LED_BIT_MASK 0x80
#define LED_SHIFT_OP <<
#else
#define LED_BIT_MASK 0x01
#define LED_SHIFT_OP >>
#endif


#define SCAN_TYPE SCAN_4LINE

#define MODULE_CONNECT_TYPE1  1		//
#define MODULE_CONNECT_TYPE2  2
#define MODULE_CONNECT_TYPE3  3
//#define MODULE_CONNECT_TYPE4  4
#define MODULE_CONNECT_TYPE MODULE_CONNECT_TYPE3

#define CLK_POLARITY_NEG_EDGE 1
#define CLK_POLARITY_POS_EDGE 2
#define CLK_POLARITY_LEVEL	   CLK_POLARITY_POS_EDGE

#define LT_POLARITY_NEG_EDGE 1
#define LT_POLARITY_POS_EDGE 2
#define LT_POLARITY_LEVEL	   LT_POLARITY_POS_EDGE

#define EN_POLARITY_NEGATIVE 1
#define EN_POLARITY_POSITIVE 2
#define EN_POLARITY_LEVEL	   EN_POLARITY_POSITIVE

//硬件连接定义
#define SA_PORT		GPIOC			//PA0
#define SA_PIN		GPIO_Pin_1
#define SA_PIN_DIR  GPIO_Mode_Out_PP
#define SA_PIN_CLK  RCC_APB2Periph_GPIOC

#define SB_PORT		GPIOC			//PA1
#define SB_PIN		GPIO_Pin_2
#define SB_PIN_DIR  GPIO_Mode_Out_PP
#define SB_PIN_CLK  RCC_APB2Periph_GPIOC

//#define SC_PORT		GPIOB
//#define SC_PIN		GPIO_Pin_3
//#define SC_PIN_DIR  GPIO_Mode_Out_PP
//#define SC_PIN_CLK  RCC_APB2Periph_GPIOB
//
//#define SD_PORT		GPIOB
//#define SD_PIN		GPIO_Pin_4
//#define SD_PIN_DIR  GPIO_Mode_Out_PP
//#define SD_PIN_CLK  RCC_APB2Periph_GPIOB

#define R1_PORT		GPIOC				//PC4	
#define R1_PIN		GPIO_Pin_4
#define R1_PIN_DIR  GPIO_Mode_Out_PP
#define R1_PIN_CLK  RCC_APB2Periph_GPIOC

#define R2_PORT		GPIOC			//PC5
#define R2_PIN		GPIO_Pin_5
#define R2_PIN_DIR  GPIO_Mode_Out_PP
#define R2_PIN_CLK  RCC_APB2Periph_GPIOC

#define R3_PORT		GPIOB			//PB8
#define R3_PIN		GPIO_Pin_0
#define R3_PIN_DIR  GPIO_Mode_Out_PP
#define R3_PIN_CLK  RCC_APB2Periph_GPIOB

#define R4_PORT		GPIOB			//PC9
#define R4_PIN		GPIO_Pin_1
#define R4_PIN_DIR  GPIO_Mode_Out_PP
#define R4_PIN_CLK  RCC_APB2Periph_GPIOB

#define SH_PORT		GPIOC			//PC3   S1
#define SH_PIN		GPIO_Pin_3
#define SH_PIN_DIR  GPIO_Mode_Out_PP
#define SH_PIN_CLK  RCC_APB2Periph_GPIOC

#define ST_PORT		GPIOA			//PA0    L1
#define ST_PIN		GPIO_Pin_0
#define ST_PIN_DIR  GPIO_Mode_Out_PP
#define ST_PIN_CLK  RCC_APB2Periph_GPIOA

#define OE_PORT		GPIOC			//PC0
#define OE_PIN		GPIO_Pin_0
#define OE_PIN_DIR  GPIO_Mode_Out_PP
#define OE_PIN_CLK  RCC_APB2Periph_GPIOC

#define OE_STATE PCout(0)// PC3
#define SH_STATE PCout(3)// PC4
#define ST_STATE PAout(0)// PC5
#define R1_STATE PCout(4)// PC6
#define R2_STATE PCout(5)// PC7
#define R3_STATE PBout(0)// PC8
#define R4_STATE PBout(1)// PC9
#define SA_STATE PCout(1)// PA0
#define SB_STATE PCout(2)// PA1

//#define SA_STATE=0	GPIO_ResetBits(SA_PORT, SA_PIN) //SA=0;
//#define SA_STATE=1	GPIO_SetBits(SA_PORT, SA_PIN)	  //SA=1;
//#define SB_STATE=0	GPIO_ResetBits(SB_PORT, SB_PIN) //SB=0;
//#define SB_STATE=1	GPIO_SetBits(SB_PORT, SB_PIN)	  //SB=1;
//#define CLR_SC	GPIO_ResetBits(SC_PORT, SC_PIN) //SC=0;
//#define SET_SC	GPIO_SetBits(SC_PORT, SC_PIN)  //SC=1;
//#define CLR_SD	GPIO_ResetBits(SC_PORT, SD_PIN) //SD=0;
//#define SET_SD	GPIO_SetBits(SC_PORT, SD_PIN)	  //SD=1;
//#define R1_STATE=0	GPIO_ResetBits(R1_PORT, R1_PIN) //R1=0;
//#define R1_STATE=1	GPIO_SetBits(R1_PORT, R1_PIN)	  //R1=1;
//#define R2_STATE=0	GPIO_ResetBits(R2_PORT, R2_PIN) //R2=0;
//#define R2_STATE=1	GPIO_SetBits(R2_PORT, R2_PIN)	  //R2=1;
//#define R3_STATE=0	GPIO_ResetBits(R3_PORT, R3_PIN) //R3=0;
//#define R3_STATE=1	GPIO_SetBits(R3_PORT, R3_PIN)	  //R3=1;
//#define R4_STATE=0	GPIO_ResetBits(R4_PORT, R4_PIN) //R4=0;
//#define R4_STATE=1	GPIO_SetBits(R4_PORT, R4_PIN)	  //R4=1;
//#define SH_STATE=0	GPIO_ResetBits(SH_PORT, SH_PIN) //SH=0;
//#define SH_STATE=1	GPIO_SetBits(SH_PORT, SH_PIN)	  //SH=1;
//#define ST_STATE=0	GPIO_ResetBits(ST_PORT, ST_PIN) //ST=0;
//#define ST_STATE=1	GPIO_SetBits(ST_PORT, ST_PIN)	  //ST=1;
//#define OE_STATE=0	GPIO_ResetBits(OE_PORT, OE_PIN) 
//#define OE_STATE=1	GPIO_SetBits(OE_PORT, OE_PIN)



#endif
