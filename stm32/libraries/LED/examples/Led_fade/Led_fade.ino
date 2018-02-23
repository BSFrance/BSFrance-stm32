/*
 Fade

 This example shows how to fade an LED on pin 9
 using the analogWrite() function.

 The analogWrite() function uses PWM, so if
 you want to change the pin you're using, be
 sure to use another PWM capable pin. On most
 Arduino, the PWM pins are identified with 
 a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

 This example code is in the public domain.
 */
#include <LED.h>   /*LEDClass: Led Led1/2/3/4/5/6/7 has be predefiend */

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
 Serial.begin(115200);  
 Led.Init();
 
 if(Led.availablePwm())
   Serial.println("fade test.");
 else
   Serial.println("no pwm define, use flash");
}

// the loop routine runs over and over again forever:
void loop() {
  if (Led.availablePwm())
    Led.fade(950);
  else
    Led.flash(10, 90, 10);
}
