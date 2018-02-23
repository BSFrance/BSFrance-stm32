#include "BSP_f429i_discovery.h"
#include "STemWin.h"                  //STemWin 5.32 lib

STemWin GUI;
extern "C" void MainTask(void);
void setup() {
  GUI.Init();
   /* Activate the use of memory device feature */
  WM_SetCreateFlags(WM_CF_MEMDEV);
  MainTask();
}

void loop() {
  // put your main code here, to run repeatedly:
}

