/**
  ******************************************************************************
  * @file           : USB_COMPOSITE  
  * @version        : v17.9_Cube
  * @brief          : This file implements the USB Device 
  ******************************************************************************
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  * modify by  huaweiwx@sina.com 2017.9.15 
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_COMPOSITE_H
#define __USBD_COMPOSITE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "msc/usbd_msc.h"
#include  "cdc/usbd_cdc.h"
#include  "cdc/usbd_cdc_if.h"

#define WBVAL(x) (x & 0xFF),((x >> 8) & 0xFF)
#define DBVAL(x) (x & 0xFF),((x >> 8) & 0xFF),((x >> 16) & 0xFF),((x >> 24) & 0xFF)

#define USBD_IAD_DESC_SIZE           0x08
#define USBD_IAD_DESCRIPTOR_TYPE     0x0B

#define USBD_CDC_FIRST_INTERFACE     0          /* CDC FirstInterface */
#define USBD_CDC_INTERFACE_NUM       2          /* CDC Interface NUM */

#define USBD_CDC_CMD_INTERFACE       0
#define USBD_CDC_DATA_INTERFACE      1

#define USBD_MSC_FIRST_INTERFACE     2          /* MSC FirstInterface */
#define USBD_MSC_INTERFACE_NUM       1          /* MSC Interface NUM */

#define USBD_MSC_INTERFACE           2


#define MSC_INDATA_NUM              (MSC_EPIN_ADDR & 0x0F)
#define MSC_OUTDATA_NUM             (MSC_EPOUT_ADDR & 0x0F)

#define CDC_INDATA_NUM              (CDC_IN_EP & 0x0F)
#define CDC_OUTDATA_NUM             (CDC_OUT_EP & 0x0F)
#define CDC_OUTCMD_NUM              (CDC_CMD_EP & 0x0F)

#define USBD_COMPOSITE_DESC_SIZE    (9  + 58 + 8 +  32 + 8)


extern USBD_ClassTypeDef    USBD_COMPOSITE;

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USBD_MSC_H */
/**
  * @}
  */

/*****************************END OF FILE****/
