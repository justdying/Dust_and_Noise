#ifndef __GT21L16S2W_H__
#define __GT21L16S2W_H__

#define 	ASC0808D2HZ_ADDR    ( 0x66c0 )      //7*8 ascii code   
#define 	ASC0812M2ZF_ADDR 	( 0x66d40 )        //6*12 ascii code   
#define 	GBEX0816ZF_ADDR  	243648             //8*16 ascii code   
   
#define 	ZF1112B2ZF_ADDR 	( 0x3cf80 )         //12*12 12点字符   
#define 	HZ1112B2HZ_ADDR  	( 0x3cf80+376*24 ) //12*12 12点汉字   
   
   
#define 	CUTS1516ZF_ADDR  	0x00               //16*16 16点字符   
#define 	JFLS1516HZ_ADDR  	27072              //16*16 16点汉字   
      
#define 	ASCII0507ZF_ADDR 	245696   
#define 	ARIAL_16B0_ADDR  	246464   
#define 	ARIAL_12B0_ADDR  	422720   
#define 	SPAC1616_ADDR    	425264   
#define   	GB2311ToUnicode_addr    (12032)   
#define   	UnicodeToGB2311_addr    (425328)     
// 要显示的汉字的类型     
#define   FONT_TYPE_8    0 //8  点字符   
#define   FONT_TYPE_12   1 //12 点汉字字符   
#define   FONT_TYPE_16   2 //16 点汉字字符   

#define DummyByte	0xff

//GT21L16S2W字库芯片引脚定义 

#define GT_SPI_CS_PORT				GPIOA
#define GT_SPI_CS_PIN					GPIO_Pin_4
#define GT_SPI_CS_PIN_DIR  		GPIO_Mode_Out_PP
#define GT_SPI_CS_PIN_CLK  		RCC_APB2Periph_GPIOA

#define GT_SPI_SCK_PORT				GPIOA
#define GT_SPI_SCK_PIN				GPIO_Pin_5
#define GT_SPI_SCK_PIN_DIR  	GPIO_Mode_Out_PP
#define GT_SPI_SCK_PIN_CLK  	RCC_APB2Periph_GPIOA

#define GT_SPI_MISO_PORT			GPIOA
#define GT_SPI_MISO_PIN				GPIO_Pin_6
#define GT_SPI_MISO_PIN_DIR  	GPIO_Mode_IPU
#define GT_SPI_MISO_PIN_CLK  	RCC_APB2Periph_GPIOA

#define GT_SPI_MOSI_PORT			GPIOA
#define GT_SPI_MOSI_PIN				GPIO_Pin_7
#define GT_SPI_MOSI_PIN_DIR  	GPIO_Mode_Out_PP
#define GT_SPI_MOSI_PIN_CLK  	RCC_APB2Periph_GPIOA
//位带操作提高速度
#define GT_SPI_CS PAout(4)// PB12

//#define GT_SPI_CS=1	GPIO_SetBits(GT_SPI_CS_PORT, GT_SPI_CS_PIN)
//#define GT_SPI_CS=0	GPIO_ResetBits(GT_SPI_CS_PORT, GT_SPI_CS_PIN)

#define GT_SPI_SCK PAout(5)// PB13

//#define GT_SPI_SCK=1	GPIO_SetBits(GT_SPI_SCK_PORT, GT_SPI_SCK_PIN)
//#define GT_SPI_SCK=0	GPIO_ResetBits(GT_SPI_SCK_PORT, GT_SPI_SCK_PIN)

#define READ_GT_SPI_MISO PAin(6)// PB14
//#define READ_GT_SPI_MISO	GPIO_ReadInputDataBit(GT_SPI_MISO_PORT, GT_SPI_MISO_PIN)

#define GT_SPI_MOSI PAout(7)// PB15
//#define GT_SPI_MOSI=1	GPIO_SetBits(GT_SPI_MOSI_PORT, GT_SPI_MOSI_PIN)
//#define GT_SPI_MOSI=0	GPIO_ResetBits(GT_SPI_MOSI_PORT, GT_SPI_MOSI_PIN)




unsigned int  GB2312_GetAddr(unsigned char *hex,unsigned char type);
void GT_FrontIC_ReadBytes( unsigned char *pBuff,unsigned int addr,unsigned int len );
unsigned char GT_FrontIC_ReadOneByte(unsigned int addr);
void GT_FontIC_Init( void );
#endif
