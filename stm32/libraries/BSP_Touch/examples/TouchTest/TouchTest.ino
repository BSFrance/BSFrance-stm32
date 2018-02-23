
#include "BSP_Touch.h"
#include "bsp.h"
#define LED0  LED_BUILTIN
#define LED1  LED1_BUILTIN

TOUCH myTP;

void setup() {
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  Serial.begin(115200);
  myTP.Init(1);
  attachInterrupt(TOUCH_NIRQ, intcallback, GPIO_MODE_IT_FALLING) ;
}

static uint8_t test = 0;
// the loop function runs over and over again forever
void loop() {
  uint16_t x, y;
  if (digitalRead(TOUCH_NIRQ) == 0) {
    myTP.readXY2(&x, &y);
    Serial << "tp x:" << x << "\n";
    Serial << "tp y:" << y << "\n";
  }
  if ( myTP.scan(0)) {
    Serial << "Info:" << _HEX(myTP.status()) << "\n";
    Serial << "x=" << myTP.pdev->x[0] << "\n";
    Serial << "y=" << myTP.pdev->y[0] << "\n";
  }
  if (test > 0) {
    Serial << "int count:" << _HEX(test) << "\n";
  }
  digitalWrite(LED0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED1, LOW);   // turn the LED off by making the voltage LOW
  delay(200);              // wait for 0.2 second
  digitalWrite(LED0, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(800);              // wait for 0.8 second
}

void intcallback(void) {
  test++;
}

