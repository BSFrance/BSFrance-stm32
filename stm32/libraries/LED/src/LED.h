/*
  LED.h - LED library head file
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

#ifndef __LEDLIB_h__
#define __LEDLIB_h__

#ifdef LED7_BUILTIN
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN,LED2_BUILTIN,LED3_BUILTIN,LED4_BUILTIN,LED5_BUILTIN,LED6_BUILTIN,LED7_BUILTIN
#elif defined(LED6_BUILTIN)
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN,LED2_BUILTIN,LED3_BUILTIN,LED4_BUILTIN,LED5_BUILTIN,LED6_BUILTIN
#elif defined(LED5_BUILTIN)
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN,LED2_BUILTIN,LED3_BUILTIN,LED4_BUILTIN,LED5_BUILTIN
#elif defined(LED4_BUILTIN)
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN,LED2_BUILTIN,LED3_BUILTIN,LED4_BUILTIN
#elif defined(LED3_BUILTIN)
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN,LED2_BUILTIN,LED3_BUILTIN
#elif defined(LED2_BUILTIN)
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN,LED2_BUILTIN
#elif defined(LED1_BUILTIN)
#define LEDS_LIST LED_BUILTIN,LED1_BUILTIN
#elif defined(LED_BUILTIN)
#define LEDS_LIST LED_BUILTIN
#else
#warrning  "unstandard arduino board "	
#define LEDS_LIST 13	
#endif

#define LED_FADEAMOUNT 5
#define LED_FADETIME   25

typedef struct {
    uint8_t  pin;
    uint8_t  on;
    uint8_t off;
    uint8_t status;
} LED_TypeDef;

#ifdef __cplusplus
extern "C"{
#endif

void LEDFlash(uint8_t pin,uint16_t timeon,uint16_t timeoff,uint8_t cnt,uint8_t on);  //add by huawei 2015.3

#ifdef __cplusplus
} //extern "C"{

class LEDClass
{
 public:
	LEDClass(uint8_t pin=LED_BUILTIN, uint8_t on=LOW);
	void Init(void);
    void setPin(uint8_t pin, uint8_t on=LOW);
	void on(void);
//analogWrite
    void on(int val);
	
#ifdef STM32GENERIC
//analogWrite
    void pwm(int val, int frequency=1000, int durationMillis=0);
    bool availablePwm(void);
#else
	bool availablePwm(void){return true;};
#endif
	void off(void);
	void toggle(void);
	void flash(uint16_t timeon,uint16_t timeoff,uint8_t cnt=1);
	void fade(uint16_t time = LED_FADETIME);
	LED_TypeDef* pdata = &sLed;
  private:
    LED_TypeDef  sLed;
	int brightness=0;    // how bright the LED is
    int fadeAmount = LED_FADEAMOUNT;    // how many points to fade the LED by
	int fadeTime = LED_FADETIME;
};

#ifdef LED_BUILTIN
extern LEDClass Led;
extern LEDClass LEDs[];
#endif

#ifdef LED1_BUILTIN
extern LEDClass Led1;
#endif

#ifdef LED2_BUILTIN
extern LEDClass Led2;
#endif

#ifdef LED3_BUILTIN
extern LEDClass Led3;
#endif

#ifdef LED4_BUILTIN
extern LEDClass Led4;
#endif

#ifdef LED5_BUILTIN
extern LEDClass Led5;
#endif

#ifdef LED6_BUILTIN
extern LEDClass Led6;
#endif

#ifdef LED7_BUILTIN
extern LEDClass Led7;
#endif


#endif /*__cplusplus*/
#endif /*__LEDLIB_h__*/

