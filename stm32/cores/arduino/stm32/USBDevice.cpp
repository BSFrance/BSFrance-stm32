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

#include "Arduino.h"

#if defined(USB_BASE) || defined(USB_OTG_DEVICE_BASE)

#include "USBDevice.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#include "cdc/usbd_cdc.h"
#include "cdc/usbd_cdc_if.h"

#include "msc/usbd_msc.h"
#include "usb_device.h"

void USBDeviceClass::reenumerate() {
    /* Re-enumerate the USB */
      volatile unsigned int i;

    #ifdef USB_DISC_PIN
        pinMode(USB_DISC_PIN, OUTPUT);
	      #ifdef USB_DISC_LOW                 //add by huaweiwx@sina.com 2017.6.18
            digitalWrite(USB_DISC_PIN, LOW);  //for HASEE_V3
	      #else
            digitalWrite(USB_DISC_PIN, HIGH); //for ArmFly/RedBull
	      #endif
        for(i=0;i<1512;i++);
	      #ifdef USB_DISC_LOW
            digitalWrite(USB_DISC_PIN, HIGH);
	      #else
            digitalWrite(USB_DISC_PIN, LOW);
	      #endif
	      for(i=0;i<512;i++);
    #else
      //pinMode(USBDP_PIN, OUTPUT);
      //digitalWrite(USBDP_PIN, LOW);
      //for(i=0;i<512;i++);
      //digitalWrite(USBDP_PIN, HIGH);
      pinMode(PA12, OUTPUT);
      digitalWrite(PA12, LOW);
      //HAL_Delay(1000);
      for(i=0;i<1512;i++){};
      pinMode(PA12, INPUT);
      //digitalWrite(PA12, HIGH);
      //HAL_Delay(1000);
      for(i=0;i<512;i++){};
    #endif
}

#ifdef MENU_USB_IAD  /*huaweiwx@sina.com 2017.9.15 add*/
bool USBDeviceClass::beginIDA() {

    reenumerate();
    USBD_Init(&hUsbDeviceFS, &MSC_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_COMPOSITE);
 //   USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_MSC_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
    return true;
}
#elif defined(MENU_USB_SERIAL)
bool USBDeviceClass::beginCDC() {

    reenumerate();
    USBD_Init(&hUsbDeviceFS, &CDC_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
    USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_CDC_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
    return true;
}
#elif defined(MENU_USB_MASS_STORAGE)
bool USBDeviceClass::beginMSC() {

    reenumerate();
    USBD_Init(&hUsbDeviceFS, &MSC_Desc, DEVICE_FS);
    USBD_RegisterClass(&hUsbDeviceFS, &USBD_MSC);
    USBD_MSC_RegisterStorage(&hUsbDeviceFS, &USBD_MSC_Interface_fops_FS);
    USBD_Start(&hUsbDeviceFS);
    return true;
}
#endif

bool USBDeviceClass::end() {
    return USBD_DeInit(&hUsbDeviceFS);
}

extern PCD_HandleTypeDef hpcd_USB_FS;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

#if defined(STM32L0)||defined(STM32F0)  /*L0/F0 huaweiwx@sina.com 2017.9.24*/
extern "C" void USB_IRQHandler(void) {
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
#elif defined(STM32L1) /*L1 huaweiwx@sina.com 2017.9.24*/
extern "C" void USB_LP_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
#elif defined(STM32F3)
extern "C" void USB_LP_CAN_RX0_IRQHandler(void) {
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
#else  //F1
extern "C" void USB_LP_CAN1_RX0_IRQHandler(void) {
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
#endif

//F105/F107/F2/F4/F7/L4
extern "C" void OTG_FS_IRQHandler(void) {
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

USBD_HandleTypeDef hUsbDeviceFS;
USBDeviceClass USBDeviceFS;

#endif
