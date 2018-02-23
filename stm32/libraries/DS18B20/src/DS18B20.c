//IO方向设置  PB3
#include <Arduino.h>
#include "util/delay.h"
#if __has_include("bsp.h")
#  include "bsp.h"
#endif

#ifndef DS18B20_DQ_PIN
#define  DS18B20_DQ_PIN 0xFF
#endif

#define DS18B20_IO_IN   pinMode(ds18b02DqPin,INPUT_PULLUP)
#define DS18B20_IO_OUT  pinMode(ds18b02DqPin,OUTPUT)

////IO操作函数	PB3										   
#define	DS18B20_DQ_OUT(x) digitalWrite(ds18b02DqPin,x) //数据端口	PB3
#define	DS18B20_DQ_IN()   digitalRead(ds18b02DqPin)  //数据端口	PB3 

#include "DS18B20.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32开发板
//DS18B20驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/4/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
  

//复位DS18B20
static uint8_t ds18b02DqPin = DS18B20_DQ_PIN;
void DS18B20_SetPin(uint8_t pin)	   
{                 
	ds18b02DqPin = pin;
}

void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT; //SET PA0 OUTPUT
    DS18B20_DQ_OUT(LOW); //拉低DQ
    _delay_us(750);    //拉低750us
    DS18B20_DQ_OUT(HIGH); //DQ=1 
	_delay_us(15);     //15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN;//SET PA0 INPUT	 
    while (DS18B20_DQ_IN()&&retry<200)
	{
		retry++;
		_delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN()&&retry<240)
	{
		retry++;
		_delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_ReadBit(void) 			 // read one bit
{
    uint8_t data;
	DS18B20_IO_OUT;//SET PA0 OUTPUT
    DS18B20_DQ_OUT(LOW); 
	_delay_us(2);
    DS18B20_DQ_OUT(HIGH); 
	DS18B20_IO_IN;//SET PA0 INPUT
	_delay_us(12);
	if(DS18B20_DQ_IN())data=1;
    else data=0;	 
    _delay_us(50);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
uint8_t DS18B20_ReadByte(void)	/*read one byte*/
{        
    uint8_t j,dat;
    dat=0;
	for (volatile uint8_t i=1;i<=8;i++) 
	{
        j=DS18B20_ReadBit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_WriteByte(uint8_t dat)     
 {             
    uint8_t testb;
	DS18B20_IO_OUT;//SET PB3 OUTPUT;
    for (volatile uint8_t j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT(LOW);// Write 1
            _delay_us(2);                            
            DS18B20_DQ_OUT(HIGH);
            _delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT(LOW);// Write 0
            _delay_us(60);             
            DS18B20_DQ_OUT(HIGH);
            _delay_us(2);                          
        }
    }
}

//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_WriteByte(0xcc);// skip rom
    DS18B20_WriteByte(0x44);// convert
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在

//JTAG模式设置,用于设置JTAG的模式
//mode:jtag,swd模式设置;00/全使能;01/使能SWD;10/全关闭;	   
//#define JTAG_SWD_ENABLE    0X00		  
//#define SWD_ENABLE         0X01
//#define JTAG_SWD_DISABLE   0X02
//void JTAG_Set(uint8_t mode)
//{
//	u32 temp;
//	temp=mode;
//	temp<<=25;
//	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
//	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
//	AFIO->MAPR|=temp;       //设置jtag模式
//}
     	 
uint8_t DS18B20_Init(void)
{
    DS18B20_IO_OUT;
	DS18B20_DQ_OUT(HIGH);
	DS18B20_Rst();
	return DS18B20_Check();
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
int DS18B20_GetTemp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
	int32_t tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_WriteByte(0xcc);// skip rom
    DS18B20_WriteByte(0xbe);// convert	    
    TL=DS18B20_ReadByte(); // LSB   
    TH=DS18B20_ReadByte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(int32_t)tem*625/1000;    //转换     
	if(temp)return tem;  //返回温度值*10
	else return -tem;    
}
















