/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : LCD_SPDF5420.h
*	版    本 : V2.2
*	说    明 : 头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/


#ifndef _LCD_SPFD5420_H
#define _LCD_SPFD5420_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* 定义LCD显示区域的分辨率 */
#define LCD_30_HEIGHT	240		/* 3.0寸宽屏 高度，单位：像素 */
#define LCD_30_WIDTH	400		/* 3.0寸宽屏 宽度，单位：像素 */

#define LCD_43_HEIGHT	272		/* 4.3寸宽屏 高度，单位：像素 */
#define LCD_43_WIDTH	480		/* 4.3寸宽屏 宽度，单位：像素 */

#define LCD_70_HEIGHT	480		/* 7.0寸宽屏 高度，单位：像素 */
#define LCD_70_WIDTH	800		/* 7.0寸宽屏 宽度，单位：像素 */

/* 支持的驱动芯片ID */
enum
{
	IC_5420		= 0x5420,
	IC_4001		= 0x4001,
	IC_61509 	= 0xB509,
	IC_8875 	= 0x0075
};

#define CHIP_STR_5420	"SPFD5420A"
#define CHIP_STR_4001	"OTM4001A"
#define CHIP_STR_61509	"R61509V"
#define CHIP_STR_8875	"RA8875"
   
/** 
  * @brief  SPFD5420 Size  
  */  
#define  SPFD5420_LCD_PIXEL_WIDTH    ((uint16_t)400)
#define  SPFD5420_LCD_PIXEL_HEIGHT   ((uint16_t)240)


/* 可供外部模块调用的函数 */
void 	 SPFD5420_Init(void);
uint16_t SPFD5420_ReadID(void);
void     SPFD5420_WriteReg(uint16_t LCDReg, uint16_t LCDRegValue);
uint16_t SPFD5420_ReadReg(uint16_t LCDReg);

void 	 SPFD5420_DisplayOn(void);
void 	 SPFD5420_DisplayOff(void);
void     SPFD5420_SetCursor(uint16_t Xpos, uint16_t Ypos);
void  	 SPFD5420_WritePixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t SPFD5420_ReadPixel(uint16_t _usX, uint16_t _usY);
void SPFD5420_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);

void 	 SPFD5420_DrawHLine(uint16_t _usColor , uint16_t _usX1 , uint16_t _usY1 , uint16_t _usL);
void 	 SPFD5420_DrawVLine(uint16_t _usColor,  uint16_t _usX1 , uint16_t _usY1 , uint16_t _usL);

void	 SPFD5420_SetDisplayWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey);

void     SPFD5420_ClrScr(uint16_t _usColor);
void 	 SPFD5420_DrawLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);

void 	 SPFD5420_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);


void SPFD5420_SetBackLight(uint8_t _bright);

void SPFD5420_DrawHColorLine(uint16_t _usX1 , uint16_t _usY1, uint16_t _usWidth, uint16_t *_pColor);

uint16_t SPFD5420_GetLcdPixelWidth(void);
uint16_t SPFD5420_GetLcdPixelHeight(void);

#ifdef __cplusplus
}
#endif

#endif


