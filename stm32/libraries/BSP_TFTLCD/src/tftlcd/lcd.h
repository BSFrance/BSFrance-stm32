/**
  ******************************************************************************
  * @file    lcd.h
  * @author  MCD Application Team
  * @version V4.0.1
  * @date    21-July-2015
  * @brief   This file contains all the functions prototypes for the LCD driver.   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H


/** chip id list 
   add by huaweiwx@sina.com 2017.7.8
  */
#define ILI93XX 0x9300

#define ILI9320	0x9320
#define ILI9325	0x9325
#define ILI9328	0x9328
#define ILI9331	0x9331
#define ILI9341	0x9341
#define ILI9481	0x9481
#define ILI9486	0x9486

#define SPFD5408	0x5408
#define SPFD5420	0x5420

#define RM68021 0x6802
#define RM68042	0x6804

#define B505	0xB505
#define C505	0xc505 
#define LGDP4531	0x4531
#define LGDP4535	0x4535
#define NT35310	0x5310
#define NT35510	0x5510

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
   
typedef struct
{
  void     (*Init)(void);
  uint16_t (*ReadID)(void);  //uCGUI  unused
  void     (*DisplayOn)(void);
  void     (*DisplayOff)(void);
  void     (*SetCursor)(uint16_t Xpos, uint16_t Ypos);
  void     (*WritePixel)(uint16_t Xpos, uint16_t Ypos,uint16_t color);
  uint16_t (*ReadPixel)(uint16_t Xpos, uint16_t Ypos);
  void     (*Fill)(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
 
 /* Optimized operation */
  void     (*SetDisplayWindow)(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
  void     (*DrawHLine)(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
  void     (*DrawVLine)(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
  
  uint16_t (*GetLcdPixelWidth)(void);
  uint16_t (*GetLcdPixelHeight)(void);
  void     (*DrawBitmap)(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);
  void     (*DrawRGBImage)(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata);
}LCD_DrvTypeDef;    

typedef struct
{
	uint16_t id;	 	//LCD的ID
	uint16_t width;	 	//LCD宽度
	uint16_t height; 	//LCD高度
	uint8_t  dir;	 	//横屏还是竖屏控制:0 竖屏,1 横屏
	uint16_t wramcmd;  	//开始写gram指令
	uint16_t setxcmd; 	//设置X坐标指令
	uint16_t setycmd;  	//设置Y坐标指令
	uint16_t TextColor;	//画笔颜色
	uint16_t BackColor; //背景色 
}LCD_DevTypeDef;    


#ifdef __cplusplus
}
#endif

#endif /* __LCD_H */
