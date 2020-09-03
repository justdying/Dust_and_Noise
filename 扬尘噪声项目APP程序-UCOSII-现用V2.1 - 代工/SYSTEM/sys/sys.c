#include "sys.h"
#include "stmflash.h"
#include "stdio.h"
#include "dtu.h"
#include "delay.h"
#include "usart3.h"
#include "usart5.h"
System_Config_Table System_Config_Table1;//系统配置表


//初始化系统配置表
void Init_System_Config_Table(void)
{
//	DISABLE_INT();
	uint16_t System_Config_Flag=0;
	STMFLASH_ReadLongData(SYSTEM_CONFIG_TABLE_ADDR,(uint8_t *)&System_Config_Flag,2);
	if(System_Config_Flag!=SYSTEM_CONFIG_FLAG_ON)		//首次配置  初始化一些值
	{
		System_Config_Flag = SYSTEM_CONFIG_FLAG_ON;
		sprintf((char *)System_Config_Table1.Calendar,"2019-5-8 9:48:44 810ms");
		sprintf((char *)System_Config_Table1.Project_Company_name,"智慧工地");
		sprintf((char *)System_Config_Table1.Equipment_number,"JXCY_YC#####");
		System_Config_Table1.PM10_low_threshold = 10;
		System_Config_Table1.PM10_high_threshold = 200;
		System_Config_Table1.PM2_5_k_factor = 1;
		System_Config_Table1.PM2_5_b_factor = 0;
		System_Config_Table1.PM10_k_factor = 1;
		System_Config_Table1.PM10_b_factor = 0;
		System_Config_Table1.Led_Show_Type = 1;
		System_Config_Table1.Table_address = FLASH_SAVE_ADDR;
		STMFLASH_Write(SYSTEM_CONFIG_TABLE_ADDR,(u16*)&System_Config_Flag,1);   //保存FLASH
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)&System_Config_Table1,sizeof(System_Config_Table1)/2);   //保存FLASH
	}
	else
		STMFLASH_ReadLongData(FLASH_SAVE_ADDR,(uint8_t *)&System_Config_Table1,sizeof(System_Config_Table1));
	Get_Socket(1);
	Get_Socket(2);
	u5_printf("usr.cn#AT+S\r");
	
//ENABLE_INT();
}
//打印系统配置表
extern const uint8_t Version[];
void Print_System_Config_Table(void)
{
	printf("*******************系统配置表*******************\n");
	printf("系统配置表更改时间:%s\n",System_Config_Table1.Calendar);
	printf("公司/项目名称：%s\n",System_Config_Table1.Project_Company_name);
	printf("设备编号：%s\n",System_Config_Table1.Equipment_number);
	printf("SOKETA：%d.%d.%d.%d,%d\n",System_Config_Table1.Socka.ip0,System_Config_Table1.Socka.ip1
		,System_Config_Table1.Socka.ip2,System_Config_Table1.Socka.ip3,System_Config_Table1.Socka.pr);
	printf("SOKETB：%d.%d.%d.%d,%d\n",System_Config_Table1.Sockb.ip0,System_Config_Table1.Sockb.ip1
		,System_Config_Table1.Sockb.ip2,System_Config_Table1.Sockb.ip3,System_Config_Table1.Sockb.pr);
//	printf("PM2.5传感器低浓度阀值:%dug/m3\n",System_Config_Table1.PM10_low_threshold);
//	printf("PM2.5传感器高浓度阀值:%dug/m3\n",System_Config_Table1.PM10_high_threshold);
	printf("PM2.5粉尘系数K值:%.1f\n",System_Config_Table1.PM2_5_k_factor);
	printf("PM2.5粉尘系数b值:%d\n",System_Config_Table1.PM2_5_b_factor);
	printf("PM10粉尘系数K值:%.1f\n",System_Config_Table1.PM10_k_factor);
	printf("PM10粉尘系数b值:%d\n",System_Config_Table1.PM10_b_factor);
	printf("程序版本:%s\n",Version);
	printf("LED显示方式:%d【1：正显  0：反显示】\n",System_Config_Table1.Led_Show_Type);
	printf("系统配置表所在Flash地址为:0x%x\n",System_Config_Table1.Table_address);
	printf("*******************系统配置表*******************\n");
}
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
//复位
__asm void SYSTEMRESET(void) 
{ 
 MOV R0, #1           //;  
 MSR FAULTMASK, R0    //; FAULTMASK 禁止一切中断产生 
 LDR R0, =0xE000ED0C  //; 
 LDR R1, =0x05FA0004  //;  
 STR R1, [R0]         //;    
  
deadloop 
    B deadloop        //;  
	NOP
} 


