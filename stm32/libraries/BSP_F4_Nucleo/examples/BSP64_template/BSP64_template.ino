/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
*/

#include "BSP_F4_Nucleo.h"

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  Serial.println(BSP_GetVersion(),DEC);  
  // initialize the BSP  LED2
  BSP_LED_Init(LED2);
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
}

// the loop routine runs over and over again forever:
void loop() {
  BSP_LED_Toggle(LED2);
  delay(500);
  BSP_LED_Toggle(LED2);
  delay(500);               // wait for a second
}

uint8_t ubKeyPressed;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 if (GPIO_Pin == KEY_BUTTON_PIN)
 {
   ubKeyPressed = SET;
 }
}
