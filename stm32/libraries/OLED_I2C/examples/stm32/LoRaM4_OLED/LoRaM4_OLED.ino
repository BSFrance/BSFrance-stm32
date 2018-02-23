#include <LED.h>
#include <OLED_I2C.h>
#define  STM32F303CC
//OLED12864/32 
//OLED:   CS  RST  DC  D1(SDA)  D0(SCL)   VCC    GND
//STM32:  -   PA3  -    PA1      PA2     3.3/5V  0V
//ZION    -   23        49       50

//OLED_12832  myOLED(SDA, SCL, 8);   //use hwi2c
//OLED_12864  myOLED(SDA, SCL, 8);   //use hwi2c
//OLED_12832  myOLED(PC5, PC4, PC1); //use soft i2c  12832
//OLED_12864  myOLED(PC5,PC4,PC1);   //use soft i2c  12864

//pb5:rst 
//pb6:scl  
//pb7:sda 

 OLED_12864  myOLED(PB7,PB6,PB5);   //use soft i2c  12864

extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

void setup()
{
//  ADC_TempSensorEn();
  Led.Init();
  Led.flash(10, 330, 3);
  myOLED.begin();
  Led.flash(10, 490, 2);
  myOLED.setFont(SmallFont);
  myOLED.printxy("LoRaM4-2.4", CENTER, 0);
  myOLED.printxy("Internal Temp Sensor", CENTER,  8);
  myOLED.printNumF(24.8,2,0,34);
  myOLED.setFont(MediumNumbers);
}

void loop()
{
  Led.flash(500, 500, 1);
  myOLED.printNumF(88.8, 2, CENTER, 16);
  myOLED.update();
}


