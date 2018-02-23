#include <Arduino.h>
#include "bspio/bspio.h"
#include "ili9331.h"
#include "ili9331_reg.h"
//#include "stdlib.h"
#include "font/font.h"
#include "tftlcd/lcd.h"
 
#define  ILI9331_WR_REG(x) 	LCD_IO_WriteReg(x)
#define  ILI9331_WR_DATA(x) LCD_IO_WriteData(x)
#define  ILI9331_RD_DATA    LCD_IO_ReadData
#define  ILI9331_WriteRegData(x,y) LCD_IO_WriteRegData(x,y)


//管理LCD重要参数
//默认为竖屏

LCD_DrvTypeDef ILI9331_DRV = {
  .Init       = ILI9331_Init,
  .ReadPixel  = ILI9331_ReadPixel,
  .WritePixel = ILI9331_WritePixel,
  .Fill	= ILI9331_Fill,
  
//option  
  .DisplayOn  = ILI9331_DisplayOn,
  .DisplayOff = ILI9331_DisplayOff,
  .SetCursor  = ILI9331_SetCursor,
};


extern LCD_DevTypeDef lcddev;

uint16_t ILI9331_ReadReg(uint16_t Reg)
{										   
	ILI9331_WR_REG(Reg);		//写入要读的寄存器序号
	delayMicroseconds(5);		  
	return ILI9331_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void ILI9331_WriteRAM_Prepare(void)
{
 	ILI9331_WR_REG(lcddev.wramcmd);
}	 
//LCD写GRAM
//RGB_Code:颜色值
void ILI9331_WriteRAM(uint16_t RGB_Code)
{							    
	ILI9331_WR_DATA(RGB_Code);//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值

uint16_t ILI9331_BGR2RGB(uint16_t c)
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
uint16_t ILI9331_ReadPixel(uint16_t x,uint16_t y)
{
 	uint16_t r=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	ILI9331_SetCursor(x,y);	    
    ILI9331_WR_REG(R34);      		 				//其他IC发送读GRAM指令
	r=ILI9331_RD_DATA();							//dummy Read	   
	opt_delay(2);	  
 	r=ILI9331_RD_DATA();  		  						//实际坐标颜色
    return ILI9331_BGR2RGB(r);						//其他IC
}			 
//LCD开启显示
void ILI9331_DisplayOn(void)
{					   
	ILI9331_WriteRegData(R7,0x0173); 				 	//开启显示
}	 
//LCD关闭显示
void ILI9331_DisplayOff(void)
{	   
	ILI9331_WriteRegData(R7,0x0);//关闭显示 
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void ILI9331_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
		ILI9331_WriteRegData(lcddev.setxcmd, Xpos);
		ILI9331_WriteRegData(lcddev.setycmd, Ypos);
	}	 
} 		 
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void ILI9331_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	if(lcddev.dir==1)//横屏时，对6804不改变扫描方向！
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
		ILI9331_WriteRegData(dirreg,regval);
	}
}   
//画点
//x,y:坐标
//lcddev.TextColor:此点的颜色
void ILI9331_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{
	ILI9331_SetCursor(x,y);		//设置光标位置 
	ILI9331_WriteRAM_Prepare();	//开始写入GRAM
	ILI9331_WR_DATA(color); 
}
//快速画点
//x,y:坐标
//color:颜色
void ILI9331_Fast_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{	   
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏其实就是调转x,y坐标
		ILI9331_WriteRegData(lcddev.setxcmd,x);
		ILI9331_WriteRegData(lcddev.setycmd,y);
	}			 
	ILI9331_WR_REG(lcddev.wramcmd); 
	ILI9331_WR_DATA(color); 
}	 


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void ILI9331_Display_Dir(uint8_t dir)
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
	ILI9331_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void ILI9331_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
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
		ILI9331_WriteRegData(hsareg,hsaval);
		ILI9331_WriteRegData(heareg,heaval);
		ILI9331_WriteRegData(vsareg,vsaval);
		ILI9331_WriteRegData(veareg,veaval);		
		ILI9331_SetCursor(sx,sy);	//设置光标位置
	}
} 
//初始化lcd
//该初始化函数可以初始化各种ILI9331液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void ILI9331_Init(void)
{ 										  
	LCD_IO_Init();		 
 	delay(50); // delay 50 ms 
 	ILI9331_WriteRegData(0x0000,0x0001);
	delay(50); // delay 50 ms 
  	lcddev.id = 0X9331;   
 	{
		ILI9331_WriteRegData(0x00E7, 0x1014);
		ILI9331_WriteRegData(0x0001, 0x0100); // set SS and SM bit
		ILI9331_WriteRegData(0x0002, 0x0200); // set 1 line inversion
        ILI9331_WriteRegData(0x0003,(1<<12)|(3<<4)|(1<<3));//65K    
		//ILI9331_WriteRegData(0x0003, 0x1030); // set GRAM write direction and BGR=1.
		ILI9331_WriteRegData(0x0008, 0x0202); // set the back porch and front porch
		ILI9331_WriteRegData(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
		ILI9331_WriteRegData(0x000A, 0x0000); // FMARK function
		ILI9331_WriteRegData(0x000C, 0x0000); // RGB interface setting
		ILI9331_WriteRegData(0x000D, 0x0000); // Frame marker Position
		ILI9331_WriteRegData(0x000F, 0x0000); // RGB interface polarity
		//*************Power On sequence ****************//
		ILI9331_WriteRegData(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
		ILI9331_WriteRegData(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
		ILI9331_WriteRegData(0x0012, 0x0000); // VREG1OUT voltage
		ILI9331_WriteRegData(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
		delay(200); // Dis-charge capacitor power voltage
		ILI9331_WriteRegData(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
		ILI9331_WriteRegData(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
		delay(50); // Delay 50ms
		ILI9331_WriteRegData(0x0012, 0x000C); // Internal reference voltage= Vci;
		delay(50); // Delay 50ms
		ILI9331_WriteRegData(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
		ILI9331_WriteRegData(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
		ILI9331_WriteRegData(0x002B, 0x000B); // Set Frame Rate
		delay(50); // Delay 50ms
		ILI9331_WriteRegData(0x0020, 0x0000); // GRAM horizontal Address
		ILI9331_WriteRegData(0x0021, 0x013f); // GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//
		ILI9331_WriteRegData(0x0030, 0x0000);
		ILI9331_WriteRegData(0x0031, 0x0106);
		ILI9331_WriteRegData(0x0032, 0x0000);
		ILI9331_WriteRegData(0x0035, 0x0204);
		ILI9331_WriteRegData(0x0036, 0x160A);
		ILI9331_WriteRegData(0x0037, 0x0707);
		ILI9331_WriteRegData(0x0038, 0x0106);
		ILI9331_WriteRegData(0x0039, 0x0707);
		ILI9331_WriteRegData(0x003C, 0x0402);
		ILI9331_WriteRegData(0x003D, 0x0C0F);
		//------------------ Set GRAM area ---------------//
		ILI9331_WriteRegData(0x0050, 0x0000); // Horizontal GRAM Start Address
		ILI9331_WriteRegData(0x0051, 0x00EF); // Horizontal GRAM End Address
		ILI9331_WriteRegData(0x0052, 0x0000); // Vertical GRAM Start Address
		ILI9331_WriteRegData(0x0053, 0x013F); // Vertical GRAM Start Address
		ILI9331_WriteRegData(0x0060, 0x2700); // Gate Scan Line
		ILI9331_WriteRegData(0x0061, 0x0001); // NDL,VLE, REV 
		ILI9331_WriteRegData(0x006A, 0x0000); // set scrolling line
		//-------------- Partial Display Control ---------//
		ILI9331_WriteRegData(0x0080, 0x0000);
		ILI9331_WriteRegData(0x0081, 0x0000);
		ILI9331_WriteRegData(0x0082, 0x0000);
		ILI9331_WriteRegData(0x0083, 0x0000);
		ILI9331_WriteRegData(0x0084, 0x0000);
		ILI9331_WriteRegData(0x0085, 0x0000);
		//-------------- Panel Control -------------------//
		ILI9331_WriteRegData(0x0090, 0x0010);
		ILI9331_WriteRegData(0x0092, 0x0600);
		ILI9331_WriteRegData(0x0007, 0x0133); // 262K color and display ON
	}		 
	ILI9331_Display_Dir(1);		 	//默认为竖屏
//	ILI9331_LED=0;					//点亮背光
//	ILI9331_Clear(lcddev.TextColor);
}  
//清屏函数
//color:要清屏的填充色
void ILI9331_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	ILI9331_SetCursor(0x00,0x0000);	//设置光标位置 

	ILI9331_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		ILI9331_WR_DATA(color);	   
	}
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI9331_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
	uint16_t i,j;
	uint16_t xlen=0;
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	ILI9331_SetCursor(sx,i);      				//设置光标位置 
			ILI9331_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)ILI9331_WR_DATA(color);	//设置光标位置 	    
		}
	}	 
}  

//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI9331_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		ILI9331_SetCursor(sx,sy+i);   	//设置光标位置 
		ILI9331_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)ILI9331_WR_DATA(color[i*height+j]);//写入数据 
	}	  
}  
