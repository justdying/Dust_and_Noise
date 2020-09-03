#include "sys.h"
#include "usart2.h"
#include "usart5.h"
#include "usart.h"
#include "delay.h"
#include "dtu.h"
#include "string.h"
#include "includes.h"
#include "stmflash.h"
/*
*********************************************************************************************************
*	函 数 名: Wait_For_Return
*	功能说明: 等待DTU回应
*	形    参: buf :想要回应的数据,
*	返 回 值: 0： 通信超时  1：匹配成功
*********************************************************************************************************
*/
uint8_t Wait_For_Return(const char *buf)
{
	int32_t time1;
	time1 = bsp_GetRunTime();	/* 记录命令发送的时刻 */
	while(1)
	{
		if((u8*)strstr((const char *)USART2_5_RX_BUF,buf)!=NULL && (USART2_5_RX_STA&0x8000)!=0)//接收完成并且匹配成功
		{
			//delay_ms(100);			//必要延时 等待数据USART2_5_RX_BUF[]接收完毕
			memset(USART2_5_RX_BUF,0,7);
			USART2_5_RX_STA=0x0000;     //清除标志位
			Usart2_5_Rec_Cnt=0;
			break;
		}
		if (bsp_CheckRunTime(time1) > TIMEOUT_DTU)		
		{
			printf("TIMEOUT_DTU-%s\n",buf);		//
			//u5_printf("AT+S\r\n");				//重启
			return 0;		/* 通信超时了 */
		}

	}
	return 1;	
}
/*
*********************************************************************************************************
*	函 数 名: TIMEOUT_printf
*	功能说明: 打印超时信息
*	形    参: buf :想要打印的数据,
*	返 回 值: 无
*********************************************************************************************************
*/
void TIMEOUT_printf(const char *buf)
{
	printf("DTU....TIMEOUT_DTU   %s\n",buf);
	u5_printf("usr.cn#AT+S\r");
}
/*
*********************************************************************************************************
*	函 数 名: Config_Socka
*	功能说明: 配置IP 端口号
*	形    参: num :要配置哪个口  1：SOCKA  2：SOCKB 
*	返 回 值: 1：成功，0：失败
*********************************************************************************************************
*/
uint8_t Config_Socka(uint8_t num,uint8_t enable)
{
	if(1 == num)										//配置SOCKETA
	{
		u5_printf("usr.cn#AT+SOCKA=\"TCP\",\"%d.%d.%d.%d\",%d\r",System_Config_Table1.Socka.ip0,System_Config_Table1.Socka.ip1			//发送地址
		,System_Config_Table1.Socka.ip2,System_Config_Table1.Socka.ip3,System_Config_Table1.Socka.pr);
//		u5_printf("AT+SOCKA=\"TCP\",\"119.23.134.117\",9446\r\n");
		if(0 == Wait_For_Return("usr.cn#"))
		{
			TIMEOUT_printf("AT+SOCKA=");
			return 0;	
		}
		if(1== enable)		
		{		
			u5_printf("usr.cn#AT+SOCKAEN=\"on\"\r");
			if(0 == Wait_For_Return("usr.cn#"))
			{
				TIMEOUT_printf("AT+SOCKAEN=on-err");
				return 0;	
			}
		}
		else
		{
			u5_printf("usr.cn#AT+SOCKAEN=\"off\"\r");
			if(0 == Wait_For_Return("usr.cn#"))
			{
				TIMEOUT_printf("AT+SOCKAEN=off--err");
				return 0;	
			}
		}
		u5_printf("usr.cn#AT+S\r");
		if(0 == Wait_For_Return("usr.cn#"))
		{
			TIMEOUT_printf("AT+S=--err");
			return 0;	
		}
		//u5_printf("SOCKA_Config ok\n");  //打印配置成功信息
		printf("SOCKA_Config ok\n");	
	}
	else if(num==2)										//配置SOCKETB
	{
		u5_printf("usr.cn#AT+SOCKB=\"TCP\",\"%d.%d.%d.%d\",%d\r",System_Config_Table1.Sockb.ip0,System_Config_Table1.Sockb.ip1
		,System_Config_Table1.Sockb.ip2,System_Config_Table1.Sockb.ip3,System_Config_Table1.Sockb.pr);
		
		if(0 == Wait_For_Return("usr.cn#"))
		{
			TIMEOUT_printf("AT+SOCKB=");
			return 0;	
		}
		if(1== enable)		
		{
			u5_printf("usr.cn#AT+SOCKBEN=\"on\"\r");
			if(0 == Wait_For_Return("usr.cn#"))
			{
				TIMEOUT_printf("AT+SOCKBEN=on-err");
				return 0;	
			}
		}
		else
		{
			u5_printf("usr.cn#AT+SOCKBEN=\"off\"\r");
			if(0 == Wait_For_Return("usr.cn#"))
			{
				TIMEOUT_printf("AT+SOCKBEN=off--err");
				return 0;	
			}
		}
		u5_printf("usr.cn#AT+S\r");
		if(0 == Wait_For_Return("usr.cn#"))
		{
			TIMEOUT_printf("AT+S=");
			return 0;	
		}	
		//u5_printf("SOCKB_Config ok\n");		//打印配置成功信息
		printf("SOCKB_Config ok\n");
	}
	return 1;									//返回成功
}

//从buf里面得到第cx个逗号所在的位置
// chr 要查找的字符
//返回值:0~0XFE,代表逗号所在位置的偏移.
//       0XFF,代表不存在第cx个逗号	

uint8_t Dtu_Comma_Pos(uint8_t *buf,uint8_t chr ,uint8_t cx)
{	 		    
	uint8_t *p=buf;
	while(cx)
	{		 
		//if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
		if(*buf==chr)cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n函数
//返回值:m^n次方.
uint32_t Dtu_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
//buf:数字存储区
//返回值:转换后的数值
int Dtu_Str2num(uint8_t *buf)
{
	uint8_t *p=buf;
	uint32_t ires=0;
	uint8_t ilen=0,i;
	uint8_t mask=0;
	while(1) //得到整数和小数的长度
	{
		if(*p=='-'){mask|=0X02;p++;}//是负数
		if(*p=='"'||(*p=='.'))break;//遇到结束了
		else if(*p>'9'||(*p<'0'))	//有非法字符
		{	
			//ilen=0;
			break;
		}	
		ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//去掉负号
	for(i=0;i<ilen;i++)	//得到整数部分数据
	{  
		ires+=Dtu_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(mask&0X02)ires=-ires;		   
	return ires;
}	  
/*
*********************************************************************************************************
*	函 数 名: Get_Socket
*	功能说明: 获取IP 端口号
*	形    参: num :要获取哪个口  1：SOCKA  2：SOCKB 
*	返 回 值: 1：成功，0：失败
*********************************************************************************************************
*/
uint8_t Get_Socket(uint8_t num)
{
	uint8_t p;	
	u5_printf("usr.cn#AT+SOCK%c?\r",num==1?'A':'B');
	if(0 == Wait_For_Return("usr.cn#"))
	{
		TIMEOUT_printf("AT+SOCKA=");
		return 0;	
	}
	p = Dtu_Comma_Pos(USART2_5_RX_BUF,'"',3);
	if(num==1)	System_Config_Table1.Socka.ip0 = Dtu_Str2num(USART2_5_RX_BUF+p);
	else System_Config_Table1.Sockb.ip0 = Dtu_Str2num(USART2_5_RX_BUF+p);
	p = Dtu_Comma_Pos(USART2_5_RX_BUF,'.',1);
	if(num==1)	System_Config_Table1.Socka.ip1 = Dtu_Str2num(USART2_5_RX_BUF+p);
	else System_Config_Table1.Sockb.ip1 = Dtu_Str2num(USART2_5_RX_BUF+p);
	p = Dtu_Comma_Pos(USART2_5_RX_BUF,'.',2);
	if(num==1)	System_Config_Table1.Socka.ip2 = Dtu_Str2num(USART2_5_RX_BUF+p);
	else System_Config_Table1.Sockb.ip2 = Dtu_Str2num(USART2_5_RX_BUF+p);
	p = Dtu_Comma_Pos(USART2_5_RX_BUF,'.',3);
	if(num==1)	System_Config_Table1.Socka.ip3 = Dtu_Str2num(USART2_5_RX_BUF+p);
	else System_Config_Table1.Sockb.ip3 = Dtu_Str2num(USART2_5_RX_BUF+p);
	p = Dtu_Comma_Pos(USART2_5_RX_BUF,',',2);
	if(num==1)	System_Config_Table1.Socka.pr = Dtu_Str2num(USART2_5_RX_BUF+p);
	else System_Config_Table1.Sockb.pr = Dtu_Str2num(USART2_5_RX_BUF+p);
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
	return 1;	
}

/*
*********************************************************************************************************
* 函 数 名: Dtu_Init
* 功能说明: DTU初始化  出厂配置
* 形    参: 无
* 返 回 值: 1：成功，0：失败
*********************************************************************************************************
*/
uint8_t Dtu_Init(void)
{
	USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("+++");
	 if(0 == Wait_For_Return("a"))
		return 0; 
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("a");
	 if(0 == Wait_For_Return("+ok"))
	 {		 
		 u5_printf("a");
		 if(0 == Wait_For_Return("+ok"))	 //小写ok'
			return 0; 
	 }
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+HEARTEN=\"off\"\r\n");
	 if(0 == Wait_For_Return("OK"))
		return 0; 
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+E=\"off\"\r\n");
	 if(0 == Wait_For_Return("OK"))
		return 0; 
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+UATEN=\"on\"\r\n");
	 if(0 == Wait_For_Return("OK"))			//大写OK
		return 0; 
	 
	  USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+SOCKA=\"TCP\",\"119.23.134.117\",9449\r\n");
	 if(0 == Wait_For_Return("OK"))			//大写OK
		return 0;	 
	 
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+SOCKAEN=\"on\"\r");
 	 if(0 == Wait_For_Return("OK"))
		return 0;	

	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+SOCKB=\"TCP\",\"119.23.134.117\",9450\r\n");
	 if(0 == Wait_For_Return("OK"))			//大写OK
		return 0;	 
	 
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+SOCKBEN=\"on\"\r");
 	 if(0 == Wait_For_Return("OK"))
		return 0;	
	 
	 USART2_5_RX_STA=0;     //清除标志位
	 u5_printf("AT+S\r\n");
	  USART2_5_RX_STA=0;     //清除标志位
	 printf("**************************************\n");
	 printf("------------DTU初始配置成功-----------\n");
	 printf("**************************************\n");
	 return 1;
}
/*
*********************************************************************************************************
* 函 数 名: Test_Connected
* 功能说明: 检测连接状态
* 形    参: 无
* 返 回 值: 1：连接中，0：掉线    -1：无法访问 
*********************************************************************************************************
*/
int8_t Test_Connected(uint8_t num)
{
	u5_printf("usr.cn#AT+SOCK%cLK?\r",num==1?'A':'B');
	if(0 == Wait_For_Return("usr.cn#"))
	{
		TIMEOUT_printf("AT+SOCK？LK?");
		return -1;	
	}
	if((u8*)strstr(USART2_5_RX_BUF+12,":disconnected")!=NULL)
		return 0;	
	else if((u8*)strstr(USART2_5_RX_BUF+12,":connected")!=NULL)
		return 1;
	else
		return 2;
}
