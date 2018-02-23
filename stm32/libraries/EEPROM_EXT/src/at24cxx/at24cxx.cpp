#include <Arduino.h>

#include "at24cxx.h"

#ifndef AT24CXX_SDA
# define   AT24CXX_SDA SDA
#endif

#ifndef AT24CXX_SCL
# define   AT24CXX_SCL SCL
#endif


EXTEEPROM::EXTEEPROM(uint16_t _dev)
       :TwoWireSoft(){

	   pdata->dev = _dev;
	   
       this->setPins(AT24CXX_SDA,AT24CXX_SCL);
 
#ifdef AT24CXX_A2A1A0
       pdata->adr = (0x50 | AT24CXX_A2A1A0)<<1;   //0x50|A2A1A0
#else
       pdata->adr = 0x50<<1 ; //A2A1A0=0      
#endif
       pdata->ctrl = 0;	
}

EXTEEPROM::EXTEEPROM (uint8_t sda,uint8_t scl)
       :TwoWireSoft(sda,scl){
       pdata->ctrl = 0;	
}

void EXTEEPROM::setPins(uint8_t sda,uint8_t scl){
       TwoWireSoft::setPins(sda,scl);
}

//初始化IIC接口

uint8_t EXTEEPROM::begin(uint8_t _devAdr)
{
    if (_devAdr >= 0xA0)  //rw addr
		pdata->adr = _devAdr;
    if (_devAdr >= 0x50)   //dev addr
       pdata->adr = (_devAdr<<1);  //0x5x
	else if(_devAdr > 0)        //a2a1a0 adr else use default rw addr;
       pdata->adr = (0x50 | _devAdr)<<1;
   
	TwoWireSoft::begin();
	return ((pdata->ctrl)?this->check():0);
}

uint8_t EXTEEPROM::begin(int _devAdr)
{
    return EXTEEPROM::begin((uint8_t)_devAdr);
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
uint8_t EXTEEPROM::readOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
    TwoWireSoft::start();  
	if((pdata->dev)>AT24C16)
	{
		TwoWireSoft::sendByte(pdata->adr);	   //发送写命令
		TwoWireSoft::waitAck();
		TwoWireSoft::sendByte(ReadAddr>>8);//发送高地址	    
	}else TwoWireSoft::sendByte((pdata->adr)+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	TwoWireSoft::waitAck(); 
    TwoWireSoft::sendByte(ReadAddr%256);   //发送低地址
	TwoWireSoft::waitAck();	    
	TwoWireSoft::start();  	 	   
	TwoWireSoft::sendByte((pdata->adr)+1);           //进入接收模式			   
	TwoWireSoft::waitAck();	 
    temp=TwoWireSoft::readByte(0);		   
    TwoWireSoft::stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void EXTEEPROM::writeOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    TwoWireSoft::start();  
	if((pdata->dev)>AT24C16)
	{
		TwoWireSoft::sendByte(pdata->adr);	    //发送写命令
		TwoWireSoft::waitAck();
		TwoWireSoft::sendByte(WriteAddr>>8);//发送高地址	  
	}else TwoWireSoft::sendByte((pdata->adr)+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	TwoWireSoft::waitAck();	   
    TwoWireSoft::sendByte(WriteAddr%256);   //发送低地址
	TwoWireSoft::waitAck(); 	 										  		   
	TwoWireSoft::sendByte(DataToWrite);     //发送字节							   
	TwoWireSoft::waitAck();  		    	   
    TwoWireSoft::stop();//产生一个停止条件 
	delay(10);	 
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void EXTEEPROM::writeLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{  	
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		this->writeOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
uint32_t EXTEEPROM::readLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=this->readOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
uint8_t EXTEEPROM::check(void)
{
	uint8_t temp;
	temp=this->readOneByte((pdata->dev));//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		this->writeOneByte((pdata->dev),0X55);
	    temp=this->readOneByte((pdata->dev));	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void EXTEEPROM::read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=this->readOneByte(ReadAddr++);	
		NumToRead--;
	}
} 
 
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void EXTEEPROM::write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		this->writeOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








