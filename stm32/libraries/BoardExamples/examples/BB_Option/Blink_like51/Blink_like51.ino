/*
  Blink_like51  use stm32 sram/peripheral bitband io opration 
  Turns an LED on for one second, then off for one second, repeatedly.
  by huaweiwx@sina.com 2017.10
*/

#if !(defined(STM32F0)||defined(STM32L0)||defined(STM32F7))
//use bidboud
# include "utils/bitband.h"
#else
# error "BitBand unsuport this mcu!"
#endif

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT); 
}

#define LED PDout(2)   /*LED_BUILTIN on PD2*/
// the loop function runs over and over again forever
void loop() { 
  LED = 1;  /* bitband opration set high*/
  delay(500);
  LED = 0;  /* bitband opration setlow*/
  delay(500);
}
