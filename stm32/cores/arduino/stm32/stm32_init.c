/*
  Copyright (c) 2017 Daniel Fekete

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
#include "stm32_def.h"
#include "Arduino.h"
#include "usb_device.h"

//RTC_HandleTypeDef RtcHandle;
uint8_t enterDfu=0, flagSet=0, rstSrc=4;
#define addr2                 (0x20000000+0x8fff) //0x9FFb
//#define BootFlagAddr          (0x20001F000-8) //bootloader 0x157F32D4 @0x20004FFFC
//#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)
#define dfuROMFlag      			(*(uint32_t *)0x20009FFC)
#define dfuFlag      			    (*(uint32_t *)0x20004FF2) // 16KB/20KB SRAM STM32L151C8 STM32F103CB
#define rstFlag      			    (*(uint32_t *)0x20004FEA) // 40KB SRAM STM32F303CC

extern void SystemClock_Config(void);
void init_pc13(void){

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  //__HAL_RCC_GPIOF_CLK_ENABLE();
  //__HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
void blinkpc13(int time, int repeats){

  int i=0;
	while (i=0,i<repeats,i++){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(time);
	}
}
void checkRstSource(){

    if ( __HAL_RCC_GET_FLAG(PWR_FLAG_SB))     rstSrc = 6;// standby resume
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) rstSrc = 5; // soft
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) rstSrc = 3;// power-on
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) rstSrc = 4; // Always set, test other cases first
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) rstSrc = 2;// watchdog
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) rstSrc = 1;// window watchdog
    if ( __HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) rstSrc = 0;// low power
    //if ( __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET) // brow out  F4 Usually set with POR
    __HAL_RCC_CLEAR_RESET_FLAGS(); // The flags cleared after use
}
void checkDfuFlag(){

  // #if defined(STM32L1) || defined(STM32F1)
  //
  // extern volatile uint32_t _blrequest;
  // if (_blrequest != 0xDEADBEEF){
  //   flagSet=1;
  // }
  // else{
	// 	flagSet=0;
	// }
  //
	// if (*((uint32_t *)BootFlagAddr) == 0x157F32D4){
	// 	*((uint32_t *)BootFlagAddr) = 0x0;
	// 	 flagSet=1;
	// }
	// else {
	// 	*((uint32_t *)BootFlagAddr) = 0x157F32D4;
	// 	 flagSet=0;
	// }
  //
  // #endif

  #ifdef STM32F3
  if (*((uint32_t *)addr2) == 0xDEADBEEF){
    *((uint32_t *)addr2) = 0x0;
     flagSet=1;
  }
  else {
    *((uint32_t *)addr2) = 0xDEADBEEF;
     flagSet=0;
  }
  #endif
}
void DfuSlector(){

  if(rstSrc == 5){ return; } // jump to user code
	if(rstSrc == 4){
		if(flagSet){
			flagSet = 0;
			enterDfu = 1;
			systemResetToBootloader();
		}
		else {
			enterDfu = 0;
		}
	}
  HAL_Delay(500);
  if(rstSrc == 4){
    *((uint32_t *)addr2) = 0;
  }
}
void usb_de_enumerate(){

  __disable_irq();
  volatile unsigned int delay;
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
  for(delay=0;delay<1512;delay++); //HAL_Delay(10);
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  for(delay=0;delay<512;delay++);//HAL_Delay(10);
  __enable_irq();
}
void usb_disable(void){

   PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)hUsbDeviceFS.pData;
   if (hpcd != NULL)
   {
      HAL_PCD_DevDisconnect(hpcd);
      USBD_DeInit(&hUsbDeviceFS);
   }
}
void systemResetToBootloader(void){

  #if defined(STM32L1)
      (*(uint32_t *)0x200027F8) = 0x157F32D4;
  #endif

  #if defined(STM32F1)
	    //dfuFlag = 0x157F32D4;
      (*(uint32_t *)0x20004FF8) = 0x157F32D4; //0x20004FF8
  #endif

  #if defined(STM32F3)
      //dfuROMFlag = 0xDEADBEEF;
      (*(uint32_t *)0x20009FFC) = 0xDEADBEEF;
  #endif
	__DSB();
	usb_disable();
	usb_de_enumerate();
	NVIC_SystemReset();
}
uint8_t getRstSrc(void){

	  uint8_t rstSrc = 255;

    if ( __HAL_RCC_GET_FLAG(PWR_FLAG_SB)) rstSrc = 6;// standby resume
    else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) rstSrc = 5;// soft
    else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) rstSrc = 3;// power-on
    else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST) != RESET) rstSrc = 2;// watchdog
    else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) rstSrc = 1;// window watchdog
    else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST) != RESET) rstSrc = 0;// low power
    else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) rstSrc = 4; // Always set, test other cases first
    //else if ( __HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET) // brow out  F4 Usually set with POR
    __HAL_RCC_CLEAR_RESET_FLAGS();
    return rstSrc;
}
void handleRst(void){

	if(getRstSrc() == 4){
		if(rstFlag == 0xB5F88B5F){
			dfuFlag = 0x157F32D4;
			rstFlag = 0;
      blinkpc13(50,4);
			//systemResetToBootloader();
		}
		else{
			//rstFlag = 0xB5F88B5F;
      *((uint32_t *)0x20004FEA) = 0xB5F88B5F;
      blinkpc13(50,2);
		}
		unsigned long us = 4002000;
		while (us--){ __asm volatile("nop"); }
		rstFlag = 0;
		// continue to user app
	}
}
void init(){

  HAL_Init();
  SystemClock_Config();
  usb_de_enumerate();

  //sinit_pc13();
  //handleRst();
  //#if defined(STM32F1) || defined(STM32L1)
  //handleRst();
  //#endif

  #if defined(STM32F3)
	checkRstSource();
	checkDfuFlag();
	DfuSlector();
  #endif

  #ifdef STM32F1
  __HAL_RCC_AFIO_CLK_ENABLE();
  #endif
}
void SysTick_Handler(void) {
  #if __has_include("FreeRTOS.h")  //huawei (huaweiwx@sina.com)
  osSystickHandler();
  #else
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  #endif
}
void Error_Handler() {

}
