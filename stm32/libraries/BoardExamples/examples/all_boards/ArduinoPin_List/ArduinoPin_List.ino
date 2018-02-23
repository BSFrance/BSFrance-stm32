/*
  ArduinoPin_List
  Arduino pin and LED list
  modified 8 Sep 2017
  by huaweiwx@sina.com
  
  stm32PinName(pin):   see core/arduino/stm32/stm32_debug.c
  
*/

#include <LED.h> /*use LED lib LEDS_LIST define*/

void print_list(void){
  Serial << "\nPIN list:\n";
  for (uint8_t i = 0; i < sizeof(variant_pin_list) / sizeof(variant_pin_list[0]); i++)
     Serial << stm32PinName(i)<< " "  << i  << "\n";
 
  uint8_t leds[] = {LEDS_LIST};
  Serial << "\nLED list:";
  for (uint8_t i = 0; i < sizeof(leds) / sizeof(leds[0]); i++)
    Serial << "\nLED" << i << "_BUILTIN  " <<  stm32PinName(leds[i]);

#ifdef KEYS_LIST
  uint8_t keys[] = {KEYS_LIST};
  Serial << "\n\nKEYS list:";
  for (uint8_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++)
    Serial << "\nBUILTIN" << i << "  " << stm32PinName(keys[i]);
#endif

#ifdef MOSI	
  Serial << "\n\nSPI pinlist:";
  Serial << "\nMOSI:" <<  stm32PinName(MOSI);
  Serial << "\nMISO:" <<  stm32PinName(MISO);
  Serial << "\nSCK :" <<  stm32PinName(SCK);
  Serial << "\nSS  :" <<  stm32PinName(SS);
#endif

#ifdef SDA  
  Serial << "\n\nI2C pinlist:\n";
  Serial << "\nSDA:" <<  stm32PinName(SDA);
  Serial << "\nSLC:" <<  stm32PinName(SCL);
#endif
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); 
  print_list();
}

// the loop function runs over and over again forever
void loop() {
  if(Serial.available()){
	char inChar = (char)Serial.read();
	print_list();
  }
  digitalToggle(LED_BUILTIN);   // turn the LED on/off
  delay(500);                   // wait for a second
}
