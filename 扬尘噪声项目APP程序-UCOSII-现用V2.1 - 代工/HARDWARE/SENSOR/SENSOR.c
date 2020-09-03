#include "SENSOR.h"
#include "string.h"
#include "modbus_host.h"
#include "usart5.h"
#include "usart3.h"
#include "link_list.h"
#include "rtc.h" 
#include "malloc.h"
#include "dtu.h"
#include "includes.h"					//ucos 使用	
#include "timer.h" 
#include "delay.h"
extern vu32 Time_Second_Count; 			//秒计时器
Sen_Protocol Sen_Protocol1;	 				//协议包结构体

u8 Protocol[1000] ={0} ;            //协议包发送缓存数组

Sen_Cache  FanS_Cache;				//风速传感器缓存结构体
Sen_Cache  Tem_Cache;					//温度传感器缓存结构体
Sen_Cache  Hum_Cache;					//湿度传感器缓存结构体
Sen_Cache  Noise_Cache;				//噪音传感器	缓存结构体	
Sen_Cache  PM2_5_Cache;				//PM2.5缓存结构体
Sen_Cache  PM10_Cache;				//PM10缓存结构体
//Sen_Cache  Light_Cache;				//光照	缓存结构体
//Sen_Cache  Atmo_Cache;				//气压缓存结构体
Sen_Cache  Win_Dir;						//风向缓存结构体
//获取传感器缓存地址
/*
*********************************************************************************************************
*	函 数 名: Get_Sen_Cache
*	功能说明: 获取对应传感器缓存地址 
*	形    参: Sensor :想要获得的数据缓冲区,
*	返 回 值: 无
*********************************************************************************************************
*/
Sen_Cache *Get_Sen_Cache(Sensor_Type Sensor)
{
	switch(Sensor)
	{
		case FAN:
			return &FanS_Cache;
		case TEM:
			return &Tem_Cache;
		case HUM:
			return &Hum_Cache;
		case NOISE:
			return &Noise_Cache;
		case PM2_5:
			return &PM2_5_Cache;
		case PM10:
			return &PM10_Cache;	
		case WINDIR:
			return &Win_Dir;	
	}
	return NULL;	
}

/*
*********************************************************************************************************
*	函 数 名: CRC16_Checkout
*	功能说明: CRC16校验  
*	形    参: _pBuf :数据缓冲区,
						_usLen ：数据长度,
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t CRC16_Checkout(uint8_t *_pBuf, uint16_t _usLen)     
{
	uint16_t i,j,crc_reg,check;
	crc_reg=0xFFFF;
	for(i=0;i<_usLen;i++)
	{
		crc_reg=(crc_reg>>8)^_pBuf[i];
		for(j=0;j<8;j++)
		{
			check=crc_reg&0x0001;
			crc_reg>>=1;
			if(check==0x0001)
			{
				crc_reg^=0xA001;
			}
		}
	}
	return crc_reg;
}
/*
*********************************************************************************************************
*	函 数 名: Data_Average
*	功能说明: 求最平均值  
*	形    参: _pBuf :数据缓冲区,
						_usLen ：数据长度,
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t Data_Average(int16_t *_pBuf, uint16_t _usLen)
{
	int16_t i,average=0;
	int32_t sum=0;
	if(_usLen == 0)     //分母不为0
		return 0;
	for(i=0;i<_usLen;i++)
	{
		sum+=_pBuf[i];
	}
	average=sum/_usLen;
	return average;
}
/*
*********************************************************************************************************
*	函 数 名: Data_Max
*	功能说明: 求最大值  
*	形    参: _pBuf :数据缓冲区,
						_usLen ：数据长度,
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t Data_Max(int16_t *_pBuf,uint16_t _usLen)
{
	int16_t max=_pBuf[0],i;
	for(i=1;i<_usLen;i++)
	{
		if(_pBuf[i]>max)
		{
			max=_pBuf[i];
		}
	}
	return max;
}
/*
*********************************************************************************************************
*	函 数 名: Data_Min
*	功能说明: 求最小值  
*	形    参: _pBuf :数据缓冲区,
						_usLen ：数据长度,
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t Data_Min(int16_t *_pBuf,uint16_t _usLen)
{
	int16_t min=_pBuf[0],i;
	for(i=1;i<_usLen;i++)
	{
		if(_pBuf[i]<min)
		{
			min=_pBuf[i];
		}
	}
	return min;
}

extern vu16 Time_STA;						//时间标志位
uint8_t Time_Update=0;					//空 去警告
/*
*********************************************************************************************************
*	函 数 名: Sensor_Add_Date
*	功能说明: 填充数据段  
*	形    参: Sen_Cache1 :数据缓存结构体,
						Sensor_Data ：要传入的数据,
					  Time_Update_SEN : 时间强制更新标识  1：强制更新，Sensor_Data无效   0：压数据
*	返 回 值: 无
*********************************************************************************************************
*/
void Sensor_Add_Date(Sen_Cache *Sen_Cache1,uint8_t fack,uint8_t Time_Update_SEN)
{
	static uint8_t Sensor_Count=0;
	int16_t Average = 0,Minimal = 0,Maximum = 0;     		//保存平均值 最小值  最大值
	if(Time_Update_SEN == 1)														//时间强制更新  不压数据
		Time_Update = Time_Update_SEN;										//去警告
	else
	{
		if(fack)			//判断是否有传感器数据
		{ 
			Sen_Cache1->sen_cache_sec[Sen_Cache1->count_sec] = Sen_Cache1->sen_cache_sec_curr;		//压数据
			Sen_Cache1->count_sec++;
			Sen_Cache1->Offline = 0;
			if(Sen_Cache1->count_sec >= 60 )									//防止超过60
				Sen_Cache1->count_sec = 60;
		}
		else
		{
			Sen_Cache1->Offline++;
			if(Sen_Cache1->Offline>=4)
			{
				Sen_Cache1->Offline = 0;
				Sen_Cache1->sen_cache_sec_curr = 0;
				Sen_Cache1->sen_cache_sec[0] = 0;   //清零  为了下面求最小值、最大值老是指向第一个元素导致非零
			}
		}
	}
	if((Time_STA & MINUTEFLAG_SEN) != 0)														//超过1分钟
	{
		Sensor_Count++;
		if(Sensor_Count == SENSOR_AMOUNT)								//SENSOR_AMOUNT传感器的数量
		{
			Sensor_Count = 0;
			Time_STA &=(~MINUTEFLAG_SEN); 								//全部传感器弄完了，清理标志位
		}
		Average = Data_Average(Sen_Cache1->sen_cache_sec,Sen_Cache1->count_sec);				//一分钟求平均
		Minimal = Data_Min(Sen_Cache1->sen_cache_sec,Sen_Cache1->count_sec);						//一分钟求最小
		Maximum = Data_Max(Sen_Cache1->sen_cache_sec,Sen_Cache1->count_sec);						//一分钟求最大
		Sen_Cache1->count_sec = 0;
		//平均值
		Sen_Cache1->sen_cache_ave_min[Sen_Cache1->count_min] = Average;
		Sen_Cache1->sen_cache_ave_min_curr = Average;
		//最小值
		Sen_Cache1->sen_cache_min_min[Sen_Cache1->count_min] = Minimal;
		Sen_Cache1->sen_cache_min_min_curr = Minimal;
		//最大值
		Sen_Cache1->sen_cache_max_min[Sen_Cache1->count_min] = Maximum;
		Sen_Cache1->sen_cache_max_min_curr = Maximum;
		Sen_Cache1->count_min++;
		if(Sen_Cache1->count_min >= 10 )							//防止超过10
			Sen_Cache1->count_min = 10;
	}
	else if((Time_STA & TENMINUTEFLAG_SEN) != 0)						//超过10分钟
	{
		Sensor_Count++;
		if(Sensor_Count == SENSOR_AMOUNT)
		{
			Sensor_Count = 0;
			Time_STA &=(~TENMINUTEFLAG_SEN); 
		}
		Average = Data_Average(Sen_Cache1->sen_cache_ave_min,Sen_Cache1->count_min);   		  //10分钟求平均
		Minimal = Data_Min(Sen_Cache1->sen_cache_min_min,Sen_Cache1->count_min);						//10分钟求最小
		Maximum = Data_Max(Sen_Cache1->sen_cache_max_min,Sen_Cache1->count_min);						//10分钟求最大	
		Sen_Cache1->count_min = 0;
		//平均值
		Sen_Cache1->sen_cache_ave_10min[Sen_Cache1->count_10min] = Average;
		Sen_Cache1->sen_cache_ave_10min_curr = Average;
		//最小值
		Sen_Cache1->sen_cache_min_10min[Sen_Cache1->count_10min] = Minimal;
		Sen_Cache1->sen_cache_min_10min_curr = Minimal;
		//最大值
		Sen_Cache1->sen_cache_max_10min[Sen_Cache1->count_10min] = Maximum;
		Sen_Cache1->sen_cache_max_10min_curr = Maximum;
		Sen_Cache1->count_10min++;
		if(Sen_Cache1->count_10min >= 6 )						//防止超过6
			Sen_Cache1->count_10min = 6;
	}
	else if((Time_STA & HOURFLAG_SEN) != 0)				//超过1小时
	{
		Sensor_Count++;
		if(Sensor_Count == SENSOR_AMOUNT)
		{
			Sensor_Count = 0;
			Time_STA &=(~HOURFLAG_SEN); 
		}
		Average = Data_Average(Sen_Cache1->sen_cache_ave_10min,Sen_Cache1->count_10min);     		//小时求平均
		Minimal = Data_Min(Sen_Cache1->sen_cache_min_10min,Sen_Cache1->count_10min);						//小时求最小
		Maximum = Data_Max(Sen_Cache1->sen_cache_max_10min,Sen_Cache1->count_10min);						//小时求最大	
		Sen_Cache1->count_10min = 0;
		//平均值
		Sen_Cache1->sen_cache_ave_hour[Sen_Cache1->count_hour] = Average;
		Sen_Cache1->sen_cache_ave_hour_curr = Average;
		//最小值
		Sen_Cache1->sen_cache_min_hour[Sen_Cache1->count_hour] = Minimal;
		Sen_Cache1->sen_cache_min_hour_curr = Minimal;
		//最大值
		Sen_Cache1->sen_cache_max_hour[Sen_Cache1->count_hour] = Maximum;
		Sen_Cache1->sen_cache_max_hour_curr = Maximum;
		Sen_Cache1->count_hour++;
		if(Sen_Cache1->count_hour >= 24 )
			Sen_Cache1->count_hour = 24;
	}
	else if((Time_STA & DAYFLAG_SEN) != 0)				//超过1天
	{
		Sensor_Count++;
		if(Sensor_Count == SENSOR_AMOUNT)
		{
			Sensor_Count = 0;
			Time_STA &=(~DAYFLAG_SEN); 
		}
		Average = Data_Average(Sen_Cache1->sen_cache_ave_hour,Sen_Cache1->count_hour);    		//天求平均
		Minimal = Data_Min(Sen_Cache1->sen_cache_min_hour,Sen_Cache1->count_hour);						//天求最小
		Maximum = Data_Max(Sen_Cache1->sen_cache_max_hour,Sen_Cache1->count_hour);						//天求最大						
		Sen_Cache1->count_hour = 0;
		Sen_Cache1->sen_cache_ave_day_curr = Average;
		Sen_Cache1->sen_cache_min_day_curr = Minimal;
		Sen_Cache1->sen_cache_max_day_curr = Maximum;
	}

}
/*
*********************************************************************************************************
*	函 数 名: Fit_Sen_Protocol
*	功能说明: 填充数据段  
*	形    参: Type：	MINUTE,				哪个时间段的数据
									TEN_MINUTE,
									HOUR,
									DAY,
*	返 回 值: 无
*********************************************************************************************************
*/
void Fit_Sen_Data(Send_Time_Type Type)
{
	sprintf((char *)Sen_Protocol1.Sen_Data1.QN,"QN=%0004d%02d%02d%02d%02d%02d%003d;",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec,calendar.msec);
	sprintf((char *)Sen_Protocol1.Sen_Data1.MN,"MN=%s;Flag=5;",System_Config_Table1.Equipment_number);
	sprintf((char *)Sen_Protocol1.Sen_Data1.ST,"ST=%d;",SYSTEM_CODING);
	sprintf((char *)Sen_Protocol1.Sen_Data1.PW,"PW=%d;",SECRET);
	sprintf((char *)Sen_Protocol1.Sen_Data1.CP0,"CP=&&DataTime=%0004d%02d%02d%02d%02d%02d;",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
	Sensor_Add_Date(&Tem_Cache,0,1);
	Sensor_Add_Date(&Hum_Cache,1,1);
	Sensor_Add_Date(&FanS_Cache,2,1);
	Sensor_Add_Date(&Noise_Cache,3,1);
	Sensor_Add_Date(&PM2_5_Cache,4,1);
	Sensor_Add_Date(&PM10_Cache,5,1);
	Sensor_Add_Date(&Win_Dir,6,1);	
	switch(Type)
	{
		case MINUTE://分钟数据
			sprintf((char *)Sen_Protocol1.Sen_Data1.CN,"CN=%d;",COMMAND_ENCODING2011);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP1,"a01001-Rtd=%.1f,a01001-Flag=N;",(float)Tem_Cache.sen_cache_sec_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP2,"a01002-Rtd=%.1f,a01002-Flag=N;",(float)Hum_Cache.sen_cache_sec_curr/10.0);
//			sprintf((char *)Sen_Protocol1.Sen_Data1.CP3,"a01006-Rtd=%.1f,a01006-Flag=N;",(float)Atmo_Cache.sen_cache_sec_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP4,"a01007-Rtd=%.1f,a01007-Flag=N;",(float)FanS_Cache.sen_cache_sec_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP5,"a01008-Rtd=%d,a01008-Flag=N;",Win_Dir.sen_cache_sec_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP6,"a34002-Rtd=%d,a34002-Flag=N;",PM10_Cache.sen_cache_sec_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP7,"a34004-Rtd=%d,a34004-Flag=N;",PM2_5_Cache.sen_cache_sec_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP8,"a50001-Rtd=%.1f,a50001-Flag=N;",(float)Noise_Cache.sen_cache_sec_curr/10.0);	
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP9,"a34001-Rtd=%.1f,a34001-Flag=N&&",(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_sec_curr));	
		
		break;
		case TEN_MINUTE://10分钟数据
			sprintf((char *)Sen_Protocol1.Sen_Data1.CN,"CN=%d;",COMMAND_ENCODING2051);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP1,"a01001-Max=%.1f,a01001-Flag=N,a01001-Min=%.1f,a01001-Avg=%.1f;",(float)Tem_Cache.sen_cache_max_10min_curr/10.0,(float)Tem_Cache.sen_cache_min_10min_curr/10.0,(float)Tem_Cache.sen_cache_ave_10min_curr/10.0);    // *******************************************       																																					//
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP2,"a01002-Max=%.1f,a01002-Flag=N,a01002-Min=%.1f,a01002-Avg=%.1f;",(float)Hum_Cache.sen_cache_max_10min_curr/10.0,(float)Hum_Cache.sen_cache_min_10min_curr/10.0,(float)Hum_Cache.sen_cache_ave_10min_curr/10.0);// *******************************************       
//			sprintf((char *)Sen_Protocol1.Sen_Data1.CP3,"a01006-Max=%.1f,a01006-Flag=N,a01006-Min=%.1f,a01006-Avg=%.1f;",(float)Atmo_Cache.sen_cache_max_10min_curr/10.0,(float)Atmo_Cache.sen_cache_min_10min_curr/10.0,(float)Atmo_Cache.sen_cache_ave_10min_curr/10.0);// *******************************************       
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP4,"a01007-Max=%.1f,a01007-Flag=N,a01007-Min=%.1f,a01007-Avg=%.1f;",(float)FanS_Cache.sen_cache_max_10min_curr/10.0,(float)FanS_Cache.sen_cache_min_10min_curr/10.0,(float)FanS_Cache.sen_cache_ave_10min_curr/10.0);// *******************************************    
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP5,"a01008-Max=%d,a01008-Flag=N,a01008-Min=%d,a01008-Avg=%d;",Win_Dir.sen_cache_max_10min_curr,Win_Dir.sen_cache_min_10min_curr,Win_Dir.sen_cache_ave_10min_curr);		
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP6,"a34002-Max=%d,a34002-Flag=N,a34002-Min=%d,a34002-Avg=%d;",PM10_Cache.sen_cache_max_10min_curr,PM10_Cache.sen_cache_min_10min_curr,PM10_Cache.sen_cache_ave_10min_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP7,"a34004-Max=%d,a34004-Flag=N,a34004-Min=%d,a34004-Avg=%d;",PM2_5_Cache.sen_cache_max_10min_curr,PM2_5_Cache.sen_cache_min_10min_curr,PM2_5_Cache.sen_cache_ave_10min_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP8,"a50001-Max=%.1f,a50001-Flag=N,a50001-Min=%.1f,a50001-Avg=%.1f;",(float)Noise_Cache.sen_cache_max_10min_curr/10.0,(float)Noise_Cache.sen_cache_min_10min_curr/10.0,(float)Noise_Cache.sen_cache_ave_10min_curr/10.0);	// *******************************************  
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP9,"a34001-Max=%.1f,a34001-Flag=N,a34001-Min=%.1f,a34001-Avg=%.1f&&",(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_max_10min_curr),(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_min_10min_curr),(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_ave_10min_curr));
		break;
		case HOUR://小时数据
			sprintf((char *)Sen_Protocol1.Sen_Data1.CN,"CN=%d;",COMMAND_ENCODING2061);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP1,"a01001-Max=%.1f,a01001-Flag=N,a01001-Min=%.1f,a01001-Avg=%.1f;",(float)Tem_Cache.sen_cache_max_hour_curr/10.0,(float)Tem_Cache.sen_cache_min_hour_curr/10.0,(float)Tem_Cache.sen_cache_ave_hour_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP2,"a01002-Max=%.1f,a01002-Flag=N,a01002-Min=%.1f,a01002-Avg=%.1f;",(float)Hum_Cache.sen_cache_max_hour_curr/10.0,(float)Hum_Cache.sen_cache_min_hour_curr/10.0,(float)Hum_Cache.sen_cache_ave_hour_curr/10.0);// *******************************************       
//			sprintf((char *)Sen_Protocol1.Sen_Data1.CP3,"a01006-Max=%.1f,a01006-Flag=N,a01006-Min=%.1f,a01006-Avg=%.1f;",(float)Atmo_Cache.sen_cache_max_hour_curr/10.0,(float)Atmo_Cache.sen_cache_min_hour_curr/10.0,(float)Atmo_Cache.sen_cache_ave_hour_curr/10.0);                
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP4,"a01007-Max=%.1f,a01007-Flag=N,a01007-Min=%.1f,a01007-Avg=%.1f;",(float)FanS_Cache.sen_cache_max_hour_curr/10.0,(float)FanS_Cache.sen_cache_min_hour_curr/10.0,(float)FanS_Cache.sen_cache_ave_hour_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP5,"a01008-Max=%d,a50001-Flag=N,a01008-Min=%d,a01008-Avg=%d;",Win_Dir.sen_cache_max_hour_curr,Win_Dir.sen_cache_min_hour_curr,Win_Dir.sen_cache_ave_hour_curr);	
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP6,"a34002-Max=%d,a34002-Flag=N,a34002-Min=%d,a34002-Avg=%d;",PM10_Cache.sen_cache_max_hour_curr,PM10_Cache.sen_cache_min_hour_curr,PM10_Cache.sen_cache_ave_hour_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP7,"a34004-Max=%d,a34004-Flag=N,a34004-Min=%d,a34004-Avg=%d;",PM2_5_Cache.sen_cache_max_hour_curr,PM2_5_Cache.sen_cache_min_hour_curr,PM2_5_Cache.sen_cache_ave_hour_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP8,"a50001-Max=%.1f,a50001-Flag=N,a50001-Min=%.1f,a50001-Avg=%.1f;",(float)Noise_Cache.sen_cache_max_hour_curr/10.0,(float)Noise_Cache.sen_cache_min_hour_curr/10.0,(float)Noise_Cache.sen_cache_ave_hour_curr/10.0);	
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP9,"a34001-Max=%.1f,a34001-Flag=N,a34001-Min=%.1f,a34001-Avg=%.1f&&",(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_max_hour_curr),(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_min_hour_curr),(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_ave_hour_curr));
		break;
		case DAY://一天数据
			sprintf((char *)Sen_Protocol1.Sen_Data1.CN,"CN=%d;",COMMAND_ENCODING2031);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP1,"a01001-Max=%.1f,a01001-Flag=N,a01001-Min=%.1f,a01001-Avg=%.1f;",(float)Tem_Cache.sen_cache_max_day_curr/10.0,(float)Tem_Cache.sen_cache_min_day_curr/10.0,(float)Tem_Cache.sen_cache_ave_day_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP2,"a01002-Max=%.1f,a01002-Flag=N,a01002-Min=%.1f,a01002-Avg=%.1f;",(float)Hum_Cache.sen_cache_max_day_curr/10.0,(float)Hum_Cache.sen_cache_min_day_curr/10.0,(float)Hum_Cache.sen_cache_ave_day_curr/10.0);
//			sprintf((char *)Sen_Protocol1.Sen_Data1.CP3,"a01006-Max=%.1f,a01006-Flag=N,a01006-Min=%.1f,a01006-Avg=%.1f;",(float)Atmo_Cache.sen_cache_max_day_curr/10.0,(float)Atmo_Cache.sen_cache_min_day_curr/10.0,(float)Atmo_Cache.sen_cache_ave_day_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP4,"a01007-Max=%.1f,a01007-Flag=N,a01007-Min=%.1f,a01007-Avg=%.1f;",(float)FanS_Cache.sen_cache_max_day_curr/10.0,(float)FanS_Cache.sen_cache_min_day_curr/10.0,(float)FanS_Cache.sen_cache_ave_day_curr/10.0);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP5,"a01008-Max=%d,a01008-Flag=N,a01008-Min=%d,a01008-Avg=%d;",Win_Dir.sen_cache_max_day_curr,Win_Dir.sen_cache_min_day_curr,Win_Dir.sen_cache_ave_day_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP6,"a34002-Max=%d,a34002-Flag=N,a34002-Min=%d,a34002-Avg=%d;",PM10_Cache.sen_cache_max_day_curr,PM10_Cache.sen_cache_min_day_curr,PM10_Cache.sen_cache_ave_day_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP7,"a34004-Max=%d,a34004-Flag=N,a34004-Min=%d,a34004-Avg=%d;",PM2_5_Cache.sen_cache_max_day_curr,PM2_5_Cache.sen_cache_min_day_curr,PM2_5_Cache.sen_cache_ave_day_curr);
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP8,"a50001-Max=%.1f,a50001-Flag=N,a50001-Min=%.1f,a50001-Avg=%.1f;",(float)Noise_Cache.sen_cache_max_day_curr/10.0,(float)Noise_Cache.sen_cache_min_day_curr/10.0,(float)Noise_Cache.sen_cache_ave_day_curr/10.0);	
			sprintf((char *)Sen_Protocol1.Sen_Data1.CP9,"a34001-Max=%.1f,a34001-Flag=N,a34001-Min=%.1f,a34001-Avg=%.1f&&",(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_max_day_curr),(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_min_day_curr),(float)(0.0349 + 1.2819*PM10_Cache.sen_cache_ave_day_curr));
		break;
		
	}
	//计算长度
	Sen_Protocol1.DataLen0=strlen((char *)Sen_Protocol1.Sen_Data1.QN)+strlen((char *)Sen_Protocol1.Sen_Data1.MN)+strlen((char *)Sen_Protocol1.Sen_Data1.ST)+strlen((char *)Sen_Protocol1.Sen_Data1.CN)+strlen((char *)Sen_Protocol1.Sen_Data1.PW)+strlen((char *)Sen_Protocol1.Sen_Data1.CP0)+strlen((char *)Sen_Protocol1.Sen_Data1.CP1)+strlen((char *)Sen_Protocol1.Sen_Data1.CP2)+strlen((char *)Sen_Protocol1.Sen_Data1.CP4)+strlen((char *)Sen_Protocol1.Sen_Data1.CP5)+strlen((char *)Sen_Protocol1.Sen_Data1.CP6) + strlen((char *)Sen_Protocol1.Sen_Data1.CP7)+ strlen((char *)Sen_Protocol1.Sen_Data1.CP8)+ strlen((char *)Sen_Protocol1.Sen_Data1.CP9);													
}       
/*
*********************************************************************************************************
*	函 数 名: Fit_Sen_Protocol
*	功能说明: 填充通讯包并连接好  
*	形    参: Type：	MINUTE,				哪个时间段的数据
									TEN_MINUTE,
									HOUR,
									DAY,
*	返 回 值: 无
*********************************************************************************************************
*/
void Fit_Sen_Protocol(Send_Time_Type Type)
{
	uint16_t CRC16=0,len=800;
	uint8_t *Protocol;
	//LinkNode *cur = usrgm3_head;
	if(Type==MINUTE)
		len = 450;
	Protocol = mymalloc(len);
	memset(Protocol,0,len);
	if(linklistSize(usrgm3_head)>=LINK_LIST_LENTH)					//链表长度过长
		linklistPopFront(&usrgm3_head);      		//头的数据删除掉
	linklistPushBack(&usrgm3_head,Protocol);		//插入链表	
	Fit_Sen_Data(Type);//填充数据段
 	strcpy((char *)Sen_Protocol1.Hender,"##");
	sprintf((char *)Sen_Protocol1.DataLen,"%0004d",Sen_Protocol1.DataLen0);
	strcpy((char *)Sen_Protocol1.SecretFlag,"0");
	strcpy((char *)Sen_Protocol1.Tail,"\r\n");
	//连接协议包
	strcat((char *)Protocol,(char *)Sen_Protocol1.Hender);
	strcat((char *)Protocol,(char *)Sen_Protocol1.DataLen);
	strcat((char *)Protocol,(char *)Sen_Protocol1.SecretFlag);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.QN);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.MN);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.ST);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CN);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.PW);
	//数据区
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP0);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP1);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP2);
//	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP3);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP4);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP5);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP6);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP7);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP8);
	strcat((char *)Protocol,(char *)Sen_Protocol1.Sen_Data1.CP9);
	CRC16=CRC16_Checkout((uint8_t *)(Protocol+7),Sen_Protocol1.DataLen0);
	sprintf((char *)Sen_Protocol1.Crc,"%0004X",CRC16);
	strcat ((char *)Protocol,(char *)Sen_Protocol1.Crc);
	strcat ((char *)Protocol,(char *)Sen_Protocol1.Tail);
}
void Send_CloudData(void)
{
	uint16_t len,use;
	uint8_t connect;
	connect = Test_Connected(1);
	len = linklistSize(usrgm3_head);
	use =	my_mem_perused();
	
	if(connect == 1)
	{
		if(len != 0)
		{
			u5_printf("%s",usrgm3_head->data);								//串口2发送协议包
			printf("%s",usrgm3_head->data);									//串口3发送协议包		
			linklistPopFront(&usrgm3_head);      		//头的数据删除掉		
		}
	}
	else if(connect == 0)
	{
		printf("链表长度:%d     内存使用率：%d%%\r\n",len,use);	
		printf("SOCKETA disconnected\r\n");	
	}
	else 
		printf("ERR\r\n");	
}
/*
*********************************************************************************************************
*	函 数 名: Heart_Beat
*	功能说明: 发送10S心跳  
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Heart_Beat(void)
{
	printf("Heart_Beating...Second_count=%d\n",Time_Second_Count+1);
	//u2_printf(1,"Heart_Beating...Second_count=%d\n",Time_Second_Count);
}


