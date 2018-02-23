/*  
  blink all LEDs 

  remark: both leds are acrive low which means writing LOW 
          turns the led on, writing HIGH turns the led off
  
  Black407VE ( STM32F407VE )
  http://wiki.stm32duino.com/images/5/5c/STM32_F4VE_SCHEMATIC.PDF

  May 2017, ChrisMicro
*/

#define LED  LED_BUILTIN     // same as LED_BUILTIN, active low
#define LED1 LED1_BUILTIN     // active low

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(LED1, OUTPUT);
  
  digitalWrite(LED, HIGH);  // led off, led is active low
  digitalWrite(LED1, HIGH); // led off, led is active low
}

void loop()
{
    digitalWrite(LED, LOW); // led on
    delay(1000);
    digitalWrite(LED, HIGH);// led off
    
    digitalWrite(LED1, LOW); // led on
    delay(1000);
    digitalWrite(LED1, HIGH); // led off
}
