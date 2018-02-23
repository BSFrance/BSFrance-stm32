#ifndef __ILI9486_H
#define __ILI9486_H		
	 
	    															  
void ILI9486_Init(void);													   	//初始化
void ILI9486_DisplayOn(void);													//开显示
void ILI9486_DisplayOff(void);													//关显示
void ILI9486_Clear(uint16_t Color);	 												//清屏
void ILI9486_SetCursor(uint16_t Xpos, uint16_t Ypos);										//设置光标
void ILI9486_WritePixel(uint16_t x,uint16_t y,uint16_t color);								//画点
uint16_t  ILI9486_ReadPixel(uint16_t x,uint16_t y); 											//读点 
void ILI9486_Display_Dir(uint8_t dir);
void ILI9486_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void ILI9486_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);

void ILI9486_WR_REG(uint16_t regval);
void ILI9486_WR_DATA(uint16_t data);
uint16_t ILI9486_RD_DATA(void);
void ILI9486_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void ILI9486_WriteRAM_Prepare(void);
void ILI9486_DrawHLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length);
void ILI9486_DrawVLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length);

#endif  
	 
	 



