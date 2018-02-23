/*
  LED.cpp - LED library
  huaweiwx@sina.com 2017.  All right reserved.

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
#include <arduino.h>
#include "LED.h"


#ifdef STM32GENERIC
 #include "stm32_search_pin.h"
 
 #if __has_include(<FreeRTOS.h>)   
 #	include <FreeRTOS.h>
 #	define  LED_DELAY(x) vTaskDelay(x)
 #else
 #  define  LED_DELAY(x) delay(x)		 
 #endif
#else
 #define  LED_DELAY(x) delay(x)	
#endif


extern "C"{	
  void LEDFlash(uint8_t pin,uint16_t timeon,uint16_t timeoff,uint8_t cnt,uint8_t on){
	for(uint8_t i=cnt;i>0;i--){
		  digitalWrite(pin,on);
		if(timeon)
		  LED_DELAY(timeon);
	  
		  digitalWrite(pin,(on?LOW:HIGH));
		if(timeoff)
		  LED_DELAY(timeoff);
	}
 }
}  //extern "C"{


void LEDClass::setPin(uint8_t pin, uint8_t on){
	pdata->pin = pin;
	pdata->on =  on?HIGH:LOW;
	pdata->off = on?LOW:HIGH;
	pdata->status = pdata->off;
	this->brightness = 0;    // how bright the LED is
    this->fadeAmount = 5;    // how many points to fade the LED by
}

LEDClass::LEDClass(uint8_t pin, uint8_t on){
    this->setPin(pin, on);

}

void LEDClass::Init(void){
	pinMode(pdata->pin,OUTPUT);
}

//digitalWrite
void LEDClass::on(void){
	digitalWrite(pdata->pin,pdata->on);
	pdata->status =pdata->on;
}

//analogWrite
void LEDClass::on(int val){
    analogWrite(pdata->pin, val);
}

#ifdef STM32GENERIC
//analogWrite
void LEDClass::pwm(int val, int frequency, int durationMillis){
    pwmWrite(pdata->pin, val, frequency, durationMillis);
}

bool LEDClass:: availablePwm(void){
	return stm32PinTim(pdata->pin);
};
#endif

void LEDClass::off(void){
	digitalWrite(pdata->pin,pdata->off);
	pdata->status = pdata->off;
}

void LEDClass::toggle(void){
	if(pdata->status == pdata->off) this->on();
	else this->off();
	
}

void LEDClass::flash(uint16_t timeon,uint16_t timeoff,uint8_t cnt){
	LEDFlash(pdata->pin, timeon,timeoff,cnt,pdata->on);
	pdata->status = pdata->off;
};

void LEDClass::fade(uint16_t time) {
  if(time < this->fadeTime) time = this->fadeTime;	
  for(uint16_t i =0; i< (time/this->fadeTime);i++){
    analogWrite(pdata->pin, this->brightness);

  // change the brightness for next time through the loop:
    this->brightness = this->brightness + this->fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
    if (this->brightness <= 0 || this->brightness >= 255) {
      this->fadeAmount = - this->fadeAmount;
    }
    delay(this->fadeTime);
  }
}


#ifdef LED_BUILTIN_MASK
# define LED_MASK LED_BUILTIN_MASK
#elif defined(STM32_LED_BUILTIN_ACTIVE_LOW)
# define LED_MASK 0
#else
# define LED_MASK 0xff
#endif

#ifdef LED_BUILTIN
LEDClass Led(LED_BUILTIN, bitRead(LED_MASK,0));
#endif

#ifdef LED1_BUILTIN
LEDClass Led1(LED1_BUILTIN, bitRead(LED_MASK,1));
#endif

#ifdef LED2_BUILTIN
LEDClass Led2(LED2_BUILTIN, bitRead(LED_MASK,2));
#endif

#ifdef LED3_BUILTIN
LEDClass Led3(LED3_BUILTIN, bitRead(LED_MASK,3));
#endif

#ifdef LED4_BUILTIN
LEDClass Led4(LED4_BUILTIN, bitRead(LED_MASK,4));
#endif

#ifdef LED5_BUILTIN
LEDClass Led5(LED5_BUILTIN, bitRead(LED_MASK,5));
#endif

#ifdef LED6_BUILTIN
LEDClass Led6(LED6_BUILTIN, bitRead(LED_MASK,6));
#endif

#ifdef LED7_BUILTIN
LEDClass Led7(LED7_BUILTIN, bitRead(LED_MASK,7));
#endif

#ifdef LED7_BUILTIN
LEDClass LEDs[]={
Led,Led1,Led2,Led3,Led4,Led5,Led6,Led7,	
};
#elif defined(LED6_BUILTIN)
LEDClass LEDs[]={
Led,Led1,Led2,Led3,Led4,Led5,Led6	
};
#elif defined(LED5_BUILTIN)
LEDClass LEDs[]={
Led,Led1,Led2,Led3,Led4,Led5	
};
#elif defined(LED4_BUILTIN)
LEDClass LEDs[]={
Led,Led1,Led2,Led3,Led4,
};
#elif defined(LED3_BUILTIN)
LEDClass LEDs[]={
Led,Led1,Led2,Led3,
};
#elif defined(LED2_BUILTIN)
LEDClass LEDs[]={
Led,Led1,Led2,
};
#elif defined(LED1_BUILTIN)
LEDClass LEDs[]={
Led,Led1
};
#else
LEDClass LEDs[]={
Led,
};
#endif

