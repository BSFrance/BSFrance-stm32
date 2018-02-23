#ifndef __STEMWIN_ARDUINO_H__
#define __STEMWIN_ARDUINO_H__

#ifdef __cplusplus
#include <FreeRTOS.h>
/* class */
#include <STemWin532.h>
#include "STemWin_Config/LCD/BSP_tft_lcd.h"
#include "STemWin_Config/TOUCH/touch.h"

class STemWin : public emWinGUI
{
  public:
      STemWin(){};
	  
int Init(void){
 	   BSP_LCD_Init();
	   return GUI_Init();
	};
	
  private:

};

#endif //__cplusplus
#endif //__STEMWIN_ARDUINO_H__