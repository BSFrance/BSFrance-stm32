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

#if __has_include("bsp_i2ceeprom.h")
#  include "bsp_i2ceeprom.h"
#include <Wire.h>
#if AT24CXX_DEV  //use i2c port
# if defined(AT24CXX_SCL) && defined(AT24CXX_SCL) 
  TwoWire Wire(AT24CXX_DEV, AT24Cxx_SDA, AT24CXX_SCL);
# else
  TwoWire Wire(AT24CXX_DEV);
# endif
#else
  SoftWire Wire(AT24Cxx_SDA, AT24CXX_SCL);	
#endif

extern "C" void EEPROM_IO_Init(void)
{
  Wire.begin();
}

/**
  * @brief Write data to I2C EEPROM driver in using DMA channel
  * @param DevAddress: Target device address
  * @param MemAddress: Internal memory address
  * @param pBuffer: Pointer to data buffer
  * @param BufferSize: Amount of data to be sent
  * @retval HAL status
  */
extern "C" HAL_StatusTypeDef EEPROM_IO_WriteData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
  return (I2Cx_WriteMultiple(DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize));
}

/**
  * @brief  Reads data from I2C EEPROM driver in using DMA channel.
  * @param  DevAddress: Target device address
  * @param  MemAddress: Internal memory address
  * @param  pBuffer: Pointer to data buffer
  * @param  BufferSize: Amount of data to be read
  * @retval HAL status
  */
extern "C" HAL_StatusTypeDef EEPROM_IO_ReadData(uint16_t DevAddress, uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize)
{
  return (I2Cx_ReadMultiple(DevAddress, MemAddress, I2C_MEMADD_SIZE_16BIT, pBuffer, BufferSize));
}

/**
  * @brief  Checks if target device is ready for communication. 
  * @note   This function is used with Memory devices
  * @param  DevAddress: Target device address
  * @param  Trials: Number of trials
  * @retval HAL status
  */
extern "C" HAL_StatusTypeDef EEPROM_IO_IsDeviceReady(uint16_t DevAddress, uint32_t Trials)
{ 
  return (I2Cx_IsDeviceReady(DevAddress, Trials));
}

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

