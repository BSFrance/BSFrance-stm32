#ifndef __INTERNALFLASHDISK_H__
#define __INTERNALFLASHDISK_H__

#include <Flash_STM32.h>
#include <FlashBlockFat.h>

/*
unused system define FLASH_LENGTH,because of 
stm32f103x8 stm32L1xxx8 as xB have 128k internalflash and xC/xD as xE have 512k flash (undocmented)
*/

#if defined(STM32F103C8)||defined(STM32F103R8)||defined(STM32L152C8)
#   define  _FLASH_LENGTH 128
#elif defined(STM32F103RC)||defined(STM32F103RD)||\
    defined(STM32F103VC)||defined(STM32F103VD)||\
    defined(STM32F407VD)
#   define  _FLASH_LENGTH 512
#else 	  
#   define  _FLASH_LENGTH FLASH_LENGTH	  
#endif
#define _FLASH_END (FLASH_BASE+_FLASH_LENGTH)

#define FLASHDIVE_BLOCKS   FLASHDISK_SIZE*1024/512

STM32EmbeddedFlashBlock block0(_FLASH_END - FLASHDISK_SIZE * 1024,    (FLASHDISK_SIZE/2) * 1024);  //64~96k
STM32EmbeddedFlashBlock block1(_FLASH_END - FLASHDISK_SIZE * 1024/2 , (FLASHDISK_SIZE/2) * 1024);  //96~128k

// combine them into one virtual storage of 512 bytes
FlashVariables<512> storageBackend(&block0, &block1);

// SdFat interface, so we can access it from the microcontroller
FlashBlockFat fat(&storageBackend);

// USB interface so we can access it through USB
FlashBlockDevice flashBlockDevice(FLASHDIVE_BLOCKS, &storageBackend);

// When we open the first time, and is not formatted, format it
void formatListener() {
  formatFat12(&flashBlockDevice, FLASHDIVE_BLOCKS);
}

//Called by USB subsystem
BlockDevice *getMassStorage() {
  storageBackend.setFormatListener(formatListener);

  //If the drive appears unformatted, try this (should not be needed):
  //storageBackend.format();
  return &flashBlockDevice;
}

#endif  /* __INTERNALFLASHDISK_H__ */
