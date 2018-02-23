#ifndef __ILI9320_H
#define __ILI9320_H		
#if __has_include("bsp.h")
#  include "bsp.h"
#endif
#include "stdlib.h"

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

void ILI9320_Init(void);													   	//初始化
void ILI9320_DisplayOn(void);													//开显示
void ILI9320_DisplayOff(void);													//关显示
void ILI9320_Clear(uint16_t Color);	 												//清屏
void ILI9320_SetCursor(uint16_t Xpos, uint16_t Ypos);										//设置光标
void ILI9320_WritePixel(uint16_t x,uint16_t y,uint16_t color);											//画点
void ILI9320_Fast_WritePixel(uint16_t x,uint16_t y,uint16_t color);								//快速画点
uint16_t  ILI9320_ReadPixel(uint16_t x,uint16_t y); 											//读点 
void ILI9320_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);		   				//填充单色
void ILI9320_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);				//填充指定颜色

void ILI9320_Scan_Dir(uint8_t dir);							//设置屏扫描方向
void ILI9320_Display_Dir(uint8_t dir);						//设置屏幕显示方向
void ILI9320_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);//设置窗口
void ILI9320_DrawHLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length);
void ILI9320_DrawVLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length);

#ifdef __cplusplus
} //extern "C"{
#endif

				  		 
#endif  
	 
	 



