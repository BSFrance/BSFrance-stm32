/*
  BB_blink  use stm32 sram/peripheral bitband io opration
  Turns an LED on for one second, then off for one second, repeatedly.
  by huaweiwx@sina.com 2017.10
  F0/L0/F7 un support
*/


#if !(defined(STM32F0)||defined(STM32L0)||defined(STM32F7))
//use bidboud
# include "utils/bitband.h"
#else
# error "BitBand unsuport this mcu!"
#endif

// the setup function runs once when you press reset or power the board
uint32_t ledaddr;  // save LED_BUILTIN bitband addr;
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

//  ledaddr = PIN_OUTADDR(LED_BUILTIN);          //get addr use 
  ledaddr = bb_pin_addr(LED_BUILTIN, OUTPUT);  //get addr use functon
}

// the loop function runs over and over again forever
void loop() { 
  MEM_ADDR(ledaddr) = 1;  /* bitband opration set high*/
  delay(500);
  MEM_ADDR(ledaddr) = 0; /* bitband opration setlow*/
  delay(500);
}
