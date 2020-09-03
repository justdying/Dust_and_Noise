#include "stmflash.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "usart5.h"
#include "includes.h"
#include "timer.h"
#include "modbus_host.h"
#include "malloc.h" 
#include "Init_IO.h"
#include "stdio.h"
#include "SENSOR.h"
#include "rtc.h" 		
#include "GT21L16S2W.h"
#include "LED_MATRIX.h"
#include "link_list.h"
#include "wdg.h"
#include "dtu.h"
//2019-5-16 ������λ���޸�����˿ڹ���
//2019-11-15 ���°���-�Ķ�����
//�汾��V2.0
/***************************************************************
ע�⣺
1�������Ӵ�����ʱ��Ҫ�޸�SENSOR_AMOUNT��ֵ
2���������ĵ�ַ�Լ����յ�λ��
#define SlaveAddr_TemHum			0x04   //��ʪ�ȴ�����
#define SlaveAddr_Noise				0x03   //����������
#define SlaveAddr_PM2_5PM10		0x0F   //PM������      //0x0f   05
#define SlaveAddr_Light				0x07   //���մ�����
#define SlaveAddr_Fan					0x08   //���ٴ�����    //08 ��   01 ��
#define SlaveAddr_Wind_Dir		0x09   //���򴫸���		//09      02
****************************************************************/
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define Time_Task_PRIO		5
//�����ջ��С
#define Time_STK_SIZE		512
//�����ջ
__align(8)  OS_STK	Time_Date_Task_STK[Time_STK_SIZE];
void Time_Date_Task(void *p_arg);

//�������ȼ�
#define M485_task_PRIO		7
//�����ջ��С	
#define M485_STK_SIZE 		512
//�����ջ	
__align(8)  OS_STK M485_task_STK[M485_STK_SIZE];
void M485_task(void *p_arg);

//�������ȼ�
#define LED_TASK_PRIO		4
//�����ջ��С	
#define LED_STK_SIZE 		512
//�����ջ	
__align(8) OS_STK LED_TASK_STK[LED_STK_SIZE];
//������
void led_task(void *p_arg);

//�������ȼ�
#define Usart3_Date_Task_PRIO		6
//�����ջ��С
#define Usart3_STK_SIZE		512
//�����ջ
__align(8) OS_STK	Usart3_Date_Task_STK[Usart3_STK_SIZE];
//������
void Usart3_Date_Task(void *p_arg);
//ȫ�ֱ���
const uint8_t Version[]="Ver:V2.1";
uint8_t Power_Once = 1;
Sen_Cache *contex = 0;     //���������ݻ���ָ��
vu32 Time_Second_Count = 0; 			//���ʱ��
uint8_t Alarm_Photo_Flag = 0; 		//׼�����ձ�־ ����200��1����⵽PM10����200�ҳ���15���ӵĻ�����
//ʱ��״̬ 
//bit8��	//����±�־-����������
//bit7��	//Сʱ���±�־-����������
//bit6��	//10���Ӹ��±�־-����������
//bit5��	//���Ӹ��±�־-����������
//bit4��	//10���ӵ���
//bit3��	//����±�־
//bit2��	//Сʱ���±�־
//bit1��	//10���Ӹ��±�־
//bit0��	//���Ӹ��±�־
vu16 Time_STA=0; 
//�����ʱ��
OS_TMR 	*Second_tmr;		//��ʱ��1
OS_TMR 	*Minute15_tmr;		//��ʱ��2
void Second_tmr_callback(void *p_tmr, void *p_arg);//��ʱ��1�Ļص�����
void Minute15_tmr_callback(void *p_tmr, void *p_arg);//��ʱ��2�Ļص�����

OS_EVENT *SYNC_SEM;		//����һ���ź�������������ͬ��
OS_EVENT *SEM_LEDCALLBACK;		//����һ���ź�����LED��ʾ�ص�
OS_EVENT *SEM_TIME;		//����һ���ź�����LED��ʾ�ص�


int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	delay_init();       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	Usart_FIFO_Init();   //485
	usart2_init(115200);		//GPRSͨѶ��
	uart5_init(115200);
	TIM2_Int_Init();			
	LED_Init();         //LED��ʼ��
	LED_Gpio_Int();     //LED��ʾ�����ų�ʼ��
	GT_FontIC_Init();    //�ֿ�оƬ��ʼ��
	//RELAY_Init();				//�̵�����ʼ��
	RTC_Init();	  			//RTC��ʼ��
	IWDG_Init(6,1875);    	   //���ʱ��Ϊ12s	���Ź���ʼ�� 
	Init_System_Config_Table(); //��ʼ��ϵͳ���ñ�
	Print_System_Config_Table();  //��ӡϵͳ���ñ�
	ClearDisplay(1);	 		//��LED����ʾ
	OSInit();  																			//UCOS��ʼ��
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //������ʼ����
	OSStart(); 			
	while(1);
}

//��ʼ������
void start_task(void *pdata)
{
	uint8_t err;
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	OSTaskCreate(M485_task,(void*)0,(OS_STK*)&M485_task_STK[M485_STK_SIZE-1],M485_task_PRIO);           //����485����
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);		            //����LED��ʾ����
	OSTaskCreate(Usart3_Date_Task,(void*)0,(OS_STK*)&Usart3_Date_Task_STK[Usart3_STK_SIZE-1],Usart3_Date_Task_PRIO);		         		 
	OSTaskCreate(Time_Date_Task,(void*)0,(OS_STK*)&Time_Date_Task_STK[Time_STK_SIZE-1],Time_Task_PRIO);		 				 
	SYNC_SEM = OSSemCreate(0);			//���������ź���
	SEM_LEDCALLBACK = OSSemCreate(0);			
	SEM_TIME = OSSemCreate(0);			
	Minute15_tmr = OSTmrCreate(90000,0,OS_TMR_OPT_ONE_SHOT,Minute15_tmr_callback,0,(INT8U*)"Minute15_tmr",&err);//���������ʱ��
	Second_tmr = OSTmrCreate(0,100,OS_TMR_OPT_PERIODIC,Second_tmr_callback,0,(INT8U*)"Second_tmr",&err);//���������ʱ��		SYNC_SEM = OSSemCreate(0);			//���������ź���		 
	OSTmrStart(Second_tmr,&err);      //�����붨ʱ��1	
	OSTaskSuspend(START_TASK_PRIO); //����ʼ����
	OS_EXIT_CRITICAL();             //�˳��ٽ���(���ж�)
}
//led0������
void M485_task(void *p_arg)
{	
	static uint8_t times = 0;
	while(1)
	{
		if(times>=7)	
		{
			times = 0;
			MODH_WriteParam_05H(SlaveAddr_PM2_5PM10,0,0xFF00);
		}
		delay_ms(100);
		MODH_ReadParam_03H(SlaveAddr_MelogiEle,0x01F4, 3);
		Sensor_Add_Date(&Tem_Cache,g_tVar.Fack_MelogiEle,0);
		Sensor_Add_Date(&Hum_Cache,g_tVar.Fack_MelogiEle,0);
		Sensor_Add_Date(&Noise_Cache,g_tVar.Fack_MelogiEle,0);
		delay_ms(100);
		MODH_ReadParam_03H(SlaveAddr_Wind_Dir,0, 1);					
		Sensor_Add_Date(&Win_Dir,g_tVar.Fack_WinDir,0);
		delay_ms(100);
		MODH_ReadParam_03H(SlaveAddr_Fan,0, 1);
		Sensor_Add_Date(&FanS_Cache,g_tVar.Fack_FanSpeed,0);
		delay_ms(9000);	
		MODH_WriteParam_05H(SlaveAddr_PM2_5PM10,0,0);
		delay_ms(100);	
		MODH_ReadParam_03H(SlaveAddr_PM2_5PM10,0x14,0x06);		
		
		Sensor_Add_Date(&PM2_5_Cache,g_tVar.Fack_PM,0);
		Sensor_Add_Date(&PM10_Cache,g_tVar.Fack_PM,0);
		memset(&g_tVar,0,sizeof(g_tVar));		
		times++;
		delay_ms(9000);
	}
}

//led������
const char *Windir[8] = {"����","����","����","����","����","����","����","����"};		//���ڷ�����ʾ
void led_task(void *p_arg)
{ 	
	const  char *wd = NULL;
	const  uint16_t time = 900;
	float tem,humi;
	uint8_t buff[30]={0};			//��ʾ���ݻ���
	static uint8_t count=0;
	uint16_t i=0;					//��ʾ�Ļ�����
	while(1)
	{
		ClearDisplay(1);		
		switch(++count)
		{
			case 1:							//��һĻ
				if(Power_Once ==1)			//�ϵ��һ�ε���
				{
					StaticStringDisplayInPos((uint8_t*)Version,0,0,0);//X,Y
					StaticStringDisplayInPos((uint8_t*)"�ﳾ����",16,16,0);//X,Y
					StaticStringDisplayInPos((uint8_t*)"ʵʱ���ϵͳ",0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();
					ClearDisplayLine(0);
					RollingStringDisplayInLine((uint8_t*)System_Config_Table1.Project_Company_name,0,0,6);//X,Y
					Power_Once = 0;
				}
				else
				{
					StaticStringDisplayInPos((uint8_t*)"�ﳾ����",16,16,0);//X,Y
					StaticStringDisplayInPos((uint8_t*)"ʵʱ���ϵͳ",0,32,0);//X,Y
					RollingStringDisplayInLine((uint8_t*)System_Config_Table1.Project_Company_name,0,0,6);//X,Y
				} 
			break;
			case 2:						//�ڶ�Ļ				
					sprintf((char *)buff,"����:%.1fdB",(float)Noise_Cache.sen_cache_sec_curr/10.0);
					StaticStringDisplayInPos((uint8_t*)"ʵʱ����",16,0,0);//X,Y
					StaticStringDisplayInPos(buff,0,16,0);//X,Y
					if(Noise_Cache.sen_cache_sec_curr<=680)																								sprintf((char *)buff,"�ȼ�:��");
					else if(Noise_Cache.sen_cache_sec_curr>680 && Noise_Cache.sen_cache_sec_curr<=900) 		sprintf((char *)buff,"�ȼ�:��");
					else if(Noise_Cache.sen_cache_sec_curr>900 && Noise_Cache.sen_cache_sec_curr<=920) 		sprintf((char *)buff,"�ȼ�:����Ⱦ");
					else if(Noise_Cache.sen_cache_sec_curr>920 && Noise_Cache.sen_cache_sec_curr<=1040) 	sprintf((char *)buff,"�ȼ�:����Ⱦ");
					else if(Noise_Cache.sen_cache_sec_curr>1040) 																					sprintf((char *)buff,"�ȼ�:����Ⱦ");
					StaticStringDisplayInPos(buff,0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();		
			break;
			case 3:						//����Ļ	
					sprintf((char *)buff,"PM2.5:%dug",PM2_5_Cache.sen_cache_sec_curr);
					StaticStringDisplayInPos(buff,0,0,0);//X,Y
					sprintf((char *)buff,"PM10 :%dug",PM10_Cache.sen_cache_sec_curr);		
					StaticStringDisplayInPos(buff,0,16,0);//X,Y
					sprintf((char *)buff,"TSP  :%dug",(uint16_t)(0.0349 + 1.2819*PM10_Cache.sen_cache_sec_curr));
					StaticStringDisplayInPos(buff,0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();	
			break;
			case 4:						//����Ļ	
				sprintf((char *)buff,"����:%.1fm/s",(float)FanS_Cache.sen_cache_sec_curr/10.0);
					StaticStringDisplayInPos(buff,0,0,0);//X,Y
					//�жϷ���
//					if((Win_Dir.sen_cache_sec_curr >= 0 && Win_Dir.sen_cache_sec_curr < 22) || (Win_Dir.sen_cache_sec_curr >= 338 && Win_Dir.sen_cache_sec_curr<=360))	wd = Windir[0];
//					else if(Win_Dir.sen_cache_sec_curr >= 22  && Win_Dir.sen_cache_sec_curr < 67)		wd = Windir[1];
//					else if(Win_Dir.sen_cache_sec_curr >= 67  && Win_Dir.sen_cache_sec_curr < 112)	wd = Windir[2];
//					else if(Win_Dir.sen_cache_sec_curr >= 112 && Win_Dir.sen_cache_sec_curr < 157)	wd = Windir[3];
//					else if(Win_Dir.sen_cache_sec_curr >= 157 && Win_Dir.sen_cache_sec_curr < 202)	wd = Windir[4];
//					else if(Win_Dir.sen_cache_sec_curr >= 202 && Win_Dir.sen_cache_sec_curr < 247)	wd = Windir[5];
//					else if(Win_Dir.sen_cache_sec_curr >= 247 && Win_Dir.sen_cache_sec_curr < 292)	wd = Windir[6];
//					else if(Win_Dir.sen_cache_sec_curr >= 292 && Win_Dir.sen_cache_sec_curr <= 337)	wd = Windir[7];
					//�˷�λ������
					if(Win_Dir.sen_cache_sec_curr == 0)			wd = Windir[0];
					else if(Win_Dir.sen_cache_sec_curr ==1)	wd = Windir[1];
					else if(Win_Dir.sen_cache_sec_curr ==2)	wd = Windir[2];
					else if(Win_Dir.sen_cache_sec_curr ==3)	wd = Windir[3];
					else if(Win_Dir.sen_cache_sec_curr ==4)	wd = Windir[4];
					else if(Win_Dir.sen_cache_sec_curr ==5)	wd = Windir[5];
					else if(Win_Dir.sen_cache_sec_curr ==6)	wd = Windir[6];
					else if(Win_Dir.sen_cache_sec_curr ==7)	wd = Windir[7];		
					sprintf((char *)buff,"����:%s",wd);
					
					StaticStringDisplayInPos((uint8_t*)buff,0,16,0);
					if(FanS_Cache.sen_cache_sec_curr<=2)																								sprintf((char *)buff,"�ȼ�:�޷�");
					else if(FanS_Cache.sen_cache_sec_curr>=3 && FanS_Cache.sen_cache_sec_curr<=15) 			sprintf((char *)buff,"�ȼ�:���");
					else if(FanS_Cache.sen_cache_sec_curr>=16 && FanS_Cache.sen_cache_sec_curr<=33) 		sprintf((char *)buff,"�ȼ�:���");
					else if(FanS_Cache.sen_cache_sec_curr>=34 && FanS_Cache.sen_cache_sec_curr<=54) 		sprintf((char *)buff,"�ȼ�:΢��");
					else if(FanS_Cache.sen_cache_sec_curr>=55 && FanS_Cache.sen_cache_sec_curr<=79) 		sprintf((char *)buff,"�ȼ�:�ͷ�");
					else if(FanS_Cache.sen_cache_sec_curr>=80 && FanS_Cache.sen_cache_sec_curr<=107) 		sprintf((char *)buff,"�ȼ�:���");
					else if(FanS_Cache.sen_cache_sec_curr>=108 && FanS_Cache.sen_cache_sec_curr<=138) 	sprintf((char *)buff,"�ȼ�:ǿ��");
					else if(FanS_Cache.sen_cache_sec_curr>=139 && FanS_Cache.sen_cache_sec_curr<=171) 	sprintf((char *)buff,"�ȼ�:����");
					else if(FanS_Cache.sen_cache_sec_curr>=172 && FanS_Cache.sen_cache_sec_curr<=207) 	sprintf((char *)buff,"�ȼ�:���");
					else if(FanS_Cache.sen_cache_sec_curr>=208 && FanS_Cache.sen_cache_sec_curr<=244) 	sprintf((char *)buff,"�ȼ�:�ҷ�");			
					else if(FanS_Cache.sen_cache_sec_curr>=245 && FanS_Cache.sen_cache_sec_curr<=284) 	sprintf((char *)buff,"�ȼ�:���");
					else if(FanS_Cache.sen_cache_sec_curr>=285 && FanS_Cache.sen_cache_sec_curr<=326) 	sprintf((char *)buff,"�ȼ�:����");
					else if(FanS_Cache.sen_cache_sec_curr>=327 && FanS_Cache.sen_cache_sec_curr<=369) 	sprintf((char *)buff,"�ȼ�:̨��");
					StaticStringDisplayInPos(buff,0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();	
			break;
			case 5:						//����Ļ	
				tem = (float)Tem_Cache.sen_cache_sec_curr/10.0;
				humi = (float)Hum_Cache.sen_cache_sec_curr/10.0;
				sprintf((char *)buff,"�¶�:%.1f��",tem);
				StaticStringDisplayInPos(buff,0,0,0);//X,Y
				sprintf((char *)buff,"ʪ��:%.1f%%RH",humi);
				StaticStringDisplayInPos(buff,0,16,0);//X,Y
				if(tem<6.0)												sprintf((char *)buff,"���ʶ�:����");
				else if(tem>=6.0 && tem<11.0) 		sprintf((char *)buff,"���ʶ�:��");
				else if(tem>=11.0 && tem<15.0) 		sprintf((char *)buff,"���ʶ�:��");
				else if(tem>=15.0 && tem<18.0) 		sprintf((char *)buff,"���ʶ�:ƫ��");
				else if(tem>=18.0 && tem<21.0) 		sprintf((char *)buff,"���ʶ�:����");
				else if(tem>=21.0 && tem<26.0) 		sprintf((char *)buff,"���ʶ�:����");
				else if(tem>=26.0 && tem<30.0) 		sprintf((char *)buff,"���ʶ�:ƫ��");
				else if(tem>=30.0) 								sprintf((char *)buff,"���ʶ�:����");			
				StaticStringDisplayInPos(buff,0,32,0);//X,Y
				for(i=0;i<time;i++)
					WriteDisplayTemp();	
			break;
			case 6:						//����Ļ  ��ʾ����
				if(calendar.w_year >=2019)
				{		
					sprintf((char *)buff,"%d-%02d-%02d",calendar.w_year,calendar.w_month,calendar.w_date);
					StaticStringDisplayInPos(buff,8,0,0);//X,Y
					Get_Week(calendar.week,buff);
					StaticStringDisplayInPos(buff,24,16,0);//X,Y
					for(i=0;i<time;i++)
					{
						sprintf((char *)buff,"%02d:%02d:%02d",calendar.hour,calendar.min,calendar.sec);
						StaticStringDisplayInPos(buff,17,32,1);//X,Y	
					}
				}
				count = 0;				//��ʾ�Ļ���������
			break;
			default:		
			break;
		}										 
		delay_ms(10);
	}
}

//Usart3���ݴ�������
void Usart3_Date_Task(void *p_arg)
{
	OS_CPU_SR cpu_sr=0;
	uint16_t crc,buffer;
	uint8_t flag=0;
	while(1)
	{
		if((USART2_5_RX_STA&0x8000)!=0)//�������
		{
			RTC_Get(); //����ʱ��
			sprintf((char *)System_Config_Table1.Calendar,"%d-%d-%d %d:%d:%d %dms",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec,calendar.msec);
			System_Config_Table1.Table_address=FLASH_SAVE_ADDR;   //�豸��ַ
			switch(USART2_5_RX_BUF[0])    //USART2_5_RX_BUF[0]�洢����
			{
				case SYSTEM_ORDER_GETCONFIG:
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						Print_System_Config_Table();  //��ӡϵͳ���ñ�					
					}
				break;
				case	SYSTEM_ORDER_RESET:
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						SYSTEMRESET();			//ϵͳ��λ			
					}
				break;				
				case SYSTEM_ORDER_DUST_GATE:     //���÷�ֵ
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						System_Config_Table1.PM10_low_threshold = (USART2_5_RX_BUF[1]<<8)+USART2_5_RX_BUF[2];
						System_Config_Table1.PM10_high_threshold = (USART2_5_RX_BUF[3]<<8)+USART2_5_RX_BUF[4];
						OS_ENTER_CRITICAL();//�����ٽ���
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
						OS_EXIT_CRITICAL();	//�˳��ٽ���	 
						Print_System_Config_Table();  //��ӡϵͳ���ñ�
					}
				break;
				case SYSTEM_ORDER_EQUIP_NUMBER:			//�����豸���
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						if(Usart2_5_Rec_Cnt>38)  //̫��ֱ���˳�
						{
							Print_System_Config_Table();  //��ӡϵͳ���ñ�		
							break;
						}
						memcpy(System_Config_Table1.Equipment_number,USART2_5_RX_BUF+1,Usart2_5_Rec_Cnt-3);
						System_Config_Table1.Equipment_number[Usart2_5_Rec_Cnt-2]='\0';					//�豸��� <=30λ			
						OS_ENTER_CRITICAL();//�����ٽ���
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
						OS_EXIT_CRITICAL();	//�˳��ٽ���	 
						Print_System_Config_Table();  //��ӡϵͳ���ñ�		
					}
				break;
				case SYSTEM_ORDER_COMPANY_NAME:			//���ù�˾����
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						if(Usart2_5_Rec_Cnt>38)  //̫��ֱ���˳�
						{
							Print_System_Config_Table();  //��ӡϵͳ���ñ�		
							break;
						}
						memcpy(System_Config_Table1.Project_Company_name,USART2_5_RX_BUF+1,Usart2_5_Rec_Cnt-3);
						System_Config_Table1.Project_Company_name[Usart2_5_Rec_Cnt-2]='\0';					//��˾���� <=30λ			
						OS_ENTER_CRITICAL();//�����ٽ���
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof (System_Config_Table1)/2);   //����FLASH
						OS_EXIT_CRITICAL();	//�˳��ٽ���	 
						Print_System_Config_Table();  //��ӡϵͳ���ñ�		
					}
				break;
				case SYSTEM_ORDER_DUST_FACTOR:			//���÷۳�ϵ��
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						System_Config_Table1.PM2_5_k_factor = ((USART2_5_RX_BUF[1]<<8)+USART2_5_RX_BUF[2])/10.0;
						System_Config_Table1.PM2_5_b_factor = USART2_5_RX_BUF[3];
						System_Config_Table1.PM10_k_factor = ((USART2_5_RX_BUF[4]<<8)+USART2_5_RX_BUF[5])/10.0;
						System_Config_Table1.PM10_b_factor = USART2_5_RX_BUF[6];
						OS_ENTER_CRITICAL();//�����ٽ���
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof (System_Config_Table1)/2);   //����FLASH
						OS_EXIT_CRITICAL();	//�˳��ٽ���	 
						Print_System_Config_Table();  //��ӡϵͳ���ñ�		
					}
				break;
				case SYSTEM_ORDER_DTU_INIT:			//DTU��ʼ��
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						USART2_5_RX_STA = 0; //���  ׼����������
						Dtu_Init();
					}
				break;
				case SYSTEM_ORDER_TIME_SET:			//ʱ��У׼
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						RTC_Set(2000 + USART2_5_RX_BUF[1],USART2_5_RX_BUF[2],USART2_5_RX_BUF[3],USART2_5_RX_BUF[4],USART2_5_RX_BUF[5],USART2_5_RX_BUF[6]);
						Power_up_rtc = 0;
						Print_Calander();    //��ӡʱ��
					}
				break;		
				case SYSTEM_ORDER_APP_UPDATE:			//APP����
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						buffer = BOOTLEADER_FLAG_ON;
						OS_ENTER_CRITICAL();//�����ٽ���
						STMFLASH_Write(BOOTLEADER_FLAG_ADDR,&buffer,1);
						OS_EXIT_CRITICAL();	//�˳��ٽ���	 
						SYSTEMRESET();			//ϵͳ��λ
					}
				break;
				case	SYSTEM_ORDER_REALTIME_DATA:		//��ȡʵʱ����
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						Print_Calander();    //��ӡʱ��
						printf(" �¶� ��%.1f��\n ʪ�� ��%.1f%%RH\n ���� ��%.1f m/s\n ���� ��%.1fdB\n PM2.5��%dug/m3\n PM10 ��%dug/m3\n TSP  ��%.1fug/m3\n ���� ��%d��\n"
						,(float)Tem_Cache.sen_cache_sec_curr/10.0
						,(float)Hum_Cache.sen_cache_sec_curr/10.0
						,(float)FanS_Cache.sen_cache_sec_curr/10.0
						,(float)Noise_Cache.sen_cache_sec_curr/10.0
						,PM2_5_Cache.sen_cache_sec_curr
						,PM10_Cache.sen_cache_sec_curr
						,(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_sec_curr)
						,Win_Dir.sen_cache_sec_curr);
					}
				break;
				case	SYSTEM_ORDER_DTU_AIP_PR:		//�ı�DTU��A IP����˿ں�	
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						memcpy(&System_Config_Table1.Socka,USART2_5_RX_BUF+2,Usart2_5_Rec_Cnt-6);
						System_Config_Table1.Socka.enable = USART2_5_RX_BUF[1];
						System_Config_Table1.Socka.pr = (USART2_5_RX_BUF[6]<<8) + USART2_5_RX_BUF[7];
						USART2_5_RX_STA = 0; //���  ׼����������
						flag = Config_Socka(1,System_Config_Table1.Socka.enable);
						if(1 == flag)
						{
							OS_ENTER_CRITICAL();//�����ٽ���
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
							OS_EXIT_CRITICAL();	//�˳��ٽ���
							Print_System_Config_Table();  //��ӡϵͳ���ñ�									
						}
					}
				break;	
				case	SYSTEM_ORDER_DTU_BIP_PR:		//�ı�DTU��B IP����˿ں�	
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						memcpy(&System_Config_Table1.Sockb,USART2_5_RX_BUF+2,Usart2_5_Rec_Cnt-6);
						System_Config_Table1.Sockb.enable = USART2_5_RX_BUF[1];
						System_Config_Table1.Sockb.pr = (USART2_5_RX_BUF[6]<<8) + USART2_5_RX_BUF[7];
						USART2_5_RX_STA = 0;		//���  ׼����������
						flag = Config_Socka(2,System_Config_Table1.Sockb.enable);
						if(1 == flag)
						{
							OS_ENTER_CRITICAL();//�����ٽ���
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
							OS_EXIT_CRITICAL();	//�˳��ٽ���	 
							Print_System_Config_Table();  //��ӡϵͳ���ñ�		
						}
					}
				break;
				case SYSTEM_ORDER_SHOW_TYPE:
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						System_Config_Table1.Led_Show_Type = USART2_5_RX_BUF[1];
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
						Print_System_Config_Table();  //��ӡϵͳ���ñ�		
					}
				break;
				default:	
				break;
			}
			USART2_5_RX_STA=0x0000;     //�����־λ
			Usart2_5_Rec_Cnt=0;
		}
		delay_ms(100);	
	}
}
//ʱ�䴦��������
void Time_Date_Task(void *p_arg)
{
	uint8_t err;
	while(1)
	{
		OSSemPend(SEM_TIME, 0, &err);
		//���ͱ���
		if((Time_STA & TENSECOND) != 0)
		{
			Time_STA &=(~TENSECOND);
			Send_CloudData();								//��������
			Heart_Beat();
		}
	  else if((Time_STA & MINUTEFLAG) != 0)						//1���ӵ���
		{
			Time_STA |= MINUTEFLAG_SEN;
			Time_STA &=(~MINUTEFLAG);
			Fit_Sen_Protocol(MINUTE);
		}
		else if((Time_STA & TENMINUTEFLAG) != 0)
		{
			Time_STA |= TENMINUTEFLAG_SEN;
			Time_STA &=(~TENMINUTEFLAG);
			Fit_Sen_Protocol(TEN_MINUTE);
		}
		else if((Time_STA & HOURFLAG) != 0)
		{
			Time_STA |= HOURFLAG_SEN;
			Time_STA &=(~HOURFLAG);
			Fit_Sen_Protocol(HOUR);
		}
		else if((Time_STA & DAYFLAG) != 0)
		{
			Time_STA |= DAYFLAG_SEN;
			Time_STA &=(~DAYFLAG);	
			Fit_Sen_Protocol(DAY);
		}
	 //System_Config_Table1.PM10_high_threshold
		//��Ũ�ȴ���PM10_high_threshold ����Alarm_Photo_FlagΪ0ʱ������ʱ��
		if(PM10_Cache.sen_cache_sec_curr >= System_Config_Table1.PM10_high_threshold  && Alarm_Photo_Flag == 0)
		{
			Alarm_Photo_Flag = 1;
			//printf("����15min��ʱ��\n");					//����ʱ���Դ򿪹۲�
			OSTmrStart(Minute15_tmr,&err);      //����15min��ʱ��
		}
		else if(PM10_Cache.sen_cache_sec_curr < System_Config_Table1.PM10_high_threshold)
		{
			Alarm_Photo_Flag = 0;
			//printf("���¿���15min��ʱ��\n");
			OSTmrStart(Minute15_tmr,&err);      //����15min��ʱ��
		}
		if(Time_Second_Count >= 0xFFFFFFFF)    //��ʱ���������
			Time_Second_Count=0;
		
		delay_ms(100);	
	}
}
	

//��ʱ��1�Ļص�����
uint8_t Update_Day_Tmr = 0;			//����±�־ ���ڶ�ʱ��Minute15
uint8_t Photo_Button_Flag = 0;  //���ձ�־
uint8_t Photo_Button_Delay = 0;  //������ʱʱ��
void Second_tmr_callback(void *p_tmr, void *p_arg)
{
	Time_Second_Count++;
	LED0=~LED0;
	IWDG->KR = (uint16_t)0xAAAA;  					//ι�������Ź�

	if((Time_Second_Count + 1) % 10==0)
	{
		Time_STA|=TENSECOND;
		OSSemPost(SEM_TIME);
	}
  else if((Time_STA & MINUTEFLAG) != 0)
	{	
		OSSemPost(SEM_TIME);
	}
	else if((Time_STA & TENMINUTEFLAG) != 0)
	{		
		OSSemPost(SEM_TIME);
	}
		//һСʱ����
	else if((Time_STA & HOURFLAG) != 0)
	{	
		OSSemPost(SEM_TIME);
	}
		//һ�쵽��
	else if((Time_STA & DAYFLAG) != 0)
	{	
		Update_Day_Tmr = 1;													//����Minute15_tmr_callback������������
		OSSemPost(SEM_TIME);
	}
	//�̵����պϣ���2S�Ͽ��̵���
	if(Photo_Button_Flag==1)
	{
		Photo_Button_Delay++;
		if(Photo_Button_Delay > 2)
		{
			Photo_Button_Delay = 0;
//			RELAY_PHOTO_BUTTON = 0;
			Photo_Button_Flag = 0;
		}
	}
} 
//��PM10���ڷ�ֵ���Ҵ���15���� ����15���ӻص�����
void Minute15_tmr_callback(void *p_tmr, void *p_arg)//��ʱ��2�Ļص�����
{
	static uint32_t minute15_count=0;
	Alarm_Photo_Flag = 0;
	if(Update_Day_Tmr == 1)							//�µ�һ����� minute15_count
	{
		Update_Day_Tmr = 0;
		minute15_count = 0;
	}
	minute15_count++;
	if(minute15_count <= 4)								//��һ��1Сʱ�� ÿ��15���ӽ�һ��
	{
//		RELAY_PHOTO_BUTTON = 1;							//�����ձ�־λ
		Photo_Button_Flag = 1;
		printf("Minute15_tmr_callback,PM10����200ug/m3 ����15min minute15_count = %d\n",minute15_count);
	}
	else if(minute15_count == 6 && minute15_count == 8 )	//����4�� ÿ��30���ӽ�һ��
	{
//		RELAY_PHOTO_BUTTON = 1;							//�����ձ�־λ
		Photo_Button_Flag = 1;
		printf("Minute15_tmr_callback,PM10����200ug/m3 ����15min minute15_count = %d\n",minute15_count);
	}
	else if(minute15_count > 8 && (minute15_count % 4 == 0))//����8�� ÿ��һСʱ��һ��
	{
//		RELAY_PHOTO_BUTTON = 1;
		Photo_Button_Flag = 1;
		printf("Minute15_tmr_callback,PM10����200ug/m3 ����15min minute15_count = %d\n",minute15_count);
	}
}
