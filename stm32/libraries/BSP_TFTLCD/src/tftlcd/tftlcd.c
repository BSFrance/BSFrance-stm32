#include <Arduino.h>
#include "bspio/bspio.h"
#include "font/font.h"
#include "lcd.h" 
#include "tftlcd.h"


extern LCD_DrvTypeDef ILI9320_DRV;
extern LCD_DrvTypeDef ILI9325_DRV;
extern LCD_DrvTypeDef ILI9328_DRV;
extern LCD_DrvTypeDef ILI9331_DRV;
extern LCD_DrvTypeDef ILI9341_DRV;
extern LCD_DrvTypeDef ILI9348_DRV;
extern LCD_DrvTypeDef ILI9481_DRV;
extern LCD_DrvTypeDef ILI9486_DRV;
extern LCD_DrvTypeDef SPFD5408_DRV;
extern LCD_DrvTypeDef SPFD5420_DRV;
extern LCD_DrvTypeDef ILI93XX_DRV;

#if BSP_TFT_LCD == ILI9320
#  define TFTLCD_DRV ILI9320_DRV
#elif BSP_TFT_LCD == ILI9325
#  define TFTLCD_DRV ILI9325_DRV
#elif BSP_TFT_LCD == ILI9328
#  define TFTLCD_DRV ILI9328_DRV
#elif BSP_TFT_LCD == ILI9331
#  define TFTLCD_DRV ILI9331_DRV
#elif BSP_TFT_LCD == ILI9341
#  define TFTLCD_DRV ILI9341_DRV
#elif BSP_TFT_LCD == ILI9348
#  define TFTLCD_DRV ILI9348_DRV
#elif BSP_TFT_LCD == ILI9481
#  define TFTLCD_DRV ILI9481_DRV
#elif BSP_TFT_LCD == ILI9486
#  define TFTLCD_DRV ILI9486_DRV
#elif BSP_TFT_LCD == SPFD5408
#  define TFTLCD_DRV SPFD5408_DRV
#elif BSP_TFT_LCD == SPFD5420
#  define TFTLCD_DRV SPFD5420_DRV
#elif (BSP_TFT_LCD == RM68021)|| (BSP_TFT_LCD == RM68042)|| (BSP_TFT_LCD == B505)\
      ||(BSP_TFT_LCD == C505)||(BSP_TFT_LCD == LGDP4531)||(BSP_TFT_LCD == LGDP4535)\
	  ||(BSP_TFT_LCD == NT35310)||(BSP_TFT_LCD == NT35510) ||(BSP_TFT_LCD == ILI93XX)
#  define TFTLCD_DRV ILI93XX_DRV
#else
# error  "!unsupport lcd drver!"	
#endif

#define  LCD_WR_REG(x) 	LCD_IO_WriteReg(x)
#define  LCD_WR_DATA(x) LCD_IO_WriteData(x)
#define  LCD_RD_DATA    LCD_IO_ReadData
#define  LCD_WriteRegData(x,y) LCD_IO_WriteRegData(x,y)

LCD_DrvTypeDef* pLcdDrv = &TFTLCD_DRV;
//管理LCD重要参数   lcd.h
//默认为竖屏  
LCD_DevTypeDef lcddev ={
	.TextColor = 0x0000,	//画笔颜色
	.BackColor = 0xffff,  //背景色 
};


//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
uint16_t LCD_ReadPixel(uint16_t x,uint16_t y)
{
	return pLcdDrv->ReadPixel(x,y);
}

//LCD开启显示
void LCD_DisplayOn(void)
{					   
     pLcdDrv->DisplayOn();
}

//LCD关闭显示
void LCD_DisplayOff(void)
{	   
	pLcdDrv->DisplayOff();
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	 
	pLcdDrv->SetCursor(Xpos, Ypos);
} 		 

//画点
//x,y:坐标
//lcddev.TextColor:此点的颜色
void LCD_WritePixel(uint16_t x,uint16_t y,uint16_t color)
{
	pLcdDrv->WritePixel(x,y,color);
}


//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//68042,横屏时不支持窗口设置!! 
void LCD_SetDisplayWindow(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{   
	pLcdDrv->SetDisplayWindow(sx,sy,width,height);
} 

//初始化lcd
//该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
//在其他型号的驱动芯片上没有测试! 
void LCDx_Init(void)
{ 										  
	pLcdDrv->Init();
}
void LCD_WriteRAM_Prepare(void){
   
};
//清屏函数
//color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
   LCD_Fill(0,0,lcddev.width-1,lcddev.height-1, color);
}  
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{
  pLcdDrv->Fill(sx,sy,ex,ey,color);
}
//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	if(x1 ==x2){
		pLcdDrv->DrawVLine(lcddev.TextColor, x1, y1, y2-y1);
		return;
	}else if(y1 ==y2){
		pLcdDrv->DrawHLine(lcddev.TextColor, x1, y1, x2-x1);
		return;
	}
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(uint16_t t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_WritePixel(uRow,uCol,lcddev.TextColor);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    
//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}
//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_WritePixel(x0+a,y0-b,lcddev.TextColor);             //5
 		LCD_WritePixel(x0+b,y0-a,lcddev.TextColor);             //0           
		LCD_WritePixel(x0+b,y0+a,lcddev.TextColor);             //4               
		LCD_WritePixel(x0+a,y0+b,lcddev.TextColor);             //6 
		LCD_WritePixel(x0-a,y0+b,lcddev.TextColor);             //1       
 		LCD_WritePixel(x0-b,y0+a,lcddev.TextColor);             
		LCD_WritePixel(x0-a,y0-b,lcddev.TextColor);             //2             
  		LCD_WritePixel(x0-b,y0-a,lcddev.TextColor);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 									  
//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{  							  
    uint8_t temp,t1,t;
	uint16_t y0=y;
	uint16_t colortemp=lcddev.TextColor;      			     
	//设置窗口		   
	num=num-' ';//得到偏移后的值
	if(!mode) //非叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)lcddev.TextColor=colortemp;
				else lcddev.TextColor=lcddev.BackColor;
				LCD_WritePixel(x,y,lcddev.TextColor);	
				temp<<=1;
				y++;
				if(x>=lcddev.width){lcddev.TextColor=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){lcddev.TextColor=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }    
	}else//叠加方式
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //调用1206字体
			else temp=asc2_1608[num][t];		 //调用1608字体 	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80)LCD_WritePixel(x,y,lcddev.TextColor); 
				temp<<=1;
				y++;
				if(x>=lcddev.height){lcddev.TextColor=colortemp;return;}//超区域了
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=lcddev.width){lcddev.TextColor=colortemp;return;}//超区域了
					break;
				}
			}  	 
	    }     
	}
	lcddev.TextColor=colortemp;	    	   	 	  
}
   
//m^n函数
//返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 
//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);	 
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{  
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 
//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}






























