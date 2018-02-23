/*
  utils.c utility for arduino huaweiwx@sina.com 2017.11.2
*/
#include <Arduino.h>
#include "ctype.h"  /*isdigit isxdigit*/
/*----------------------------------------------------------------------------*/
uint8_t UTIL_toDigit(char x) {
  if (x >= '0' && x <= '9') return x - '0';
  if (x >= 'a' && x <= 'f') return x - 'a' + 10;
  if (x >= 'A' && x <= 'F') return x - 'A' + 10;
  return 0;
}

int32_t UTIL_num(char * str) {
  return atol(str);
}

uint8_t isHex(char* str) {
  if (str[0] == '0')
    if ((str[1] == 'x') || (str[1] == 'X')) return 1;
  return 0;
}

uint32_t UTIL_hexNum(char* str) {
  int32_t xval = 0;

  for (uint8_t i = 2; i < 10; i++) //i=2 skip  0x or 0X
  {
    if (isxdigit(str[i])) {
      uint8_t val = UTIL_toDigit(str[i]);
      xval = xval * 16 + val;
    } else {
      break;
    }
  }
  return xval;
}

uint32_t UTIL_getNum(char* str) {
  return ((isHex(str) == 0) ? UTIL_num(str) : UTIL_hexNum(str));
}

//convert pinmask to num
//in GPIO_PIN_12
//return 12
uint8_t bitMaskPos(uint32_t mask){
	if(mask) return (__builtin_ffs(mask)-1); 
	return 0xff;
}

uint32_t UTIL_Str2PortPin(char* str) {
    if (str[0] != 'P') return 0xffff; 

    stm32_port_pin_type port_pin;;

    #ifdef GPIOA	
    if (str[1] == 'A') port_pin.port = GPIOA;
    #endif
	
    #ifdef GPIOB
    if (str[1] == 'B') port_pin.port = GPIOB;
    #endif
    #ifdef GPIOC
    if (str[1] == 'C') port_pin.port = GPIOC;
    #endif
    #ifdef GPIOD
    if (str[1] == 'D') port_pin.port = GPIOD;
    #endif
    #ifdef GPIOE
     if (str[1] == 'E') port_pin.port = GPIOE;
   #endif
    #ifdef GPIOF
    if (str[1] == 'F') port_pin.port = GPIOF;
    #endif
    #ifdef GPIOG
    if (str[1] == 'G') port_pin.port = GPIOG;
    #endif
    #ifdef GPIOH
    if (str[1] == 'H') port_pin.port = GPIOH;
    #endif
    #ifdef GPIOI
    if (str[1] == 'I') port_pin.port = GPIOI;
    #endif
    #ifdef GPIOJ
    if (str[1] == 'J') port_pin.port = GPIOJ;
    #endif
    #ifdef GPIOK
    if (str[1] == 'K') port_pin.port = GPIOK;
    #endif
	if(!isdigit(str[2])) return 0xffff;
	
	uint16_t pin = str[2]-'0';
	if(isdigit(str[3])) pin= pin*10+str[3]-'0';
	
	port_pin.pinMask = 1U << pin;
	
    for(uint8_t i=0; i<sizeof(variant_pin_list)/sizeof(variant_pin_list[0]); i++) {
        if ((port_pin.port == variant_pin_list[i].port)&&
		    (port_pin.pinMask == variant_pin_list[i].pinMask))
			return i;
    }
	return 0xffff;
}
