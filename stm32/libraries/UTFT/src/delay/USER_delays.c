#include <Arduino.h>

// 不精确，用于软件中延时；
#if defined(STM32GENERIC)||defined(ARDUINO_ARCH_MAPLE_STM32F1)||\
    defined(ARDUINO_ARCH_MAPLE_STM32F3)||defined(ARDUINO_ARCH_MAPLE_STM32F4)||\
    defined(ARDUINO_ARCH_MAPLE_STM32F0)||defined(ARDUINO_ARCH_MAPLE_STM32F7)||\
	defined(ARDUINO_ARCH_F4_OTTO)||defined(ARDUINO_ARCH_KODUINO)||defined(ARDUINO_ARCH_HALMX_RTOS)
	#define USER_DELAY_MUL  (F_CPU/1000000/10)

#elif defined(ARDUINO_ARCH_EFM32)||defined(ARDUINO_ARCH_NUVOTON)||defined(ARDUINO_ARCH_NUVOTON_M051)||\
	  defined(ARDUINO_ARCH_NUVOTON_MINI51)||defined(ARDUINO_ARCH_NXP)||defined(ARDUINO_ARCH_TEENSY3)||defined(ARDUINO_ARCH_TI_LM3S)
	#define USER_DELAY_MUL  (F_CPU/1000000/10)
#else
	#error  "undef board,please fixed me!!"
#endif


void USER_delay_us(uint32_t us)
{
  volatile uint32_t nCount =us*USER_DELAY_MUL;	
  for(; nCount != 0; nCount--);
}

void USER_delay_ms(uint32_t ms)
{
  volatile uint32_t nCount = ms;	
  for(; nCount != 0; nCount--){
	  USER_delay_us(935);
  }
}


























