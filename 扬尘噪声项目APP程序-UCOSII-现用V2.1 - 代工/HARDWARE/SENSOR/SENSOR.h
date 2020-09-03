#ifndef __SENSOR_H
#define __SENSOR_H	 
#include "sys.h"  


//#define REISSUAN_MODE    				//�ط�ģʽ
#define MAX_SEND_TIMES (2)			//����ط�����	
#define SENSOR_AMOUNT (7)				//�������������ı�ʱ  һ��Ҫ�޸����ֵ
#define SYSTEM_CODING (22)      //�ﳾ��ض���ϵͳ����ͳһΪ22
#define COMMAND_ENCODING2011 (2011)
#define COMMAND_ENCODING2031 (2031)
#define COMMAND_ENCODING2051 (2051)
#define COMMAND_ENCODING2061 (2061)
#define COMMAND_ENCODING9014 (9014)
#define SECRET (123456)

typedef struct sen_data
{	
	uint8_t QN[26];					//�������
	uint8_t MN[40];					//�豸Ψһ��ʶ��
	uint8_t ST[10];					//ϵͳ����
	uint8_t CN[10];					//�������
	uint8_t PW[12];					//��������
//	uint8_t CP[500];				//ָ�����
	uint8_t CP0[80];				//ָ��������������
	uint8_t CP1[80];				//ָ�����������1
	uint8_t CP2[80];				//ָ�����������2
	uint8_t CP3[80];				//ָ�����������3
	uint8_t CP4[80];				//ָ�����������4
	uint8_t CP5[80];				//ָ�����������5
	uint8_t CP6[80];				//ָ�����������6
	uint8_t CP7[80];				//ָ�����������7
	uint8_t CP8[80];				//ָ�����������8
	uint8_t CP9[80];				//ָ�����������9
}Sen_Data;     						//Э��������
//�����ϴ�Э���
typedef struct sen_protocol
{
	uint8_t Hender[4];     	//��ͷ
	uint16_t DataLen0;			//���ݳ��� 
	uint8_t DataLen[6];			//���ݳ��ȣ��ַ���ʾ��
	uint8_t SecretFlag[2];  //���ܱ�ʶ
	Sen_Data Sen_Data1;			//������
	uint8_t Crc[8];					//CRCУ��
	uint8_t Tail[10];				//��β
}Sen_Protocol;
//���������ݻ���
typedef struct sen_cache
{
	int16_t sen_cache_sec_curr;					//��ǰʵʱ����
	//������ƽ��ֵ����
	int16_t sen_cache_sec[61];					//�뻺��
	int16_t sen_cache_ave_min[11];			//���ӻ���
	int16_t sen_cache_ave_min_curr;
	int16_t sen_cache_ave_10min[7];			//10���ӻ���
	int16_t sen_cache_ave_10min_curr;
	int16_t sen_cache_ave_hour[25];			//Сʱ����
	int16_t sen_cache_ave_hour_curr;
	int16_t sen_cache_ave_day_curr;
	//���������ֵ����
	int16_t sen_cache_max_min[11];			//���ӻ���
	int16_t sen_cache_max_min_curr;
	int16_t sen_cache_max_10min[7];			//10���ӻ���
	int16_t sen_cache_max_10min_curr;
	int16_t sen_cache_max_hour[25];			//Сʱ����
	int16_t sen_cache_max_hour_curr;
	uint16_t sen_cache_max_day_curr;
	//��������Сֵ����
	int16_t sen_cache_min_min[11];			//���ӻ���
	int16_t sen_cache_min_min_curr;
	int16_t sen_cache_min_10min[7];			//10���ӻ���
	int16_t sen_cache_min_10min_curr;
	int16_t sen_cache_min_hour[25];			//Сʱ����
	int16_t sen_cache_min_hour_curr;
	int16_t sen_cache_min_day_curr;
  uint8_t count_sec;									//�����
	uint8_t count_min;									//���Ӽ���
	uint8_t count_10min;								//10���Ӽ���
	uint8_t count_hour;									//Сʱ����
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
extern Sen_Cache  FanS_Cache;				//���ٴ�����
extern Sen_Cache  Tem_Cache;					//�¶ȴ�����
extern Sen_Cache  Hum_Cache;					//ʪ�ȴ�����
extern Sen_Cache  Noise_Cache;				//����������		
extern Sen_Cache  PM2_5_Cache;				//PM2.5
extern Sen_Cache  PM10_Cache;				//PM10
//extern Sen_Cache  Light_Cache;				//����		
//extern Sen_Cache  Atmo_Cache;				//��ѹ
extern Sen_Cache  Win_Dir;						//����
extern Sen_Protocol Sen_Protocol1;

extern void Fit_Sen_Protocol(Send_Time_Type Type);
extern void Sensor_Add_Date(Sen_Cache *Sen_Cache1,uint8_t fack,uint8_t Time_Update_SEN);
extern Sen_Cache *Get_Sen_Cache(Sensor_Type Sensor);
extern void Heart_Beat(void);   //����
void Send_CloudData(void);
#endif
