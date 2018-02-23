#include <Arduino.h>
#include "bspio/bspio.h"
#include "ili9481.h"
//#include "ili9481_reg.h"
//#include "stdlib.h"
#include "font/font.h"
#include "tftlcd/lcd.h"
 
#define  ILI9481_WR_REG(x) 	LCD_IO_WriteReg(x)
#define  ILI9481_WR_DATA(x) LCD_IO_WriteData(x)
#define  ILI9481_RD_DATA    LCD_IO_ReadData
#define  ILI9481_WriteRegData(x,y) LCD_IO_WriteRegData(x,y)

//管理LCD重要参数
//默认为竖屏

LCD_DrvTypeDef ILI9481_DRV = {
  .Init       = ILI9481_Init,
  .ReadPixel  = ILI9481_ReadPixel,
  .WritePixel = ILI9481_WritePixel,
  .Fill	= ILI9481_Fill,
  
//option  
  .DisplayOn  = ILI9481_DisplayOn,
  .DisplayOff = ILI9481_DisplayOff,
  .SetCursor  = ILI9481_SetCursor,
};

extern LCD_DevTypeDef lcddev;

//读寄存器
//LCD_Reg:寄存器编号
//返回值:读到的值
uint16_t ILI9481_ReadReg(uint16_t LCD_Reg)
{										   
 	ILI9481_WR_REG(LCD_Reg);  //写入要读的寄存器号  
	return ILI9481_RD_DATA(); 
} 

//开始写GRAM
void ILI9481_WriteRAM_Prepare(void)
{
	ILI9481_WR_REG(lcddev.wramcmd);
} 

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
uint16_t ILI9481_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
}		 
//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
uint16_t ILI9481_ReadPixel(uint16_t x,uint16_t y)
{
 	uint16_t r;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	ILI9481_SetCursor(x,y);
	ILI9481_WR_REG(0x2E);      		 		//其他IC发送读GRAM指令
 	//dummy READ
	delayMicroseconds(1);//延时1us					   
 	r=DATAIN;  	//实际坐标颜色
	delayMicroseconds(1);//延时1us					   
 	r=DATAIN;  	//实际坐标颜色
	return (r);
}
//LCD开启显示
void ILI9481_DisplayOn(void)
{	
 ILI9481_WriteRegData(R7,0x0173); 			//开启显示
}	 
//LCD关闭显示
void ILI9481_DisplayOff(void)
{	   
 ILI9481_WriteRegData(R7, 0x0);//关闭显示 
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void ILI9481_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
 	ILI9481_Set_Window(Xpos,Ypos,Xpos,Ypos);
}

//画点
//x,y:坐标
//lcddev.TextColor:此点的颜色
void ILI9481_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{
	ILI9481_SetCursor(x,y);		//设置光标位置 
	ILI9481_WR_DATA(color); 
}  	 
//快速画点
//x,y:坐标
//color:颜色
void ILI9481_Fast_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{	   
	ILI9481_SetCursor(x,y);		//设置光标位置 
	ILI9481_WR_DATA(color);		//写数据
}
//设置LCD显示方向
//dir:0,竖屏；1,横屏
void ILI9481_Display_Dir(uint8_t dir)
{
	if(dir==0)			//竖屏
	{
	
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 	
			lcddev.width=320;
			lcddev.height=480;
			ILI9481_WriteRegData(0x36,0X0A);//BGR==1,MY==0,MX==0,MV==0
	}
		else 				//横屏
	{	  				
			lcddev.dir=1;	//横屏
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 		 
			lcddev.width=480;
			lcddev.height=320; 	
			ILI9481_WriteRegData(0x36,0x28);//BGR==1,MY==1,MX==0,MV==1  63		
		}
	

}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void ILI9481_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{   
	ILI9481_WR_REG(lcddev.setxcmd);	
	ILI9481_WR_DATA(sx>>8);
	ILI9481_WR_DATA(0x00FF&sx);		
	ILI9481_WR_DATA((width-0)>>8);
	ILI9481_WR_DATA(0x00FF&(width-0));

	ILI9481_WR_REG(lcddev.setycmd);	
	ILI9481_WR_DATA(sy>>8);
	ILI9481_WR_DATA(0x00FF&sy);		
	ILI9481_WR_DATA((height-0)>>8);
	ILI9481_WR_DATA(0x00FF&(height-0));

	ILI9481_WriteRAM_Prepare();	//开始写入GRAM
} 

//初始化lcd
//该初始化函数可以初始化各种ALIENTEK出品的LCD液晶屏
//本函数占用较大flash,用户可以根据自己的实际情况,删掉未用到的LCD初始化代码.以节省空间.
void ILI9481_Init(void)
{ 
	LCD_IO_Init();		 
	
	
	//液晶屏初始化
	ILI9481_WR_REG(0x11);
	delay(20);
	ILI9481_WR_REG(0xD0);
	ILI9481_WR_DATA(0x07);
	ILI9481_WR_DATA(0x42);
	ILI9481_WR_DATA(0x18);
	
	ILI9481_WR_REG(0xD1);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x07);//07
	ILI9481_WR_DATA(0x10);
	
	ILI9481_WR_REG(0xD2);
	ILI9481_WR_DATA(0x01);
	ILI9481_WR_DATA(0x02);
	
	ILI9481_WR_REG(0xC0);
	ILI9481_WR_DATA(0x10);
	ILI9481_WR_DATA(0x3B);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x02);
	ILI9481_WR_DATA(0x11);
	
	ILI9481_WR_REG(0xC5);
	ILI9481_WR_DATA(0x03);
	
	ILI9481_WR_REG(0xC8);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x32);
	ILI9481_WR_DATA(0x36);
	ILI9481_WR_DATA(0x45);
	ILI9481_WR_DATA(0x06);
	ILI9481_WR_DATA(0x16);
	ILI9481_WR_DATA(0x37);
	ILI9481_WR_DATA(0x75);
	ILI9481_WR_DATA(0x77);
	ILI9481_WR_DATA(0x54);
	ILI9481_WR_DATA(0x0C);
	ILI9481_WR_DATA(0x00);
	
	ILI9481_WR_REG(0x36);
	ILI9481_WR_DATA(0x0A);
	
	ILI9481_WR_REG(0x3A);
	ILI9481_WR_DATA(0x55);
	
	ILI9481_WR_REG(0x2A);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x01);
	ILI9481_WR_DATA(0x3F);
	
	ILI9481_WR_REG(0x2B);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x00);
	ILI9481_WR_DATA(0x01);
	ILI9481_WR_DATA(0xE0);
	delay(120);
	ILI9481_WR_REG(0x29);
	ILI9481_WR_REG(0x002c); 

	
	ILI9481_Display_Dir(1);		 	//默认为竖屏
//	LCD_Clear(WHITE);
}  		  
  
//清屏函数
//color:要清屏的填充色
void ILI9481_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	ILI9481_Set_Window(0,0,lcddev.width-1,lcddev.height-1);				
	for(index=0;index<totalpoint;index++)ILI9481_WR_DATA(color);	
}  
//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void ILI9481_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
	ILI9481_Set_Window(sx,sy,ex,ey);	
 	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)ILI9481_WR_DATA(color);//写入数据 
	}	
	ILI9481_Set_Window(0,0,lcddev.width-1,lcddev.height-1);		
}  
//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI9481_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
	ILI9481_Set_Window(sx,sy,ex,ey);		
 	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)ILI9481_WR_DATA(color[i*width+j]);//写入数据 
	}	 
	ILI9481_Set_Window(0,0,lcddev.width-1,lcddev.height-1);			
} 
