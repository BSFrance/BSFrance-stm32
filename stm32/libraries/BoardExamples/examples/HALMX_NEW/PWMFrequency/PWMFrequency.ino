/*
  PWMFrequency.ino  setPwmFrequency.
  modified for STM32 HALMX   by huaweiwx@sina.com , May 2 2017

  I modify from stm32_PWM.c 
    const uint32_t PWM_FREQUENCY_HZ = 1000;   
  to  
    #define PWM_FREQUENCY_HZ  1000
    static uint32_t pwmFrequecyHz = PWM_FREQUENCY_HZ;
  add this function:	 

  void setPwmFrequency(uint32_t freqHz){
    pwmFrequecyHz = freqHz;
  }

  Now we can use it set PWM frequency exp:
     setPwmFrequency(10000);	//set 10kHZ 
*/

#ifdef  LED_BUILTIN
# define LED    LED_BUILTIN
#	define LED_ON bitRead(LED_BUILTIN_MASK,0)
#else
#	define LED  13    //fixd me
#	define LED_ON 1   //fixd me
#endif


void void setup()
{
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  setPwmFrequency(10000);  //set PWM Frequency as 10kHZ
  analogWrite(LED,0x80);
}

// the loop function runs over and over again forever
void loop() {
  /*
  */
}
