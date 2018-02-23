/*
  uCGUI_TEST.ino  a uCGUI sample skatch  by huaweiwx@sina.com 2017.8.5
  use BSP_TFTLCD & uCGUI391 libraries
  tested OK: ili9320/9325/ili9341 lcd fsmc drivers
*/

#include <uCGUI.h>
uCGUI  GUI;

#define BK_COLOR GUI_BLUE
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  GUI.Init();
  
  int XSize = LCD_GetXSize();
  int YSize = LCD_GetYSize();
   
  GUI.SetBkColor(BK_COLOR);
  GUI.SetFont(&GUI_Font8x16);
  GUI.Clear();
  
  GUI.SetDrawMode(0);
  GUI.SetColor(~BK_COLOR);
  GUI.FillCircle (XSize / 2, YSize / 2, 50);
  
  GUI.SetColor(GUI_RED);
  GUI.DrawLine(0,0,XSize,YSize); //line
  GUI.DrawLine(0,YSize,XSize,0);
  
  GUI.SetColor(GUI_WHITE);
  GUI.DispStringAt("HELLO",XSize/2-8*3,30);  //text
  
  GUI.SetColor(~BK_COLOR);
  GUI.SetDrawMode(GUI_DM_XOR);
  for (;;) {
    GUI.FillCircle (XSize / 2, YSize / 2, 60);
    digitalToggle(LED_BUILTIN);
    delay(500);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
