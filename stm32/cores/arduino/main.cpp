/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*GCC tools chain check  add by huaweiwx@sina.com 2017.12.5*/
#if __GNUC__ > 5 || (__GNUC__ == 5 && (__GNUC_MINOR__ > 4 || \
   (__GNUC_MINOR__ == 4 && __GNUC_PATCHLEVEL__ > 0)))

#include <Arduino.h>

#if __has_include("ARDUINOConfig.h") //test user config ARDUINO huawei (huaweiwx@sina.com)
# include "ARDUINOConfig.h"
#endif

#include "USBDevice.h"

// Declared weak in Arduino.h to allow user redefinitions.
int atexit(void (* /*func*/ )()) { return 0; }

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __weak;
void initVariant() { }

void setupUSB() __weak;
void setupUSB() { }

// Weak empty main may be use CubMX main.c source program.
#ifndef USE_USERS_MAINPROGRAM





int main(void)
{

#ifdef STM32F7
    SCB_EnableICache();
    SCB_EnableDCache();
#endif

	init();
	initVariant();

#ifdef STM32F1
 #if defined(MENU_DEBUG_DISABLED)
    __HAL_AFIO_REMAP_SWJ_DISABLE();
 #elif defined(MENU_DEBUG_SWD)
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
 #endif
#endif

#if defined(USB_BASE) || defined(USB_OTG_DEVICE_BASE)

    #ifdef MENU_USB_SERIAL
        USBDeviceFS.beginCDC();
    #elif  MENU_USB_MASS_STORAGE
        USBDeviceFS.beginMSC();
    #elif  MENU_USB_IAD
        USBDeviceFS.beginIDA();/*add by huaweiwx@sina.com 2017.9.15*/
    #endif

#endif

	setup();

	for (;;) {
		loop();
	}

	return 0;
}
#endif
#else
#error "Please update to GCC ver 5-2016q2 https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads"
#endif
