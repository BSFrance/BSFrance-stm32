/*
  Copyright (c) 2017 Daniel Fekete

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef _SERIAL_USBDEVICE_H_INCLUDED
#define _SERIAL_USBDEVICE_H_INCLUDED

#include "Arduino.h"

#include "msc/usbd_msc.h"

#ifdef MENU_USB_IAD  /*huaweiwx@sina.com 2017.9.15 add*/
#  include "usbd_composite.h"
#endif
extern USBD_StorageTypeDef USBD_MSC_Interface_fops_FS;

class USBDeviceClass {
  public:
    void reenumerate();
#ifdef MENU_USB_IAD  /*huaweiwx@sina.com 2017.9.15 add*/
    bool beginIDA();
#elif defined(MENU_USB_SERIAL)
    bool beginCDC();
#elif defined(MENU_USB_MASS_STORAGE)
    bool beginMSC();
#endif	
    bool end();
};

extern USBDeviceClass USBDeviceFS;


#endif
