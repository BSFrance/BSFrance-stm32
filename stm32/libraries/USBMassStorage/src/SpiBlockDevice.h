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

#ifndef __SPIBLOCKDEVICE_H__
#define __SPIBLOCKDEVICE_H__

#include <SpiFlash.h>
#include "USBDevice.h"
#include "BlockDevice.h"
#include "FlashVariables.h"


extern SPIFLASH SpiFlash;

typedef void (*listener)(uint32_t block, size_t blocks);

class SpiBlockDevice: public BlockDevice {
public:
  SpiBlockDevice(uint32_t blockCount, FlashVariables<512> *flashVariables): blockCount(blockCount), flashVariables(flashVariables) {};

  bool begin();
  bool end();
  
  uint32_t getBlockCount() {
        return blockCount;
    }

  bool readBlocks(uint32_t block, uint8_t* dst, size_t blocks);
  bool writeBlocks(uint32_t block, uint8_t* src, size_t blocks);

  void setReadListener(listener l) {readListener = l;};
  void setWriteListener(listener l) {writeListener = l;};

private:
  uint32_t blockCount;
  FlashVariables<512> *flashVariables;
  listener readListener = NULL;
  listener writeListener = NULL;
  
};

#endif /* __SPIBLOCKDEVICE_H__ */
