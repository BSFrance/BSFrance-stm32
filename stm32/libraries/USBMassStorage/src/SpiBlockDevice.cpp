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

#include "SpiBlockDevice.h"
#include "MassStorage.h"

SPIFLASH SpiFlash;

bool SpiBlockDevice::begin() {
	SpiFlash.begin();    //move to setup() init
    return true;
};

bool SpiBlockDevice::end() {
    return true;
};


bool SpiBlockDevice::readBlocks(uint32_t block, uint8_t* dst, size_t blocks) {
    for(uint32_t offset = 0; offset < blocks; offset++) {
        flashVariables->read(block + offset, dst + offset * 512);
    }
//   SPIFLASH.read(dst,block*512,blocks*512);   //读取flash
   return true;
};

bool SpiBlockDevice::writeBlocks(uint32_t block, uint8_t* src, size_t blocks) {

    for(uint32_t offset = 0; offset < blocks; offset++) {
       flashVariables->write(block + offset, src + offset * 512);
   }
//   SPIFLASH.write(src,block*512,blocks*512);   //读取flash
   return true;
};



