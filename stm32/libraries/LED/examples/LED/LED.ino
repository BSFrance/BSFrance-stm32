#include <LED.h>

//Led Led1 Led2 is predefined in LED.h

void setup() {
  //  put your setup code here, to run once:
  Serial.begin(115200);
  Led.Init();

#ifdef LED1_BUILTIN
  Led1.Init();
#endif

#ifdef LED2_BUILTIN
  Led2.Init();
#endif
  Serial.println("I'm going to sleep.......");
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Led.availablePwm())
    Led.fade(950);
  else
    Led.flash(10, 90, 10);

#ifdef LED1_BUILTIN
  Led1.flash(10, 0, 1);
#endif

#ifdef LED2_BUILTIN
  Led2.flash(10, 320, 3);
#endif
}