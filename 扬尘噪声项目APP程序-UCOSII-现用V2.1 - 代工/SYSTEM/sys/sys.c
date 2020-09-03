#include "sys.h"
#include "stmflash.h"
#include "stdio.h"
#include "dtu.h"
#include "delay.h"
#include "usart3.h"
#include "usart5.h"
System_Config_Table System_Config_Table1;//ϵͳ���ñ�


//��ʼ��ϵͳ���ñ�
void Init_System_Config_Table(void)
{
//	DISABLE_INT();
	uint16_t System_Config_Flag=0;
	STMFLASH_ReadLongData(SYSTEM_CONFIG_TABLE_ADDR,(uint8_t *)&System_Config_Flag,2);
	if(System_Config_Flag!=SYSTEM_CONFIG_FLAG_ON)		//�״�����  ��ʼ��һЩֵ
	{
		System_Config_Flag = SYSTEM_CONFIG_FLAG_ON;
		sprintf((char *)System_Config_Table1.Calendar,"2019-5-8 9:48:44 810ms");
		sprintf((char *)System_Config_Table1.Project_Company_name,"�ǻ۹���");
		sprintf((char *)System_Config_Table1.Equipment_number,"JXCY_YC#####");
		System_Config_Table1.PM10_low_threshold = 10;
		System_Config_Table1.PM10_high_threshold = 200;
		System_Config_Table1.PM2_5_k_factor = 1;
		System_Config_Table1.PM2_5_b_factor = 0;
		System_Config_Table1.PM10_k_factor = 1;
		System_Config_Table1.PM10_b_factor = 0;
		System_Config_Table1.Led_Show_Type = 1;
		System_Config_Table1.Table_address = FLASH_SAVE_ADDR;
		STMFLASH_Write(SYSTEM_CONFIG_TABLE_ADDR,(u16*)&System_Config_Flag,1);   //����FLASH
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //����FLASH
	}
	else
		STMFLASH_ReadLongData(FLASH_SAVE_ADDR,(uint8_t *)&System_Config_Table1,sizeof(System_Config_Table1));
	Get_Socket(1);
	Get_Socket(2);
	u5_printf("usr.cn#AT+S\r");
	
//ENABLE_INT();
}
//��ӡϵͳ���ñ�
extern const uint8_t Version[];
void Print_System_Config_Table(void)
{
	printf("*******************ϵͳ���ñ�*******************\n");
	printf("ϵͳ���ñ����ʱ��:%s\n",System_Config_Table1.Calendar);
	printf("��˾/��Ŀ���ƣ�%s\n",System_Config_Table1.Project_Company_name);
	printf("�豸��ţ�%s\n",System_Config_Table1.Equipment_number);
	printf("SOKETA��%d.%d.%d.%d,%d\n",System_Config_Table1.Socka.ip0,System_Config_Table1.Socka.ip1
		,System_Config_Table1.Socka.ip2,System_Config_Table1.Socka.ip3,System_Config_Table1.Socka.pr);
	printf("SOKETB��%d.%d.%d.%d,%d\n",System_Config_Table1.Sockb.ip0,System_Config_Table1.Sockb.ip1
		,System_Config_Table1.Sockb.ip2,System_Config_Table1.Sockb.ip3,System_Config_Table1.Sockb.pr);
//	printf("PM2.5��������Ũ�ȷ�ֵ:%dug/m3\n",System_Config_Table1.PM10_low_threshold);
//	printf("PM2.5��������Ũ�ȷ�ֵ:%dug/m3\n",System_Config_Table1.PM10_high_threshold);
	printf("PM2.5�۳�ϵ��Kֵ:%.1f\n",System_Config_Table1.PM2_5_k_factor);
	printf("PM2.5�۳�ϵ��bֵ:%d\n",System_Config_Table1.PM2_5_b_factor);
	printf("PM10�۳�ϵ��Kֵ:%.1f\n",System_Config_Table1.PM10_k_factor);
	printf("PM10�۳�ϵ��bֵ:%d\n",System_Config_Table1.PM10_b_factor);
	printf("����汾:%s\n",Version);
	printf("LED��ʾ��ʽ:%d��1������  0������ʾ��\n",System_Config_Table1.Led_Show_Type);
	printf("ϵͳ���ñ�����Flash��ַΪ:0x%x\n",System_Config_Table1.Table_address);
	printf("*******************ϵͳ���ñ�*******************\n");
}
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//��λ
__asm void SYSTEMRESET(void) 
{ 
 MOV R0, #1           //;  
 MSR FAULTMASK, R0    //; FAULTMASK ��ֹһ���жϲ��� 
 LDR R0, =0xE000ED0C  //; 
 LDR R1, =0x05FA0004  //;  
 STR R1, [R0]         //;    
  
deadloop 
    B deadloop        //;  
	NOP
} 


