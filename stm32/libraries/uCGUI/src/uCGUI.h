#ifndef __UCGUI_H__
#define __UCGUI_H__
#include <BSP_TFTLCD.h>
#include <uCGUI391.h>
#ifdef __cplusplus

extern "C" LCD_DrvTypeDef*  pTFTLCD;
class uCGUI : public uCGUIClass
{
  public:
      uCGUI(){};
	  void setDrv(LCD_DrvTypeDef* pDrv){ pTFTLCD = pDrv;};
      int Init(void){
	     return GUI_Init();
	  };
  private:
};
#endif //__cplusplus
#endif //__UCGUI_ARDUINO_H__