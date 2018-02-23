/*
 * Copyright (c) 2015  huawei huaweiwx@wxmedi.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


#include "Arduino.h"
#include "stm32_gpio_af.h"
#include CHIP_PERIPHERAL_INCLUDE
/*
T* pTable :
stm32_af_pin_list;
stm32_tim_pin_list;

*/
template<typename T>
T*  stm32SearchPin(GPIO_TypeDef *port,uint32_t pinMask,  T* pTable,uint8_t len){
    for(uint8_t  i=0; i<len ; i++) {
        if (pTable[i].port == port && pTable[i].pinMask == pinMask) {
            return &pTable[i];
        }
    }
  return NULL;	
};

#ifdef STM32GENERIC
template<typename T>
T*  stm32SearchPin(uint8_t pin,  T* pTable, uint8_t len){
  if (pin >= NUM_DIGITAL_PINS) return NULL;
  
  stm32_port_pin_type port_pin = variant_pin_list[pin];
  return stm32SearchPin(port_pin.port,port_pin.pinMask,pTable,len);	
};

#ifdef TIM2
# define pwwtable chip_tim2
#else
# define pwmtable chip_tim3	
#endif

bool stm32PinTim(uint8_t pin){
  stm32_port_pin_type port_pin = variant_pin_list[pin];
  return (stm32SearchPin(port_pin.port,port_pin.pinMask,pwwtable, sizeof(pwwtable) / sizeof(pwwtable[0])))?true:false;	
}

#endif