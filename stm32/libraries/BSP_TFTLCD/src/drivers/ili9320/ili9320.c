#include <Arduino.h>
#include "bspio/bspio.h"
#include "ili9320.h"
#include "ili9320_reg.h"
//#include "stdlib.h"
#include "font/font.h"
#include "tftlcd/lcd.h"
 
#define  ILI9320_WR_REG(x) 	LCD_IO_WriteReg(x)
#define  ILI9320_WR_DATA(x) LCD_IO_WriteData(x)
#define  ILI9320_RD_DATA    LCD_IO_ReadData
#define  ILI9320_WriteRegData(x,y) LCD_IO_WriteRegData(x,y)



LCD_DrvTypeDef ILI9320_DRV = {
  .Init       = ILI9320_Init,
  .DisplayOn  = ILI9320_DisplayOn,
  .DisplayOff = ILI9320_DisplayOff,
  .SetCursor  = ILI9320_SetCursor,
  .ReadPixel  = ILI9320_ReadPixel,
  .WritePixel = ILI9320_WritePixel,
  .Fill	= ILI9320_Fill,

//option
  .DrawHLine = ILI9320_DrawHLine,
  .DrawVLine = ILI9320_DrawVLine,
};


extern LCD_DevTypeDef lcddev;

uint16_t ILI9320_ReadReg(uint16_t Reg)
{										   
	ILI9320_WR_REG(Reg);		//写入要读的寄存器序号
	delayMicroseconds(5);		  
	return ILI9320_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void ILI9320_WriteRAM_Prepare(void)
{
 	ILI9320_WR_REG(lcddev.wramcmd);
}	 
//LCD写GRAM
//RGB_Code:颜色值
void ILI9320_WriteRAM(uint16_t RGB_Code)
{							    
	ILI9320_WR_DATA(RGB_Code);//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值

uint16_t ILI9320_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 
//当mdk -O1时间优化时需要设置
//延时i
static void opt_delay(uint8_t i)
{
	while(i--);
}
//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
uint16_t ILI9320_ReadPixel(uint16_t x,uint16_t y)
{
 	uint16_t r=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	ILI9320_SetCursor(x,y);	    
	ILI9320_WR_REG(R34);      		 				//其他IC发送读GRAM指令
 	opt_delay(2);				//FOR 9320,延时2us	    
	r=ILI9320_RD_DATA();							//dummy Read	   
	opt_delay(2);	  
 	r=ILI9320_RD_DATA();  		  						//实际坐标颜色
    return ILI9320_BGR2RGB(r);						//其他IC
}			 
//LCD开启显示
void ILI9320_DisplayOn(void)
{					   
    ILI9320_WriteRegData(R7,0x0173); 				 	//开启显示
}	 
//LCD关闭显示
void ILI9320_DisplayOff(void)
{	   
	ILI9320_WriteRegData(R7,0x0);//关闭显示 
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void ILI9320_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
	if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
	ILI9320_WriteRegData(lcddev.setxcmd, Xpos);
	ILI9320_WriteRegData(lcddev.setycmd, Ypos);
} 		 
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void ILI9320_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	if(lcddev.dir==1)  //横屏时，对6804不改变扫描方向！
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(1<<5)|(1<<4)|(0<<3); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(0<<5)|(1<<4)|(0<<3); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(1<<5)|(0<<4)|(0<<3);
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(0<<5)|(0<<4)|(0<<3); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(1<<5)|(1<<4)|(1<<3); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(1<<5)|(0<<4)|(1<<3); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(0<<5)|(1<<4)|(1<<3); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(0<<5)|(0<<4)|(1<<3); 
				break;	 
		}
		dirreg=0X03;
		regval|=1<<12;  
		ILI9320_WriteRegData(dirreg,regval);
	}
}   
//画点
//x,y:坐标
//lcddev.TextColor:此点的颜色
void ILI9320_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{
	ILI9320_SetCursor(x,y);		//设置光标位置 
	ILI9320_WriteRAM_Prepare();	//开始写入GRAM
	ILI9320_WR_DATA(color); 
}



//设置LCD显示方向
//dir:0,竖屏；1,横屏
void ILI9320_Display_Dir(uint8_t dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R32;
			lcddev.setycmd=R33;  
		}
	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R33;
			lcddev.setycmd=R32;  
		}
	} 
	ILI9320_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void ILI9320_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{   
	uint8_t hsareg,heareg,vsareg,veareg;
	uint16_t hsaval,heaval,vsaval,veaval; 
	width=sx+width-1;
	height=sy+height-1;
	{
		if(lcddev.dir==1)//横屏
		{
			//窗口值
			hsaval=sy;				
			heaval=height;
			vsaval=lcddev.width-width-1;
			veaval=lcddev.width-sx-1;				
		}else
		{ 
			hsaval=sx;				
			heaval=width;
			vsaval=sy;
			veaval=height;
		}
		{
			hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
			vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器	  
		}								  
		//设置寄存器值
		ILI9320_WriteRegData(hsareg,hsaval);
		ILI9320_WriteRegData(heareg,heaval);
		ILI9320_WriteRegData(vsareg,vsaval);
		ILI9320_WriteRegData(veareg,veaval);		
		ILI9320_SetCursor(sx,sy);	//设置光标位置
	}
} 
//初始化lcd
//该初始化函数可以初始化各种ILI9320液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void ILI9320_Init(void)
{ 										  
	LCD_IO_Init();		 
 	delay(50); // delay 50 ms 
 	ILI9320_WriteRegData(0x0000,0x0001);
	delay(50); // delay 50 ms 
  	lcddev.id = ILI9320_ReadReg(0x0000);   
	{
		ILI9320_WriteRegData(0x00,0x0000);
		ILI9320_WriteRegData(0x01,0x0100);	//Driver Output Contral.
		ILI9320_WriteRegData(0x02,0x0700);	//LCD Driver Waveform Contral.
		ILI9320_WriteRegData(0x03,0x1030);//Entry Mode Set.
		//ILI9320_WriteRegData(0x03,0x1018);	//Entry Mode Set.
	
		ILI9320_WriteRegData(0x04,0x0000);	//Scalling Contral.
		ILI9320_WriteRegData(0x08,0x0202);	//Display Contral 2.(0x0207)
		ILI9320_WriteRegData(0x09,0x0000);	//Display Contral 3.(0x0000)
		ILI9320_WriteRegData(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		ILI9320_WriteRegData(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		ILI9320_WriteRegData(0x0d,0x0000);	//Frame Maker Position.
		ILI9320_WriteRegData(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		delay(50); 
		ILI9320_WriteRegData(0x07,0x0101);	//Display Contral.
		delay(50); 								  
		ILI9320_WriteRegData(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		ILI9320_WriteRegData(0x11,0x0007);								//Power Control 2.(0x0001)
		ILI9320_WriteRegData(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		ILI9320_WriteRegData(0x13,0x0b00);								//Power Control 4.
		ILI9320_WriteRegData(0x29,0x0000);								//Power Control 7.
	
		ILI9320_WriteRegData(0x2b,(1<<14)|(1<<4));	    
		ILI9320_WriteRegData(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		ILI9320_WriteRegData(0x51,239);	//Set Y Star
		ILI9320_WriteRegData(0x52,0);	//Set Y End.t.
		ILI9320_WriteRegData(0x53,319);	//
	
		ILI9320_WriteRegData(0x60,0x2700);	//Driver Output Control.
		ILI9320_WriteRegData(0x61,0x0001);	//Driver Output Control.
		ILI9320_WriteRegData(0x6a,0x0000);	//Vertical Srcoll Control.
	
		ILI9320_WriteRegData(0x80,0x0000);	//Display Position? Partial Display 1.
		ILI9320_WriteRegData(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		ILI9320_WriteRegData(0x82,0x0000);	//RAM Address End-Partial Display 1.
		ILI9320_WriteRegData(0x83,0x0000);	//Displsy Position? Partial Display 2.
		ILI9320_WriteRegData(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		ILI9320_WriteRegData(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		ILI9320_WriteRegData(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		ILI9320_WriteRegData(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		ILI9320_WriteRegData(0x93,0x0001);	//Panel Interface Contral 3.
		ILI9320_WriteRegData(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		ILI9320_WriteRegData(0x97,(0<<8));	//
		ILI9320_WriteRegData(0x98,0x0000);	//Frame Cycle Contral.	   
		ILI9320_WriteRegData(0x07,0x0173);	//(0x0173)
	}
	ILI9320_Display_Dir(1);		 	//默认为竖屏
//	ILI9320_LED=0;					//点亮背光
//	ILI9320_Clear(lcddev.TextColor);
}

//清屏函数
//color:要清屏的填充色
void ILI9320_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
    ILI9320_SetCursor(0x00,0x0000);	//设置光标位置 
	ILI9320_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		ILI9320_WR_DATA(color);	   
	}
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI9320_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
	uint16_t i,j;
	uint16_t xlen=0;
	xlen=ex-sx+1;	 
	for(i=sy;i<=ey;i++)
		{
		 	ILI9320_SetCursor(sx,i);      				//设置光标位置 
			ILI9320_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)ILI9320_WR_DATA(color);	//设置光标位置 	    
		}
}  

void ILI9320_DrawHLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length){
	ILI9320_Fill(sx,sy,(sx+Length),sy,color);
}

void ILI9320_DrawVLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length){
	ILI9320_Fill(sx,sy,sx,(sy+Length),color);
}
