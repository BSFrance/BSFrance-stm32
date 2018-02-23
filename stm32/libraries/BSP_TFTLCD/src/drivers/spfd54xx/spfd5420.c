#include <Arduino.h>
#include "bspio/bspio.h"
#include "spfd5420.h"
#include "font/font.h"
#include "tftlcd/lcd.h"


#define  SPFD5420_WR_DATA(x) 	LCD_IO_WriteData(x)
#define  SPFD5420_WR_DATA(x)    LCD_IO_WriteData(x)
#define  SPFD5420_RD_DATA()     LCD_IO_ReadData()
#define  SPFD5420_WriteRegData(x,y)	LCD_IO_WriteRegData(x,y)


LCD_DrvTypeDef SPFD5420_DRV = {
  .Init       = SPFD5420_Init,
  .DisplayOn  = SPFD5420_DisplayOn,
  .DisplayOff = SPFD5420_DisplayOff,
  .SetCursor  = SPFD5420_SetCursor,
  .ReadPixel  = SPFD5420_ReadPixel,
  .WritePixel = SPFD5420_WritePixel,
  .Fill	= SPFD5420_Fill,
  
//option  
  .DrawHLine = SPFD5420_DrawHLine,
  .DrawVLine = SPFD5420_DrawVLine,
};


extern LCD_DevTypeDef lcddev;

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_Delaly10ms
*	功能说明: 延迟函数，不准
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
static void SPFD5420_Delaly10ms(void)
{
	for (volatile uint16_t i = 0; i < 50000; i++);
}

uint16_t SPFD5420_ReadID(void){
  return LCD_IO_ReadID();
}


/*
*********************************************************************************************************
*	函 数 名: SPFD5420_SetDisplayWindow
*	功能说明: 设置显示窗口，进入窗口显示模式。TFT驱动芯片支持窗口显示模式，这是和一般的12864点阵LCD的最大区别。
*	形    参：
*		_usX : 水平坐标
*		_usY : 垂直坐标
*		_usHeight: 窗口高度
*		_usWidth : 窗口宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_SetDisplayWindow(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey)
{
	/*
		240x400屏幕物理坐标(px,py)如下:
		    R003 = 0x1018                  R003 = 0x1008
		  -------------------          -------------------
		 |(0,0)              |        |(0,0)              |
		 |                   |        |					  |
		 |  ^           ^    |        |   ^           ^   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |  ------>  |    |        |   | <------   |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |                   |        |					  |
		 |                   |        |                   |
		 |      (x=239,y=399)|        |      (x=239,y=399)|
		 |-------------------|        |-------------------|
		 |                   |        |                   |
		  -------------------          -------------------

		按照安富莱开发板LCD的方向，我们期望的虚拟坐标和扫描方向如下：(和上图第1个吻合)
		 --------------------------------
		|  |(0,0)                        |
		|  |     --------->              |
		|  |         |                   |
		|  |         |                   |
		|  |         |                   |
		|  |         V                   |
		|  |     --------->              |
		|  |                    (399,239)|
		 --------------------------------
	虚拟坐标和物理坐标转换关系：
		x = 399 - py;
		y = px;

		py = 399 - x;
		px = y;
	*/

	/* 设置显示窗口 WINDOWS */
	SPFD5420_WriteRegData(0x0210, sx);						/* 水平起始地址 */
	SPFD5420_WriteRegData(0x0211, ex);		/* 水平结束坐标 */
	SPFD5420_WriteRegData(0x0212, sy );		/* 垂直起始地址 */
	SPFD5420_WriteRegData(0x0213, ey);						/* 垂直结束地址 */

	SPFD5420_SetCursor(sx, sy);
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void SPFD5420_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
//	uint16_t height,width;
//	SPFD5420_SetDisplayWindow(sx,sy,ex,ey);	
 	for(volatile int j=sy;j<=ey;j++)
		for(volatile int i=sx;i<ex;i++) 
			SPFD5420_WritePixel(i, j, color);
//			SPFD5420_WR_DATA(color);//写入数据 
//	SPFD5420_SetDisplayWindow(0,0,lcddev.width-1,lcddev.height-1);		
}  

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_SetCursor
*	功能说明: 设置光标位置
*	形    参：_usX : X坐标; _usY: Y坐标
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_SetCursor(uint16_t _usX, uint16_t _usY)
{
	SPFD5420_WriteRegData(0x0200, _usX);  		/* px */
	SPFD5420_WriteRegData(0x0201, _usY);	/* py */
}

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_BGR2RGB
*	功能说明: RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
*	形    参：RGB颜色代码
*	返 回 值: 转化后的颜色代码
*********************************************************************************************************
*/
uint16_t SPFD5420_BGR2RGB(uint16_t _usRGB)
{
	uint16_t  r, g, b, rgb;

	b = (_usRGB >> 0)  & 0x1F;
	g = (_usRGB >> 5)  & 0x3F;
	r = (_usRGB >> 11) & 0x1F;

	rgb = (b<<11) + (g<<5) + (r<<0);

	return( rgb );
}

/*
*********************************************************************************************************
*	函 数 名: Init_5420_4001
*	功能说明: 初始化5420和4001屏
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Init_5420_4001(void)
{
	/* 初始化LCD，写LCD寄存器进行配置 */
	SPFD5420_WriteRegData(0x0000, 0x0000);
	SPFD5420_WriteRegData(0x0001, 0x0100);
	SPFD5420_WriteRegData(0x0002, 0x0100);

	/*
		R003H 寄存器很关键， Entry Mode ，决定了扫描方向
		参见：SPFD5420A.pdf 第15页

		240x400屏幕物理坐标(px,py)如下:
		    R003 = 0x1018                  R003 = 0x1008
		  -------------------          -------------------
		 |(0,0)              |        |(0,0)              |
		 |                   |        |					  |
		 |  ^           ^    |        |   ^           ^   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |  ------>  |    |        |   | <------   |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |                   |        |					  |
		 |                   |        |                   |
		 |      (x=239,y=399)|        |      (x=239,y=399)|
		 |-------------------|        |-------------------|
		 |                   |        |                   |
		  -------------------          -------------------

		按照安富莱开发板LCD的方向，我们期望的虚拟坐标和扫描方向如下：(和上图第1个吻合)
		 --------------------------------
		|  |(0,0)                        |
		|  |     --------->              |
		|  |         |                   |
		|  |         |                   |
		|  |         |                   |
		|  |         V                   |
		|  |     --------->              |
		|  |                    (399,239)|
		 --------------------------------

		虚拟坐标(x,y) 和物理坐标的转换关系
		x = 399 - py;
		y = px;

		py = 399 - x;
		px = y;

	*/
	SPFD5420_WriteRegData(0x0003, 0x1030); /* 0x1018 1030 */

	SPFD5420_WriteRegData(0x0008, 0x0808);
	SPFD5420_WriteRegData(0x0009, 0x0001);
	SPFD5420_WriteRegData(0x000B, 0x0010);
	SPFD5420_WriteRegData(0x000C, 0x0000);
	SPFD5420_WriteRegData(0x000F, 0x0000);
	SPFD5420_WriteRegData(0x0007, 0x0001);
	SPFD5420_WriteRegData(0x0010, 0x0013);
	SPFD5420_WriteRegData(0x0011, 0x0501);
	SPFD5420_WriteRegData(0x0012, 0x0300);
	SPFD5420_WriteRegData(0x0020, 0x021E);
	SPFD5420_WriteRegData(0x0021, 0x0202);
	SPFD5420_WriteRegData(0x0090, 0x8000);
	SPFD5420_WriteRegData(0x0100, 0x17B0);
	SPFD5420_WriteRegData(0x0101, 0x0147);
	SPFD5420_WriteRegData(0x0102, 0x0135);
	SPFD5420_WriteRegData(0x0103, 0x0700);
	SPFD5420_WriteRegData(0x0107, 0x0000);
	SPFD5420_WriteRegData(0x0110, 0x0001);
	SPFD5420_WriteRegData(0x0210, 0x0000);
	SPFD5420_WriteRegData(0x0211, 0x00EF);
	SPFD5420_WriteRegData(0x0212, 0x0000);
	SPFD5420_WriteRegData(0x0213, 0x018F);
	SPFD5420_WriteRegData(0x0280, 0x0000);
	SPFD5420_WriteRegData(0x0281, 0x000f);
	SPFD5420_WriteRegData(0x0282, 0x0000);
	SPFD5420_WriteRegData(0x0300, 0x0101);
	SPFD5420_WriteRegData(0x0301, 0x0B2C);
	SPFD5420_WriteRegData(0x0302, 0x1030);
	SPFD5420_WriteRegData(0x0303, 0x3010);
	SPFD5420_WriteRegData(0x0304, 0x2C0B);
	SPFD5420_WriteRegData(0x0305, 0x0101);
	SPFD5420_WriteRegData(0x0306, 0x0807);
	SPFD5420_WriteRegData(0x0307, 0x0708);
	SPFD5420_WriteRegData(0x0308, 0x0107);
	SPFD5420_WriteRegData(0x0309, 0x0105);
	SPFD5420_WriteRegData(0x030A, 0x0F04);
	SPFD5420_WriteRegData(0x030B, 0x0F00);
	SPFD5420_WriteRegData(0x030C, 0x000F);
	SPFD5420_WriteRegData(0x030D, 0x040F);
	SPFD5420_WriteRegData(0x030E, 0x0300);
	SPFD5420_WriteRegData(0x030F, 0x0701);
	SPFD5420_WriteRegData(0x0400, 0x3500);
	SPFD5420_WriteRegData(0x0401, 0x0001);
	SPFD5420_WriteRegData(0x0404, 0x0000);
	SPFD5420_WriteRegData(0x0500, 0x0000);
	SPFD5420_WriteRegData(0x0501, 0x0000);
	SPFD5420_WriteRegData(0x0502, 0x0000);
	SPFD5420_WriteRegData(0x0503, 0x0000);
	SPFD5420_WriteRegData(0x0504, 0x0000);
	SPFD5420_WriteRegData(0x0505, 0x0000);
	SPFD5420_WriteRegData(0x0600, 0x0000);
	SPFD5420_WriteRegData(0x0606, 0x0000);
	SPFD5420_WriteRegData(0x06F0, 0x0000);
	SPFD5420_WriteRegData(0x07F0, 0x5420);
	SPFD5420_WriteRegData(0x07DE, 0x0000);
	SPFD5420_WriteRegData(0x07F2, 0x00DF);
	SPFD5420_WriteRegData(0x07F3, 0x0810);
	SPFD5420_WriteRegData(0x07F4, 0x0077);
	SPFD5420_WriteRegData(0x07F5, 0x0021);
	SPFD5420_WriteRegData(0x07F0, 0x0000);
	SPFD5420_WriteRegData(0x0007, 0x0173);

	/* 设置显示窗口 WINDOWS */
	SPFD5420_WriteRegData(0x0210, 0);	/* 水平起始地址 */
	SPFD5420_WriteRegData(0x0211, 239);	/* 水平结束坐标 */
	SPFD5420_WriteRegData(0x0212, 0);	/* 垂直起始地址 */
	SPFD5420_WriteRegData(0x0213, 399);	/* 垂直结束地址 */

	SPFD5420_WriteRegData(0x0200, 0);
	SPFD5420_WriteRegData(0x0201, 0);
	
}
/*
*********************************************************************************************************
*	函 数 名: SPFD5420_InitHard
*	功能说明: 初始化LCD
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_Init(void)
{
	LCD_IO_Init();
    SPFD5420_Delaly10ms();
	Init_5420_4001();
    lcddev.id = IC_5420;
	lcddev.wramcmd=0x202;
	lcddev.setxcmd=0x0200;
	lcddev.setycmd=0x0201;  	 	
	lcddev.width=240;
	lcddev.height=400;
}

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_DispOn
*	功能说明: 打开显示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_DisplayOn(void)
{
		SPFD5420_WriteRegData(7, 0x0173); /* 设置262K颜色并且打开显示 */
}

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_DispOff
*	功能说明: 关闭显示
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_DisplayOff(void)
{
	SPFD5420_WriteRegData(7, 0x0000);
}


/*
*********************************************************************************************************
*	函 数 名: SPFD5420_PutPixel
*	功能说明: 画1个像素
*	形    参：
*			_usX,_usY : 像素坐标
*			_usColor  ：像素颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_WritePixel(uint16_t x, uint16_t y, uint16_t color)
{
	SPFD5420_SetCursor(x, y);	/* 设置光标位置 */
	/* 写显存 */
	SPFD5420_WriteRegData(0x202,color);
}

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_GetPixel
*	功能说明: 读取1个像素
*	形    参：
*			_usX,_usY : 像素坐标
*			_usColor  ：像素颜色
*	返 回 值: RGB颜色值
*********************************************************************************************************
*/
uint16_t SPFD5420_ReadPixel(uint16_t _usX, uint16_t _usY)
{
	uint16_t usRGB;

	SPFD5420_SetCursor(_usX, _usY);	/* 设置光标位置 */
	/* 准备写显存 */
	LCD_IO_WriteReg(0x202);
	usRGB = SPFD5420_RD_DATA(); //dumy
    usRGB = SPFD5420_RD_DATA();

	return usRGB;
}

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_DrawHLine
*	功能说明: 绘制一条水平线 （主要用于UCGUI的接口函数）
*	形    参：_usX1    ：起始点X坐标
*			  _usY1    ：水平线的Y坐标
*			  _usX2    ：结束点X坐标
*			  _usColor : 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_DrawHLine(uint16_t _usColor,uint16_t _usX1 , uint16_t _usY1 , uint16_t _usL )
{

	/* 展开 SPFD5420_SetCursor(_usX1, _usY1) 函数，提高执行效率 */
	/*
		px，py 是物理坐标， x，y是虚拟坐标
		转换公式:
		py = 399 - x;
		px = y;
	*/
	SPFD5420_WriteRegData(0x0200, _usX1);  		/* px */
	SPFD5420_WriteRegData(0x0201, _usY1);	/* py */

	/* 写显存 */
	LCD_IO_WriteReg(0x202);
	for (volatile uint16_t i=0; i < _usL + 1; i++)
	{
		LCD_IO_WriteData(_usColor);
	}
}

/*
*********************************************************************************************************
*	函 数 名: SPFD5420_DrawVLine
*	功能说明: 绘制一条垂直线条 （主要用于UCGUI的接口函数）
*	形    参： _usX1    : 垂直线的X坐标
*			  _usY1    : 起始点Y坐标
*			  _usY2    : 结束点Y坐标
*			  _usColor : 颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void SPFD5420_DrawVLine(uint16_t _usColor,uint16_t _usX1 , uint16_t _usY1 , uint16_t _usL )
{

	/* 展开 SPFD5420_SetCursor(_usX1, _usY1) 函数，提高执行效率 */
	/*
		px，py 是物理坐标， x，y是虚拟坐标
		转换公式:
		py = 399 - x;
		px = y;
	*/
	SPFD5420_WriteRegData(0x0200, _usX1);  		/* px */
	SPFD5420_WriteRegData(0x0201, _usY1);	/* py */

	/* 写显存 */
	for (volatile uint16_t i = 1; i <= _usL + 1; i++)
	{
		SPFD5420_WriteRegData(0x202,_usColor);

		/* 展开 RA8875_SetCursor(_usX1, _usY1 + i) 函数 */
		SPFD5420_WriteRegData(0x0200, _usX1 + i);  		/* px */
		SPFD5420_WriteRegData(0x0201, _usY1);	/* py */
	}
}

uint16_t SPFD5420_GetLcdPixelWidth(void)
{
	return((uint16_t)lcddev.width);
};

uint16_t SPFD5420_GetLcdPixelHeight(void)
{
	return((uint16_t)lcddev.height);
};


