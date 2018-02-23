/****************************************Copyright (c)**************************************************                         
**--------------File Info-------------------------------------------------------------------------------
** File name:			LCDConf.c
** Descriptions:		LCD Configuration
**
**------------------------------------------------------------------------------------------------------
** Modified by:			huaweiwx@sina.com
** Modified date:	    2017-08-02
** Version:             17.8.2
** Descriptions:		for arduino halmx
********************************************************************************************************/


#ifndef LCDCONF_H
#define LCDCONF_H

#if __has_include("configs/LCDConfig.h")
# include "configs/LCDConfig.h"    
#endif

#ifndef  LCD_XSIZE
# define LCD_XSIZE          (320)
#endif

#ifndef LCD_YSIZE
# define LCD_YSIZE          (240)
#endif

#ifndef  LCD_CONTROLLER
# define HY32D              (8989)
# define LCD_CONTROLLER     (HY32D)
#endif

#ifndef LCD_BITSPERPIXEL
# define LCD_BITSPERPIXEL   (16)
#endif

#ifndef LCD_FIXEDPALETTE
# define LCD_FIXEDPALETTE   (565)
#endif

#ifndef LCD_SWAP_RB
# define LCD_SWAP_RB        (1)
#endif

#ifndef LCD_SWAP_XY
# define LCD_SWAP_XY        (0)
#endif

#ifndef  LCD_INIT_CONTROLLER
# define LCD_INIT_CONTROLLER()  ili9320_Initializtion()
#endif

#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

