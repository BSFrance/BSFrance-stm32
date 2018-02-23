#ifndef _TOUCH_H
#define _TOUCH_H
#include "ott2001a.h"

#define TP_PRES_DOWN	0X80  //触屏被按下
#define TP_CATH_PRES	0X40  //有按键按下了
typedef struct
{
	uint8_t (*init)(void);  //初始化触摸屏
	uint8_t (*scan)(uint8_t);		 //扫描触摸屏0,屏幕扫描;1,物理坐标
	void (*adjust)(void); //触摸屏校准
#ifdef OTT_MAX_TOUCH
	uint16_t x[OTT_MAX_TOUCH];  //当前坐标
	uint16_t y[OTT_MAX_TOUCH];  //电容屏有最多5组坐标,电阻屏则为x[0],y[0]代表:此次扫描时,触摸屏的坐标用
#else
	uint16_t x[1];  //当前坐标
	uint16_t y[1];  //
#endif

	//x[4],y[4]存储第一次按下时的坐标
	uint8_t sta;  	//笔的状态
						//b7:按下1/松开0; 
						//b6:0,没有按键按下;1,有按键按下. 
						//b5:保留
						//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////			
	float xfac;
	float yfac;
	int16_t xoff;
	int16_t yoff;
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
	uint8_t CMD_RDX;
	uint8_t CMD_RDY;
	uint8_t miso;
    uint8_t mosi;
	uint8_t sck;
	uint8_t cs;
	uint8_t irq;
}_m_tp_dev;

extern _m_tp_dev tp_dev; //触屏控制器在touch.c里面定义

//电阻屏芯片连接引脚	   
//#define PEN  		PCin(5)  	//T_PEN
//#define DOUT 		PBin(14)   	//T_MISO
//#define TDIN 		PBout(15)  	//T_MOSI
//#define TCLK 		PBout(13)  	//T_SCK
//#define TCS  		PBout(12)  	//T_CS  

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

//电阻屏/电容屏 共用函数
uint8_t TP_Scan(uint8_t tp);								//扫描
uint8_t TP_Init(void);								//初始化

//电阻屏函数
void TP_Write_Byte(uint8_t num);						//向控制芯片写入一个数据
uint16_t TP_Read_AD(uint8_t CMD);							//读取AD转换值
uint16_t TP_Read_XOY(uint8_t xy);							//带滤波的坐标读取(X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);					//双方向读取(X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);					//带加强滤波的双方向坐标读取
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);//画一个坐标校准点
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	//画一个大点
void TP_Save_Adjdata(void);						//保存校准参数
uint8_t TP_Get_Adjdata(void);						//读取校准参数
void TP_Adjust(void);							//触摸屏校准
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//显示校准信息

#ifdef __cplusplus
}  // extern "C" {

class TOUCH
{
 public:
	TOUCH(){
		pdev = &tp_dev;
	};
	TOUCH(uint8_t mosi, uint8_t miso, uint8_t sck,uint8_t cs, uint8_t irq){
		pdev->mosi = mosi;
		pdev->miso = miso;
		pdev->sck = sck;
		pdev->cs = cs;
		pdev->irq = irq;
	};		
	uint8_t Init(uint8_t typ = 0){
		pdev->touchtype |= typ&0x01;
		return TP_Init();};
    void     writeByte(uint8_t num){TP_Write_Byte(num);};						//向控制芯片写入一个数据
    uint16_t readAD(uint8_t CMD){return TP_Read_AD( CMD);};							//读取AD转换值
	uint16_t readXOY(uint8_t xy){return TP_Read_XOY(xy);};							//带滤波的坐标读取(X/Y)
	uint8_t  readXY(uint16_t *x,uint16_t *y){return TP_Read_XY(x,y);};					//双方向读取(X+Y)
	uint8_t  readXY2(uint16_t *x,uint16_t *y){return TP_Read_XY2(x,y);};					//带加强滤波的双方向坐标读取
//	void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);//画一个坐标校准点
//	void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	//画一个大点
//	void TP_Save_Adjdata(void);						//保存校准参数
//	uint8_t TP_Get_Adjdata(void);					//读取校准参数
	void     adjust(void){TP_Adjust();};			//触摸屏校准
//	void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//显示校准信息
	//电阻屏/电容屏 共用函数
	uint8_t  scan(uint8_t tp=0){return TP_Scan(tp);};				//扫描
	uint8_t  status(void){return (pdev->sta >>6);};				//扫描
	_m_tp_dev* Info(void){
		return pdev;
	}
	_m_tp_dev* pdev; 
};
#endif /* __cplusplus */ 
#endif

