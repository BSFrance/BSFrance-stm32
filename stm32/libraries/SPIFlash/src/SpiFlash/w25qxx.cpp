#include <Arduino.h>

//user board &spiflashconfig file
#if __has_include("bsp.h")
#  include "bsp.h"
#endif

#if __has_include("configs/spiFlashConfig.h")
#  include "configs/spiFlashConfig.h"
#endif

#include "w25qxx.h" 
#include <SPI.h>

//user unconfig spiflash use variant.h define
#ifndef  SPIFLASH_MOSI
# define SPIFLASH_MOSI	MOSI
#endif
#ifndef	SPIFLASH_MISO
#define SPIFLASH_MISO	MISO
#endif
#ifndef	SPIFLASH_SCK
#define SPIFLASH_SCK	SCK
#endif
#ifndef	SPIFLASH_CS
#define SPIFLASH_CS		SS
#endif

#ifndef SPIFLASH_SPEED_Mhz
#define SPIFLASH_SPEED_Mhz 20
#endif

////////////////////////////////////////////////////////////////////////////////// 
//command list 指令表
#define W25X_WriteStatusReg		0x01 
#define W25X_PageProgram		0x02 
#define W25X_ReadData			0x03 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteEnable		0x06 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9
#define W25X_DummyByte          0xA5 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F
 
#define CHIP_Unlock             0x98    /*mx25x128 unlock*/ 
//IDO
#define ID0_SPANSION	0x01
#define ID0_MICRON		0x20
#define ID0_MACRONIX	0xC2
#define IDO_GIGA	 	0xC8
#define ID0_SST			0xBF
#define ID0_WINBOND		0xEF
 
 
SPIFLASH::SPIFLASH()
       :SPIClass(SPIFLASH_MOSI,SPIFLASH_MISO,SPIFLASH_SCK){};
SPIFLASH::SPIFLASH(SPI_TypeDef *instance)
       :SPIClass(instance){};
SPIFLASH::SPIFLASH(uint8_t mosi, uint8_t miso, uint8_t sck)
	   :SPIClass(mosi, miso, sck){};


void SPIFLASH::begin(uint8_t cs)
{
  if(pdata->inited == 0){
	pdata->inited = 1;  
  if (cs == 0xff)
    pdata->cs_pin = SPIFLASH_CS;
  else
    pdata->cs_pin = cs;

  pinMode(pdata->cs_pin,OUTPUT);
  digitalWrite(pdata->cs_pin,HIGH);			//SPI FLASH不选中
  SPIClass::begin();	//初始化SPI
  SPIClass::beginTransaction(SPISettings(1000000UL*SPIFLASH_SPEED_Mhz, MSBFIRST, SPI_MODE0));
//  SPI1_SetSpeed(SPI_BaudRatePrescaler_2);		//设置为42M时钟,高速模式 
  this->jedecDeviceID(&pdata->ID[0]);	//读取FLASH ID.
  
  }
}  

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t SPIFLASH::readSR(void)   
{  
	uint8_t byte=0;   
	digitalWrite(pdata->cs_pin,LOW);                            //使能器件   
	SPIClass::transfer(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
	byte=SPIClass::transfer(0Xff);             //读取一个字节  
	digitalWrite(pdata->cs_pin,HIGH);                            //取消片选     
	return byte;   
} 
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void SPIFLASH::writeSR(uint8_t sr)   
{   
	digitalWrite(pdata->cs_pin,LOW);                               //使能器件   
	SPIClass::transfer(W25X_WriteStatusReg);   //发送写取状态寄存器命令    
	SPIClass::transfer(sr);                    //写入一个字节  
	digitalWrite(pdata->cs_pin,HIGH);                              //取消片选     	      
}   
//W25QXX写使能	
//将WEL置位   
void SPIFLASH::writeEnable(void)   
{
	digitalWrite(pdata->cs_pin,LOW); 			 //使能器件   
    SPIClass::transfer(W25X_WriteEnable);        //发送写使能  
	digitalWrite(pdata->cs_pin,HIGH);  			 //取消片选     	      
} 

//W25QXX写禁止	
//将WEL清零  
void SPIFLASH::writeDisable(void)   
{  
	digitalWrite(pdata->cs_pin,LOW);			//使能器件   
    SPIClass::transfer(W25X_WriteDisable);     //发送写禁止指令    
	digitalWrite(pdata->cs_pin,HIGH); 			//取消片选     	      
} 
		
//读取芯片ID
//返回值如下:				   
//0XEF12,表示芯片型号为W25Q40  
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
uint16_t SPIFLASH::readID(void)
{
	uint32_t temp = 0;	  
	digitalWrite(pdata->cs_pin,LOW);				    
	SPIClass::transfer(W25X_ManufactDeviceID);//发送读取ID命令	    
	SPIClass::transfer(W25X_DummyByte); 	    
	SPIClass::transfer(W25X_DummyByte); 	    
	SPIClass::transfer(0x00); 	    
	temp |= SPIClass::transfer(W25X_DummyByte)<< 8;  
	temp |= SPIClass::transfer(W25X_DummyByte);
	digitalWrite(pdata->cs_pin,HIGH);				    
	return temp;
}

void SPIFLASH::jedecDeviceID(uint8_t *buf)
{
	digitalWrite(pdata->cs_pin,LOW);				    
	SPIClass::transfer(W25X_JedecDeviceID);//发送读取JedecDeviceID 0x9F命令	    
	buf[0] = SPIClass::transfer(W25X_DummyByte); 	    
	buf[1]= SPIClass::transfer(W25X_DummyByte);
	buf[2]= SPIClass::transfer(W25X_DummyByte);
	if (buf[0] == ID0_SPANSION) {
		buf[3] = SPIClass::transfer(W25X_DummyByte); // ID-CFI
		buf[4] = SPIClass::transfer(W25X_DummyByte); // sector size
	}	
	digitalWrite(pdata->cs_pin,HIGH);				    
}   		    

uint32_t SPIFLASH::capacity(void)
{
	uint32_t n = 1048576; // unknown chips, default to 1 MByte
	if (pdata->ID[2] >= 0x10 && pdata->ID[2] <= 0x1F) {
		n = 1ul << pdata->ID[2];
	} else if (pdata->ID[2] >= 0x20 && pdata->ID[2] <= 0x25) {
		n = 1ul << (pdata->ID[2] - 6);
	} else	if ((pdata->ID[0]==0 && pdata->ID[1]==0 && pdata->ID[2]==0) || 
		(pdata->ID[0]==255 && pdata->ID[1]==255 && pdata->ID[2]==255)) {
		n = 0;
	}
	return n;
}

//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void SPIFLASH::read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
 	digitalWrite(pdata->cs_pin,LOW);   		   //使能器件   
    SPIClass::transfer(W25X_ReadData);         //发送读取命令   
    SPIClass::transfer((uint8_t)((ReadAddr)>>16));  //发送24bit地址    
    SPIClass::transfer((uint8_t)((ReadAddr)>>8));   
    SPIClass::transfer((uint8_t)ReadAddr);   
    for(uint16_t i=0;i<NumByteToRead;i++)
	{ 
        pBuffer[i]=SPIClass::transfer(0XFF);   //循环读数  
    }
	digitalWrite(pdata->cs_pin,HIGH);  				    	      
}  

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void SPIFLASH::writePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    this->writeEnable();                  //SET WEL 
	digitalWrite(pdata->cs_pin,LOW);  		//使能器件   
    SPIClass::transfer(W25X_PageProgram);      //发送写页命令   
    SPIClass::transfer((uint8_t)((WriteAddr)>>16)); //发送24bit地址    
    SPIClass::transfer((uint8_t)((WriteAddr)>>8));   
    SPIClass::transfer((uint8_t)WriteAddr);   
    for(uint16_t i=0;i<NumByteToWrite;i++)SPIClass::transfer(pBuffer[i]);//循环写数  
	digitalWrite(pdata->cs_pin,HIGH);                            //取消片选 
	this->waitBusy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPIFLASH::writeNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		this->writePage(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
	 
void SPIFLASH::write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
   	W25QXX_BUF = pdata->aBuf;	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		this->read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}

		if(i<secremain)//需要擦除
		{
			this->eraseSector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			this->writeNoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  
		}else
			this->writeNoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   

		if(NumByteToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	};	 
}
//擦除整个芯片		  
//等待时间超长...
void SPIFLASH::eraseChip(void)   
{                                   
    this->writeEnable();                  //SET WEL 
    this->waitBusy();   

//	digitalWrite(pdata->cs_pin,LOW); 			 //使能器件   
//    SPIClass::transfer(CHIP_Unlock);        //发送写使能  
//	digitalWrite(pdata->cs_pin,HIGH);  			 //取消片选     	      

  	digitalWrite(pdata->cs_pin,LOW);                            //使能器件   
    SPIClass::transfer(W25X_ChipErase);        //发送片擦除命令  
	digitalWrite(pdata->cs_pin,HIGH);                            //取消片选     	      
	this->waitBusy();   				   //等待芯片擦除结束
}   
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个扇区的最少时间:150ms
void SPIFLASH::eraseSector(uint32_t Dst_Addr)   
{  
	  
// 	printf("fe:%x\r\n",Dst_Addr); //监视falsh擦除情况,测试用  
 	Dst_Addr*=4096;
    this->writeEnable();     			        //SET WEL 	 
    this->waitBusy();   
  	digitalWrite(pdata->cs_pin,LOW);            //使能器件   
    SPIClass::transfer(W25X_SectorErase);      //发送扇区擦除指令 
    SPIClass::transfer((uint8_t)((Dst_Addr)>>16));  //发送24bit地址    
    SPIClass::transfer((uint8_t)((Dst_Addr)>>8));   
    SPIClass::transfer((uint8_t)Dst_Addr);  
	digitalWrite(pdata->cs_pin,HIGH);		//取消片选     	      
    this->waitBusy();						//等待擦除完成
}
//等待空闲
inline void SPIFLASH::waitBusy(void)   
{   
	while((this->readSR()&0x01)==0x01);   // 等待BUSY位清空
}
//进入掉电模式
void SPIFLASH::powerDown(void)   
{ 
  	digitalWrite(pdata->cs_pin,LOW); 	        //使能器件   
    SPIClass::transfer(W25X_PowerDown);         //发送掉电命令  
	digitalWrite(pdata->cs_pin,HIGH);  			//取消片选
    delay(3);                     //等待TRES1
}   
//唤醒
void SPIFLASH::WAKEUP(void)   
{  
  	digitalWrite(pdata->cs_pin,LOW);             //使能器件   
    SPIClass::transfer(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	digitalWrite(pdata->cs_pin,HIGH);            //取消片选
    delay(3);									//等待TRES1
}
