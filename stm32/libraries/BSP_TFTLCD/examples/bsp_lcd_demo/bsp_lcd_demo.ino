/** bsp_lcd_arduino_demo.ino  TFTLCD for halmx demo
    test board:   红牛 RedBull 2.0(stm32f103zet6)
                  安福莱 Armfly X2  (stm32f103vet6)
                  BLACK_F407VE(YL136 stm32f407vet6)
                  神舟III(stm32f103zet6)
         TFT:     ILI9320
                  ILI9341
                  SPFD5420
    huaweiwx   <huaweiwx@sina.com>    2017.7.12
*/

/* lib include */

#include <BSP_TFTLCD.h>

#define LED1 LED_BUILTIN
void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  /* LCD Initialization */
  LCDx_Init();
  lcddev.BackColor = BLUE;
  LCD_Clear(BLUE);
  
  /*setup color & fonts*/
  lcddev.TextColor = RED;

  /*draw & show text*/
  BSP_LCD_DrawLine(0, 0, 239, 319);
  BSP_LCD_DrawLine(0, 319, 239,0 );
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalToggle(LED1);
  delay(500);
}
