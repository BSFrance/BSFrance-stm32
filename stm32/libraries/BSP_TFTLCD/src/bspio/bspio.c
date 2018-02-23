/**
  ******************************************************************************
  * @file    bsp_fsmc_lcdio.c
  * @author  huaweiwx  <huaweiwx@sina.com>
  * @version V1.0.0
  * @date    2017-7-2
  * @brief   This file provides a set of firmware functions to manage lcd_io
  *
  ******************************************************************************
  */
 
/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#if __has_include("bsp.h")
#  include "bsp.h"
#endif

#if __has_include("configs/LCDConfig.h")
#  include "configs/LCDConfig.h"
#endif

#ifdef  TFT_LCD_BASE
typedef struct
{
	volatile uint16_t  REG;
	volatile uint16_t  RAM;
} LCD_IO_TypeDef;

#define TFT_LCD			((LCD_IO_TypeDef *) TFT_LCD_BASE)


void        LCD_IO_Init(void);
void        LCD_IO_WriteReg(uint16_t Reg);
void        LCD_IO_WriteData(uint16_t RegValue);
void 		LCD_IO_WriteRegData(uint16_t Reg, uint16_t RegValue);
void        LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);
uint16_t    LCD_IO_ReadReg(uint16_t Reg);
uint16_t 	LCD_IO_ReadData(void);
void        LCD_Delay (uint32_t delay);


/********************************* LINK LCD ***********************************/

/**
  * @brief  Initializes LCD low level.
  * @retval None
  */
void LCD_IO_Init(void) 
{
  STM_FSMC_LCD_Init();
}

/**
  * @brief  Writes register on LCD register.
  * @param  Reg: Register to be written
  * @retval None
  */
void LCD_IO_WriteReg(uint16_t Reg) 
{
  TFT_LCD->REG = (Reg);
}

/**
  * @brief  Writes data on LCD data register.
  * @param  RegValue: Data to be written
  * @retval None
  */
void  LCD_IO_WriteData(uint16_t RegValue)
{
    TFT_LCD->RAM = (RegValue);
}

/**
  * @brief  Writes data on LCD data register.
  * @param  RegValue: Data to be written
  * @retval None
  */
void  LCD_IO_WriteRegData(uint16_t Reg, uint16_t RegValue)
{
    LCD_IO_WriteReg(Reg);
	LCD_IO_WriteData(RegValue);
}

/**
  * @brief  Writes multiple data on LCD data register.
  * @param  pData: Data to be written
  * @param  Size: number of data to write
  * @retval None
  */
void LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size)
{
  uint32_t counter = 0;
  uint16_t regvalue;

  regvalue = *pData | (*(pData+1) << 8);
  for (counter = Size; counter != 0; counter--)
  {
    /* Write 16-bit Reg */
    TFT_LCD->RAM = regvalue;
    counter--;
    pData += 2;
    regvalue = *pData | (*(pData+1) << 8);
  }
}


/**
  * @brief  Reads data from LCD data register.
  * @param  Reg: Register to be read
  * @retval Read data.
  */
uint16_t LCD_IO_ReadReg(uint16_t Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  TFT_LCD->REG = Reg;
  /* Read 16-bit Reg */
  return (TFT_LCD->RAM);
}

uint16_t LCD_IO_ReadData(void)
{
  /* Read 16-bit FSMC data */
  return (TFT_LCD->RAM);
}


/**
  * @brief  Wait for loop in ms.
  * @param  Delay in ms.
  * @retval None
  */
void LCD_Delay (uint32_t Delay)
{
  HAL_Delay(Delay);
}

uint16_t LCD_IO_ReadID(void){
	uint16_t id =0;
    STM_FSMC_LCD_TimeSet(30, 50);        //设置较长的总线时间，保证读取ID的正确性;
	LCD_IO_WriteRegData(0x0000,0x0001);  //reset lcd
	LCD_Delay(50);
	LCD_IO_WriteReg(0x0000); 
	id = LCD_IO_ReadData();

  	if(id<0XFF||id==0XFFFF||id==0X9300)//读到ID不正确,新增id==0X9300判断，因为9341在未被复位的情况下会被读成9300
	{	
 		//尝试9341 ID的读取		
		LCD_IO_WriteReg(0XD3);				   
		id = LCD_IO_ReadData();	//dummy read 	
 		id = LCD_IO_ReadData(); 	//读到0X00
  		id = LCD_IO_ReadData();   	//读取93								   
 		id <<= 8;
		id |= LCD_IO_ReadData();  	//读取41 	   			   
 		if(id!=0x9341)		//非9341,尝试是不是6804
		{	
 			LCD_IO_WriteReg(0XBF);				   
			id=LCD_IO_ReadData();//dummy read 	 
	 		id=LCD_IO_ReadData();//读回0X01			   
	 		id=LCD_IO_ReadData();//读回0XD0 			  	
	  		id=LCD_IO_ReadData();//这里读回0X68 
			id<<=8;
	  		id|=LCD_IO_ReadData();//这里读回0X04	  
			if(id!=0x6804)	//也不是RM68042,尝试看看是不是5310
			{ 
				LCD_IO_WriteReg(0XD4);				   
				id = LCD_IO_ReadData();	//dummy read  
				id = LCD_IO_ReadData();	//读回0X01	 
				id = LCD_IO_ReadData();	//读回0X53	
				id <<= 8;	 
				id |= LCD_IO_ReadData();	//这里读回0X10	 
				if(id!= 0x5310)		    //也不是NT35310,尝试看看是不是NT35510
				{
					LCD_IO_WriteReg(0XDA00);	
					id=LCD_IO_ReadData();//读回0X00	 
					LCD_IO_WriteReg(0XDB00);	
					id=LCD_IO_ReadData();//读回0X80
					id<<=8;	
					LCD_IO_WriteReg(0XDC00);	
					id|=LCD_IO_ReadData();//读回0X00		
					if(id==0x8000)id= 0x5510;//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
				}
			}
 		}  	
	}  
  LCD_IO_Init();  //lcd reset normal speed;
  return id;
}
#endif //TFT_LCD_BASE
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

