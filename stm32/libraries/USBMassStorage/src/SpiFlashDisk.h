/*
  Copyright (c) 2017 huaweiwx@sina.com

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#ifndef  _SPIFLASHDISK_H_
#define _SPIFLASHDISK_H_

#include <Flash_STM32.h>
#include <FlashBlockFat.h>


#ifndef SPIFLASHDISK_SIZE
#  define SPIFLASHDISK_SIZE   ((uint32_t)512)                /*W25X40 512k*/
#else
#  if  SPIFLASHDISK_SIZE>2040
#    error "FAT12 SPIFLASHDISK_SIZE must <=2040"
#  endif	
#endif

#ifndef SPIFLASHDISK_START
#  define SPIFLASHDISK_START    ((uint32_t)0)
#endif


//2 x 32KB flash pages (see reference manual / flash module organization)
# define FLASHDIVE_BLOCKS    SPIFLASHDISK_SIZE*1024/512  /*W25X40 max 512k*/

SpiEmbeddedFlashBlock block0(SPIFLASHDISK_START                             ,  (SPIFLASHDISK_SIZE/2) * 1024); 
SpiEmbeddedFlashBlock block1(SPIFLASHDISK_START +  (SPIFLASHDISK_SIZE/2) * 1024,  (SPIFLASHDISK_SIZE/2) * 1024);

// combine them into one virtual storage of 512 bytes
FlashVariables<512> SpiStorageBackend(&block0, &block1);

// SdFat interface, so we can access it from the microcontroller
FlashBlockFat fat(&SpiStorageBackend);

// USB interface so we can access it through USB
SpiBlockDevice SpiBlockDevice(FLASHDIVE_BLOCKS, &SpiStorageBackend);

// When we open the first time, and is not formatted, format it
void formatListener() {
  formatFat12(&SpiBlockDevice, FLASHDIVE_BLOCKS);
}

//Called by USB subsystem
BlockDevice *getMassStorage() {
  SpiBlockDevice.begin();
  SpiStorageBackend.setFormatListener(formatListener);
  //If the drive appears unformbeginatted, try this (should not be needed):
  //storageBackend.format();
  return &SpiBlockDevice;
}

#endif