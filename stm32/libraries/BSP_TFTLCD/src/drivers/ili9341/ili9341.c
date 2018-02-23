#include <Arduino.h>
#include "bspio/bspio.h"
#include "ili9341.h"
#include "ili9341_reg.h"
#include "font/font.h"
#include "tftlcd/lcd.h"
 
#define  ILI9341_WR_REG(x) 	LCD_IO_WriteReg(x)
#define  ILI9341_WR_DATA(x) LCD_IO_WriteData(x)
#define  ILI9341_RD_DATA    LCD_IO_ReadData
#define  ILI9341_WriteRegData(x,y) LCD_IO_WriteRegData(x,y)

//管理LCD重要参数
//默认为竖屏

LCD_DrvTypeDef ILI9341_DRV = {
  .Init       = ILI9341_Init,
  .DisplayOn  = ILI9341_DisplayOn,
  .DisplayOff = ILI9341_DisplayOff,
  .SetCursor  = ILI9341_SetCursor,
  .ReadPixel  = ILI9341_ReadPixel,
  .WritePixel = ILI9341_WritePixel,
  .Fill	= ILI9341_Fill,
  
//option  
  .DrawHLine = ILI9341_DrawHLine,
  .DrawVLine = ILI9341_DrawVLine,
};


extern LCD_DevTypeDef lcddev;

uint16_t ILI9341_ReadReg(uint16_t Reg)
{										   
	ILI9341_WR_REG(Reg);		//写入要读的寄存器序号
	delayMicroseconds(5);		  
	return ILI9341_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void ILI9341_WriteRAM_Prepare(void)
{
 	ILI9341_WR_REG(lcddev.wramcmd);
}	 
//LCD写GRAM
//RGB_Code:颜色值
void ILI9341_WriteRAM(uint16_t RGB_Code)
{							    
	ILI9341_WR_DATA(RGB_Code);//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值

uint16_t ILI9341_BGR2RGB(uint16_t c)
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
uint16_t ILI9341_ReadPixel(uint16_t x,uint16_t y)
{
 	uint16_t r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	ILI9341_SetCursor(x,y);	    
	ILI9341_WR_REG(0X2E);//9341/6804/3510 发送读GRAM指令
	if(ILI9341_RD_DATA())r=0;							//dummy Read	   
	opt_delay(2);	  
 	r=ILI9341_RD_DATA();  		  						//实际坐标颜色
 	opt_delay(2);	  
	b=ILI9341_RD_DATA(); 
	g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
	g<<=8;
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
}			 
//LCD开启显示
void ILI9341_DisplayOn(void)
{					   
	ILI9341_WR_REG(0X29);	//开启显示
}	 
//LCD关闭显示
void ILI9341_DisplayOff(void)
{	   
	ILI9341_WR_REG(0X28);	//关闭显示
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void ILI9341_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
		ILI9341_WR_REG(lcddev.setxcmd); 
		ILI9341_WR_DATA(Xpos>>8); 
		ILI9341_WR_DATA(Xpos&0XFF);	 
		ILI9341_WR_REG(lcddev.setycmd); 
		ILI9341_WR_DATA(Ypos>>8); 
		ILI9341_WR_DATA(Ypos&0XFF);
} 		 
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void ILI9341_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;  
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
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		dirreg=0X36;
 		regval|=0X08;//5310/5510不需要BGR   
		ILI9341_WriteRegData(dirreg,regval);
 		if((regval&0X20)||lcddev.dir==1)
		{
			if(lcddev.width<lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}else  
		{
			if(lcddev.width>lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}  
		{
			ILI9341_WR_REG(lcddev.setxcmd); 
			ILI9341_WR_DATA(0);ILI9341_WR_DATA(0);
			ILI9341_WR_DATA((lcddev.width-1)>>8);ILI9341_WR_DATA((lcddev.width-1)&0XFF);
			ILI9341_WR_REG(lcddev.setycmd); 
			ILI9341_WR_DATA(0);ILI9341_WR_DATA(0);
			ILI9341_WR_DATA((lcddev.height-1)>>8);ILI9341_WR_DATA((lcddev.height-1)&0XFF);  
		}
}   
//画点
//x,y:坐标
//lcddev.TextColor:此点的颜色
void ILI9341_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{
	ILI9341_SetCursor(x,y);		//设置光标位置 
	ILI9341_WriteRAM_Prepare();	//开始写入GRAM
	ILI9341_WR_DATA(color); 
}
//快速画点
//x,y:坐标
//color:颜色
void ILI9341_Fast_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{	   
	ILI9341_WR_REG(lcddev.setxcmd); 
	ILI9341_WR_DATA(x>>8); 
	ILI9341_WR_DATA(x&0XFF);	 
	ILI9341_WR_REG(lcddev.setycmd); 
	ILI9341_WR_DATA(y>>8); 
	ILI9341_WR_DATA(y&0XFF);
	ILI9341_WR_REG(lcddev.wramcmd); 
	ILI9341_WR_DATA(color); 
}	 


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void ILI9341_Display_Dir(uint8_t dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 
	}else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		lcddev.wramcmd=0X2C;
	 	lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 
	} 
	ILI9341_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void ILI9341_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{   
	width=sx+width-1;
	height=sy+height-1;
	ILI9341_WR_REG(lcddev.setxcmd); 
	ILI9341_WR_DATA(sx>>8); 
	ILI9341_WR_DATA(sx&0XFF);	 
	ILI9341_WR_DATA(width>>8); 
	ILI9341_WR_DATA(width&0XFF);  
	ILI9341_WR_REG(lcddev.setycmd); 
	ILI9341_WR_DATA(sy>>8); 
	ILI9341_WR_DATA(sy&0XFF); 
	ILI9341_WR_DATA(height>>8); 
	ILI9341_WR_DATA(height&0XFF); 
} 
//初始化lcd
//该初始化函数可以初始化各种ILI9341液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void ILI9341_Init(void)
{ 										  
	LCD_IO_Init();		 
 	delay(50); // delay 50 ms 
	
	lcddev.id = 0x9341; 	   			   
	
	ILI9341_WR_REG(0xCF);  
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0xC1); 
	ILI9341_WR_DATA(0X30); 
	ILI9341_WR_REG(0xED);  
	ILI9341_WR_DATA(0x64); 
	ILI9341_WR_DATA(0x03); 
	ILI9341_WR_DATA(0X12); 
	ILI9341_WR_DATA(0X81); 
	ILI9341_WR_REG(0xE8);  
	ILI9341_WR_DATA(0x85); 
	ILI9341_WR_DATA(0x10); 
	ILI9341_WR_DATA(0x7A); 
	ILI9341_WR_REG(0xCB);  
	ILI9341_WR_DATA(0x39); 
	ILI9341_WR_DATA(0x2C); 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0x34); 
	ILI9341_WR_DATA(0x02); 
	ILI9341_WR_REG(0xF7);  
	ILI9341_WR_DATA(0x20); 
	ILI9341_WR_REG(0xEA);  
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_REG(0xC0);    //Power control 
	ILI9341_WR_DATA(0x1B);   //VRH[5:0] 
	ILI9341_WR_REG(0xC1);    //Power control 
	ILI9341_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
	ILI9341_WR_REG(0xC5);    //VCM control 
	ILI9341_WR_DATA(0x30); 	 //3F
	ILI9341_WR_DATA(0x30); 	 //3C
	ILI9341_WR_REG(0xC7);    //VCM control2 
	ILI9341_WR_DATA(0XB7); 
	ILI9341_WR_REG(0x36);    // Memory Access Control 
	ILI9341_WR_DATA(0x48); 
	ILI9341_WR_REG(0x3A);   
	ILI9341_WR_DATA(0x55); 
	ILI9341_WR_REG(0xB1);   
	ILI9341_WR_DATA(0x00);   
	ILI9341_WR_DATA(0x1A); 
	ILI9341_WR_REG(0xB6);    // Display Function Control 
	ILI9341_WR_DATA(0x0A); 
	ILI9341_WR_DATA(0xA2); 
	ILI9341_WR_REG(0xF2);    // 3Gamma Function Disable 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_REG(0x26);    //Gamma curve selected 
	ILI9341_WR_DATA(0x01); 
	ILI9341_WR_REG(0xE0);    //Set Gamma 
	ILI9341_WR_DATA(0x0F); 
	ILI9341_WR_DATA(0x2A); 
	ILI9341_WR_DATA(0x28); 
	ILI9341_WR_DATA(0x08); 
	ILI9341_WR_DATA(0x0E); 
	ILI9341_WR_DATA(0x08); 
	ILI9341_WR_DATA(0x54); 
	ILI9341_WR_DATA(0XA9); 
	ILI9341_WR_DATA(0x43); 
	ILI9341_WR_DATA(0x0A); 
	ILI9341_WR_DATA(0x0F); 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0x00); 		 
	ILI9341_WR_REG(0XE1);    //Set Gamma 
	ILI9341_WR_DATA(0x00); 
	ILI9341_WR_DATA(0x15); 
	ILI9341_WR_DATA(0x17); 
	ILI9341_WR_DATA(0x07); 
	ILI9341_WR_DATA(0x11); 
	ILI9341_WR_DATA(0x06); 
	ILI9341_WR_DATA(0x2B); 
	ILI9341_WR_DATA(0x56); 
	ILI9341_WR_DATA(0x3C); 
	ILI9341_WR_DATA(0x05); 
	ILI9341_WR_DATA(0x10); 
	ILI9341_WR_DATA(0x0F); 
	ILI9341_WR_DATA(0x3F); 
	ILI9341_WR_DATA(0x3F); 
	ILI9341_WR_DATA(0x0F); 
	ILI9341_WR_REG(0x2B); 
	ILI9341_WR_DATA(0x00);
	ILI9341_WR_DATA(0x00);
	ILI9341_WR_DATA(0x01);
	ILI9341_WR_DATA(0x3f);
	ILI9341_WR_REG(0x2A); 
	ILI9341_WR_DATA(0x00);
	ILI9341_WR_DATA(0x00);
	ILI9341_WR_DATA(0x00);
	ILI9341_WR_DATA(0xef);	 
	ILI9341_WR_REG(0x11); //Exit Sleep
	delay(120);
	ILI9341_WR_REG(0x29); //display on	
	ILI9341_Display_Dir(1);		 	//默认为竖屏
//	ILI9341_LED=0;					//点亮背光
//	ILI9341_Clear(lcddev.TextColor);
}  
//清屏函数
//color:要清屏的填充色
void ILI9341_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
    ILI9341_SetCursor(0x00,0x0000);	//设置光标位置 

	ILI9341_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		ILI9341_WR_DATA(color);	   
	}
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI9341_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
	uint16_t i,j;
	uint16_t xlen=0;
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	ILI9341_SetCursor(sx,i);      				//设置光标位置 
			ILI9341_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)ILI9341_WR_DATA(color);	//设置光标位置 	    
		}
	}	 
}  

//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI9341_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		ILI9341_SetCursor(sx,sy+i);   	//设置光标位置 
		ILI9341_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)ILI9341_WR_DATA(color[i*height+j]);//写入数据 
	}	  
}
  
void ILI9341_DrawHLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length){
	ILI9341_Fill(sx,sy,(sx+Length),sy,color);
}

void ILI9341_DrawVLine(uint16_t color, uint16_t sx, uint16_t sy, uint16_t Length){
	ILI9341_Fill(sx,sy,sx,(sy+Length),color);
}
