/**
  ******************************************************************************
  * @file    stm3210e_eval_nor.c
  * @author  MCD Application Team
  * @version V7.0.0
  * @date    14-April-2017
  * @brief   This file includes a standard driver for the M29W128GL70ZA6E NOR flash memory 
  *          device mounted on STM3210E-EVAL evaluation board.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive the M29W128GL NOR flash external memory mounted
     on STM3210E-EVAL evaluation board.
   - This driver does not need a specific component driver for the NOR device
     to be included with.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the NOR external memory using the NOR_Init() function. This 
       function includes the MSP layer hardware resources initialization and the
       FSMC controller configuration to interface with the external NOR memory.
  
  + NOR flash operations
     o NOR external memory can be accessed with read/write operations once it is
       initialized.
       Read/write operation can be performed with AHB access using the functions
       NOR_ReadData()/NOR_WriteData(). The NOR_WriteData() performs write operation
       of an amount of data by unit (halfword). You can also perform a program data
       operation of an amount of data using the function NOR_ProgramData().
     o The function NOR_Read_ID() returns the chip IDs stored in the structure 
       "NOR_IDTypeDef". (see the NOR IDs in the memory data sheet)
     o Perform erase block operation using the function NOR_Erase_Block() and by
       specifying the block address. You can perform an erase operation of the whole 
       chip by calling the function NOR_Erase_Chip(). 
     o After other operations, the function NOR_ReturnToReadMode() allows the NOR 
       flash to return to read mode to perform read operations on it. 
 
------------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#if __has_include("bsp.h")
 #include "bsp.h"
#endif

#ifdef  HAVE_NOR  // else skip this code  
#include <Arduino.h>
#include "stm32_eval_nor.h"

#define ADDR_SHIFT16(A) (NOR_DEVICE_ADDR + (2 * (A)))
//#define NOR_WRITE(Address, Data)  (*(vu16 *)(Address) = (Data))

/** @defgroup STM3210E_EVAL_NOR_Private_Function_Prototypes STM3210E EVAL NOR Private Function Prototypes
  * @{
  */ 
/** @defgroup STM3210E_EVAL_NOR_Exported_Functions STM3210E EVAL NOR Exported Functions
  * @{
  */ 
extern  NOR_HandleTypeDef norHandle;

/**
  * @brief  Initializes the NOR device.
  * @retval NOR memory status
  */
uint8_t NOR_Init(void)
{ 
  return STM_FSMC_NOR_Init();
}

/**
  * @brief  Reads an amount of data from the NOR device.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of data to read    
  * @retval NOR memory status
  */
uint8_t NOR_ReadData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);
  NOR_WRITE((NOR_DEVICE_ADDR + uwStartAddress), 0x00F0);

  for(; uwDataSize != 0x00; uwDataSize--) /* while there is data to read */
  {
    /* Read a Halfword from the NOR */
    *pData++ = *(volatile uint16_t *)((NOR_DEVICE_ADDR + uwStartAddress));
    uwStartAddress = uwStartAddress + 2; 
  }  
	
  return NOR_STATUS_OK;
}

/**
  * @brief  Returns the NOR memory to read mode.
  */
void NOR_ReturnToReadMode(void)
{
   HAL_NOR_ReturnToReadMode(&norHandle);
}


/**
  * @brief  Programs an amount of data to the NOR device.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of data to write    
  * @retval NOR memory status
  */
uint8_t NOR_ProgramData(uint32_t WriteAddr, uint16_t* pBuffer, uint32_t uwDataSize)
{
  /* Send NOR program buffer operation */
//  HAL_NOR_ProgramBuffer(&norHandle, uwStartAddress, pData, uwDataSize);
  uint32_t lastloadedaddress = 0x00;
  uint32_t currentaddress = 0x00;
  uint32_t endaddress = 0x00;

  /* Initialize variables */
  currentaddress = WriteAddr;
  endaddress = WriteAddr + uwDataSize - 1;
  lastloadedaddress = WriteAddr;

  /* Issue unlock command sequence */
  NOR_WRITE(ADDR_SHIFT16(0x005555), 0x00AA);

  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);  

  /* Write Write Buffer Load Command */
  NOR_WRITE(ADDR_SHIFT16(WriteAddr), 0x0025);
  NOR_WRITE(ADDR_SHIFT16(WriteAddr), (uwDataSize - 1));

  /* Load Data into NOR Buffer */
  while(currentaddress <= endaddress)
  {
    /* Store last loaded address & data value (for polling) */
    lastloadedaddress = currentaddress;
 
    NOR_WRITE(ADDR_SHIFT(currentaddress), *pBuffer++);
    currentaddress += 1; 
  }

  NOR_WRITE(ADDR_SHIFT(lastloadedaddress), 0x29);
  
  /* Return the NOR memory status */
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, PROGRAM_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @brief  Writes an amount of data to the NOR device.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of data to write    
  * @retval NOR memory status
  */
uint8_t NOR_WriteData(uint32_t uwStartAddress, uint16_t* pData, uint32_t uwDataSize)
{
	
  do
  {
    /* Transfer data to the memory */
    NOR_ProgramData(uwStartAddress, *pData++,1);
    uwStartAddress = uwStartAddress + 2;
    uwDataSize--;
  }  while(uwDataSize != 0);

  return NOR_STATUS_OK;
}

/**
  * @brief  Erases the specified block of the NOR device. 
  * @param  BlockAddress: Block address to erase  
  * @retval NOR memory status
  */
uint8_t NOR_EraseBlock(uint32_t BlockAddress)
{
  /* Send NOR erase block operation */
//  HAL_NOR_Erase_Block(&norHandle, BlockAddress, NOR_DEVICE_ADDR);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x0080);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);
  NOR_WRITE((NOR_DEVICE_ADDR + BlockAddress), 0x30);
  
  /* Return the NOR memory status */  
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, BLOCKERASE_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  }
}

/**
  * @brief  Erases the entire NOR chip.
  * @retval NOR memory status
  */
uint8_t NOR_EraseChip(void)
{
  /* Send NOR Erase chip operation */
  
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x0080);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x0010);
  
  /* Return the NOR memory status */
  if(HAL_NOR_GetStatus(&norHandle, NOR_DEVICE_ADDR, CHIPERASE_TIMEOUT) != HAL_NOR_STATUS_SUCCESS)
  {
    return NOR_STATUS_ERROR;
  }
  else
  {
    return NOR_STATUS_OK;
  } 
}

/**
  * @brief  Reads NOR flash IDs.
  * @param  pNOR_ID : Pointer to NOR ID structure
  * @retval NOR memory status
  */
uint8_t NOR_ReadID(NOR_IDTypeDef *pNOR_ID)
{
//  if(HAL_NOR_Read_ID(&norHandle, pNOR_ID) != HAL_OK)
//  {
//    return NOR_STATUS_ERROR;
//  }
//  else
//  {
//    return NOR_STATUS_OK;
//  }
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x00AA);
  NOR_WRITE(ADDR_SHIFT16(0x02AAA), 0x0055);
  NOR_WRITE(ADDR_SHIFT16(0x05555), 0x0090);

  pNOR_ID->Manufacturer_Code = *(volatile uint16_t *) ADDR_SHIFT16(0x0000);
  pNOR_ID->Device_Code1 = *(volatile uint16_t *) ADDR_SHIFT16(0x0001);
  pNOR_ID->Device_Code2 = *(volatile uint16_t *) ADDR_SHIFT16(0x0002);
  pNOR_ID->Device_Code3 = *(volatile uint16_t *) ADDR_SHIFT16(0x0003);
  
  return NOR_STATUS_OK;
}

/**
  * @brief  NOR BSP Wait for Ready/Busy signal.
  * @param  hnor: Pointer to NOR handle
  * @param  Timeout: Timeout duration  
  */
void HAL_NOR_MspWait(NOR_HandleTypeDef *hnor, uint32_t Timeout)
{
  uint32_t timeout = Timeout;
  
  /* Polling on Ready/Busy signal */
#ifndef   NOR_WAIT_PIN
  while((HAL_GPIO_ReadPin(NOR_READY_BUSY_GPIO, NOR_READY_BUSY_PIN) != NOR_BUSY_STATE) && (timeout > 0)) 
#else
  while((digitalRead(NOR_WAIT_PIN) != NOR_BUSY_STATE) && (timeout > 0))
#endif
  {
    timeout--;
  }
  
  timeout = Timeout;
  
  /* Polling on Ready/Busy signal */
//  while((HAL_GPIO_ReadPin(NOR_READY_BUSY_GPIO, NOR_READY_BUSY_PIN) != NOR_READY_STATE) && (timeout > 0)) 
  while((digitalRead(NOR_WAIT_PIN) != NOR_READY_STATE) && (timeout > 0)) 
  {
    timeout--;
  }  
}

/**
  * @}
  */  
  
/**
  * @}
  */ 
  
/**
  * @}
  */ 
  
/**
  * @}
  */ 
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
