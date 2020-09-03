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
//2019-5-16 增加上位机修改网络端口功能
//2019-11-15 换新板子-改动串口
//版本：V2.0
/***************************************************************
注意：
1、当增加传感器时，要修改SENSOR_AMOUNT的值
2、传感器的地址以及接收的位置
#define SlaveAddr_TemHum			0x04   //温湿度传感器
#define SlaveAddr_Noise				0x03   //噪音传感器
#define SlaveAddr_PM2_5PM10		0x0F   //PM传感器      //0x0f   05
#define SlaveAddr_Light				0x07   //光照传感器
#define SlaveAddr_Fan					0x08   //风速传感器    //08 旧   01 新
#define SlaveAddr_Wind_Dir		0x09   //风向传感器		//09      02
****************************************************************/
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define Time_Task_PRIO		5
//任务堆栈大小
#define Time_STK_SIZE		512
//任务堆栈
__align(8)  OS_STK	Time_Date_Task_STK[Time_STK_SIZE];
void Time_Date_Task(void *p_arg);

//任务优先级
#define M485_task_PRIO		7
//任务堆栈大小	
#define M485_STK_SIZE 		512
//任务堆栈	
__align(8)  OS_STK M485_task_STK[M485_STK_SIZE];
void M485_task(void *p_arg);

//任务优先级
#define LED_TASK_PRIO		4
//任务堆栈大小	
#define LED_STK_SIZE 		512
//任务堆栈	
__align(8) OS_STK LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *p_arg);

//任务优先级
#define Usart3_Date_Task_PRIO		6
//任务堆栈大小
#define Usart3_STK_SIZE		512
//任务堆栈
__align(8) OS_STK	Usart3_Date_Task_STK[Usart3_STK_SIZE];
//任务函数
void Usart3_Date_Task(void *p_arg);
//全局变量
const uint8_t Version[]="Ver:V2.1";
uint8_t Power_Once = 1;
Sen_Cache *contex = 0;     //传感器数据缓存指针
vu32 Time_Second_Count = 0; 			//秒计时器
uint8_t Alarm_Photo_Flag = 0; 		//准备拍照标志 大于200置1，检测到PM10大于200且超过15分钟的话拍照
//时间状态 
//bit8，	//天更新标志-缓存数据用
//bit7，	//小时更新标志-缓存数据用
//bit6，	//10分钟更新标志-缓存数据用
//bit5，	//分钟更新标志-缓存数据用
//bit4，	//10秒钟到了
//bit3，	//天更新标志
//bit2，	//小时更新标志
//bit1，	//10分钟更新标志
//bit0，	//分钟更新标志
vu16 Time_STA=0; 
//软件定时器
OS_TMR 	*Second_tmr;		//定时器1
OS_TMR 	*Minute15_tmr;		//定时器2
void Second_tmr_callback(void *p_tmr, void *p_arg);//定时器1的回调函数
void Minute15_tmr_callback(void *p_tmr, void *p_arg);//定时器2的回调函数

OS_EVENT *SYNC_SEM;		//定义一个信号量，用于任务同步
OS_EVENT *SEM_LEDCALLBACK;		//定义一个信号量，LED显示回调
OS_EVENT *SEM_TIME;		//定义一个信号量，LED显示回调


int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	Usart_FIFO_Init();   //485
	usart2_init(115200);		//GPRS通讯口
	uart5_init(115200);
	TIM2_Int_Init();			
	LED_Init();         //LED初始化
	LED_Gpio_Int();     //LED显示屏引脚初始化
	GT_FontIC_Init();    //字库芯片初始化
	//RELAY_Init();				//继电器初始化
	RTC_Init();	  			//RTC初始化
	IWDG_Init(6,1875);    	   //溢出时间为12s	看门狗初始化 
	Init_System_Config_Table(); //初始化系统配置表
	Print_System_Config_Table();  //打印系统配置表
	ClearDisplay(1);	 		//清LED屏显示
	OSInit();  																			//UCOS初始化
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //创建开始任务
	OSStart(); 			
	while(1);
}

//开始任务函数
void start_task(void *pdata)
{
	uint8_t err;
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
	OSTaskCreate(M485_task,(void*)0,(OS_STK*)&M485_task_STK[M485_STK_SIZE-1],M485_task_PRIO);           //创建485任务
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);		            //创建LED显示屏任
	OSTaskCreate(Usart3_Date_Task,(void*)0,(OS_STK*)&Usart3_Date_Task_STK[Usart3_STK_SIZE-1],Usart3_Date_Task_PRIO);		         		 
	OSTaskCreate(Time_Date_Task,(void*)0,(OS_STK*)&Time_Date_Task_STK[Time_STK_SIZE-1],Time_Task_PRIO);		 				 
	SYNC_SEM = OSSemCreate(0);			//创建罗拉信号量
	SEM_LEDCALLBACK = OSSemCreate(0);			
	SEM_TIME = OSSemCreate(0);			
	Minute15_tmr = OSTmrCreate(90000,0,OS_TMR_OPT_ONE_SHOT,Minute15_tmr_callback,0,(INT8U*)"Minute15_tmr",&err);//创建软件定时器
	Second_tmr = OSTmrCreate(0,100,OS_TMR_OPT_PERIODIC,Second_tmr_callback,0,(INT8U*)"Second_tmr",&err);//创建软件定时器		SYNC_SEM = OSSemCreate(0);			//创建罗拉信号量		 
	OSTmrStart(Second_tmr,&err);      //开启秒定时器1	
	OSTaskSuspend(START_TASK_PRIO); //挂起开始任务
	OS_EXIT_CRITICAL();             //退出临界区(开中断)
}
//led0任务函数
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

//led任务函数
const char *Windir[8] = {"正北","东北","正东","东南","正南","西南","正西","西北"};		//用于风向显示
void led_task(void *p_arg)
{ 	
	const  char *wd = NULL;
	const  uint16_t time = 900;
	float tem,humi;
	uint8_t buff[30]={0};			//显示数据缓存
	static uint8_t count=0;
	uint16_t i=0;					//显示的画面编号
	while(1)
	{
		ClearDisplay(1);		
		switch(++count)
		{
			case 1:							//第一幕
				if(Power_Once ==1)			//上电第一次调用
				{
					StaticStringDisplayInPos((uint8_t*)Version,0,0,0);//X,Y
					StaticStringDisplayInPos((uint8_t*)"扬尘噪声",16,16,0);//X,Y
					StaticStringDisplayInPos((uint8_t*)"实时监测系统",0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();
					ClearDisplayLine(0);
					RollingStringDisplayInLine((uint8_t*)System_Config_Table1.Project_Company_name,0,0,6);//X,Y
					Power_Once = 0;
				}
				else
				{
					StaticStringDisplayInPos((uint8_t*)"扬尘噪声",16,16,0);//X,Y
					StaticStringDisplayInPos((uint8_t*)"实时监测系统",0,32,0);//X,Y
					RollingStringDisplayInLine((uint8_t*)System_Config_Table1.Project_Company_name,0,0,6);//X,Y
				} 
			break;
			case 2:						//第二幕				
					sprintf((char *)buff,"噪声:%.1fdB",(float)Noise_Cache.sen_cache_sec_curr/10.0);
					StaticStringDisplayInPos((uint8_t*)"实时数据",16,0,0);//X,Y
					StaticStringDisplayInPos(buff,0,16,0);//X,Y
					if(Noise_Cache.sen_cache_sec_curr<=680)																								sprintf((char *)buff,"等级:优");
					else if(Noise_Cache.sen_cache_sec_curr>680 && Noise_Cache.sen_cache_sec_curr<=900) 		sprintf((char *)buff,"等级:良");
					else if(Noise_Cache.sen_cache_sec_curr>900 && Noise_Cache.sen_cache_sec_curr<=920) 		sprintf((char *)buff,"等级:轻污染");
					else if(Noise_Cache.sen_cache_sec_curr>920 && Noise_Cache.sen_cache_sec_curr<=1040) 	sprintf((char *)buff,"等级:中污染");
					else if(Noise_Cache.sen_cache_sec_curr>1040) 																					sprintf((char *)buff,"等级:重污染");
					StaticStringDisplayInPos(buff,0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();		
			break;
			case 3:						//第三幕	
					sprintf((char *)buff,"PM2.5:%dug",PM2_5_Cache.sen_cache_sec_curr);
					StaticStringDisplayInPos(buff,0,0,0);//X,Y
					sprintf((char *)buff,"PM10 :%dug",PM10_Cache.sen_cache_sec_curr);		
					StaticStringDisplayInPos(buff,0,16,0);//X,Y
					sprintf((char *)buff,"TSP  :%dug",(uint16_t)(0.0349 + 1.2819*PM10_Cache.sen_cache_sec_curr));
					StaticStringDisplayInPos(buff,0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();	
			break;
			case 4:						//第四幕	
				sprintf((char *)buff,"风速:%.1fm/s",(float)FanS_Cache.sen_cache_sec_curr/10.0);
					StaticStringDisplayInPos(buff,0,0,0);//X,Y
					//判断风向
//					if((Win_Dir.sen_cache_sec_curr >= 0 && Win_Dir.sen_cache_sec_curr < 22) || (Win_Dir.sen_cache_sec_curr >= 338 && Win_Dir.sen_cache_sec_curr<=360))	wd = Windir[0];
//					else if(Win_Dir.sen_cache_sec_curr >= 22  && Win_Dir.sen_cache_sec_curr < 67)		wd = Windir[1];
//					else if(Win_Dir.sen_cache_sec_curr >= 67  && Win_Dir.sen_cache_sec_curr < 112)	wd = Windir[2];
//					else if(Win_Dir.sen_cache_sec_curr >= 112 && Win_Dir.sen_cache_sec_curr < 157)	wd = Windir[3];
//					else if(Win_Dir.sen_cache_sec_curr >= 157 && Win_Dir.sen_cache_sec_curr < 202)	wd = Windir[4];
//					else if(Win_Dir.sen_cache_sec_curr >= 202 && Win_Dir.sen_cache_sec_curr < 247)	wd = Windir[5];
//					else if(Win_Dir.sen_cache_sec_curr >= 247 && Win_Dir.sen_cache_sec_curr < 292)	wd = Windir[6];
//					else if(Win_Dir.sen_cache_sec_curr >= 292 && Win_Dir.sen_cache_sec_curr <= 337)	wd = Windir[7];
					//八方位传感器
					if(Win_Dir.sen_cache_sec_curr == 0)			wd = Windir[0];
					else if(Win_Dir.sen_cache_sec_curr ==1)	wd = Windir[1];
					else if(Win_Dir.sen_cache_sec_curr ==2)	wd = Windir[2];
					else if(Win_Dir.sen_cache_sec_curr ==3)	wd = Windir[3];
					else if(Win_Dir.sen_cache_sec_curr ==4)	wd = Windir[4];
					else if(Win_Dir.sen_cache_sec_curr ==5)	wd = Windir[5];
					else if(Win_Dir.sen_cache_sec_curr ==6)	wd = Windir[6];
					else if(Win_Dir.sen_cache_sec_curr ==7)	wd = Windir[7];		
					sprintf((char *)buff,"风向:%s",wd);
					
					StaticStringDisplayInPos((uint8_t*)buff,0,16,0);
					if(FanS_Cache.sen_cache_sec_curr<=2)																								sprintf((char *)buff,"等级:无风");
					else if(FanS_Cache.sen_cache_sec_curr>=3 && FanS_Cache.sen_cache_sec_curr<=15) 			sprintf((char *)buff,"等级:软风");
					else if(FanS_Cache.sen_cache_sec_curr>=16 && FanS_Cache.sen_cache_sec_curr<=33) 		sprintf((char *)buff,"等级:轻风");
					else if(FanS_Cache.sen_cache_sec_curr>=34 && FanS_Cache.sen_cache_sec_curr<=54) 		sprintf((char *)buff,"等级:微风");
					else if(FanS_Cache.sen_cache_sec_curr>=55 && FanS_Cache.sen_cache_sec_curr<=79) 		sprintf((char *)buff,"等级:和风");
					else if(FanS_Cache.sen_cache_sec_curr>=80 && FanS_Cache.sen_cache_sec_curr<=107) 		sprintf((char *)buff,"等级:清风");
					else if(FanS_Cache.sen_cache_sec_curr>=108 && FanS_Cache.sen_cache_sec_curr<=138) 	sprintf((char *)buff,"等级:强风");
					else if(FanS_Cache.sen_cache_sec_curr>=139 && FanS_Cache.sen_cache_sec_curr<=171) 	sprintf((char *)buff,"等级:疾风");
					else if(FanS_Cache.sen_cache_sec_curr>=172 && FanS_Cache.sen_cache_sec_curr<=207) 	sprintf((char *)buff,"等级:大风");
					else if(FanS_Cache.sen_cache_sec_curr>=208 && FanS_Cache.sen_cache_sec_curr<=244) 	sprintf((char *)buff,"等级:烈风");			
					else if(FanS_Cache.sen_cache_sec_curr>=245 && FanS_Cache.sen_cache_sec_curr<=284) 	sprintf((char *)buff,"等级:狂风");
					else if(FanS_Cache.sen_cache_sec_curr>=285 && FanS_Cache.sen_cache_sec_curr<=326) 	sprintf((char *)buff,"等级:暴风");
					else if(FanS_Cache.sen_cache_sec_curr>=327 && FanS_Cache.sen_cache_sec_curr<=369) 	sprintf((char *)buff,"等级:台风");
					StaticStringDisplayInPos(buff,0,32,0);//X,Y
					for(i=0;i<time;i++)
						WriteDisplayTemp();	
			break;
			case 5:						//第五幕	
				tem = (float)Tem_Cache.sen_cache_sec_curr/10.0;
				humi = (float)Hum_Cache.sen_cache_sec_curr/10.0;
				sprintf((char *)buff,"温度:%.1f℃",tem);
				StaticStringDisplayInPos(buff,0,0,0);//X,Y
				sprintf((char *)buff,"湿度:%.1f%%RH",humi);
				StaticStringDisplayInPos(buff,0,16,0);//X,Y
				if(tem<6.0)												sprintf((char *)buff,"舒适度:寒冷");
				else if(tem>=6.0 && tem<11.0) 		sprintf((char *)buff,"舒适度:冷");
				else if(tem>=11.0 && tem<15.0) 		sprintf((char *)buff,"舒适度:凉");
				else if(tem>=15.0 && tem<18.0) 		sprintf((char *)buff,"舒适度:偏凉");
				else if(tem>=18.0 && tem<21.0) 		sprintf((char *)buff,"舒适度:温凉");
				else if(tem>=21.0 && tem<26.0) 		sprintf((char *)buff,"舒适度:舒适");
				else if(tem>=26.0 && tem<30.0) 		sprintf((char *)buff,"舒适度:偏热");
				else if(tem>=30.0) 								sprintf((char *)buff,"舒适度:炎热");			
				StaticStringDisplayInPos(buff,0,32,0);//X,Y
				for(i=0;i<time;i++)
					WriteDisplayTemp();	
			break;
			case 6:						//第六幕  显示日期
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
				count = 0;				//显示的画面编号清零
			break;
			default:		
			break;
		}										 
		delay_ms(10);
	}
}

//Usart3数据处理任务
void Usart3_Date_Task(void *p_arg)
{
	OS_CPU_SR cpu_sr=0;
	uint16_t crc,buffer;
	uint8_t flag=0;
	while(1)
	{
		if((USART2_5_RX_STA&0x8000)!=0)//接收完成
		{
			RTC_Get(); //更新时钟
			sprintf((char *)System_Config_Table1.Calendar,"%d-%d-%d %d:%d:%d %dms",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec,calendar.msec);
			System_Config_Table1.Table_address=FLASH_SAVE_ADDR;   //设备地址
			switch(USART2_5_RX_BUF[0])    //USART2_5_RX_BUF[0]存储命令
			{
				case SYSTEM_ORDER_GETCONFIG:
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						Print_System_Config_Table();  //打印系统配置表					
					}
				break;
				case	SYSTEM_ORDER_RESET:
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						SYSTEMRESET();			//系统复位			
					}
				break;				
				case SYSTEM_ORDER_DUST_GATE:     //设置阀值
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						System_Config_Table1.PM10_low_threshold = (USART2_5_RX_BUF[1]<<8)+USART2_5_RX_BUF[2];
						System_Config_Table1.PM10_high_threshold = (USART2_5_RX_BUF[3]<<8)+USART2_5_RX_BUF[4];
						OS_ENTER_CRITICAL();//进入临界区
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
						OS_EXIT_CRITICAL();	//退出临界区	 
						Print_System_Config_Table();  //打印系统配置表
					}
				break;
				case SYSTEM_ORDER_EQUIP_NUMBER:			//设置设备编号
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						if(Usart2_5_Rec_Cnt>38)  //太长直接退出
						{
							Print_System_Config_Table();  //打印系统配置表		
							break;
						}
						memcpy(System_Config_Table1.Equipment_number,USART2_5_RX_BUF+1,Usart2_5_Rec_Cnt-3);
						System_Config_Table1.Equipment_number[Usart2_5_Rec_Cnt-2]='\0';					//设备编号 <=30位			
						OS_ENTER_CRITICAL();//进入临界区
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
						OS_EXIT_CRITICAL();	//退出临界区	 
						Print_System_Config_Table();  //打印系统配置表		
					}
				break;
				case SYSTEM_ORDER_COMPANY_NAME:			//设置公司名称
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						if(Usart2_5_Rec_Cnt>38)  //太长直接退出
						{
							Print_System_Config_Table();  //打印系统配置表		
							break;
						}
						memcpy(System_Config_Table1.Project_Company_name,USART2_5_RX_BUF+1,Usart2_5_Rec_Cnt-3);
						System_Config_Table1.Project_Company_name[Usart2_5_Rec_Cnt-2]='\0';					//公司名称 <=30位			
						OS_ENTER_CRITICAL();//进入临界区
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof (System_Config_Table1)/2);   //保存FLASH
						OS_EXIT_CRITICAL();	//退出临界区	 
						Print_System_Config_Table();  //打印系统配置表		
					}
				break;
				case SYSTEM_ORDER_DUST_FACTOR:			//设置粉尘系数
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						System_Config_Table1.PM2_5_k_factor = ((USART2_5_RX_BUF[1]<<8)+USART2_5_RX_BUF[2])/10.0;
						System_Config_Table1.PM2_5_b_factor = USART2_5_RX_BUF[3];
						System_Config_Table1.PM10_k_factor = ((USART2_5_RX_BUF[4]<<8)+USART2_5_RX_BUF[5])/10.0;
						System_Config_Table1.PM10_b_factor = USART2_5_RX_BUF[6];
						OS_ENTER_CRITICAL();//进入临界区
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof (System_Config_Table1)/2);   //保存FLASH
						OS_EXIT_CRITICAL();	//退出临界区	 
						Print_System_Config_Table();  //打印系统配置表		
					}
				break;
				case SYSTEM_ORDER_DTU_INIT:			//DTU初始化
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						USART2_5_RX_STA = 0; //清空  准备接收数据
						Dtu_Init();
					}
				break;
				case SYSTEM_ORDER_TIME_SET:			//时间校准
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						RTC_Set(2000 + USART2_5_RX_BUF[1],USART2_5_RX_BUF[2],USART2_5_RX_BUF[3],USART2_5_RX_BUF[4],USART2_5_RX_BUF[5],USART2_5_RX_BUF[6]);
						Power_up_rtc = 0;
						Print_Calander();    //打印时间
					}
				break;		
				case SYSTEM_ORDER_APP_UPDATE:			//APP升级
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						buffer = BOOTLEADER_FLAG_ON;
						OS_ENTER_CRITICAL();//进入临界区
						STMFLASH_Write(BOOTLEADER_FLAG_ADDR,&buffer,1);
						OS_EXIT_CRITICAL();	//退出临界区	 
						SYSTEMRESET();			//系统复位
					}
				break;
				case	SYSTEM_ORDER_REALTIME_DATA:		//获取实时数据
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						Print_Calander();    //打印时间
						printf(" 温度 ：%.1f℃\n 湿度 ：%.1f%%RH\n 风速 ：%.1f m/s\n 噪音 ：%.1fdB\n PM2.5：%dug/m3\n PM10 ：%dug/m3\n TSP  ：%.1fug/m3\n 风向 ：%d度\n"
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
				case	SYSTEM_ORDER_DTU_AIP_PR:		//改变DTU的A IP及其端口号	
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						memcpy(&System_Config_Table1.Socka,USART2_5_RX_BUF+2,Usart2_5_Rec_Cnt-6);
						System_Config_Table1.Socka.enable = USART2_5_RX_BUF[1];
						System_Config_Table1.Socka.pr = (USART2_5_RX_BUF[6]<<8) + USART2_5_RX_BUF[7];
						USART2_5_RX_STA = 0; //清空  准备接收数据
						flag = Config_Socka(1,System_Config_Table1.Socka.enable);
						if(1 == flag)
						{
							OS_ENTER_CRITICAL();//进入临界区
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
							OS_EXIT_CRITICAL();	//退出临界区
							Print_System_Config_Table();  //打印系统配置表									
						}
					}
				break;	
				case	SYSTEM_ORDER_DTU_BIP_PR:		//改变DTU的B IP及其端口号	
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						memcpy(&System_Config_Table1.Sockb,USART2_5_RX_BUF+2,Usart2_5_Rec_Cnt-6);
						System_Config_Table1.Sockb.enable = USART2_5_RX_BUF[1];
						System_Config_Table1.Sockb.pr = (USART2_5_RX_BUF[6]<<8) + USART2_5_RX_BUF[7];
						USART2_5_RX_STA = 0;		//清空  准备接收数据
						flag = Config_Socka(2,System_Config_Table1.Sockb.enable);
						if(1 == flag)
						{
							OS_ENTER_CRITICAL();//进入临界区
							STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
							OS_EXIT_CRITICAL();	//退出临界区	 
							Print_System_Config_Table();  //打印系统配置表		
						}
					}
				break;
				case SYSTEM_ORDER_SHOW_TYPE:
					crc = CRC16_Modbus(USART2_5_RX_BUF,Usart2_5_Rec_Cnt-2);
					if(crc == (USART2_5_RX_BUF[Usart2_5_Rec_Cnt-1]<<8) + USART2_5_RX_BUF[Usart2_5_Rec_Cnt-2])
					{
						System_Config_Table1.Led_Show_Type = USART2_5_RX_BUF[1];
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
						Print_System_Config_Table();  //打印系统配置表		
					}
				break;
				default:	
				break;
			}
			USART2_5_RX_STA=0x0000;     //清楚标志位
			Usart2_5_Rec_Cnt=0;
		}
		delay_ms(100);	
	}
}
//时间处理任务函数
void Time_Date_Task(void *p_arg)
{
	uint8_t err;
	while(1)
	{
		OSSemPend(SEM_TIME, 0, &err);
		//发送报文
		if((Time_STA & TENSECOND) != 0)
		{
			Time_STA &=(~TENSECOND);
			Send_CloudData();								//发送数据
			Heart_Beat();
		}
	  else if((Time_STA & MINUTEFLAG) != 0)						//1分钟到了
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
		//当浓度大于PM10_high_threshold 并且Alarm_Photo_Flag为0时开启定时器
		if(PM10_Cache.sen_cache_sec_curr >= System_Config_Table1.PM10_high_threshold  && Alarm_Photo_Flag == 0)
		{
			Alarm_Photo_Flag = 1;
			//printf("开启15min定时器\n");					//调试时可以打开观察
			OSTmrStart(Minute15_tmr,&err);      //开启15min定时器
		}
		else if(PM10_Cache.sen_cache_sec_curr < System_Config_Table1.PM10_high_threshold)
		{
			Alarm_Photo_Flag = 0;
			//printf("重新开启15min定时器\n");
			OSTmrStart(Minute15_tmr,&err);      //开启15min定时器
		}
		if(Time_Second_Count >= 0xFFFFFFFF)    //秒时间溢出处理
			Time_Second_Count=0;
		
		delay_ms(100);	
	}
}
	

//定时器1的回调函数
uint8_t Update_Day_Tmr = 0;			//天更新标志 用于定时器Minute15
uint8_t Photo_Button_Flag = 0;  //拍照标志
uint8_t Photo_Button_Delay = 0;  //拍照延时时间
void Second_tmr_callback(void *p_tmr, void *p_arg)
{
	Time_Second_Count++;
	LED0=~LED0;
	IWDG->KR = (uint16_t)0xAAAA;  					//喂独立看门狗

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
		//一小时到了
	else if((Time_STA & HOURFLAG) != 0)
	{	
		OSSemPost(SEM_TIME);
	}
		//一天到了
	else if((Time_STA & DAYFLAG) != 0)
	{	
		Update_Day_Tmr = 1;													//用于Minute15_tmr_callback里面清数据用
		OSSemPost(SEM_TIME);
	}
	//继电器闭合，隔2S断开继电器
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
//当PM10大于阀值，且大于15分钟 调用15分钟回调函数
void Minute15_tmr_callback(void *p_tmr, void *p_arg)//定时器2的回调函数
{
	static uint32_t minute15_count=0;
	Alarm_Photo_Flag = 0;
	if(Update_Day_Tmr == 1)							//新的一天更新 minute15_count
	{
		Update_Day_Tmr = 0;
		minute15_count = 0;
	}
	minute15_count++;
	if(minute15_count <= 4)								//第一次1小时内 每隔15分钟进一次
	{
//		RELAY_PHOTO_BUTTON = 1;							//置拍照标志位
		Photo_Button_Flag = 1;
		printf("Minute15_tmr_callback,PM10大于200ug/m3 超过15min minute15_count = %d\n",minute15_count);
	}
	else if(minute15_count == 6 && minute15_count == 8 )	//大于4次 每隔30分钟进一次
	{
//		RELAY_PHOTO_BUTTON = 1;							//置拍照标志位
		Photo_Button_Flag = 1;
		printf("Minute15_tmr_callback,PM10大于200ug/m3 超过15min minute15_count = %d\n",minute15_count);
	}
	else if(minute15_count > 8 && (minute15_count % 4 == 0))//大于8次 每隔一小时进一次
	{
//		RELAY_PHOTO_BUTTON = 1;
		Photo_Button_Flag = 1;
		printf("Minute15_tmr_callback,PM10大于200ug/m3 超过15min minute15_count = %d\n",minute15_count);
	}
}
