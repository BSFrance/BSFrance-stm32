#ifndef __W25QXX_H__
#define __W25QXX_H__

#include <SPI.h>


//W25X系列/Q系列芯片列表
#define W25Q40 	0XEF12  /*WINBOND*/
#define W25Q80 	0XEF13 	/*WINBOND*/
#define W25Q16 	0XEF14	/*WINBOND*/
#define W25Q32 	0XEF15	/*WINBOND*/
#define W25Q64 	0XEF16	/*WINBOND*/
#define W25Q128	0XEF17	/*WINBOND*/

#define M25P64	0x2016  /*MICRON*/
#define M25P128	0x2017  /*MICRON*/

#define SST25VF032  0xBF4A /*SST*/

#define GD25Q40    0xC812	  /*GIGA*/   

#define AT45DB161  0x1F2600 /*AT*/

#define MX25L16  0xC214   /*MACRONIX*/
#define MX25L32  0xC215   /*MACRONIX*/
#define MX25L64  0xC216   /*MACRONIX*/
#define MX25L128 0xC217   /*MACRONIX*/

#ifdef __cplusplus

#define  W25QXX_BUFFERSIZE 4096

typedef struct {
    uint8_t cs_pin;
	uint8_t	aBuf[W25QXX_BUFFERSIZE];	
    uint8_t	ID[5];  //定义W25QXX芯片ID
	uint8_t inited = 0;
} W25QXX_TypeDef;


class SPIFLASH : public SPIClass 
{
  public:
	SPIFLASH();
    SPIFLASH(uint8_t mosi, uint8_t miso, uint8_t sck);
	SPIFLASH(SPI_TypeDef *instance);
    void begin(uint8_t cs = 0xff);
	uint16_t readID(void);  	    	//读取FLASH ID
    uint32_t capacity(void);
	uint8_t	 readSR(void);        		//读取状态寄存器 
    void writeSR(uint8_t sr);  			//写状态寄存器
    void writeEnable(void);  			//写使能 
    void writeDisable(void);			//写保护
    void read 		 (uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
    void write		 (uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash
    void writeNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
    void eraseChip(void);    	  	//整片擦除
    void eraseSector(uint32_t Dst_Addr);	//扇区擦除
    void waitBusy(void);           	//等待空闲
    void powerDown(void);        	//进入掉电模式
    void WAKEUP(void);				//唤醒
	W25QXX_TypeDef *pdata =&sdata;
  private:   
	void jedecDeviceID(uint8_t *buf);
	void writePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
	W25QXX_TypeDef sdata;
};

#endif  //__cplusplus
#endif  //__W25QXX_H















