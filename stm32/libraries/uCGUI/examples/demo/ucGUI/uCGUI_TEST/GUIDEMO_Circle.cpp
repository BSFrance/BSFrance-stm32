/**********************************************************************************
                                               uC/GUI
        Universal graphic software for embedded applications
           (c) Copyright 2002, Micrium Inc., Weston, FL
           (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
        uC/GUI is protected by international copyright laws. Knowledge of the
        source code may not be used to write a similar product. This file may
        only be used in accordance with a license and should not be redistributed
        in any way. We appreciate your understanding and fairness.
  --------------------------------------
  File        : GUIDEMO_Circle.c
  Purpose     : Drawing circles
  --------------------------------------
*/
#include <Arduino.h>
#include <uCGUI.h>

/********************************
 *        GUIDEMO_Circle        *
*********************************
*/

void GUIDEMO_Circle(void) {
  int XSize = LCD_GetXSize();
  int YSize = LCD_GetYSize();
  GUI_Clear();
  GUI_SetFont(&GUI_Font8x16);
  GUI_SetColor(GUI_WHITE);
  GUI_FillCircle (XSize / 2, YSize / 2, 50);
  GUI_SetDrawMode(GUI_DM_XOR);
  delay(500);
  GUI_FillCircle (XSize / 2, YSize / 2, 60);
  GUI_SetDrawMode(0);
}

