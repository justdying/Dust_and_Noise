#ifndef __SENSOR_H
#define __SENSOR_H	 
#include "sys.h"  


//#define REISSUAN_MODE    				//重发模式
#define MAX_SEND_TIMES (2)			//最大重发次数	
#define SENSOR_AMOUNT (7)				//当传感器数量改变时  一定要修改这个值
#define SYSTEM_CODING (22)      //扬尘监控对于系统编码统一为22
#define COMMAND_ENCODING2011 (2011)
#define COMMAND_ENCODING2031 (2031)
#define COMMAND_ENCODING2051 (2051)
#define COMMAND_ENCODING2061 (2061)
#define COMMAND_ENCODING9014 (9014)
#define SECRET (123456)

typedef struct sen_data
{	
	uint8_t QN[26];					//请求编码
	uint8_t MN[40];					//设备唯一标识符
	uint8_t ST[10];					//系统编码
	uint8_t CN[10];					//命令编码
	uint8_t PW[12];					//访问密码
//	uint8_t CP[500];				//指令参数
	uint8_t CP0[80];				//指令参数日期密码等
	uint8_t CP1[80];				//指令参数传感器1
	uint8_t CP2[80];				//指令参数传感器2
	uint8_t CP3[80];				//指令参数传感器3
	uint8_t CP4[80];				//指令参数传感器4
	uint8_t CP5[80];				//指令参数传感器5
	uint8_t CP6[80];				//指令参数传感器6
	uint8_t CP7[80];				//指令参数传感器7
	uint8_t CP8[80];				//指令参数传感器8
	uint8_t CP9[80];				//指令参数传感器9
}Sen_Data;     						//协议数据区
//整个上传协议包
typedef struct sen_protocol
{
	uint8_t Hender[4];     	//包头
	uint16_t DataLen0;			//数据长度 
	uint8_t DataLen[6];			//数据长度（字符表示）
	uint8_t SecretFlag[2];  //加密标识
	Sen_Data Sen_Data1;			//数据区
	uint8_t Crc[8];					//CRC校验
	uint8_t Tail[10];				//包尾
}Sen_Protocol;
//传感器数据缓存
typedef struct sen_cache
{
	int16_t sen_cache_sec_curr;					//当前实时数据
	//传感器平均值缓存
	int16_t sen_cache_sec[61];					//秒缓存
	int16_t sen_cache_ave_min[11];			//分钟缓存
	int16_t sen_cache_ave_min_curr;
	int16_t sen_cache_ave_10min[7];			//10分钟缓存
	int16_t sen_cache_ave_10min_curr;
	int16_t sen_cache_ave_hour[25];			//小时缓存
	int16_t sen_cache_ave_hour_curr;
	int16_t sen_cache_ave_day_curr;
	//传感器最大值缓存
	int16_t sen_cache_max_min[11];			//分钟缓存
	int16_t sen_cache_max_min_curr;
	int16_t sen_cache_max_10min[7];			//10分钟缓存
	int16_t sen_cache_max_10min_curr;
	int16_t sen_cache_max_hour[25];			//小时缓存
	int16_t sen_cache_max_hour_curr;
	uint16_t sen_cache_max_day_curr;
	//传感器最小值缓存
	int16_t sen_cache_min_min[11];			//分钟缓存
	int16_t sen_cache_min_min_curr;
	int16_t sen_cache_min_10min[7];			//10分钟缓存
	int16_t sen_cache_min_10min_curr;
	int16_t sen_cache_min_hour[25];			//小时缓存
	int16_t sen_cache_min_hour_curr;
	int16_t sen_cache_min_day_curr;
  uint8_t count_sec;									//秒计数
	uint8_t count_min;									//分钟计数
	uint8_t count_10min;								//10分钟计数
	uint8_t count_hour;									//小时计数
	uint8_t Offline;
}Sen_Cache;
typedef enum
{
	FAN,
	TEM,
	HUM,
	NOISE,
	PM2_5,
	PM10,
	WINDIR,
}Sensor_Type;
typedef enum
{
	MINUTE,
	TEN_MINUTE,
	HOUR,
	DAY,
}Send_Time_Type;
extern Sen_Cache  FanS_Cache;				//风速传感器
extern Sen_Cache  Tem_Cache;					//温度传感器
extern Sen_Cache  Hum_Cache;					//湿度传感器
extern Sen_Cache  Noise_Cache;				//噪音传感器		
extern Sen_Cache  PM2_5_Cache;				//PM2.5
extern Sen_Cache  PM10_Cache;				//PM10
//extern Sen_Cache  Light_Cache;				//光照		
//extern Sen_Cache  Atmo_Cache;				//气压
extern Sen_Cache  Win_Dir;						//风向
extern Sen_Protocol Sen_Protocol1;

extern void Fit_Sen_Protocol(Send_Time_Type Type);
extern void Sensor_Add_Date(Sen_Cache *Sen_Cache1,uint8_t fack,uint8_t Time_Update_SEN);
extern Sen_Cache *Get_Sen_Cache(Sensor_Type Sensor);
extern void Heart_Beat(void);   //心跳
void Send_CloudData(void);
#endif
