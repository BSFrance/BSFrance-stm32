#ifndef SDIO_STM32_INCLUDED
#define SDIO_STM32_INCLUDED

#include "SdFat.h"

#if (defined(STM32GENERIC)|| defined(ARDUINO_ARCH_HALMX_RTOS)||defined(ARDUINO_ARCH_MAPLE_STM32F1)\
	 ||defined(ARDUINO_ARCH_MAPLE_STM32F2)||defined(ARDUINO_ARCH_MAPLE_STM32F3)\
	 ||defined(ARDUINO_ARCH_MAPLE_STM32F4)||defined(ARDUINO_ARCH_MAPLE_STM32F7))\
     && (defined(SD_InitTypeDef))

#include "SDIO.h"

SDIOClass sdio;

bool SdioCard::begin() {
    return sdio.begin();
}
bool SdioCard::syncBlocks() {
    return true;
}
bool SdioCard::readBlock(uint32_t block, uint8_t* dst) {
    return sdio.readBlocks(block, dst, 1);
};
bool SdioCard::writeBlock(uint32_t block, const uint8_t* src) {
    return sdio.writeBlocks(block, src, 1);
};
bool SdioCard::readBlocks(uint32_t block, uint8_t* dst, size_t nb) {
    return sdio.readBlocks(block, dst, nb);
};
bool SdioCard::writeBlocks(uint32_t block, const uint8_t* src, size_t nb) {
    return sdio.writeBlocks(block, src, nb);
};
bool SdioCard::erase(uint32_t firstBlock, uint32_t lastBlock) {
    return sdio.erase(firstBlock, lastBlock);
};
bool SdioCard::readCID(void* cid){
    return sdio.readCID(cid);
};
bool SdioCard::readCSD(void* csd){
    return sdio.readCSD(csd);
};
uint32_t SdioCard::cardSize(){
    return sdio.cardSize();
};
uint8_t SdioCard::errorCode() {
      return sdio.errorCode();
}
uint32_t SdioCard::errorData() {
  return sdio.errorData();
}

#endif

#endif