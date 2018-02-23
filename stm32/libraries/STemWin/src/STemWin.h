#ifndef __STEMWIN_ARDUINO_H__
#define __STEMWIN_ARDUINO_H__

#ifdef __cplusplus
/* class */
#include <BSP_Drivers.h>
#include <STemWin532.h>

class STemWin : public emWinGUI
{
  public:
    STemWin(){};
	  
  int Init(void){
    BSP_LCD_Init();
	__HAL_RCC_CRC_CLK_ENABLE();/* Enable the CRC Module */
	return GUI_Init();
  };
	
  private:

};

#endif //__cplusplus
#endif //__STEMWIN_ARDUINO_H__