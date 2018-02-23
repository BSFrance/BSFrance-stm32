#include <Arduino.h>
#include "bspio/bspio.h"
#include "ili93xx.h"
#include "ili93xx_reg.h"
//#include "stdlib.h"
#include "font/font.h"
#include "tftlcd/lcd.h"
 
#define  ILI93XX_WR_REG(x) 	LCD_IO_WriteReg(x)
#define  ILI93XX_WR_DATA(x) LCD_IO_WriteData(x)
#define  ILI93XX_RD_DATA    LCD_IO_ReadData
#define  ILI93XX_WriteRegData(x,y) LCD_IO_WriteRegData(x,y)

//管理LCD重要参数
//默认为竖屏

LCD_DrvTypeDef ILI93XX_DRV = {
  .Init       = ILI93XX_Init,
  .ReadPixel  = ILI93XX_ReadPixel,
  .WritePixel = ILI93XX_WritePixel,
  .Fill	= ILI93XX_Fill,
  
//option  
  .DisplayOn  = ILI93XX_DisplayOn,
  .DisplayOff = ILI93XX_DisplayOff,
  .SetCursor  = ILI93XX_SetCursor,
};


extern LCD_DevTypeDef lcddev;

uint16_t ILI93XX_ReadReg(uint16_t Reg)
{										   
	ILI93XX_WR_REG(Reg);		//写入要读的寄存器序号
	delayMicroseconds(5);		  
	return ILI93XX_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void ILI93XX_WriteRAM_Prepare(void)
{
 	ILI93XX_WR_REG(lcddev.wramcmd);
}	 
//LCD写GRAM
//RGB_Code:颜色值
void ILI93XX_WriteRAM(uint16_t RGB_Code)
{							    
	ILI93XX_WR_DATA(RGB_Code);//写十六位GRAM
}
//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值

uint16_t ILI93XX_BGR2RGB(uint16_t c)
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
uint16_t ILI93XX_ReadPixel(uint16_t x,uint16_t y)
{
 	uint16_t r=0,g=0,b=0;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回		   
	ILI93XX_SetCursor(x,y);	    
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)ILI93XX_WR_REG(0X2E);//9341/6804/3510 发送读GRAM指令
	else if(lcddev.id==0X5510)ILI93XX_WR_REG(0X2E00);	//5510 发送读GRAM指令
	else ILI93XX_WR_REG(R34);      		 				//其他IC发送读GRAM指令
 	if(lcddev.id==0X9320)opt_delay(2);				//FOR 9320,延时2us	    
	if(ILI93XX_RD_DATA())r=0;							//dummy Read	   
	opt_delay(2);	  
 	r=ILI93XX_RD_DATA();  		  						//实际坐标颜色
 	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)		//9341/NT35310/NT35510要分2次读出
 	{
		opt_delay(2);	  
		b=ILI93XX_RD_DATA(); 
		g=r&0XFF;		//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
		g<<=8;
	}else if(lcddev.id==0X6804)r=ILI93XX_RD_DATA();		//6804第二次读取的才是真实值 
	if(lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505)return r;	//这几种IC直接返回颜色值
	else if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X5510)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));//ILI9341/NT35310/NT35510需要公式转换一下
	else return ILI93XX_BGR2RGB(r);						//其他IC
}			 
//LCD开启显示
void ILI93XX_DisplayOn(void)
{					   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)ILI93XX_WR_REG(0X29);	//开启显示
	else if(lcddev.id==0X5510)ILI93XX_WR_REG(0X2900);	//开启显示
	else ILI93XX_WriteRegData(R7,0x0173); 				 	//开启显示
}	 
//LCD关闭显示
void ILI93XX_DisplayOff(void)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)ILI93XX_WR_REG(0X28);	//关闭显示
	else if(lcddev.id==0X5510)ILI93XX_WR_REG(0X2800);	//关闭显示
	else ILI93XX_WriteRegData(R7,0x0);//关闭显示 
}   
//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void ILI93XX_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
 	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{		    
		ILI93XX_WR_REG(lcddev.setxcmd); 
		ILI93XX_WR_DATA(Xpos>>8); 
		ILI93XX_WR_DATA(Xpos&0XFF);	 
		ILI93XX_WR_REG(lcddev.setycmd); 
		ILI93XX_WR_DATA(Ypos>>8); 
		ILI93XX_WR_DATA(Ypos&0XFF);
	}else if(lcddev.id==0X6804)
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏时处理
		ILI93XX_WR_REG(lcddev.setxcmd); 
		ILI93XX_WR_DATA(Xpos>>8); 
		ILI93XX_WR_DATA(Xpos&0XFF);	 
		ILI93XX_WR_REG(lcddev.setycmd); 
		ILI93XX_WR_DATA(Ypos>>8); 
		ILI93XX_WR_DATA(Ypos&0XFF);
	}else if(lcddev.id==0X5510)
	{
		ILI93XX_WR_REG(lcddev.setxcmd); 
		ILI93XX_WR_DATA(Xpos>>8); 
		ILI93XX_WR_REG(lcddev.setxcmd+1); 
		ILI93XX_WR_DATA(Xpos&0XFF);	 
		ILI93XX_WR_REG(lcddev.setycmd); 
		ILI93XX_WR_DATA(Ypos>>8); 
		ILI93XX_WR_REG(lcddev.setycmd+1); 
		ILI93XX_WR_DATA(Ypos&0XFF);		
	}else
	{
		if(lcddev.dir==1)Xpos=lcddev.width-1-Xpos;//横屏其实就是调转x,y坐标
		ILI93XX_WriteRegData(lcddev.setxcmd, Xpos);
		ILI93XX_WriteRegData(lcddev.setycmd, Ypos);
	}	 
} 		 
//设置LCD的自动扫描方向
//注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
//所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h)
//9320/9325/9328/4531/4535/1505/b505/8989/5408/9341/5310/5510等IC已经实际测试	   	   
void ILI93XX_Scan_Dir(uint8_t dir)
{
	uint16_t regval=0;
	uint16_t dirreg=0;
	uint16_t temp;  
	if(lcddev.dir==1&&lcddev.id!=0X6804)//横屏时，对6804不改变扫描方向！
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
	if(lcddev.id==0x9341||lcddev.id==0X6804||lcddev.id==0X5310||lcddev.id==0X5510)//9341/6804/5310/5510,很特殊
	{
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
		if(lcddev.id==0X5510)dirreg=0X3600;
		else dirreg=0X36;
 		if((lcddev.id!=0X5310)&&(lcddev.id!=0X5510))regval|=0X08;//5310/5510不需要BGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
		ILI93XX_WriteRegData(dirreg,regval);
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
		if(lcddev.id==0X5510)
		{
			ILI93XX_WR_REG(lcddev.setxcmd);ILI93XX_WR_DATA(0); 
			ILI93XX_WR_REG(lcddev.setxcmd+1);ILI93XX_WR_DATA(0); 
			ILI93XX_WR_REG(lcddev.setxcmd+2);ILI93XX_WR_DATA((lcddev.width-1)>>8); 
			ILI93XX_WR_REG(lcddev.setxcmd+3);ILI93XX_WR_DATA((lcddev.width-1)&0XFF); 
			ILI93XX_WR_REG(lcddev.setycmd);ILI93XX_WR_DATA(0); 
			ILI93XX_WR_REG(lcddev.setycmd+1);ILI93XX_WR_DATA(0); 
			ILI93XX_WR_REG(lcddev.setycmd+2);ILI93XX_WR_DATA((lcddev.height-1)>>8); 
			ILI93XX_WR_REG(lcddev.setycmd+3);ILI93XX_WR_DATA((lcddev.height-1)&0XFF);
		}else
		{
			ILI93XX_WR_REG(lcddev.setxcmd); 
			ILI93XX_WR_DATA(0);ILI93XX_WR_DATA(0);
			ILI93XX_WR_DATA((lcddev.width-1)>>8);ILI93XX_WR_DATA((lcddev.width-1)&0XFF);
			ILI93XX_WR_REG(lcddev.setycmd); 
			ILI93XX_WR_DATA(0);ILI93XX_WR_DATA(0);
			ILI93XX_WR_DATA((lcddev.height-1)>>8);ILI93XX_WR_DATA((lcddev.height-1)&0XFF);  
		}
  	}else 
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
		if(lcddev.id==0x8989)//8989 IC
		{
			dirreg=0X11;
			regval|=0X6040;	//65K   
	 	}else//其他驱动IC		  
		{
			dirreg=0X03;
			regval|=1<<12;  
		}
		ILI93XX_WriteRegData(dirreg,regval);
	}
}   
//画点
//x,y:坐标
//lcddev.TextColor:此点的颜色
void ILI93XX_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{
	ILI93XX_SetCursor(x,y);		//设置光标位置 
	ILI93XX_WriteRAM_Prepare();	//开始写入GRAM
	ILI93XX_WR_DATA(color); 
}
//快速画点
//x,y:坐标
//color:颜色
void ILI93XX_Fast_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{	   
	if(lcddev.id==0X9341||lcddev.id==0X5310)
	{
		ILI93XX_WR_REG(lcddev.setxcmd); 
		ILI93XX_WR_DATA(x>>8); 
		ILI93XX_WR_DATA(x&0XFF);	 
		ILI93XX_WR_REG(lcddev.setycmd); 
		ILI93XX_WR_DATA(y>>8); 
		ILI93XX_WR_DATA(y&0XFF);
	}else if(lcddev.id==0X5510)
	{
		ILI93XX_WR_REG(lcddev.setxcmd);ILI93XX_WR_DATA(x>>8);  
		ILI93XX_WR_REG(lcddev.setxcmd+1);ILI93XX_WR_DATA(x&0XFF);	  
		ILI93XX_WR_REG(lcddev.setycmd);ILI93XX_WR_DATA(y>>8);  
		ILI93XX_WR_REG(lcddev.setycmd+1);ILI93XX_WR_DATA(y&0XFF); 
	}else if(lcddev.id==0X6804)
	{		    
		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏时处理
		ILI93XX_WR_REG(lcddev.setxcmd); 
		ILI93XX_WR_DATA(x>>8); 
		ILI93XX_WR_DATA(x&0XFF);	 
		ILI93XX_WR_REG(lcddev.setycmd); 
		ILI93XX_WR_DATA(y>>8); 
		ILI93XX_WR_DATA(y&0XFF);
	}else
	{
 		if(lcddev.dir==1)x=lcddev.width-1-x;//横屏其实就是调转x,y坐标
		ILI93XX_WriteRegData(lcddev.setxcmd,x);
		ILI93XX_WriteRegData(lcddev.setycmd,y);
	}			 
	ILI93XX_WR_REG(lcddev.wramcmd); 
	ILI93XX_WR_DATA(color); 
}	 


//设置LCD显示方向
//dir:0,竖屏；1,横屏
void ILI93XX_Display_Dir(uint8_t dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804||lcddev.id==0X5310)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}else if(lcddev.id==0X8989)
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=0X4E;
			lcddev.setycmd=0X4F;  
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=480;
			lcddev.height=800;
		}else
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
		if(lcddev.id==0X9341||lcddev.id==0X5310)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}else if(lcddev.id==0X6804)	 
		{
 			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2B;
			lcddev.setycmd=0X2A; 
		}else if(lcddev.id==0X8989)
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=0X4F;
			lcddev.setycmd=0X4E;   
		}else if(lcddev.id==0x5510)
		{
			lcddev.wramcmd=0X2C00;
	 		lcddev.setxcmd=0X2A00;
			lcddev.setycmd=0X2B00; 
			lcddev.width=800;
			lcddev.height=480;
		}else
		{
			lcddev.wramcmd=R34;
	 		lcddev.setxcmd=R33;
			lcddev.setycmd=R32;  
		}
		if(lcddev.id==0X6804||lcddev.id==0X5310)
		{ 	 
			lcddev.width=480;
			lcddev.height=320; 			
		}
	} 
	ILI93XX_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}	 
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void ILI93XX_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{   
	uint8_t hsareg,heareg,vsareg,veareg;
	uint16_t hsaval,heaval,vsaval,veaval; 
	width=sx+width-1;
	height=sy+height-1;
	if(lcddev.id==0X9341||lcddev.id==0X5310||lcddev.id==0X6804)//6804横屏不支持
	{
		ILI93XX_WR_REG(lcddev.setxcmd); 
		ILI93XX_WR_DATA(sx>>8); 
		ILI93XX_WR_DATA(sx&0XFF);	 
		ILI93XX_WR_DATA(width>>8); 
		ILI93XX_WR_DATA(width&0XFF);  
		ILI93XX_WR_REG(lcddev.setycmd); 
		ILI93XX_WR_DATA(sy>>8); 
		ILI93XX_WR_DATA(sy&0XFF); 
		ILI93XX_WR_DATA(height>>8); 
		ILI93XX_WR_DATA(height&0XFF); 
	}else if(lcddev.id==0X5510)
	{
		ILI93XX_WR_REG(lcddev.setxcmd);ILI93XX_WR_DATA(sx>>8);  
		ILI93XX_WR_REG(lcddev.setxcmd+1);ILI93XX_WR_DATA(sx&0XFF);	  
		ILI93XX_WR_REG(lcddev.setxcmd+2);ILI93XX_WR_DATA(width>>8);   
		ILI93XX_WR_REG(lcddev.setxcmd+3);ILI93XX_WR_DATA(width&0XFF);   
		ILI93XX_WR_REG(lcddev.setycmd);ILI93XX_WR_DATA(sy>>8);   
		ILI93XX_WR_REG(lcddev.setycmd+1);ILI93XX_WR_DATA(sy&0XFF);  
		ILI93XX_WR_REG(lcddev.setycmd+2);ILI93XX_WR_DATA(height>>8);   
		ILI93XX_WR_REG(lcddev.setycmd+3);ILI93XX_WR_DATA(height&0XFF);  
	}else	//其他驱动IC
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
	 	if(lcddev.id==0X8989)//8989 IC
		{
			hsareg=0X44;heareg=0X44;//水平方向窗口寄存器 (1289的由一个寄存器控制)
			hsaval|=(heaval<<8);	//得到寄存器值.
			heaval=hsaval;
			vsareg=0X45;veareg=0X46;//垂直方向窗口寄存器	  
		}else  //其他驱动IC
		{
			hsareg=0X50;heareg=0X51;//水平方向窗口寄存器
			vsareg=0X52;veareg=0X53;//垂直方向窗口寄存器	  
		}								  
		//设置寄存器值
		ILI93XX_WriteRegData(hsareg,hsaval);
		ILI93XX_WriteRegData(heareg,heaval);
		ILI93XX_WriteRegData(vsareg,vsaval);
		ILI93XX_WriteRegData(veareg,veaval);		
		ILI93XX_SetCursor(sx,sy);	//设置光标位置
	}
} 
//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void ILI93XX_Init(void)
{ 										  
	LCD_IO_Init();		 
 	delay(50); // delay 50 ms 
 	ILI93XX_WriteRegData(0x0000,0x0001);
	delay(50); // delay 50 ms 
  	lcddev.id = ILI93XX_ReadReg(0x0000);   
  	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
	{	
 		//尝试9341 ID的读取		
		ILI93XX_WR_REG(0XD3);				   
		ILI93XX_RD_DATA(); 				//dummy read 	
 		ILI93XX_RD_DATA();   	    	//读到0X00
  		lcddev.id=ILI93XX_RD_DATA();   	//读取93								   
 		lcddev.id<<=8;
		lcddev.id|=ILI93XX_RD_DATA();  	//读取41 	   			   
 		if(lcddev.id!=0X9341)		//非9341,尝试是不是6804
		{	
 			ILI93XX_WR_REG(0XBF);				   
			ILI93XX_RD_DATA(); 			//dummy read 	 
	 		ILI93XX_RD_DATA();   	    //读回0X01			   
	 		ILI93XX_RD_DATA(); 			//读回0XD0 			  	
	  		lcddev.id=ILI93XX_RD_DATA();//这里读回0X68 
			lcddev.id<<=8;
	  		lcddev.id|=ILI93XX_RD_DATA();//这里读回0X04	  
			if(lcddev.id!=0X6804)	//也不是6804,尝试看看是不是NT35310
			{ 
				ILI93XX_WR_REG(0XD4);				   
				ILI93XX_RD_DATA(); 				//dummy read  
				ILI93XX_RD_DATA();   			//读回0X01	 
				lcddev.id=ILI93XX_RD_DATA();	//读回0X53	
				lcddev.id<<=8;	 
				lcddev.id|=ILI93XX_RD_DATA();	//这里读回0X10	 
				if(lcddev.id!=0X5310)		//也不是NT35310,尝试看看是不是NT35510
				{
					ILI93XX_WR_REG(0XDA00);	
					ILI93XX_RD_DATA();   		//读回0X00	 
					ILI93XX_WR_REG(0XDB00);	
					lcddev.id=ILI93XX_RD_DATA();//读回0X80
					lcddev.id<<=8;	
					ILI93XX_WR_REG(0XDC00);	
					lcddev.id|=ILI93XX_RD_DATA();//读回0X00		
					if(lcddev.id==0x8000)lcddev.id=0x5510;//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
				}
			}
 		}  	
	} 
// 	printf(" LCD ID:%x\r\n",lcddev.id); //打印LCD ID  
	if(lcddev.id==0X9341)	//9341初始化
	{	 
		ILI93XX_WR_REG(0xCF);  
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0xC1); 
		ILI93XX_WR_DATA(0X30); 
		ILI93XX_WR_REG(0xED);  
		ILI93XX_WR_DATA(0x64); 
		ILI93XX_WR_DATA(0x03); 
		ILI93XX_WR_DATA(0X12); 
		ILI93XX_WR_DATA(0X81); 
		ILI93XX_WR_REG(0xE8);  
		ILI93XX_WR_DATA(0x85); 
		ILI93XX_WR_DATA(0x10); 
		ILI93XX_WR_DATA(0x7A); 
		ILI93XX_WR_REG(0xCB);  
		ILI93XX_WR_DATA(0x39); 
		ILI93XX_WR_DATA(0x2C); 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0x34); 
		ILI93XX_WR_DATA(0x02); 
		ILI93XX_WR_REG(0xF7);  
		ILI93XX_WR_DATA(0x20); 
		ILI93XX_WR_REG(0xEA);  
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_REG(0xC0);    //Power control 
		ILI93XX_WR_DATA(0x1B);   //VRH[5:0] 
		ILI93XX_WR_REG(0xC1);    //Power control 
		ILI93XX_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
		ILI93XX_WR_REG(0xC5);    //VCM control 
		ILI93XX_WR_DATA(0x30); 	 //3F
		ILI93XX_WR_DATA(0x30); 	 //3C
		ILI93XX_WR_REG(0xC7);    //VCM control2 
		ILI93XX_WR_DATA(0XB7); 
		ILI93XX_WR_REG(0x36);    // Memory Access Control 
		ILI93XX_WR_DATA(0x48); 
		ILI93XX_WR_REG(0x3A);   
		ILI93XX_WR_DATA(0x55); 
		ILI93XX_WR_REG(0xB1);   
		ILI93XX_WR_DATA(0x00);   
		ILI93XX_WR_DATA(0x1A); 
		ILI93XX_WR_REG(0xB6);    // Display Function Control 
		ILI93XX_WR_DATA(0x0A); 
		ILI93XX_WR_DATA(0xA2); 
		ILI93XX_WR_REG(0xF2);    // 3Gamma Function Disable 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_REG(0x26);    //Gamma curve selected 
		ILI93XX_WR_DATA(0x01); 
		ILI93XX_WR_REG(0xE0);    //Set Gamma 
		ILI93XX_WR_DATA(0x0F); 
		ILI93XX_WR_DATA(0x2A); 
		ILI93XX_WR_DATA(0x28); 
		ILI93XX_WR_DATA(0x08); 
		ILI93XX_WR_DATA(0x0E); 
		ILI93XX_WR_DATA(0x08); 
		ILI93XX_WR_DATA(0x54); 
		ILI93XX_WR_DATA(0XA9); 
		ILI93XX_WR_DATA(0x43); 
		ILI93XX_WR_DATA(0x0A); 
		ILI93XX_WR_DATA(0x0F); 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0x00); 		 
		ILI93XX_WR_REG(0XE1);    //Set Gamma 
		ILI93XX_WR_DATA(0x00); 
		ILI93XX_WR_DATA(0x15); 
		ILI93XX_WR_DATA(0x17); 
		ILI93XX_WR_DATA(0x07); 
		ILI93XX_WR_DATA(0x11); 
		ILI93XX_WR_DATA(0x06); 
		ILI93XX_WR_DATA(0x2B); 
		ILI93XX_WR_DATA(0x56); 
		ILI93XX_WR_DATA(0x3C); 
		ILI93XX_WR_DATA(0x05); 
		ILI93XX_WR_DATA(0x10); 
		ILI93XX_WR_DATA(0x0F); 
		ILI93XX_WR_DATA(0x3F); 
		ILI93XX_WR_DATA(0x3F); 
		ILI93XX_WR_DATA(0x0F); 
		ILI93XX_WR_REG(0x2B); 
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x01);
		ILI93XX_WR_DATA(0x3f);
		ILI93XX_WR_REG(0x2A); 
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xef);	 
		ILI93XX_WR_REG(0x11); //Exit Sleep
		delay(120);
		ILI93XX_WR_REG(0x29); //display on	
	}else if(lcddev.id==0x6804) //6804初始化
	{
		ILI93XX_WR_REG(0X11);
		delay(20);
		ILI93XX_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
		ILI93XX_WR_DATA(0X07); 
		ILI93XX_WR_DATA(0X42); 
		ILI93XX_WR_DATA(0X1D); 
		ILI93XX_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X1a);
		ILI93XX_WR_DATA(0X09); 
		ILI93XX_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
		ILI93XX_WR_DATA(0X01);
		ILI93XX_WR_DATA(0X22);
		ILI93XX_WR_REG(0XC0);//REV SM GS 
		ILI93XX_WR_DATA(0X10);
		ILI93XX_WR_DATA(0X3B);
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X02);
		ILI93XX_WR_DATA(0X11);
		
		ILI93XX_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
		ILI93XX_WR_DATA(0X03);
		
		ILI93XX_WR_REG(0XC8);//Gamma setting
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X25);
		ILI93XX_WR_DATA(0X21);
		ILI93XX_WR_DATA(0X05);
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X0a);
		ILI93XX_WR_DATA(0X65);
		ILI93XX_WR_DATA(0X25);
		ILI93XX_WR_DATA(0X77);
		ILI93XX_WR_DATA(0X50);
		ILI93XX_WR_DATA(0X0f);
		ILI93XX_WR_DATA(0X00);	  
						  
   		ILI93XX_WR_REG(0XF8);
		ILI93XX_WR_DATA(0X01);	  

 		ILI93XX_WR_REG(0XFE);
 		ILI93XX_WR_DATA(0X00);
 		ILI93XX_WR_DATA(0X02);
		
		ILI93XX_WR_REG(0X20);//Exit invert mode

		ILI93XX_WR_REG(0X36);
		ILI93XX_WR_DATA(0X08);//原来是a
		
		ILI93XX_WR_REG(0X3A);
		ILI93XX_WR_DATA(0X55);//16位模式	  
		ILI93XX_WR_REG(0X2B);
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X01);
		ILI93XX_WR_DATA(0X3F);
		
		ILI93XX_WR_REG(0X2A);
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X00);
		ILI93XX_WR_DATA(0X01);
		ILI93XX_WR_DATA(0XDF);
		delay(120);
		ILI93XX_WR_REG(0X29); 	 
 	}else if(lcddev.id==0x5310)
	{ 
		ILI93XX_WR_REG(0xED);
		ILI93XX_WR_DATA(0x01);
		ILI93XX_WR_DATA(0xFE);

		ILI93XX_WR_REG(0xEE);
		ILI93XX_WR_DATA(0xDE);
		ILI93XX_WR_DATA(0x21);

		ILI93XX_WR_REG(0xF1);
		ILI93XX_WR_DATA(0x01);
		ILI93XX_WR_REG(0xDF);
		ILI93XX_WR_DATA(0x10);

		//VCOMvoltage//
		ILI93XX_WR_REG(0xC4);
		ILI93XX_WR_DATA(0x8F);	  //5f

		ILI93XX_WR_REG(0xC6);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xE2);
		ILI93XX_WR_DATA(0xE2);
		ILI93XX_WR_DATA(0xE2);
		ILI93XX_WR_REG(0xBF);
		ILI93XX_WR_DATA(0xAA);

		ILI93XX_WR_REG(0xB0);
		ILI93XX_WR_DATA(0x0D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x0D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x11);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x19);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x21);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x2D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x5D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x5D);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB1);
		ILI93XX_WR_DATA(0x80);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x8B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x96);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x02);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x03);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB4);
		ILI93XX_WR_DATA(0x8B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x96);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA1);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB5);
		ILI93XX_WR_DATA(0x02);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x03);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x04);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB6);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB7);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3F);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x5E);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x64);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x8C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xAC);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xDC);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x70);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x90);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xEB);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xDC);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xB8);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xBA);
		ILI93XX_WR_DATA(0x24);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC1);
		ILI93XX_WR_DATA(0x20);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x54);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xFF);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC2);
		ILI93XX_WR_DATA(0x0A);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x04);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC3);
		ILI93XX_WR_DATA(0x3C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3A);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x39);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x37);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x36);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x32);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x2F);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x2C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x29);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x26);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x24);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x24);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x23);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x36);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x32);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x2F);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x2C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x29);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x26);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x24);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x24);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x23);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC4);
		ILI93XX_WR_DATA(0x62);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x05);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x84);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF0);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x18);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA4);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x18);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x50);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x0C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x17);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x95);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xE6);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC5);
		ILI93XX_WR_DATA(0x32);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x65);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x76);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x88);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC6);
		ILI93XX_WR_DATA(0x20);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x17);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x01);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC7);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC8);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xC9);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE0);
		ILI93XX_WR_DATA(0x16);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x1C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x21);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x36);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x46);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x52);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x64);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x7A);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x8B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA8);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xB9);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC4);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xCA);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD9);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xE0);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE1);
		ILI93XX_WR_DATA(0x16);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x1C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x22);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x36);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x45);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x52);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x64);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x7A);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x8B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA8);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xB9);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC4);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xCA);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD8);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xE0);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE2);
		ILI93XX_WR_DATA(0x05);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x0B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x1B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x34);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x4F);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x61);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x79);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x88);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x97);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA6);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xB7);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC7);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD1);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD6);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xDD);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_REG(0xE3);
		ILI93XX_WR_DATA(0x05);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x1C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x33);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x50);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x62);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x78);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x88);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x97);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA6);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xB7);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC7);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD1);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD5);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xDD);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE4);
		ILI93XX_WR_DATA(0x01);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x01);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x02);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x2A);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x4B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x5D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x74);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x84);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x93);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xB3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xBE);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC4);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xCD);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xDD);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_REG(0xE5);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x02);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x29);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x3C);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x4B);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x5D);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x74);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x84);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x93);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xA2);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xB3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xBE);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xC4);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xCD);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xD3);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xDC);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xF3);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE6);
		ILI93XX_WR_DATA(0x11);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x34);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x56);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x76);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x77);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x66);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x88);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xBB);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x66);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x55);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x55);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x45);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x43);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE7);
		ILI93XX_WR_DATA(0x32);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x55);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x76);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x66);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x67);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x67);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x87);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xBB);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x77);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x56);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x23); 
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x33);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x45);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE8);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x87);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x88);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x77);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x66);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x88);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xAA);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0xBB);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x99);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x66);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x55);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x55);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x44);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x55);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xE9);
		ILI93XX_WR_DATA(0xAA);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0x00);
		ILI93XX_WR_DATA(0xAA);

		ILI93XX_WR_REG(0xCF);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xF0);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x50);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xF3);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0xF9);
		ILI93XX_WR_DATA(0x06);
		ILI93XX_WR_DATA(0x10);
		ILI93XX_WR_DATA(0x29);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0x3A);
		ILI93XX_WR_DATA(0x55);	//66

		ILI93XX_WR_REG(0x11);
		delay(100);
		ILI93XX_WR_REG(0x29);
		ILI93XX_WR_REG(0x35);
		ILI93XX_WR_DATA(0x00);

		ILI93XX_WR_REG(0x51);
		ILI93XX_WR_DATA(0xFF);
		ILI93XX_WR_REG(0x53);
		ILI93XX_WR_DATA(0x2C);
		ILI93XX_WR_REG(0x55);
		ILI93XX_WR_DATA(0x82);
		ILI93XX_WR_REG(0x2c);
	}else if(lcddev.id==0x5510)
	{
		ILI93XX_WriteRegData(0xF000,0x55);
		ILI93XX_WriteRegData(0xF001,0xAA);
		ILI93XX_WriteRegData(0xF002,0x52);
		ILI93XX_WriteRegData(0xF003,0x08);
		ILI93XX_WriteRegData(0xF004,0x01);
		//AVDD Set AVDD 5.2V
		ILI93XX_WriteRegData(0xB000,0x0D);
		ILI93XX_WriteRegData(0xB001,0x0D);
		ILI93XX_WriteRegData(0xB002,0x0D);
		//AVDD ratio
		ILI93XX_WriteRegData(0xB600,0x34);
		ILI93XX_WriteRegData(0xB601,0x34);
		ILI93XX_WriteRegData(0xB602,0x34);
		//AVEE -5.2V
		ILI93XX_WriteRegData(0xB100,0x0D);
		ILI93XX_WriteRegData(0xB101,0x0D);
		ILI93XX_WriteRegData(0xB102,0x0D);
		//AVEE ratio
		ILI93XX_WriteRegData(0xB700,0x34);
		ILI93XX_WriteRegData(0xB701,0x34);
		ILI93XX_WriteRegData(0xB702,0x34);
		//VCL -2.5V
		ILI93XX_WriteRegData(0xB200,0x00);
		ILI93XX_WriteRegData(0xB201,0x00);
		ILI93XX_WriteRegData(0xB202,0x00);
		//VCL ratio
		ILI93XX_WriteRegData(0xB800,0x24);
		ILI93XX_WriteRegData(0xB801,0x24);
		ILI93XX_WriteRegData(0xB802,0x24);
		//VGH 15V (Free pump)
		ILI93XX_WriteRegData(0xBF00,0x01);
		ILI93XX_WriteRegData(0xB300,0x0F);
		ILI93XX_WriteRegData(0xB301,0x0F);
		ILI93XX_WriteRegData(0xB302,0x0F);
		//VGH ratio
		ILI93XX_WriteRegData(0xB900,0x34);
		ILI93XX_WriteRegData(0xB901,0x34);
		ILI93XX_WriteRegData(0xB902,0x34);
		//VGL_REG -10V
		ILI93XX_WriteRegData(0xB500,0x08);
		ILI93XX_WriteRegData(0xB501,0x08);
		ILI93XX_WriteRegData(0xB502,0x08);
		ILI93XX_WriteRegData(0xC200,0x03);
		//VGLX ratio
		ILI93XX_WriteRegData(0xBA00,0x24);
		ILI93XX_WriteRegData(0xBA01,0x24);
		ILI93XX_WriteRegData(0xBA02,0x24);
		//VGMP/VGSP 4.5V/0V
		ILI93XX_WriteRegData(0xBC00,0x00);
		ILI93XX_WriteRegData(0xBC01,0x78);
		ILI93XX_WriteRegData(0xBC02,0x00);
		//VGMN/VGSN -4.5V/0V
		ILI93XX_WriteRegData(0xBD00,0x00);
		ILI93XX_WriteRegData(0xBD01,0x78);
		ILI93XX_WriteRegData(0xBD02,0x00);
		//VCOM
		ILI93XX_WriteRegData(0xBE00,0x00);
		ILI93XX_WriteRegData(0xBE01,0x64);
		//Gamma Setting
		ILI93XX_WriteRegData(0xD100,0x00);
		ILI93XX_WriteRegData(0xD101,0x33);
		ILI93XX_WriteRegData(0xD102,0x00);
		ILI93XX_WriteRegData(0xD103,0x34);
		ILI93XX_WriteRegData(0xD104,0x00);
		ILI93XX_WriteRegData(0xD105,0x3A);
		ILI93XX_WriteRegData(0xD106,0x00);
		ILI93XX_WriteRegData(0xD107,0x4A);
		ILI93XX_WriteRegData(0xD108,0x00);
		ILI93XX_WriteRegData(0xD109,0x5C);
		ILI93XX_WriteRegData(0xD10A,0x00);
		ILI93XX_WriteRegData(0xD10B,0x81);
		ILI93XX_WriteRegData(0xD10C,0x00);
		ILI93XX_WriteRegData(0xD10D,0xA6);
		ILI93XX_WriteRegData(0xD10E,0x00);
		ILI93XX_WriteRegData(0xD10F,0xE5);
		ILI93XX_WriteRegData(0xD110,0x01);
		ILI93XX_WriteRegData(0xD111,0x13);
		ILI93XX_WriteRegData(0xD112,0x01);
		ILI93XX_WriteRegData(0xD113,0x54);
		ILI93XX_WriteRegData(0xD114,0x01);
		ILI93XX_WriteRegData(0xD115,0x82);
		ILI93XX_WriteRegData(0xD116,0x01);
		ILI93XX_WriteRegData(0xD117,0xCA);
		ILI93XX_WriteRegData(0xD118,0x02);
		ILI93XX_WriteRegData(0xD119,0x00);
		ILI93XX_WriteRegData(0xD11A,0x02);
		ILI93XX_WriteRegData(0xD11B,0x01);
		ILI93XX_WriteRegData(0xD11C,0x02);
		ILI93XX_WriteRegData(0xD11D,0x34);
		ILI93XX_WriteRegData(0xD11E,0x02);
		ILI93XX_WriteRegData(0xD11F,0x67);
		ILI93XX_WriteRegData(0xD120,0x02);
		ILI93XX_WriteRegData(0xD121,0x84);
		ILI93XX_WriteRegData(0xD122,0x02);
		ILI93XX_WriteRegData(0xD123,0xA4);
		ILI93XX_WriteRegData(0xD124,0x02);
		ILI93XX_WriteRegData(0xD125,0xB7);
		ILI93XX_WriteRegData(0xD126,0x02);
		ILI93XX_WriteRegData(0xD127,0xCF);
		ILI93XX_WriteRegData(0xD128,0x02);
		ILI93XX_WriteRegData(0xD129,0xDE);
		ILI93XX_WriteRegData(0xD12A,0x02);
		ILI93XX_WriteRegData(0xD12B,0xF2);
		ILI93XX_WriteRegData(0xD12C,0x02);
		ILI93XX_WriteRegData(0xD12D,0xFE);
		ILI93XX_WriteRegData(0xD12E,0x03);
		ILI93XX_WriteRegData(0xD12F,0x10);
		ILI93XX_WriteRegData(0xD130,0x03);
		ILI93XX_WriteRegData(0xD131,0x33);
		ILI93XX_WriteRegData(0xD132,0x03);
		ILI93XX_WriteRegData(0xD133,0x6D);
		ILI93XX_WriteRegData(0xD200,0x00);
		ILI93XX_WriteRegData(0xD201,0x33);
		ILI93XX_WriteRegData(0xD202,0x00);
		ILI93XX_WriteRegData(0xD203,0x34);
		ILI93XX_WriteRegData(0xD204,0x00);
		ILI93XX_WriteRegData(0xD205,0x3A);
		ILI93XX_WriteRegData(0xD206,0x00);
		ILI93XX_WriteRegData(0xD207,0x4A);
		ILI93XX_WriteRegData(0xD208,0x00);
		ILI93XX_WriteRegData(0xD209,0x5C);
		ILI93XX_WriteRegData(0xD20A,0x00);

		ILI93XX_WriteRegData(0xD20B,0x81);
		ILI93XX_WriteRegData(0xD20C,0x00);
		ILI93XX_WriteRegData(0xD20D,0xA6);
		ILI93XX_WriteRegData(0xD20E,0x00);
		ILI93XX_WriteRegData(0xD20F,0xE5);
		ILI93XX_WriteRegData(0xD210,0x01);
		ILI93XX_WriteRegData(0xD211,0x13);
		ILI93XX_WriteRegData(0xD212,0x01);
		ILI93XX_WriteRegData(0xD213,0x54);
		ILI93XX_WriteRegData(0xD214,0x01);
		ILI93XX_WriteRegData(0xD215,0x82);
		ILI93XX_WriteRegData(0xD216,0x01);
		ILI93XX_WriteRegData(0xD217,0xCA);
		ILI93XX_WriteRegData(0xD218,0x02);
		ILI93XX_WriteRegData(0xD219,0x00);
		ILI93XX_WriteRegData(0xD21A,0x02);
		ILI93XX_WriteRegData(0xD21B,0x01);
		ILI93XX_WriteRegData(0xD21C,0x02);
		ILI93XX_WriteRegData(0xD21D,0x34);
		ILI93XX_WriteRegData(0xD21E,0x02);
		ILI93XX_WriteRegData(0xD21F,0x67);
		ILI93XX_WriteRegData(0xD220,0x02);
		ILI93XX_WriteRegData(0xD221,0x84);
		ILI93XX_WriteRegData(0xD222,0x02);
		ILI93XX_WriteRegData(0xD223,0xA4);
		ILI93XX_WriteRegData(0xD224,0x02);
		ILI93XX_WriteRegData(0xD225,0xB7);
		ILI93XX_WriteRegData(0xD226,0x02);
		ILI93XX_WriteRegData(0xD227,0xCF);
		ILI93XX_WriteRegData(0xD228,0x02);
		ILI93XX_WriteRegData(0xD229,0xDE);
		ILI93XX_WriteRegData(0xD22A,0x02);
		ILI93XX_WriteRegData(0xD22B,0xF2);
		ILI93XX_WriteRegData(0xD22C,0x02);
		ILI93XX_WriteRegData(0xD22D,0xFE);
		ILI93XX_WriteRegData(0xD22E,0x03);
		ILI93XX_WriteRegData(0xD22F,0x10);
		ILI93XX_WriteRegData(0xD230,0x03);
		ILI93XX_WriteRegData(0xD231,0x33);
		ILI93XX_WriteRegData(0xD232,0x03);
		ILI93XX_WriteRegData(0xD233,0x6D);
		ILI93XX_WriteRegData(0xD300,0x00);
		ILI93XX_WriteRegData(0xD301,0x33);
		ILI93XX_WriteRegData(0xD302,0x00);
		ILI93XX_WriteRegData(0xD303,0x34);
		ILI93XX_WriteRegData(0xD304,0x00);
		ILI93XX_WriteRegData(0xD305,0x3A);
		ILI93XX_WriteRegData(0xD306,0x00);
		ILI93XX_WriteRegData(0xD307,0x4A);
		ILI93XX_WriteRegData(0xD308,0x00);
		ILI93XX_WriteRegData(0xD309,0x5C);
		ILI93XX_WriteRegData(0xD30A,0x00);

		ILI93XX_WriteRegData(0xD30B,0x81);
		ILI93XX_WriteRegData(0xD30C,0x00);
		ILI93XX_WriteRegData(0xD30D,0xA6);
		ILI93XX_WriteRegData(0xD30E,0x00);
		ILI93XX_WriteRegData(0xD30F,0xE5);
		ILI93XX_WriteRegData(0xD310,0x01);
		ILI93XX_WriteRegData(0xD311,0x13);
		ILI93XX_WriteRegData(0xD312,0x01);
		ILI93XX_WriteRegData(0xD313,0x54);
		ILI93XX_WriteRegData(0xD314,0x01);
		ILI93XX_WriteRegData(0xD315,0x82);
		ILI93XX_WriteRegData(0xD316,0x01);
		ILI93XX_WriteRegData(0xD317,0xCA);
		ILI93XX_WriteRegData(0xD318,0x02);
		ILI93XX_WriteRegData(0xD319,0x00);
		ILI93XX_WriteRegData(0xD31A,0x02);
		ILI93XX_WriteRegData(0xD31B,0x01);
		ILI93XX_WriteRegData(0xD31C,0x02);
		ILI93XX_WriteRegData(0xD31D,0x34);
		ILI93XX_WriteRegData(0xD31E,0x02);
		ILI93XX_WriteRegData(0xD31F,0x67);
		ILI93XX_WriteRegData(0xD320,0x02);
		ILI93XX_WriteRegData(0xD321,0x84);
		ILI93XX_WriteRegData(0xD322,0x02);
		ILI93XX_WriteRegData(0xD323,0xA4);
		ILI93XX_WriteRegData(0xD324,0x02);
		ILI93XX_WriteRegData(0xD325,0xB7);
		ILI93XX_WriteRegData(0xD326,0x02);
		ILI93XX_WriteRegData(0xD327,0xCF);
		ILI93XX_WriteRegData(0xD328,0x02);
		ILI93XX_WriteRegData(0xD329,0xDE);
		ILI93XX_WriteRegData(0xD32A,0x02);
		ILI93XX_WriteRegData(0xD32B,0xF2);
		ILI93XX_WriteRegData(0xD32C,0x02);
		ILI93XX_WriteRegData(0xD32D,0xFE);
		ILI93XX_WriteRegData(0xD32E,0x03);
		ILI93XX_WriteRegData(0xD32F,0x10);
		ILI93XX_WriteRegData(0xD330,0x03);
		ILI93XX_WriteRegData(0xD331,0x33);
		ILI93XX_WriteRegData(0xD332,0x03);
		ILI93XX_WriteRegData(0xD333,0x6D);
		ILI93XX_WriteRegData(0xD400,0x00);
		ILI93XX_WriteRegData(0xD401,0x33);
		ILI93XX_WriteRegData(0xD402,0x00);
		ILI93XX_WriteRegData(0xD403,0x34);
		ILI93XX_WriteRegData(0xD404,0x00);
		ILI93XX_WriteRegData(0xD405,0x3A);
		ILI93XX_WriteRegData(0xD406,0x00);
		ILI93XX_WriteRegData(0xD407,0x4A);
		ILI93XX_WriteRegData(0xD408,0x00);
		ILI93XX_WriteRegData(0xD409,0x5C);
		ILI93XX_WriteRegData(0xD40A,0x00);
		ILI93XX_WriteRegData(0xD40B,0x81);

		ILI93XX_WriteRegData(0xD40C,0x00);
		ILI93XX_WriteRegData(0xD40D,0xA6);
		ILI93XX_WriteRegData(0xD40E,0x00);
		ILI93XX_WriteRegData(0xD40F,0xE5);
		ILI93XX_WriteRegData(0xD410,0x01);
		ILI93XX_WriteRegData(0xD411,0x13);
		ILI93XX_WriteRegData(0xD412,0x01);
		ILI93XX_WriteRegData(0xD413,0x54);
		ILI93XX_WriteRegData(0xD414,0x01);
		ILI93XX_WriteRegData(0xD415,0x82);
		ILI93XX_WriteRegData(0xD416,0x01);
		ILI93XX_WriteRegData(0xD417,0xCA);
		ILI93XX_WriteRegData(0xD418,0x02);
		ILI93XX_WriteRegData(0xD419,0x00);
		ILI93XX_WriteRegData(0xD41A,0x02);
		ILI93XX_WriteRegData(0xD41B,0x01);
		ILI93XX_WriteRegData(0xD41C,0x02);
		ILI93XX_WriteRegData(0xD41D,0x34);
		ILI93XX_WriteRegData(0xD41E,0x02);
		ILI93XX_WriteRegData(0xD41F,0x67);
		ILI93XX_WriteRegData(0xD420,0x02);
		ILI93XX_WriteRegData(0xD421,0x84);
		ILI93XX_WriteRegData(0xD422,0x02);
		ILI93XX_WriteRegData(0xD423,0xA4);
		ILI93XX_WriteRegData(0xD424,0x02);
		ILI93XX_WriteRegData(0xD425,0xB7);
		ILI93XX_WriteRegData(0xD426,0x02);
		ILI93XX_WriteRegData(0xD427,0xCF);
		ILI93XX_WriteRegData(0xD428,0x02);
		ILI93XX_WriteRegData(0xD429,0xDE);
		ILI93XX_WriteRegData(0xD42A,0x02);
		ILI93XX_WriteRegData(0xD42B,0xF2);
		ILI93XX_WriteRegData(0xD42C,0x02);
		ILI93XX_WriteRegData(0xD42D,0xFE);
		ILI93XX_WriteRegData(0xD42E,0x03);
		ILI93XX_WriteRegData(0xD42F,0x10);
		ILI93XX_WriteRegData(0xD430,0x03);
		ILI93XX_WriteRegData(0xD431,0x33);
		ILI93XX_WriteRegData(0xD432,0x03);
		ILI93XX_WriteRegData(0xD433,0x6D);
		ILI93XX_WriteRegData(0xD500,0x00);
		ILI93XX_WriteRegData(0xD501,0x33);
		ILI93XX_WriteRegData(0xD502,0x00);
		ILI93XX_WriteRegData(0xD503,0x34);
		ILI93XX_WriteRegData(0xD504,0x00);
		ILI93XX_WriteRegData(0xD505,0x3A);
		ILI93XX_WriteRegData(0xD506,0x00);
		ILI93XX_WriteRegData(0xD507,0x4A);
		ILI93XX_WriteRegData(0xD508,0x00);
		ILI93XX_WriteRegData(0xD509,0x5C);
		ILI93XX_WriteRegData(0xD50A,0x00);
		ILI93XX_WriteRegData(0xD50B,0x81);

		ILI93XX_WriteRegData(0xD50C,0x00);
		ILI93XX_WriteRegData(0xD50D,0xA6);
		ILI93XX_WriteRegData(0xD50E,0x00);
		ILI93XX_WriteRegData(0xD50F,0xE5);
		ILI93XX_WriteRegData(0xD510,0x01);
		ILI93XX_WriteRegData(0xD511,0x13);
		ILI93XX_WriteRegData(0xD512,0x01);
		ILI93XX_WriteRegData(0xD513,0x54);
		ILI93XX_WriteRegData(0xD514,0x01);
		ILI93XX_WriteRegData(0xD515,0x82);
		ILI93XX_WriteRegData(0xD516,0x01);
		ILI93XX_WriteRegData(0xD517,0xCA);
		ILI93XX_WriteRegData(0xD518,0x02);
		ILI93XX_WriteRegData(0xD519,0x00);
		ILI93XX_WriteRegData(0xD51A,0x02);
		ILI93XX_WriteRegData(0xD51B,0x01);
		ILI93XX_WriteRegData(0xD51C,0x02);
		ILI93XX_WriteRegData(0xD51D,0x34);
		ILI93XX_WriteRegData(0xD51E,0x02);
		ILI93XX_WriteRegData(0xD51F,0x67);
		ILI93XX_WriteRegData(0xD520,0x02);
		ILI93XX_WriteRegData(0xD521,0x84);
		ILI93XX_WriteRegData(0xD522,0x02);
		ILI93XX_WriteRegData(0xD523,0xA4);
		ILI93XX_WriteRegData(0xD524,0x02);
		ILI93XX_WriteRegData(0xD525,0xB7);
		ILI93XX_WriteRegData(0xD526,0x02);
		ILI93XX_WriteRegData(0xD527,0xCF);
		ILI93XX_WriteRegData(0xD528,0x02);
		ILI93XX_WriteRegData(0xD529,0xDE);
		ILI93XX_WriteRegData(0xD52A,0x02);
		ILI93XX_WriteRegData(0xD52B,0xF2);
		ILI93XX_WriteRegData(0xD52C,0x02);
		ILI93XX_WriteRegData(0xD52D,0xFE);
		ILI93XX_WriteRegData(0xD52E,0x03);
		ILI93XX_WriteRegData(0xD52F,0x10);
		ILI93XX_WriteRegData(0xD530,0x03);
		ILI93XX_WriteRegData(0xD531,0x33);
		ILI93XX_WriteRegData(0xD532,0x03);
		ILI93XX_WriteRegData(0xD533,0x6D);
		ILI93XX_WriteRegData(0xD600,0x00);
		ILI93XX_WriteRegData(0xD601,0x33);
		ILI93XX_WriteRegData(0xD602,0x00);
		ILI93XX_WriteRegData(0xD603,0x34);
		ILI93XX_WriteRegData(0xD604,0x00);
		ILI93XX_WriteRegData(0xD605,0x3A);
		ILI93XX_WriteRegData(0xD606,0x00);
		ILI93XX_WriteRegData(0xD607,0x4A);
		ILI93XX_WriteRegData(0xD608,0x00);
		ILI93XX_WriteRegData(0xD609,0x5C);
		ILI93XX_WriteRegData(0xD60A,0x00);
		ILI93XX_WriteRegData(0xD60B,0x81);

		ILI93XX_WriteRegData(0xD60C,0x00);
		ILI93XX_WriteRegData(0xD60D,0xA6);
		ILI93XX_WriteRegData(0xD60E,0x00);
		ILI93XX_WriteRegData(0xD60F,0xE5);
		ILI93XX_WriteRegData(0xD610,0x01);
		ILI93XX_WriteRegData(0xD611,0x13);
		ILI93XX_WriteRegData(0xD612,0x01);
		ILI93XX_WriteRegData(0xD613,0x54);
		ILI93XX_WriteRegData(0xD614,0x01);
		ILI93XX_WriteRegData(0xD615,0x82);
		ILI93XX_WriteRegData(0xD616,0x01);
		ILI93XX_WriteRegData(0xD617,0xCA);
		ILI93XX_WriteRegData(0xD618,0x02);
		ILI93XX_WriteRegData(0xD619,0x00);
		ILI93XX_WriteRegData(0xD61A,0x02);
		ILI93XX_WriteRegData(0xD61B,0x01);
		ILI93XX_WriteRegData(0xD61C,0x02);
		ILI93XX_WriteRegData(0xD61D,0x34);
		ILI93XX_WriteRegData(0xD61E,0x02);
		ILI93XX_WriteRegData(0xD61F,0x67);
		ILI93XX_WriteRegData(0xD620,0x02);
		ILI93XX_WriteRegData(0xD621,0x84);
		ILI93XX_WriteRegData(0xD622,0x02);
		ILI93XX_WriteRegData(0xD623,0xA4);
		ILI93XX_WriteRegData(0xD624,0x02);
		ILI93XX_WriteRegData(0xD625,0xB7);
		ILI93XX_WriteRegData(0xD626,0x02);
		ILI93XX_WriteRegData(0xD627,0xCF);
		ILI93XX_WriteRegData(0xD628,0x02);
		ILI93XX_WriteRegData(0xD629,0xDE);
		ILI93XX_WriteRegData(0xD62A,0x02);
		ILI93XX_WriteRegData(0xD62B,0xF2);
		ILI93XX_WriteRegData(0xD62C,0x02);
		ILI93XX_WriteRegData(0xD62D,0xFE);
		ILI93XX_WriteRegData(0xD62E,0x03);
		ILI93XX_WriteRegData(0xD62F,0x10);
		ILI93XX_WriteRegData(0xD630,0x03);
		ILI93XX_WriteRegData(0xD631,0x33);
		ILI93XX_WriteRegData(0xD632,0x03);
		ILI93XX_WriteRegData(0xD633,0x6D);
		//LV2 Page 0 enable
		ILI93XX_WriteRegData(0xF000,0x55);
		ILI93XX_WriteRegData(0xF001,0xAA);
		ILI93XX_WriteRegData(0xF002,0x52);
		ILI93XX_WriteRegData(0xF003,0x08);
		ILI93XX_WriteRegData(0xF004,0x00);
		//Display control
		ILI93XX_WriteRegData(0xB100, 0xCC);
		ILI93XX_WriteRegData(0xB101, 0x00);
		//Source hold time
		ILI93XX_WriteRegData(0xB600,0x05);
		//Gate EQ control
		ILI93XX_WriteRegData(0xB700,0x70);
		ILI93XX_WriteRegData(0xB701,0x70);
		//Source EQ control (Mode 2)
		ILI93XX_WriteRegData(0xB800,0x01);
		ILI93XX_WriteRegData(0xB801,0x03);
		ILI93XX_WriteRegData(0xB802,0x03);
		ILI93XX_WriteRegData(0xB803,0x03);
		//Inversion mode (2-dot)
		ILI93XX_WriteRegData(0xBC00,0x02);
		ILI93XX_WriteRegData(0xBC01,0x00);
		ILI93XX_WriteRegData(0xBC02,0x00);
		//Timing control 4H w/ 4-delay
		ILI93XX_WriteRegData(0xC900,0xD0);
		ILI93XX_WriteRegData(0xC901,0x02);
		ILI93XX_WriteRegData(0xC902,0x50);
		ILI93XX_WriteRegData(0xC903,0x50);
		ILI93XX_WriteRegData(0xC904,0x50);
		ILI93XX_WriteRegData(0x3500,0x00);
		ILI93XX_WriteRegData(0x3A00,0x55);  //16-bit/pixel
		ILI93XX_WR_REG(0x1100);
		delayMicroseconds(120);
		ILI93XX_WR_REG(0x2900);
	}else if(lcddev.id==0x9325)//9325
	{
		ILI93XX_WriteRegData(0x00E5,0x78F0); 
		ILI93XX_WriteRegData(0x0001,0x0100); 
		ILI93XX_WriteRegData(0x0002,0x0700); 
		ILI93XX_WriteRegData(0x0003,0x1030); 
		ILI93XX_WriteRegData(0x0004,0x0000); 
		ILI93XX_WriteRegData(0x0008,0x0202);  
		ILI93XX_WriteRegData(0x0009,0x0000);
		ILI93XX_WriteRegData(0x000A,0x0000); 
		ILI93XX_WriteRegData(0x000C,0x0000); 
		ILI93XX_WriteRegData(0x000D,0x0000);
		ILI93XX_WriteRegData(0x000F,0x0000);
		//power on sequence VGHVGL
		ILI93XX_WriteRegData(0x0010,0x0000);   
		ILI93XX_WriteRegData(0x0011,0x0007);  
		ILI93XX_WriteRegData(0x0012,0x0000);  
		ILI93XX_WriteRegData(0x0013,0x0000); 
		ILI93XX_WriteRegData(0x0007,0x0000); 
		//vgh 
		ILI93XX_WriteRegData(0x0010,0x1690);   
		ILI93XX_WriteRegData(0x0011,0x0227);
		//delayms(100);
		//vregiout 
		ILI93XX_WriteRegData(0x0012,0x009D); //0x001b
		//delayms(100); 
		//vom amplitude
		ILI93XX_WriteRegData(0x0013,0x1900);
		//delayms(100); 
		//vom H
		ILI93XX_WriteRegData(0x0029,0x0025); 
		ILI93XX_WriteRegData(0x002B,0x000D); 
		//gamma
		ILI93XX_WriteRegData(0x0030,0x0007);
		ILI93XX_WriteRegData(0x0031,0x0303);
		ILI93XX_WriteRegData(0x0032,0x0003);// 0006
		ILI93XX_WriteRegData(0x0035,0x0206);
		ILI93XX_WriteRegData(0x0036,0x0008);
		ILI93XX_WriteRegData(0x0037,0x0406); 
		ILI93XX_WriteRegData(0x0038,0x0304);//0200
		ILI93XX_WriteRegData(0x0039,0x0007); 
		ILI93XX_WriteRegData(0x003C,0x0602);// 0504
		ILI93XX_WriteRegData(0x003D,0x0008); 
		//ram
		ILI93XX_WriteRegData(0x0050,0x0000); 
		ILI93XX_WriteRegData(0x0051,0x00EF);
		ILI93XX_WriteRegData(0x0052,0x0000); 
		ILI93XX_WriteRegData(0x0053,0x013F);  
		ILI93XX_WriteRegData(0x0060,0xA700); 
		ILI93XX_WriteRegData(0x0061,0x0001); 
		ILI93XX_WriteRegData(0x006A,0x0000); 
		//
		ILI93XX_WriteRegData(0x0080,0x0000); 
		ILI93XX_WriteRegData(0x0081,0x0000); 
		ILI93XX_WriteRegData(0x0082,0x0000); 
		ILI93XX_WriteRegData(0x0083,0x0000); 
		ILI93XX_WriteRegData(0x0084,0x0000); 
		ILI93XX_WriteRegData(0x0085,0x0000); 
		//
		ILI93XX_WriteRegData(0x0090,0x0010); 
		ILI93XX_WriteRegData(0x0092,0x0600); 
		
		ILI93XX_WriteRegData(0x0007,0x0133);
		ILI93XX_WriteRegData(0x00,0x0022);//
	}else if(lcddev.id==0x9328)//ILI9328   OK  
	{
  		ILI93XX_WriteRegData(0x00EC,0x108F);// internal timeing      
 		ILI93XX_WriteRegData(0x00EF,0x1234);// ADD        
		//ILI93XX_WriteRegData(0x00e7,0x0010);      
        //ILI93XX_WriteRegData(0x0000,0x0001);//开启内部时钟
        ILI93XX_WriteRegData(0x0001,0x0100);     
        ILI93XX_WriteRegData(0x0002,0x0700);//电源开启                    
		//ILI93XX_WriteRegData(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
		//DRIVE TABLE(寄存器 03H)
		//BIT3=AM BIT4:5=ID0:1
		//AM ID0 ID1   FUNCATION
		// 0  0   0	   R->L D->U
		// 1  0   0	   D->U	R->L
		// 0  1   0	   L->R D->U
		// 1  1   0    D->U	L->R
		// 0  0   1	   R->L U->D
		// 1  0   1    U->D	R->L
		// 0  1   1    L->R U->D 正常就用这个.
		// 1  1   1	   U->D	L->R
        ILI93XX_WriteRegData(0x0003,(1<<12)|(3<<4)|(0<<3) );//65K    
        ILI93XX_WriteRegData(0x0004,0x0000);                                   
        ILI93XX_WriteRegData(0x0008,0x0202);	           
        ILI93XX_WriteRegData(0x0009,0x0000);         
        ILI93XX_WriteRegData(0x000a,0x0000);//display setting         
        ILI93XX_WriteRegData(0x000c,0x0001);//display setting          
        ILI93XX_WriteRegData(0x000d,0x0000);//0f3c          
        ILI93XX_WriteRegData(0x000f,0x0000);
		//电源配置
        ILI93XX_WriteRegData(0x0010,0x0000);   
        ILI93XX_WriteRegData(0x0011,0x0007);
        ILI93XX_WriteRegData(0x0012,0x0000);                                                                 
        ILI93XX_WriteRegData(0x0013,0x0000);                 
     	ILI93XX_WriteRegData(0x0007,0x0001);                 
       	delay(50); 
        ILI93XX_WriteRegData(0x0010,0x1490);   
        ILI93XX_WriteRegData(0x0011,0x0227);
        delay(50); 
        ILI93XX_WriteRegData(0x0012,0x008A);                  
        delay(50); 
        ILI93XX_WriteRegData(0x0013,0x1a00);   
        ILI93XX_WriteRegData(0x0029,0x0006);
        ILI93XX_WriteRegData(0x002b,0x000d);
        delay(50); 
        ILI93XX_WriteRegData(0x0020,0x0000);                                                            
        ILI93XX_WriteRegData(0x0021,0x0000);           
		delay(50); 
		//伽马校正
        ILI93XX_WriteRegData(0x0030,0x0000); 
        ILI93XX_WriteRegData(0x0031,0x0604);   
        ILI93XX_WriteRegData(0x0032,0x0305);
        ILI93XX_WriteRegData(0x0035,0x0000);
        ILI93XX_WriteRegData(0x0036,0x0C09); 
        ILI93XX_WriteRegData(0x0037,0x0204);
        ILI93XX_WriteRegData(0x0038,0x0301);        
        ILI93XX_WriteRegData(0x0039,0x0707);     
        ILI93XX_WriteRegData(0x003c,0x0000);
        ILI93XX_WriteRegData(0x003d,0x0a0a);
        delay(50); 
        ILI93XX_WriteRegData(0x0050,0x0000); //水平GRAM起始位置 
        ILI93XX_WriteRegData(0x0051,0x00ef); //水平GRAM终止位置                    
        ILI93XX_WriteRegData(0x0052,0x0000); //垂直GRAM起始位置                    
        ILI93XX_WriteRegData(0x0053,0x013f); //垂直GRAM终止位置  
 
         ILI93XX_WriteRegData(0x0060,0xa700);        
        ILI93XX_WriteRegData(0x0061,0x0001); 
        ILI93XX_WriteRegData(0x006a,0x0000);
        ILI93XX_WriteRegData(0x0080,0x0000);
        ILI93XX_WriteRegData(0x0081,0x0000);
        ILI93XX_WriteRegData(0x0082,0x0000);
        ILI93XX_WriteRegData(0x0083,0x0000);
        ILI93XX_WriteRegData(0x0084,0x0000);
        ILI93XX_WriteRegData(0x0085,0x0000);
      
        ILI93XX_WriteRegData(0x0090,0x0010);     
        ILI93XX_WriteRegData(0x0092,0x0600);  
        //开启显示设置    
        ILI93XX_WriteRegData(0x0007,0x0133); 
	}else if(lcddev.id==0x9320)//测试OK.
	{
		ILI93XX_WriteRegData(0x00,0x0000);
		ILI93XX_WriteRegData(0x01,0x0100);	//Driver Output Contral.
		ILI93XX_WriteRegData(0x02,0x0700);	//LCD Driver Waveform Contral.
		ILI93XX_WriteRegData(0x03,0x1030);//Entry Mode Set.
		//ILI93XX_WriteRegData(0x03,0x1018);	//Entry Mode Set.
	
		ILI93XX_WriteRegData(0x04,0x0000);	//Scalling Contral.
		ILI93XX_WriteRegData(0x08,0x0202);	//Display Contral 2.(0x0207)
		ILI93XX_WriteRegData(0x09,0x0000);	//Display Contral 3.(0x0000)
		ILI93XX_WriteRegData(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
		ILI93XX_WriteRegData(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
		ILI93XX_WriteRegData(0x0d,0x0000);	//Frame Maker Position.
		ILI93XX_WriteRegData(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
		delay(50); 
		ILI93XX_WriteRegData(0x07,0x0101);	//Display Contral.
		delay(50); 								  
		ILI93XX_WriteRegData(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
		ILI93XX_WriteRegData(0x11,0x0007);								//Power Control 2.(0x0001)
		ILI93XX_WriteRegData(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
		ILI93XX_WriteRegData(0x13,0x0b00);								//Power Control 4.
		ILI93XX_WriteRegData(0x29,0x0000);								//Power Control 7.
	
		ILI93XX_WriteRegData(0x2b,(1<<14)|(1<<4));	    
		ILI93XX_WriteRegData(0x50,0);	//Set X Star
		//水平GRAM终止位置Set X End.
		ILI93XX_WriteRegData(0x51,239);	//Set Y Star
		ILI93XX_WriteRegData(0x52,0);	//Set Y End.t.
		ILI93XX_WriteRegData(0x53,319);	//
	
		ILI93XX_WriteRegData(0x60,0x2700);	//Driver Output Control.
		ILI93XX_WriteRegData(0x61,0x0001);	//Driver Output Control.
		ILI93XX_WriteRegData(0x6a,0x0000);	//Vertical Srcoll Control.
	
		ILI93XX_WriteRegData(0x80,0x0000);	//Display Position? Partial Display 1.
		ILI93XX_WriteRegData(0x81,0x0000);	//RAM Address Start? Partial Display 1.
		ILI93XX_WriteRegData(0x82,0x0000);	//RAM Address End-Partial Display 1.
		ILI93XX_WriteRegData(0x83,0x0000);	//Displsy Position? Partial Display 2.
		ILI93XX_WriteRegData(0x84,0x0000);	//RAM Address Start? Partial Display 2.
		ILI93XX_WriteRegData(0x85,0x0000);	//RAM Address End? Partial Display 2.
	
		ILI93XX_WriteRegData(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
		ILI93XX_WriteRegData(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
		ILI93XX_WriteRegData(0x93,0x0001);	//Panel Interface Contral 3.
		ILI93XX_WriteRegData(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
		ILI93XX_WriteRegData(0x97,(0<<8));	//
		ILI93XX_WriteRegData(0x98,0x0000);	//Frame Cycle Contral.	   
		ILI93XX_WriteRegData(0x07,0x0173);	//(0x0173)
	}else if(lcddev.id==0X9331)//OK |/|/|			 
	{
		ILI93XX_WriteRegData(0x00E7, 0x1014);
		ILI93XX_WriteRegData(0x0001, 0x0100); // set SS and SM bit
		ILI93XX_WriteRegData(0x0002, 0x0200); // set 1 line inversion
        ILI93XX_WriteRegData(0x0003,(1<<12)|(3<<4)|(1<<3));//65K    
		//ILI93XX_WriteRegData(0x0003, 0x1030); // set GRAM write direction and BGR=1.
		ILI93XX_WriteRegData(0x0008, 0x0202); // set the back porch and front porch
		ILI93XX_WriteRegData(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
		ILI93XX_WriteRegData(0x000A, 0x0000); // FMARK function
		ILI93XX_WriteRegData(0x000C, 0x0000); // RGB interface setting
		ILI93XX_WriteRegData(0x000D, 0x0000); // Frame marker Position
		ILI93XX_WriteRegData(0x000F, 0x0000); // RGB interface polarity
		//*************Power On sequence ****************//
		ILI93XX_WriteRegData(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
		ILI93XX_WriteRegData(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
		ILI93XX_WriteRegData(0x0012, 0x0000); // VREG1OUT voltage
		ILI93XX_WriteRegData(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
		delay(200); // Dis-charge capacitor power voltage
		ILI93XX_WriteRegData(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
		ILI93XX_WriteRegData(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
		delay(50); // Delay 50ms
		ILI93XX_WriteRegData(0x0012, 0x000C); // Internal reference voltage= Vci;
		delay(50); // Delay 50ms
		ILI93XX_WriteRegData(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
		ILI93XX_WriteRegData(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
		ILI93XX_WriteRegData(0x002B, 0x000B); // Set Frame Rate
		delay(50); // Delay 50ms
		ILI93XX_WriteRegData(0x0020, 0x0000); // GRAM horizontal Address
		ILI93XX_WriteRegData(0x0021, 0x013f); // GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//
		ILI93XX_WriteRegData(0x0030, 0x0000);
		ILI93XX_WriteRegData(0x0031, 0x0106);
		ILI93XX_WriteRegData(0x0032, 0x0000);
		ILI93XX_WriteRegData(0x0035, 0x0204);
		ILI93XX_WriteRegData(0x0036, 0x160A);
		ILI93XX_WriteRegData(0x0037, 0x0707);
		ILI93XX_WriteRegData(0x0038, 0x0106);
		ILI93XX_WriteRegData(0x0039, 0x0707);
		ILI93XX_WriteRegData(0x003C, 0x0402);
		ILI93XX_WriteRegData(0x003D, 0x0C0F);
		//------------------ Set GRAM area ---------------//
		ILI93XX_WriteRegData(0x0050, 0x0000); // Horizontal GRAM Start Address
		ILI93XX_WriteRegData(0x0051, 0x00EF); // Horizontal GRAM End Address
		ILI93XX_WriteRegData(0x0052, 0x0000); // Vertical GRAM Start Address
		ILI93XX_WriteRegData(0x0053, 0x013F); // Vertical GRAM Start Address
		ILI93XX_WriteRegData(0x0060, 0x2700); // Gate Scan Line
		ILI93XX_WriteRegData(0x0061, 0x0001); // NDL,VLE, REV 
		ILI93XX_WriteRegData(0x006A, 0x0000); // set scrolling line
		//-------------- Partial Display Control ---------//
		ILI93XX_WriteRegData(0x0080, 0x0000);
		ILI93XX_WriteRegData(0x0081, 0x0000);
		ILI93XX_WriteRegData(0x0082, 0x0000);
		ILI93XX_WriteRegData(0x0083, 0x0000);
		ILI93XX_WriteRegData(0x0084, 0x0000);
		ILI93XX_WriteRegData(0x0085, 0x0000);
		//-------------- Panel Control -------------------//
		ILI93XX_WriteRegData(0x0090, 0x0010);
		ILI93XX_WriteRegData(0x0092, 0x0600);
		ILI93XX_WriteRegData(0x0007, 0x0133); // 262K color and display ON
	}else if(lcddev.id==0x5408)
	{
		ILI93XX_WriteRegData(0x01,0x0100);								  
		ILI93XX_WriteRegData(0x02,0x0700);//LCD Driving Waveform Contral 
		ILI93XX_WriteRegData(0x03,0x1030);//Entry Mode设置 	   
		//指针从左至右自上而下的自动增模式
		//Normal Mode(Window Mode disable)
		//RGB格式
		//16位数据2次传输的8总线设置
		ILI93XX_WriteRegData(0x04,0x0000); //Scalling Control register     
		ILI93XX_WriteRegData(0x08,0x0207); //Display Control 2 
		ILI93XX_WriteRegData(0x09,0x0000); //Display Control 3	 
		ILI93XX_WriteRegData(0x0A,0x0000); //Frame Cycle Control	 
		ILI93XX_WriteRegData(0x0C,0x0000); //External Display Interface Control 1 
		ILI93XX_WriteRegData(0x0D,0x0000); //Frame Maker Position		 
		ILI93XX_WriteRegData(0x0F,0x0000); //External Display Interface Control 2 
 		delay(20);
		//TFT 液晶彩色图像显示方法14
		ILI93XX_WriteRegData(0x10,0x16B0); //0x14B0 //Power Control 1
		ILI93XX_WriteRegData(0x11,0x0001); //0x0007 //Power Control 2
		ILI93XX_WriteRegData(0x17,0x0001); //0x0000 //Power Control 3
		ILI93XX_WriteRegData(0x12,0x0138); //0x013B //Power Control 4
		ILI93XX_WriteRegData(0x13,0x0800); //0x0800 //Power Control 5
		ILI93XX_WriteRegData(0x29,0x0009); //NVM read data 2
		ILI93XX_WriteRegData(0x2a,0x0009); //NVM read data 3
		ILI93XX_WriteRegData(0xa4,0x0000);	 
		ILI93XX_WriteRegData(0x50,0x0000); //设置操作窗口的X轴开始列
		ILI93XX_WriteRegData(0x51,0x00EF); //设置操作窗口的X轴结束列
		ILI93XX_WriteRegData(0x52,0x0000); //设置操作窗口的Y轴开始行
		ILI93XX_WriteRegData(0x53,0x013F); //设置操作窗口的Y轴结束行
		ILI93XX_WriteRegData(0x60,0x2700); //Driver Output Control
		//设置屏幕的点数以及扫描的起始行
		ILI93XX_WriteRegData(0x61,0x0001); //Driver Output Control
		ILI93XX_WriteRegData(0x6A,0x0000); //Vertical Scroll Control
		ILI93XX_WriteRegData(0x80,0x0000); //Display Position – Partial Display 1
		ILI93XX_WriteRegData(0x81,0x0000); //RAM Address Start – Partial Display 1
		ILI93XX_WriteRegData(0x82,0x0000); //RAM address End - Partial Display 1
		ILI93XX_WriteRegData(0x83,0x0000); //Display Position – Partial Display 2
		ILI93XX_WriteRegData(0x84,0x0000); //RAM Address Start – Partial Display 2
		ILI93XX_WriteRegData(0x85,0x0000); //RAM address End – Partail Display2
		ILI93XX_WriteRegData(0x90,0x0013); //Frame Cycle Control
		ILI93XX_WriteRegData(0x92,0x0000);  //Panel Interface Control 2
		ILI93XX_WriteRegData(0x93,0x0003); //Panel Interface control 3
		ILI93XX_WriteRegData(0x95,0x0110);  //Frame Cycle Control
		ILI93XX_WriteRegData(0x07,0x0173);		 
		delay(50);
	}	
	else if(lcddev.id==0x1505)//OK
	{
		// second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
        ILI93XX_WriteRegData(0x0007,0x0000);
        delay(50); 
        ILI93XX_WriteRegData(0x0012,0x011C);//0x011A   why need to set several times?
        ILI93XX_WriteRegData(0x00A4,0x0001);//NVM	 
        ILI93XX_WriteRegData(0x0008,0x000F);
        ILI93XX_WriteRegData(0x000A,0x0008);
        ILI93XX_WriteRegData(0x000D,0x0008);	    
  		//伽马校正
        ILI93XX_WriteRegData(0x0030,0x0707);
        ILI93XX_WriteRegData(0x0031,0x0007); //0x0707
        ILI93XX_WriteRegData(0x0032,0x0603); 
        ILI93XX_WriteRegData(0x0033,0x0700); 
        ILI93XX_WriteRegData(0x0034,0x0202); 
        ILI93XX_WriteRegData(0x0035,0x0002); //?0x0606
        ILI93XX_WriteRegData(0x0036,0x1F0F);
        ILI93XX_WriteRegData(0x0037,0x0707); //0x0f0f  0x0105
        ILI93XX_WriteRegData(0x0038,0x0000); 
        ILI93XX_WriteRegData(0x0039,0x0000); 
        ILI93XX_WriteRegData(0x003A,0x0707); 
        ILI93XX_WriteRegData(0x003B,0x0000); //0x0303
        ILI93XX_WriteRegData(0x003C,0x0007); //?0x0707
        ILI93XX_WriteRegData(0x003D,0x0000); //0x1313//0x1f08
        delay(50); 
        ILI93XX_WriteRegData(0x0007,0x0001);
        ILI93XX_WriteRegData(0x0017,0x0001);//开启电源
        delay(50); 
  		//电源配置
        ILI93XX_WriteRegData(0x0010,0x17A0); 
        ILI93XX_WriteRegData(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
        ILI93XX_WriteRegData(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
        ILI93XX_WriteRegData(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
        ILI93XX_WriteRegData(0x002A,0x0000);  
        ILI93XX_WriteRegData(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
        ILI93XX_WriteRegData(0x0012,0x013E);// 0x013C  power supply on
        //Coordinates Control//
        ILI93XX_WriteRegData(0x0050,0x0000);//0x0e00
        ILI93XX_WriteRegData(0x0051,0x00EF); 
        ILI93XX_WriteRegData(0x0052,0x0000); 
        ILI93XX_WriteRegData(0x0053,0x013F); 
    	//Pannel Image Control//
        ILI93XX_WriteRegData(0x0060,0x2700); 
        ILI93XX_WriteRegData(0x0061,0x0001); 
        ILI93XX_WriteRegData(0x006A,0x0000); 
        ILI93XX_WriteRegData(0x0080,0x0000); 
    	//Partial Image Control//
        ILI93XX_WriteRegData(0x0081,0x0000); 
        ILI93XX_WriteRegData(0x0082,0x0000); 
        ILI93XX_WriteRegData(0x0083,0x0000); 
        ILI93XX_WriteRegData(0x0084,0x0000); 
        ILI93XX_WriteRegData(0x0085,0x0000); 
  		//Panel Interface Control//
        ILI93XX_WriteRegData(0x0090,0x0013);//0x0010 frenqucy
        ILI93XX_WriteRegData(0x0092,0x0300); 
        ILI93XX_WriteRegData(0x0093,0x0005); 
        ILI93XX_WriteRegData(0x0095,0x0000); 
        ILI93XX_WriteRegData(0x0097,0x0000); 
        ILI93XX_WriteRegData(0x0098,0x0000); 
  
        ILI93XX_WriteRegData(0x0001,0x0100); 
        ILI93XX_WriteRegData(0x0002,0x0700); 
        ILI93XX_WriteRegData(0x0003,0x1038);//扫描方向 上->下  左->右 
        ILI93XX_WriteRegData(0x0004,0x0000); 
        ILI93XX_WriteRegData(0x000C,0x0000); 
        ILI93XX_WriteRegData(0x000F,0x0000); 
        ILI93XX_WriteRegData(0x0020,0x0000); 
        ILI93XX_WriteRegData(0x0021,0x0000); 
        ILI93XX_WriteRegData(0x0007,0x0021); 
        delay(20);
        ILI93XX_WriteRegData(0x0007,0x0061); 
        delay(20);
        ILI93XX_WriteRegData(0x0007,0x0173); 
        delay(20);
	}else if(lcddev.id==0xB505)
	{
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
		
		ILI93XX_WriteRegData(0x00a4,0x0001);
		delay(20);		  
		ILI93XX_WriteRegData(0x0060,0x2700);
		ILI93XX_WriteRegData(0x0008,0x0202);
		
		ILI93XX_WriteRegData(0x0030,0x0214);
		ILI93XX_WriteRegData(0x0031,0x3715);
		ILI93XX_WriteRegData(0x0032,0x0604);
		ILI93XX_WriteRegData(0x0033,0x0e16);
		ILI93XX_WriteRegData(0x0034,0x2211);
		ILI93XX_WriteRegData(0x0035,0x1500);
		ILI93XX_WriteRegData(0x0036,0x8507);
		ILI93XX_WriteRegData(0x0037,0x1407);
		ILI93XX_WriteRegData(0x0038,0x1403);
		ILI93XX_WriteRegData(0x0039,0x0020);
		
		ILI93XX_WriteRegData(0x0090,0x001a);
		ILI93XX_WriteRegData(0x0010,0x0000);
		ILI93XX_WriteRegData(0x0011,0x0007);
		ILI93XX_WriteRegData(0x0012,0x0000);
		ILI93XX_WriteRegData(0x0013,0x0000);
		delay(20);
		
		ILI93XX_WriteRegData(0x0010,0x0730);
		ILI93XX_WriteRegData(0x0011,0x0137);
		delay(20);
		
		ILI93XX_WriteRegData(0x0012,0x01b8);
		delay(20);
		
		ILI93XX_WriteRegData(0x0013,0x0f00);
		ILI93XX_WriteRegData(0x002a,0x0080);
		ILI93XX_WriteRegData(0x0029,0x0048);
		delay(20);
		
		ILI93XX_WriteRegData(0x0001,0x0100);
		ILI93XX_WriteRegData(0x0002,0x0700);
        ILI93XX_WriteRegData(0x0003,0x1038);//扫描方向 上->下  左->右 
		ILI93XX_WriteRegData(0x0008,0x0202);
		ILI93XX_WriteRegData(0x000a,0x0000);
		ILI93XX_WriteRegData(0x000c,0x0000);
		ILI93XX_WriteRegData(0x000d,0x0000);
		ILI93XX_WriteRegData(0x000e,0x0030);
		ILI93XX_WriteRegData(0x0050,0x0000);
		ILI93XX_WriteRegData(0x0051,0x00ef);
		ILI93XX_WriteRegData(0x0052,0x0000);
		ILI93XX_WriteRegData(0x0053,0x013f);
		ILI93XX_WriteRegData(0x0060,0x2700);
		ILI93XX_WriteRegData(0x0061,0x0001);
		ILI93XX_WriteRegData(0x006a,0x0000);
		//ILI93XX_WriteRegData(0x0080,0x0000);
		//ILI93XX_WriteRegData(0x0081,0x0000);
		ILI93XX_WriteRegData(0x0090,0X0011);
		ILI93XX_WriteRegData(0x0092,0x0600);
		ILI93XX_WriteRegData(0x0093,0x0402);
		ILI93XX_WriteRegData(0x0094,0x0002);
		delay(20);
		
		ILI93XX_WriteRegData(0x0007,0x0001);
		delay(20);
		ILI93XX_WriteRegData(0x0007,0x0061);
		ILI93XX_WriteRegData(0x0007,0x0173);
		
		ILI93XX_WriteRegData(0x0020,0x0000);
		ILI93XX_WriteRegData(0x0021,0x0000);	  
		ILI93XX_WriteRegData(0x00,0x22);  
	}else if(lcddev.id==0xC505)
	{
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
		delay(20);		  
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
		ILI93XX_WriteRegData(0x0000,0x0000);
 		ILI93XX_WriteRegData(0x00a4,0x0001);
		delay(20);		  
		ILI93XX_WriteRegData(0x0060,0x2700);
		ILI93XX_WriteRegData(0x0008,0x0806);
		
		ILI93XX_WriteRegData(0x0030,0x0703);//gamma setting
		ILI93XX_WriteRegData(0x0031,0x0001);
		ILI93XX_WriteRegData(0x0032,0x0004);
		ILI93XX_WriteRegData(0x0033,0x0102);
		ILI93XX_WriteRegData(0x0034,0x0300);
		ILI93XX_WriteRegData(0x0035,0x0103);
		ILI93XX_WriteRegData(0x0036,0x001F);
		ILI93XX_WriteRegData(0x0037,0x0703);
		ILI93XX_WriteRegData(0x0038,0x0001);
		ILI93XX_WriteRegData(0x0039,0x0004);
		
		
		
		ILI93XX_WriteRegData(0x0090, 0x0015);	//80Hz
		ILI93XX_WriteRegData(0x0010, 0X0410);	//BT,AP
		ILI93XX_WriteRegData(0x0011,0x0247);	//DC1,DC0,VC
		ILI93XX_WriteRegData(0x0012, 0x01BC);
		ILI93XX_WriteRegData(0x0013, 0x0e00);
		delay(120);
		ILI93XX_WriteRegData(0x0001, 0x0100);
		ILI93XX_WriteRegData(0x0002, 0x0200);
		ILI93XX_WriteRegData(0x0003, 0x1030);
		
		ILI93XX_WriteRegData(0x000A, 0x0008);
		ILI93XX_WriteRegData(0x000C, 0x0000);
		
		ILI93XX_WriteRegData(0x000E, 0x0020);
		ILI93XX_WriteRegData(0x000F, 0x0000);
		ILI93XX_WriteRegData(0x0020, 0x0000);	//H Start
		ILI93XX_WriteRegData(0x0021, 0x0000);	//V Start
		ILI93XX_WriteRegData(0x002A,0x003D);	//vcom2
		delay(20);
		ILI93XX_WriteRegData(0x0029, 0x002d);
		ILI93XX_WriteRegData(0x0050, 0x0000);
		ILI93XX_WriteRegData(0x0051, 0xD0EF);
		ILI93XX_WriteRegData(0x0052, 0x0000);
		ILI93XX_WriteRegData(0x0053, 0x013F);
		ILI93XX_WriteRegData(0x0061, 0x0000);
		ILI93XX_WriteRegData(0x006A, 0x0000);
		ILI93XX_WriteRegData(0x0092,0x0300); 
 
 		ILI93XX_WriteRegData(0x0093, 0x0005);
		ILI93XX_WriteRegData(0x0007, 0x0100);
	}else if(lcddev.id==0x8989)//OK |/|/|
	{	   
		ILI93XX_WriteRegData(0x0000,0x0001);//打开晶振
    	ILI93XX_WriteRegData(0x0003,0xA8A4);//0xA8A4
    	ILI93XX_WriteRegData(0x000C,0x0000);    
    	ILI93XX_WriteRegData(0x000D,0x080C);   
    	ILI93XX_WriteRegData(0x000E,0x2B00);    
    	ILI93XX_WriteRegData(0x001E,0x00B0);    
    	ILI93XX_WriteRegData(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
    	ILI93XX_WriteRegData(0x0002,0x0600);
    	ILI93XX_WriteRegData(0x0010,0x0000);  
    	ILI93XX_WriteRegData(0x0011,0x6078); //定义数据格式  16位色 		横屏 0x6058
    	ILI93XX_WriteRegData(0x0005,0x0000);  
    	ILI93XX_WriteRegData(0x0006,0x0000);  
    	ILI93XX_WriteRegData(0x0016,0xEF1C);  
    	ILI93XX_WriteRegData(0x0017,0x0003);  
    	ILI93XX_WriteRegData(0x0007,0x0233); //0x0233       
    	ILI93XX_WriteRegData(0x000B,0x0000);  
    	ILI93XX_WriteRegData(0x000F,0x0000); //扫描开始地址
    	ILI93XX_WriteRegData(0x0041,0x0000);  
    	ILI93XX_WriteRegData(0x0042,0x0000);  
    	ILI93XX_WriteRegData(0x0048,0x0000);  
    	ILI93XX_WriteRegData(0x0049,0x013F);  
    	ILI93XX_WriteRegData(0x004A,0x0000);  
    	ILI93XX_WriteRegData(0x004B,0x0000);  
    	ILI93XX_WriteRegData(0x0044,0xEF00);  
    	ILI93XX_WriteRegData(0x0045,0x0000);  
    	ILI93XX_WriteRegData(0x0046,0x013F);  
    	ILI93XX_WriteRegData(0x0030,0x0707);  
    	ILI93XX_WriteRegData(0x0031,0x0204);  
    	ILI93XX_WriteRegData(0x0032,0x0204);  
    	ILI93XX_WriteRegData(0x0033,0x0502);  
    	ILI93XX_WriteRegData(0x0034,0x0507);  
    	ILI93XX_WriteRegData(0x0035,0x0204);  
    	ILI93XX_WriteRegData(0x0036,0x0204);  
    	ILI93XX_WriteRegData(0x0037,0x0502);  
    	ILI93XX_WriteRegData(0x003A,0x0302);  
    	ILI93XX_WriteRegData(0x003B,0x0302);  
    	ILI93XX_WriteRegData(0x0023,0x0000);  
    	ILI93XX_WriteRegData(0x0024,0x0000);  
    	ILI93XX_WriteRegData(0x0025,0x8000);  
    	ILI93XX_WriteRegData(0x004f,0);        //行首址0
    	ILI93XX_WriteRegData(0x004e,0);        //列首址0
	}else if(lcddev.id==0x4531)//OK |/|/|
	{
		ILI93XX_WriteRegData(0X00,0X0001);   
		delay(10);   
		ILI93XX_WriteRegData(0X10,0X1628);   
		ILI93XX_WriteRegData(0X12,0X000e);//0x0006    
		ILI93XX_WriteRegData(0X13,0X0A39);   
		delay(10);   
		ILI93XX_WriteRegData(0X11,0X0040);   
		ILI93XX_WriteRegData(0X15,0X0050);   
		delay(10);   
		ILI93XX_WriteRegData(0X12,0X001e);//16    
		delay(10);   
		ILI93XX_WriteRegData(0X10,0X1620);   
		ILI93XX_WriteRegData(0X13,0X2A39);   
		delay(10);   
		ILI93XX_WriteRegData(0X01,0X0100);   
		ILI93XX_WriteRegData(0X02,0X0300);   
		ILI93XX_WriteRegData(0X03,0X1038);//改变方向的   
		ILI93XX_WriteRegData(0X08,0X0202);   
		ILI93XX_WriteRegData(0X0A,0X0008);   
		ILI93XX_WriteRegData(0X30,0X0000);   
		ILI93XX_WriteRegData(0X31,0X0402);   
		ILI93XX_WriteRegData(0X32,0X0106);   
		ILI93XX_WriteRegData(0X33,0X0503);   
		ILI93XX_WriteRegData(0X34,0X0104);   
		ILI93XX_WriteRegData(0X35,0X0301);   
		ILI93XX_WriteRegData(0X36,0X0707);   
		ILI93XX_WriteRegData(0X37,0X0305);   
		ILI93XX_WriteRegData(0X38,0X0208);   
		ILI93XX_WriteRegData(0X39,0X0F0B);   
		ILI93XX_WriteRegData(0X41,0X0002);   
		ILI93XX_WriteRegData(0X60,0X2700);   
		ILI93XX_WriteRegData(0X61,0X0001);   
		ILI93XX_WriteRegData(0X90,0X0210);   
		ILI93XX_WriteRegData(0X92,0X010A);   
		ILI93XX_WriteRegData(0X93,0X0004);   
		ILI93XX_WriteRegData(0XA0,0X0100);   
		ILI93XX_WriteRegData(0X07,0X0001);   
		ILI93XX_WriteRegData(0X07,0X0021);   
		ILI93XX_WriteRegData(0X07,0X0023);   
		ILI93XX_WriteRegData(0X07,0X0033);   
		ILI93XX_WriteRegData(0X07,0X0133);   
		ILI93XX_WriteRegData(0XA0,0X0000); 
	}else if(lcddev.id==0x4535)
	{			      
		ILI93XX_WriteRegData(0X15,0X0030);   
		ILI93XX_WriteRegData(0X9A,0X0010);   
 		ILI93XX_WriteRegData(0X11,0X0020);   
 		ILI93XX_WriteRegData(0X10,0X3428);   
		ILI93XX_WriteRegData(0X12,0X0002);//16    
 		ILI93XX_WriteRegData(0X13,0X1038);   
		delay(40);   
		ILI93XX_WriteRegData(0X12,0X0012);//16    
		delay(40);   
  		ILI93XX_WriteRegData(0X10,0X3420);   
 		ILI93XX_WriteRegData(0X13,0X3038);   
		delay(70);   
		ILI93XX_WriteRegData(0X30,0X0000);   
		ILI93XX_WriteRegData(0X31,0X0402);   
		ILI93XX_WriteRegData(0X32,0X0307);   
		ILI93XX_WriteRegData(0X33,0X0304);   
		ILI93XX_WriteRegData(0X34,0X0004);   
		ILI93XX_WriteRegData(0X35,0X0401);   
		ILI93XX_WriteRegData(0X36,0X0707);   
		ILI93XX_WriteRegData(0X37,0X0305);   
		ILI93XX_WriteRegData(0X38,0X0610);   
		ILI93XX_WriteRegData(0X39,0X0610); 
		  
		ILI93XX_WriteRegData(0X01,0X0100);   
		ILI93XX_WriteRegData(0X02,0X0300);   
		ILI93XX_WriteRegData(0X03,0X1030);//改变方向的   
		ILI93XX_WriteRegData(0X08,0X0808);   
		ILI93XX_WriteRegData(0X0A,0X0008);   
 		ILI93XX_WriteRegData(0X60,0X2700);   
		ILI93XX_WriteRegData(0X61,0X0001);   
		ILI93XX_WriteRegData(0X90,0X013E);   
		ILI93XX_WriteRegData(0X92,0X0100);   
		ILI93XX_WriteRegData(0X93,0X0100);   
 		ILI93XX_WriteRegData(0XA0,0X3000);   
 		ILI93XX_WriteRegData(0XA3,0X0010);   
		ILI93XX_WriteRegData(0X07,0X0001);   
		ILI93XX_WriteRegData(0X07,0X0021);   
		ILI93XX_WriteRegData(0X07,0X0023);   
		ILI93XX_WriteRegData(0X07,0X0033);   
		ILI93XX_WriteRegData(0X07,0X0133);   
	}		 
	ILI93XX_Display_Dir(1);		 	//默认为竖屏
//	ILI93XX_LED=0;					//点亮背光
//	ILI93XX_Clear(lcddev.TextColor);
}  
//清屏函数
//color:要清屏的填充色
void ILI93XX_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	if((lcddev.id==0X6804)&&(lcddev.dir==1))//6804横屏的时候特殊处理  
	{						    
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		ILI93XX_SetCursor(0x00,0x0000);		//设置光标位置  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else ILI93XX_SetCursor(0x00,0x0000);	//设置光标位置 

	ILI93XX_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		ILI93XX_WR_DATA(color);	   
	}
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI93XX_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
	uint16_t i,j;
	uint16_t xlen=0;
	uint16_t temp;
	if((lcddev.id==0X6804)&&(lcddev.dir==1))	//6804横屏的时候特殊处理  
	{
		temp=sx;
		sx=sy;
		sy=lcddev.width-ex-1;	  
		ex=ey;
		ey=lcddev.width-temp-1;
 		lcddev.dir=0;	 
 		lcddev.setxcmd=0X2A;
		lcddev.setycmd=0X2B;  	 			
		ILI93XX_Fill(sx,sy,ex,ey,color);  
 		lcddev.dir=1;	 
  		lcddev.setxcmd=0X2B;
		lcddev.setycmd=0X2A;  	 
 	}else
	{
		xlen=ex-sx+1;	 
		for(i=sy;i<=ey;i++)
		{
		 	ILI93XX_SetCursor(sx,i);      				//设置光标位置 
			ILI93XX_WriteRAM_Prepare();     			//开始写入GRAM	  
			for(j=0;j<xlen;j++)ILI93XX_WR_DATA(color);	//设置光标位置 	    
		}
	}	 
}  

//在指定区域内填充指定颜色块			 
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void ILI93XX_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
	uint16_t height,width;
	uint16_t i,j;
	width=ex-sx+1; 			//得到填充的宽度
	height=ey-sy+1;			//高度
 	for(i=0;i<height;i++)
	{
 		ILI93XX_SetCursor(sx,sy+i);   	//设置光标位置 
		ILI93XX_WriteRAM_Prepare();     //开始写入GRAM
		for(j=0;j<width;j++)ILI93XX_WR_DATA(color[i*height+j]);//写入数据 
	}	  
}  
