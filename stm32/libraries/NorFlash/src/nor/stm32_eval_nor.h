/**
  ******************************************************************************
  * @file    stm3210e_eval_nor.h
  * @author  MCD Application Team
  * @version V7.0.0
  * @date    14-April-2017
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm3210e_eval_nor.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EVAL_NOR_H
#define __STM32_EVAL_NOR_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
//#include "stm32f1xx_hal.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM3210E_EVAL
  * @{
  */
    
/** @addtogroup STM3210E_EVAL_NOR
  * @{
  */    

/* Exported constants --------------------------------------------------------*/ 
/** @defgroup STM3210E_EVAL_NOR_Exported_Constants STM3210E EVAL NOR Exported Constants
  * @{
  */
  
/** 
  * @brief  NOR status structure definition  
  */     
#define   NOR_STATUS_OK         0x00
#define   NOR_STATUS_ERROR      0x01

//#define NOR_DEVICE_ADDR  ((uint32_t)0x64000000)  
  
/* #define NOR_MEMORY_WIDTH    FSMC_NORSRAM_MEM_BUS_WIDTH_8  */
#define NOR_MEMORY_WIDTH    FSMC_NORSRAM_MEM_BUS_WIDTH_16

#define NOR_BURSTACCESS    FSMC_BURST_ACCESS_MODE_DISABLE  
/* #define NOR_BURSTACCESS    FSMC_BURST_ACCESS_MODE_ENABLE*/
  
#define NOR_WRITEBURST    FSMC_WRITE_BURST_DISABLE  
/* #define NOR_WRITEBURST   FSMC_WRITE_BURST_ENABLE */
 
/* NOR operations Timeout definitions */
#define BLOCKERASE_TIMEOUT   ((uint32_t)0x00A00000)  /* NOR block erase timeout */
#define CHIPERASE_TIMEOUT    ((uint32_t)0x30000000)  /* NOR chip erase timeout  */ 
#define PROGRAM_TIMEOUT      ((uint32_t)0x00004400)  /* NOR program timeout     */ 

/* NOR Ready/Busy signal GPIO definitions */
#define NOR_READY_BUSY_PIN    GPIO_PIN_6 
#define NOR_READY_BUSY_GPIO   GPIOD
#define NOR_READY_STATE       GPIO_PIN_SET
#define NOR_BUSY_STATE        GPIO_PIN_RESET 

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup STM3210E_EVAL_NOR_Exported_Functions
  * @{
  */  
uint8_t NORFLASHInit(void);
uint8_t NORFLASHReadData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t NORFLASHWriteData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t NORFLASHProgramData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize);
uint8_t NORFLASHEraseBlock(uint32_t BlockAddress);
uint8_t NORFLASHEraseChip(void);
uint8_t NORFLASHReadID(NOR_IDTypeDef *pNOR_ID);
void NORFLASHReturnToReadMode(void);

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

#ifdef __cplusplus
}

class NORFLASH
{
  public:
	
	NORFLASH(){};
    uint8_t Init(void){return NOR_Init();};
	uint8_t readData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize){
		return NOR_ReadData(uwStartAddress, pData, uwDataSize);};
	uint8_t writeData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize){
		return NOR_WriteData(uwStartAddress, pData, uwDataSize);};
	uint8_t programData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize){
		return NOR_ProgramData(uwStartAddress, pData, uwDataSize);};
	uint8_t eraseBlock(uint32_t BlockAddress){return NOR_EraseBlock(BlockAddress);};
	uint8_t eraseChip(void){return NOR_EraseChip();};
	uint8_t readID(NOR_IDTypeDef *pNOR_ID){return NOR_ReadID(pNOR_ID);};
	void returnToReadMode(void){NOR_ReturnToReadMode();};
  private:   

};

#endif

#endif /* __STM3210E_EVAL_NOR_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
