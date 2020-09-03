#include "sys.h"
//#include "Hw_platform.h"
#include "GT21L16S2W.h"
#include "stdio.h"
//初始化字模芯片IC的IO口
void GT_FontIC_Init( void )
{
	uint8_t check=0;
	GPIO_InitTypeDef GPIO_Output_diy;//定义I/O口设置结构体
  RCC_APB2PeriphClockCmd(GT_SPI_CS_PIN_CLK|GT_SPI_SCK_PIN_CLK|GT_SPI_MISO_PIN_CLK|GT_SPI_MOSI_PIN_CLK,ENABLE);//开GPIO的时钟
	
  GPIO_Output_diy.GPIO_Speed=GPIO_Speed_50MHz;	//IO口输出最大速率
  
  GPIO_Output_diy.GPIO_Pin = GT_SPI_CS_PIN;
	GPIO_Output_diy.GPIO_Mode = GT_SPI_CS_PIN_DIR;
  GPIO_Init(GT_SPI_CS_PORT, &GPIO_Output_diy);
	
	GPIO_Output_diy.GPIO_Pin = GT_SPI_SCK_PIN;
	GPIO_Output_diy.GPIO_Mode = GT_SPI_SCK_PIN_DIR;
  GPIO_Init(GT_SPI_SCK_PORT, &GPIO_Output_diy);
	
	GPIO_Output_diy.GPIO_Pin = GT_SPI_MISO_PIN;
	GPIO_Output_diy.GPIO_Mode = GT_SPI_MISO_PIN_DIR;
  GPIO_Init(GT_SPI_MISO_PORT, &GPIO_Output_diy);
	
	GPIO_Output_diy.GPIO_Pin = GT_SPI_MOSI_PIN;
	GPIO_Output_diy.GPIO_Mode = GT_SPI_MOSI_PIN_DIR;
  GPIO_Init(GT_SPI_MOSI_PORT, &GPIO_Output_diy);
	GT_SPI_CS=1;
	check=GT_FrontIC_ReadOneByte(0x1F742);		
	if(check == 0x11)
		printf("字库初始化成功\n");
	else
		printf("字库初始化失败\n");
}   
//读写GT字库芯片
static unsigned char GT_FontIC_SPI_RW( unsigned char dat )   
{   
    unsigned char cnt_clk;   
    unsigned char temp;   
    unsigned char ret_data;   
   
    for( cnt_clk = 0; cnt_clk < 8; cnt_clk++ )   
    {   
        GT_SPI_SCK=0;   
        ret_data = ret_data << 1;	
        if( READ_GT_SPI_MISO )  
				{
            ret_data |= 0x01;  
				}			
        else   
				{
            ret_data &= 0xfe;   
				}
        temp = dat & 0x80;   
        if( temp > 0 ) 
				{			
            GT_SPI_MOSI=1;  
				}			
        else   
				{
            GT_SPI_MOSI=0;   
				}
        dat = dat << 1;   
        GT_SPI_SCK=1;   
    }   
		GT_SPI_SCK=0;  
    return(ret_data);   
}   
 //读GT字库芯片前设定读的地址
static void SPI_ReadAddressSet(unsigned char addrh,unsigned char addm,unsigned char addrl)   
{   
    GT_SPI_CS=0; 
    GT_FontIC_SPI_RW(0x03);   
    GT_FontIC_SPI_RW(addrh);   
    GT_FontIC_SPI_RW(addm);   
    GT_FontIC_SPI_RW(addrl);   
}   
// 读一个字节
unsigned char GT_FrontIC_ReadOneByte(unsigned int addr) 
{   
    unsigned int i = addr;   
    unsigned char ret_data;   
    unsigned char *p = (unsigned char *)(&i);   
    SPI_ReadAddressSet(*(p+2),*(p+1),*(p+0));   
    ret_data = GT_FontIC_SPI_RW(DummyByte);   
    GT_SPI_CS=1;  
    return(ret_data);   
}   

void GT_FrontIC_ReadBytes( unsigned char * const pBuff,unsigned int addr,unsigned int len ) //
{   
    unsigned int i;   
    for(i = 0; i < len; i++ )   
	{
       *(pBuff+i) =  GT_FrontIC_ReadOneByte(addr + i);   
	}
}  

unsigned int  GB2312_GetAddr(unsigned char *hex,unsigned char type)   
{   
    unsigned int temp = 0;
    unsigned char c1,c2;   
	//Ascii  半角字符   
	if(hex[0]<0x80)   
	{   

		if( hex[0] < ' ' )   
			hex[0] = ' ';   
		temp = hex[0] - ' ';   
		if( type == FONT_TYPE_8 )   
			temp = temp*8 + ASC0808D2HZ_ADDR;   
		else if( type == FONT_TYPE_12 )   
			temp = temp*12 + ASC0812M2ZF_ADDR;   
		else if( type == FONT_TYPE_16 )   
			temp = temp*16 + GBEX0816ZF_ADDR;   
	}   
	else //中文字符   
	{   
		c1=hex[0];   
		c2=hex[1];   
		//全角符号区  
		if(c1>=0xA1 && c1 <= 0xAB && c2>=0xa1)   
		{   

				if( type == FONT_TYPE_12 )   
				{   
					if( c1>=0xA1 && c1 <= 0xA3 && c2>=0xa1 )//ZF 272 ZONE   
						temp = (c1 - 0xA1) * 94 + (c2 - 0xA1);   
					if( c1 == 0xa9 && c2>=0xa1  )//tab 94 zone   
						temp = 282 + ( c2 - 0xA1 );   
					return( temp *24 + ZF1112B2ZF_ADDR);   
				}   
				else   
				{   
					temp = (c1 - 0xA1) * 94 + (c2 - 0xA1);   
					return(temp *32 + CUTS1516ZF_ADDR);   
				}   

		}   
		//GBK双字节2区 6768   
		else if(c1>=0xb0 && c1 <= 0xf7 && c2>=0xa1)   
		{   
			temp= (c1 - 0xB0) * 94 + (c2 - 0xA1);   
			if( type == FONT_TYPE_12 )   
				temp = temp*24 + HZ1112B2HZ_ADDR;   
			else if( type == FONT_TYPE_16 )   
				temp = temp*32 + JFLS1516HZ_ADDR;   
		}   
	}   
    return temp;   
}    
   
// void main( void )   
// {   
//    unsigned char a[2] = {0xb0,0xa1 }; //16点汉字 “啊”  的GB2312内码。   
//    unsigned char a_dot[32];           //存储汉字 “啊”  的点阵数据。   
//    unsigned long a_dot_address;   
//    unsigned char count;   
//    a_dot_address = GB2312_Addr(a,TYPE_16); //得到“啊” 点阵数据的起始地址   
//    for( count = 0; count < 32; count++ )   //得到“啊” 点阵数据   
//         a_dot[count] =  GT_FrontIC_ReadOneByte(a_dot_address+count);   
//    display(x,y,a_dot,TYPE_16);             //在坐标（X，Y），显示汉字“啊”   
// }   
