// UTFT Memory Saver
// -----------------
//
// Since most people have only one or possibly two different display modules a lot
// of memory has been wasted to keep support for many unneeded controller chips.
// You now have the option to remove this unneeded code from the library with
// this file.
// By disabling the controllers you don't need you can reduce the memory footprint
// of the library by several Kb.
//
// Uncomment the lines for the displaycontrollers that you don't use to save
// some flash memory by not including the init code for that particular
// controller.

//auto save memory by MACRO BSP_TFT_LCD defined in variant path bsp.h file. huaweiwx@sina.com 2017.6
#if __has_include("bsp.h")
#  include "bsp.h"
#endif

#ifdef BSP_TFT_LCD
#define DISABLE_HX8347A			1	// 0 ITDB32
#if (BSP_TFT_LCD !=ILI9327)
 #define DISABLE_ILI9327		1	// 1 ILI9327
#endif

#define DISABLE_SSD1289			1	// 2 ITDB32S / TFT01_32 / ELEE32_REVA / ELEE32_REVB / CTE32	- This single define will disable both 8bit, 16bit and latched mode for this controller
#define DISABLE_ILI9325D  		1	// 4 ILI9325D_8
#define DISABLE_ILI9325C		1	// 3 ITDB24

#if (BSP_TFT_LCD !=ILI9325)
 #define DISABLE_ILI9325D_ALT 	1	// 5 ILI9325D_16
#endif

#define DISABLE_HX8340B_8 		1	// 6 ITDB22 8bit mode
#define DISABLE_HX8340B_S 		1	// 7 ITDB22 Serial mode

#define DISABLE_HX8352A			1	// 8 ITDB32WD / TFT01_32WD / CTE32W
#define DISABLE_ST7735			1	// 9ITDB18SP
#define DISABLE_PCF8833			1	// 10 LPH9135
#define DISABLE_S1D19122  		1	//11 ITDB25H
#define DISABLE_SSD1963_480		1	//12 ITDB43 / TFT01_43
#define DISABLE_SSD1963_800		1	//13 ITDB50 / TFT01_50 / CTE50 / EHOUSE50
#define DISABLE_S6D1121			1	// 14 ITDB24E	- This single define will disable both 8bit and 16bit mode for this controller

#if (BSP_TFT_LCD !=ILI9341)
 #define DISABLE_ILI9341_16		1	//17 CTE32_R2
#endif
//                                  //18 not in use
//                    SSD1289       //19
#define DISABLE_SSD1963_800_ALT	1	//20 TFT01_70 / CTE70 / EHOUSE70
#if (BSP_TFT_LCD !=ILI9481)
 #define DISABLE_ILI9481		1	//21  CTE32HR
#endif
//define DISABLE_ILI9325D_16ALT	    //22
#define DISABLE_S6D0164			1	//23 CTE22 / DMTFT22102
#define DISABLE_ST7735S			1	//24 TFT01_18SP
#define DISABLE_ILI9341_S5P		1	//25 TFT01_22SP / TFT01_24SP / TFT22SHLD / DMTFT28105

#define DISABLE_ILI9341_S4P		1	//26 MI0283QT9
#define DISABLE_R61581			1	//27 CTE35IPS
#if (BSP_TFT_LCD !=ILI9486)
 #define DISABLE_ILI9486		1	//28  CTE40
#endif
#define DISABLE_CPLD		 	1	//29 CTE50CPLD / CTE70CPLD / EHOUSE50CPLD
#define DISABLE_HX8353C         1	//30 DMTFT18101
#define DISABLE_ST7735_ALT		1	//31 TFT01_18SHLD

#if (BSP_TFT_LCD !=ILI9320)
  #define DISABLE_ILI9320		1	//32 ILI9320
#endif

#endif