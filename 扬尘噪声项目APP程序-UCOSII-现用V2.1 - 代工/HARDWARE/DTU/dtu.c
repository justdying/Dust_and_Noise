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
*	�� �� ��: Wait_For_Return
*	����˵��: �ȴ�DTU��Ӧ
*	��    ��: buf :��Ҫ��Ӧ������,
*	�� �� ֵ: 0�� ͨ�ų�ʱ  1��ƥ��ɹ�
*********************************************************************************************************
*/
uint8_t Wait_For_Return(const char *buf)
{
	int32_t time1;
	time1 = bsp_GetRunTime();	/* ��¼����͵�ʱ�� */
	while(1)
	{
		if((u8*)strstr((const char *)USART2_5_RX_BUF,buf)!=NULL && (USART2_5_RX_STA&0x8000)!=0)//������ɲ���ƥ��ɹ�
		{
			//delay_ms(100);			//��Ҫ��ʱ �ȴ�����USART2_5_RX_BUF[]�������
			memset(USART2_5_RX_BUF,0,7);
			USART2_5_RX_STA=0x0000;     //�����־λ
			Usart2_5_Rec_Cnt=0;
			break;
		}
		if (bsp_CheckRunTime(time1) > TIMEOUT_DTU)		
		{
			printf("TIMEOUT_DTU-%s\n",buf);		//
			//u5_printf("AT+S\r\n");				//����
			return 0;		/* ͨ�ų�ʱ�� */
		}

	}
	return 1;	
}
/*
*********************************************************************************************************
*	�� �� ��: TIMEOUT_printf
*	����˵��: ��ӡ��ʱ��Ϣ
*	��    ��: buf :��Ҫ��ӡ������,
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIMEOUT_printf(const char *buf)
{
	printf("DTU....TIMEOUT_DTU   %s\n",buf);
	u5_printf("usr.cn#AT+S\r");
}
/*
*********************************************************************************************************
*	�� �� ��: Config_Socka
*	����˵��: ����IP �˿ں�
*	��    ��: num :Ҫ�����ĸ���  1��SOCKA  2��SOCKB 
*	�� �� ֵ: 1���ɹ���0��ʧ��
*********************************************************************************************************
*/
uint8_t Config_Socka(uint8_t num,uint8_t enable)
{
	if(1 == num)										//����SOCKETA
	{
		u5_printf("usr.cn#AT+SOCKA=\"TCP\",\"%d.%d.%d.%d\",%d\r",System_Config_Table1.Socka.ip0,System_Config_Table1.Socka.ip1			//���͵�ַ
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
		//u5_printf("SOCKA_Config ok\n");  //��ӡ���óɹ���Ϣ
		printf("SOCKA_Config ok\n");	
	}
	else if(num==2)										//����SOCKETB
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
		//u5_printf("SOCKB_Config ok\n");		//��ӡ���óɹ���Ϣ
		printf("SOCKB_Config ok\n");
	}
	return 1;									//���سɹ�
}

//��buf����õ���cx���������ڵ�λ��
// chr Ҫ���ҵ��ַ�
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������	

uint8_t Dtu_Comma_Pos(uint8_t *buf,uint8_t chr ,uint8_t cx)
{	 		    
	uint8_t *p=buf;
	while(cx)
	{		 
		//if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==chr)cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n����
//����ֵ:m^n�η�.
uint32_t Dtu_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}
//buf:���ִ洢��
//����ֵ:ת�������ֵ
int Dtu_Str2num(uint8_t *buf)
{
	uint8_t *p=buf;
	uint32_t ires=0;
	uint8_t ilen=0,i;
	uint8_t mask=0;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p=='"'||(*p=='.'))break;//����������
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			//ilen=0;
			break;
		}	
		ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=Dtu_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(mask&0X02)ires=-ires;		   
	return ires;
}	  
/*
*********************************************************************************************************
*	�� �� ��: Get_Socket
*	����˵��: ��ȡIP �˿ں�
*	��    ��: num :Ҫ��ȡ�ĸ���  1��SOCKA  2��SOCKB 
*	�� �� ֵ: 1���ɹ���0��ʧ��
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
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
	return 1;	
}

/*
*********************************************************************************************************
* �� �� ��: Dtu_Init
* ����˵��: DTU��ʼ��  ��������
* ��    ��: ��
* �� �� ֵ: 1���ɹ���0��ʧ��
*********************************************************************************************************
*/
uint8_t Dtu_Init(void)
{
	USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("+++");
	 if(0 == Wait_For_Return("a"))
		return 0; 
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("a");
	 if(0 == Wait_For_Return("+ok"))
	 {		 
		 u5_printf("a");
		 if(0 == Wait_For_Return("+ok"))	 //Сдok'
			return 0; 
	 }
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+HEARTEN=\"off\"\r\n");
	 if(0 == Wait_For_Return("OK"))
		return 0; 
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+E=\"off\"\r\n");
	 if(0 == Wait_For_Return("OK"))
		return 0; 
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+UATEN=\"on\"\r\n");
	 if(0 == Wait_For_Return("OK"))			//��дOK
		return 0; 
	 
	  USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+SOCKA=\"TCP\",\"119.23.134.117\",9449\r\n");
	 if(0 == Wait_For_Return("OK"))			//��дOK
		return 0;	 
	 
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+SOCKAEN=\"on\"\r");
 	 if(0 == Wait_For_Return("OK"))
		return 0;	

	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+SOCKB=\"TCP\",\"119.23.134.117\",9450\r\n");
	 if(0 == Wait_For_Return("OK"))			//��дOK
		return 0;	 
	 
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+SOCKBEN=\"on\"\r");
 	 if(0 == Wait_For_Return("OK"))
		return 0;	
	 
	 USART2_5_RX_STA=0;     //�����־λ
	 u5_printf("AT+S\r\n");
	  USART2_5_RX_STA=0;     //�����־λ
	 printf("**************************************\n");
	 printf("------------DTU��ʼ���óɹ�-----------\n");
	 printf("**************************************\n");
	 return 1;
}
/*
*********************************************************************************************************
* �� �� ��: Test_Connected
* ����˵��: �������״̬
* ��    ��: ��
* �� �� ֵ: 1�������У�0������    -1���޷����� 
*********************************************************************************************************
*/
int8_t Test_Connected(uint8_t num)
{
	u5_printf("usr.cn#AT+SOCK%cLK?\r",num==1?'A':'B');
	if(0 == Wait_For_Return("usr.cn#"))
	{
		TIMEOUT_printf("AT+SOCK��LK?");
		return -1;	
	}
	if((u8*)strstr(USART2_5_RX_BUF+12,":disconnected")!=NULL)
		return 0;	
	else if((u8*)strstr(USART2_5_RX_BUF+12,":connected")!=NULL)
		return 1;
	else
		return 2;
}
