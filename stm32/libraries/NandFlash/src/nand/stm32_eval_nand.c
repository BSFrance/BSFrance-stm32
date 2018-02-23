/**
  ******************************************************************************
  * @file    stm3210e_eval_nand.c
  * @author  MCD Application Team
  * @version V7.0.0
  * @date    14-April-2017
  * @brief   This file includes a standard driver for the NAND512W3A2CN6E NAND flash memory 
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
   - This driver is used to drive the NAND512W3A2CN6E NAND flash external memory mounted
     on STM3210E-EVAL evaluation board.
   - This driver does not need a specific component driver for the NAND device
     to be included with.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the NAND external memory using the BSP_NAND_Init() function. This 
       function includes the MSP layer hardware resources initialization and the
       FMC controller configuration to interface with the external NAND memory.
  
  + NAND flash operations
     o NAND external memory can be accessed with read/write operations once it is
       initialized.
       Read/write operation can be performed with AHB access using the functions
       BSP_NAND_ReadData()/BSP_NAND_WriteData(). The BSP_NAND_WriteData() performs write operation
       of an amount of data by unit (halfword).
     o The function BSP_NAND_Read_ID() returns the chip IDs stored in the structure 
       "NAND_IDTypeDef". (see the NAND IDs in the memory data sheet)
     o Perform erase block operation using the function BSP_NAND_Erase_Block() and by
       specifying the block address. 
    
------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#if __has_include("bsp.h")
 #include "bsp.h"
#endif

#ifdef  HAVE_NAND  // else skip this code  
#include <Arduino.h>
#include "stm32_eval_nand.h"

extern NAND_HandleTypeDef nandHandle;

/**
  * @brief  Initializes the NAND device.
  * @retval NAND memory status
  */
uint8_t BSP_NAND_Init(void)
{
	STM_FSMC_NAND_Init();
	return HAL_OK;
}

/**
  * @brief  Reads an amount of data from the NAND device.
  * @param  BlockAddress: Block address to Read
  * @param  pData: Pointer to data to be read
  * @param  uwNumPage: Number of Pages to read to Block    
  * @retval NAND memory status
  */
uint8_t BSP_NAND_ReadData(NAND_AddressTypeDef BlockAddress, uint8_t* pData, uint32_t uwNumPage)
{
  /* Read data from NAND */
  if (HAL_NAND_Read_Page_8b(&nandHandle, &BlockAddress, pData, uwNumPage) != HAL_OK)
  {
    return NAND_ERROR;
  }
  
  return NAND_OK;
}

/**
  * @brief  Writes an amount of data to the NAND device.
  * @param  BlockAddress: Block address to Write
  * @param  pData: Pointer to data to be written
  * @param  uwNumPage: Number of Pages to write to Block
  * @retval NAND memory status
  */
uint8_t BSP_NAND_WriteData(NAND_AddressTypeDef BlockAddress, uint8_t* pData, uint32_t uwNumPage)
{
  /* Write data to NAND */
  if (HAL_NAND_Write_Page_8b(&nandHandle, &BlockAddress, pData, uwNumPage) != HAL_OK)
  {
    return NAND_ERROR;
  }
  
  return NAND_OK;
}

/**
  * @brief  Erases the specified block of the NAND device. 
  * @param  BlockAddress: Block address to Erase  
  * @retval NAND memory status
  */
uint8_t BSP_NAND_EraseBlock(NAND_AddressTypeDef BlockAddress)
{
  /* Send NAND erase block operation */
  if (HAL_NAND_Erase_Block(&nandHandle, &BlockAddress) != HAL_OK)
  {
    return NAND_ERROR;
  }
  
  return NAND_OK;
}

/**
  * @brief  Reads NAND flash IDs.
  * @param  pNAND_ID : Pointer to NAND ID structure
  * @retval NAND memory status
  */
uint8_t BSP_NAND_ReadID(NAND_IDTypeDef *pNAND_ID)
{
  if(HAL_NAND_Read_ID(&nandHandle, pNAND_ID) != HAL_OK)
  {
    return NAND_ERROR;
  }
  else
  {
    return NAND_OK;
  }
}

#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
