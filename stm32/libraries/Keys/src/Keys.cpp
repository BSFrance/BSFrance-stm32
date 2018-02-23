#include <Arduino.h>
#include "Keys.h"

//typedef struct botton_led_type {
//    uint8 pins[16];             /**< pin max 8*/
//    uint8 nr;             /**< keys num. */
//    uint16 on;             /**< on state, 1 high or 0 low on. */
//    uint16_t mode;
//} botton_led_type;

BOTTONClass::BOTTONClass() {
  pdata->nr = 0;
  pdata->on = 0;
  pdata->mode = 0
}

BOTTONClass::BOTTONClass (uint16_t mask,uint8_t k0,uint8_t k1, uint8_t k2, uint8_t k3,
                            uint8_t k4, uint8_t k5, uint8_t k6, uint8_t k7,
                            uint8_t k8, uint8_t k9, uint8_t k10,uint8_t k11,
						    uint8_t k12,uint8_t k13,uint8_t k14,uint8_t k15)
{
  uint8_t nr = 0;	
  pdata->on = mask;
  pdata->pins[0] = k0;
  pdata->pins[1] = k1;
  pdata->pins[2] = k2;
  pdata->pins[3] = k3;
  pdata->pins[4] = k4;
  pdata->pins[5] = k5;
  pdata->pins[6] = k6;
  pdata->pins[7] = k7;
  
  pdata->pins[8] = k8;
  pdata->pins[9] = k9;
  pdata->pins[10] = k10;
  pdata->pins[11] = k11;
  pdata->pins[12] = k12;
  pdata->pins[13] = k13;
  pdata->pins[14] = k14;
  pdata->pins[15] = k15;
  for (uint8_t i = 0; i < 16; i++) {
	if(pdata->pins[i] < 0xff){   
 	  nr++;
	}  
  }
  pdata->nr = nr;
}

void BOTTONClass::setKey(uint8_t pin,uint8_t on,uint8_t mode){
  uint8_t nr = pdata->nr;
  uint8_t mask = on? 0x01:0x0;
  uint8_t mode = mode? 0x01:0x0;
  if(nr <16)
  {
	pdata->pins[nr] = pin;
    pdata->on |= mask << nr;
	pdata->nr +=1;
	pdata->mode |=mode << nr;
  }
}

void  BOTTONClass::Init()
{
  for (uint8_t i = 0; i < 16; i++) {
	if(pdata->pins[i] < 0xff){   
      if (bitRead(pdata->on, i)) {
         pinMode(pdata->pins[i], INPUT_PULLDOWN);
      } else {
         pinMode(pdata->pins[i], INPUT_PULLUP);
      };
	}  
  }
}


uint8_t BOTTONClass::getKey(uint8_t key) {
  uint8_t rtn = 0;
  if (key <(pdata->nr)){
      rtn = digitalRead(pdata->pins[key]);
      if (bitRead(pdata->on, key))
         return (rtn) ? 1: 0;
      else
         return (rtn) ? 0 : 1;
  }
  return 0;
}

uint16_t BOTTONClass::scan(uint8_t mode)
{
  static uint8_t key_up = 1;
  uint16_t keypress = 0;

  if (mode) key_up = 1;
  for (uint8_t i = 0; i < pdata->nr; i++)
    if (getKey(i)) bitSet(keypress, i);

  if (keypress) {
    return  keypress;
  }
  key_up = 0;
  return 0;
}

