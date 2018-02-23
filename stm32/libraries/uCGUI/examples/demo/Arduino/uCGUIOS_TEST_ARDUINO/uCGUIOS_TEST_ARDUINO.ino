/*
  uCGUIOS_TEST.ino  a uCGUI sample skatch  by huaweiwx@sina.com 2017.8.5
  use BSP_TFTLCD & uCGUI391 libraries
  tested OK: ili9320/9325/ili9341/ili9486/spdf5420  lcd fsmc drivers
*/

#include <uCGUI_OS.h>

#ifdef  LED_BUILTIN
# define LED    LED_BUILTIN
# define LED_ON bitRead(LED_BUILTIN_MASK,0)
#endif

uCGUI  GUI;
static void myTask1(void  __attribute__ ((unused)) *argument)
{
  /*Task setup*/
  pinMode(LED, OUTPUT);

  Serial.begin(115200);
  uint32_t i = 0;
  Serial << "Count:" << _HEX(i) << " in myTask1\n";

  /* USER CODE BEGIN Task1 */
  /* Infinite loop */
  for (;;)
  {
    digitalWrite(LED, LED_ON); // turn the LED on (HIGH is the voltage level)
    vTaskDelay(50);       // wait for a second
    digitalToggle(LED);     // turn the LED off by making the voltage LOW
    vTaskDelay(950);  // wait for a second
    i++;
    Serial << "Count:" << _HEX(i) << " in myTask1\n";
  }
  /* USER CODE END Task1 */
}

#define BK_COLOR GUI_BLUE
void Taskloop(void __attribute__ ((unused)) *argument) {
  // put your main code here, to run repeatedly:
  GUI.Init();
  int XSize = LCD_GetXSize(); //if gui config xy swap must change X to Y
  int YSize = LCD_GetYSize(); //if gui config xy swap must change Y to X
  
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
    vTaskDelay(500);
  }
}

void setup() {
  // put your setup code here, to run once:
  xTaskCreate(myTask1,
              NULL,
              configMINIMAL_STACK_SIZE,
              NULL,
              3,
              NULL);
  xTaskCreate(Taskloop,
              NULL,
              configMINIMAL_STACK_SIZE,
              NULL,
              3,
              NULL);
  vTaskStartScheduler();
  // osKernelStart(); //FreeRTOS start and never return!
}

// the loop function runs over and over again forever
void loop() {
  /*
    !!! this no runed in FreeRTOS  !!!
  */
}
