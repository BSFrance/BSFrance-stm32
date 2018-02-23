/*
  key_test.ino
  This example code is in the public domain.
   by huaweiwx@sina.com 25-001-2017 
*/

#include <Keys.h>  
 
#ifdef KEY_LIST
	BOTTONClass myKeys(KEY_LIST);  //KEY_LIST defined in variant.h
#else
//    BOTTONClass myKeys(mask,botton1,botton2,.....)
    BOTTONClass myKeys(0x01,PA0,PE3,PE4);  //PA0 PE3 PE4 as key for stm32f407 V11 & YL136
#endif

#define LED LED_BUILTIN

// the setup function runs once when you press reset or power the board
void setup() {

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
  myKeys.Init();
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  uint8_t k = myKeys.scan();
  switch (k) {
    case KEY_UNPRESED:
      break;
    case KEY0:
      Serial.println("Key0 pressed!");
      break;
    case KEY1:
      Serial.println("Key1 pressed!");
      break;
    case KEY2:
      Serial.println("Key2 pressed!");
      break;
    case KEY3:
      Serial.println("Key3 pressed!");
      break;
    case KEY4:
      Serial.println("Key4 pressed!");
      break;
    case KEY5:
      Serial.println("Key5 pressed!");
      break;
    case KEY6:
      Serial.println("Key6 pressed!");
      break;
    case KEY7:
      Serial.println("Key7 pressed!");
      break;
    default:
      Serial.print("Multi keys pressed!");
      Serial.println(k, HEX);
      break;
  }
  digitalToggle(LED);   // toggle the LED on/off
  delay(500);              // wait for 0.5 second
}
