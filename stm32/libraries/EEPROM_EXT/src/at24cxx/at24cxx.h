//////////////////////////////////////////////////////////////////////////////////	 
//24CXX 驱动代码	   
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com								  
////////////////////////////////////////////////////////////////////////////////// 	

#ifndef __AT24CXX_H
#define __AT24CXX_H

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767
#define AT24C512	65535

#if __has_include("configs/i2cEepromConfig.h")
#  include "configs/i2cEepromConfig.h"
#endif

#include "wireSoft.h"
 
#ifdef __cplusplus

typedef struct {
	uint16_t dev;
	uint8_t  adr;
	uint8_t  ctrl;
} AT24CXX_TypeDef;


#ifndef AT24CXX_TYPE
# define AT24CXX_TYPE  AT24C02
#endif

class EXTEEPROM:public TwoWireSoft 
{
 public:
    EXTEEPROM(uint16_t _dev = AT24CXX_TYPE);
	EXTEEPROM(uint8_t sda,uint8_t scl);
	void setPins(uint8_t sda,uint8_t scl),
		writeOneByte(uint16_t WriteAddr,uint8_t DataToWrite),		
		writeLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len),
		write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite),	
		read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead); 
	uint8_t	check(void),  //检查器件
		readOneByte(uint16_t ReadAddr),
        begin(uint8_t _devAdr=0x50), //初始化IIC	
		begin(int _devAdr); //初始化IIC
	uint32_t readLenByte(uint16_t ReadAddr,uint8_t Len);
	AT24CXX_TypeDef *pdata =&sdata;
 private:
    AT24CXX_TypeDef sdata;
}; 

#endif //__cplusplus
#endif
















