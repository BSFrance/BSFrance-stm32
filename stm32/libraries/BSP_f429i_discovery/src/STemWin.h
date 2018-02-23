//STemWin for ARDUINO 
//huaweiwx<huaweiwx@sina.com>

#ifndef __STEMWIN_ARDUINO_H__
#define __STEMWIN_ARDUINO_H__
#include <STemWin532.h>
#ifdef __cplusplus

/* class */
class STemWin : public emWinGUI{
  public:
    STemWin(){};
int Init(void){
	BSP_SDRAM_Init();	     /* Initializes the SDRAM device */
	BSP_TS_Init(240, 320);   /* Initialize the Touch screen */
//	BSP_LCD_Init();
	__HAL_RCC_CRC_CLK_ENABLE();/* Enable the CRC Module */
	return GUI_Init();
    };
};

#endif //__cplusplus
#endif //__STEMWIN_ARDUINO_H__
