#include <LED.h>

void setup() {
  //  put your setup code here, to run once:
  for(uint8_t i=0;i<sizeof(LEDs)/sizeof(LEDs[0];i++)
     LEDs[i].Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  for(uint8_t i=0;i<i<sizeof(LEDs)/sizeof(LEDs[0];i++){
     LEDs[i].flash(10, 240, i+1);
     delay(1000);
  }
}
