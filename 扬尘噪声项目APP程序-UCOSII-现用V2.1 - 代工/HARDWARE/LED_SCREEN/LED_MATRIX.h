/***************************************************************************************************/
/*****************代码文件：LED_MATRIX.h************************************************************/
/*****************程序名称：全彩LED点阵屏驱动*******************************************************/
/*****************程序内容：全彩LED点阵屏显示驱动程序头文件*****************************************/
/*****************程序编写：7426笨蛋****************************************************************/
/*****************编写日期：2015年12月**************************************************************/
/*****************修改记录：NA**********************************************************************/
/*****************杭州迷梦电子科技******************************************************************/
/*****************CopyRight2015 MiMeng Electronics Technology Ltd.**********************************/
/***************************************************************************************************/
#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__
typedef unsigned char      u8 ;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef signed char        s8 ;
typedef signed short       s16;
typedef signed int         s32;
#define Width 32*3
#define Height 16*3
#define LED_DOT  Width*Height
//#define LED_TEM_DOT  LED_DOT/256


#define Line1	0
#define Line2	16
#define Line3	32
#define Line4	48
#define Line5	64
#define Line6	80
// #define Line7	96
// #define Line8	112

#define Row1	0
#define Row2	16
#define Row3	32
#define Row4	48
#define Row5	64
#define Row6	80

#define COL1	0
#define COL2	8
#define COL3	16
#define COL4	24
#define COL5	32
#define COL6	40
#define COL7	48
#define COL8	56
#define COL9	64
#define COL10	72
#define COL11	80
#define COL12	88

#define LeftMove 0
#define RightMove 1
#define UpMove	2
#define DownMove	3
#define DisplayZero 0x80

extern void LED_Gpio_Int(void);
extern void FullDisplay(void);
extern void ClearDisplay(uint8_t mode);
extern void ClearDisplayLine(u8 line);
extern void WriteDisplayTemp(void);
extern void DisplayPoint(u8 x,u8 y);
extern void DisplayPointNow(u8 x,u8 y);
extern void StaticAscllDisplay(u8 *ch,u8 rows);
extern void StaticChineseDisplayTest(u8 *ch,u8 rows);
extern void FrontIcStaticChineseDisplayTest(u8 *ch,u8 rows);

extern void StaticStringDisplayInPos(uint8_t *ch, uint8_t x, uint8_t y,uint8_t mode);
extern void RollingStringDisplayInLine(u8 *ch,u8 y ,u8 dir,u16 speed);

extern void DisplayIntValue(u16 val , u8 x , u8 y , u8 type);
extern void DisplayFloatValue(float dat , u8 x , u8 y , u8 type);
extern void DisplayStringAndIntValue(u8 *str1,u16 val , u8 col , u8 row , u8 type , u8 *str2);
extern void DisplayStringAndFloatValue(u8 *str1,float val , u8 col , u8 row , u8 type , u8 *str2);
extern void DisplayStringAndTime(u16 year,u8 month , u8 date , u8 hour , u8 min , u8 sec , u8 weak ,u8 y);

extern void Get_Week(uint8_t week,uint8_t* buff);
#endif
