#ifndef __DTU_H
#define __DTU_H	 

#define TIMEOUT_DTU		400		/* �������ʱʱ��, ��λms */
uint8_t Get_Socket(uint8_t num);
uint8_t Dtu_Init(void);
uint8_t Config_Socka(uint8_t num,uint8_t enable);
int8_t Test_Connected(uint8_t num);
#endif

