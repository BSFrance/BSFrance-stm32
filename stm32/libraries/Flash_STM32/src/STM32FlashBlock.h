#ifndef __STM32FLASHBLOCK_H__
#define __STM32FLASHBLOCK_H__

#if defined(STM32F2)||defined(STM32F4)||defined(STM32F7)  /*add F2 huaweiwx@sina.com 2017.10.12*/
#  include "STM32F4FlashBlock.h"
#  define STM32EmbeddedFlashBlock  STM32F4EmbeddedFlashBlock
#else //F0 1 2 3 L0 1 4
#  include "STM32F1FlashBlock.h"
#  define STM32EmbeddedFlashBlock  STM32F1EmbeddedFlashBlock
#endif

#endif  /*__STM32FLASHBLOCK_H__*/
