#ifndef __ILI93XX_H
#define __ILI93XX_H		
#if __has_include("bsp.h")
#  include "bsp.h"
#endif
#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////	 

//2.4寸/2.8寸/3.5寸/4.3寸 TFT液晶驱动	  
//支持驱动IC型号包括:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
//                  SPFD5408/SSD1289/1505/B505/C505/NT35310/NT35510等		    

//********************************************************************************
//V1.2修改说明
//支持了SPFD5408的驱动,另外把液晶ID直接打印成HEX格式.方便查看LCD驱动IC.
//V1.3
//加入了快速IO的支持
//修改了背光控制的极性（适用于V1.8及以后的开发板版本）
//对于1.8版本之前(不包括1.8)的液晶模块,请修改LCD_Init函数的LCD_LED=1;为LCD_LED=1;
//V1.4
//修改了LCD_ShowChar函数，使用画点功能画字符。
//加入了横竖屏显示的支持
//V1.5 20110730
//1,修改了B505液晶读颜色有误的bug.
//2,修改了快速IO及横竖屏的设置方式.
//V1.6 20111116
//1,加入对LGDP4535液晶的驱动支持
//V1.7 20120713
//1,增加LCD_RD_DATA函数
//2,增加对ILI9341的支持
//3,增加ILI9325的独立驱动代码
//4,增加LCD_Scan_Dir函数(慎重使用)	  
//6,另外修改了部分原来的函数,以适应9341的操作
//V1.8 20120905
//1,加入LCD重要参数设置结构体lcddev
//2,加入LCD_Display_Dir函数,支持在线横竖屏切换
//V1.9 20120911
//1,新增RM68042驱动（ID:6804），但是6804不支持横屏显示！！原因：改变扫描方式，
//导致6804坐标设置失效，试过很多方法都不行，暂时无解。
//V2.0 20120924
//在不硬件复位的情况下,ILI9341的ID读取会被误读成9300,修改LCD_Init,将无法识别
//的情况（读到ID为9300/非法ID）,强制指定驱动IC为ILI9341，执行9341的初始化。
//V2.1 20120930
//修正ILI9325读颜色的bug。
//V2.2 20121007
//修正LCD_Scan_Dir的bug。
//V2.3 20130120
//新增6804支持横屏显示
//V2.4 20131120
//1,新增NT35310（ID:5310）驱动器的支持
//2,新增LCD_SetDisplayWindow函数,用于设置窗口,对快速填充,比较有用,但是该函数在横屏时,不支持6804.
//V2.5 20140211
//1,新增NT35510（ID:5510）驱动器的支持
//////////////////////////////////////////////////////////////////////////////////	

 
  

//请根据自己的LCD尺寸,进行修改!!!!!!!!!!!!!
//定义LCD尺寸大小
//1：2.8寸
//2：3.5寸
//3：4.3寸(电容屏) 
#define   LCD_SIZE_TYPE    1  //uCGUI用,默认设置为2.8寸屏


//////////////////////////////////////////////////////////////////////////////////
	 
//扫描方向定义
#define L2R_U2D  0 //从左到右,从上到下
#define L2R_D2U  1 //从左到右,从下到上
#define R2L_U2D  2 //从右到左,从上到下
#define R2L_D2U  3 //从右到左,从下到上

#define U2D_L2R  4 //从上到下,从左到右
#define U2D_R2L  5 //从上到下,从右到左
#define D2U_L2R  6 //从下到上,从左到右
#define D2U_R2L  7 //从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  //默认的扫描方向


#ifdef __cplusplus
extern "C"{
#endif

void ILI93XX_Init(void);													   	//初始化
void ILI93XX_DisplayOn(void);													//开显示
void ILI93XX_DisplayOff(void);													//关显示
void ILI93XX_Clear(uint16_t Color);	 												//清屏
void ILI93XX_SetCursor(uint16_t Xpos, uint16_t Ypos);										//设置光标
void ILI93XX_WritePixel(uint16_t x,uint16_t y,uint16_t color);											//画点
void ILI93XX_Fast_WritePixel(uint16_t x,uint16_t y,uint16_t color);								//快速画点
uint16_t  ILI93XX_ReadPixel(uint16_t x,uint16_t y); 											//读点 
void ILI93XX_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);		   				//填充单色
void ILI93XX_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);				//填充指定颜色

void ILI93XX_Scan_Dir(uint8_t dir);							//设置屏扫描方向
void ILI93XX_Display_Dir(uint8_t dir);						//设置屏幕显示方向
void ILI93XX_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);//设置窗口

#ifdef __cplusplus
} //extern "C"{
#endif

				  		 
#endif  
	 
	 



