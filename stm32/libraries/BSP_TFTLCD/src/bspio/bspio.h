/**
  ******************************************************************************
  * @file    bsp_io.h
  * @author  huaweiwx <huaweiwx@sina.com>
  * @version V1.0.
  * @date    2017.7.2
  * @brief   This file contains all the functions prototypes for the bsp_fsmc_lcdio.c
  *           and  bsp_spi_id.c   
  ******************************************************************************
  */ 

#ifndef __BSPIO_H__
 #define __BSPIO_H__
 
 #ifdef __cplusplus
 extern "C" {
#endif 

/* FSMC_LCD IO functions */
void		LCD_IO_Init(void);
void		LCD_IO_WriteReg(uint16_t Reg);
uint16_t	LCD_IO_ReadReg(uint16_t Reg);
void		LCD_IO_WriteData(uint16_t data);
uint16_t	LCD_IO_ReadData(void);
void		LCD_IO_WriteRegData(uint16_t Reg, uint16_t RegValue);
void		LCD_IO_WriteMultipleData(uint8_t *pData, uint32_t Size);

//utils
void	 	LCD_Delay (uint32_t delay);
uint16_t    LCD_IO_ReadID(void);
 
#ifdef __cplusplus
 } // extern "C" {
#endif 
 
#endif
