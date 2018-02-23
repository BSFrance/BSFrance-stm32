/*
  Template_FreeRTOS90.ino running in FreeRTOS V9.0.
  This example code is in the public domain.
  modified for STM32 HALMX  by huaweiwx@sina.com , May 2 2017
*/

#include <FreeRTOS.h>

static void myTask1(void  __attribute__ ((unused)) *argument)
{
  /*Task1 setup BEGIN*/
  
  /*Task1 setup END*/
  /* Infinite loop */
   for (;;)
  {
  /* USER CODE BEGIN Task1 */
  
  
 /* USER CODE END Task1 */
   vTaskDelay(1000);    //set delay 1s 
  }
}


static void myTask2(void __attribute__ ((unused)) *argument)
{
  /*Task2 setup BEGIN*/
  
  /*Task2 setup END*/
  /* Infinite loop */
  for (;;)
  {
  /* USER CODE BEGIN Task2 */
 
  /* USER CODE END Task2 */
   vTaskDelay(1000);    //set delay 1s
  }
}

// the setup function runs once when you press reset or power the board.
void setup() {
  xTaskCreate(myTask1,
              NULL,
              configMINIMAL_STACK_SIZE,
              NULL,
              3,
              NULL);
  xTaskCreate(myTask2,
              NULL,
              configMINIMAL_STACK_SIZE,
              NULL,
              3,
              NULL);
  vTaskStartScheduler();  //FreeRTOS start and never return!
}

// the loop function runs over and over again forever
void loop() {
  /*
    !!! this no runed in FreeRTOS  !!!
  */
}
