/*
  uCGUI_TEST.ino  a uCGUI sample skatch  by huaweiwx@sina.com 2017.8.5

  use BSP_TFTLCD & uCGUI391 libraries
  tested OK: ili9320/9325/ili9341 lcd fsmc drivers
*/
#include <uCGUI.h>

void GUIDEMO_Circle(void);
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  GUI_Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalToggle(LED_BUILTIN);
  GUIDEMO_Circle();
  delay(1000);
}

