
// OLED_I2C_TinyFont_View
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my OLED_I2C library.
//
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// Display:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//           SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//
// Arduino Leonardo:
// ----------------------
// Display:  SDA pin   -> Arduino Digital 2 or the dedicated SDA pin
//           SCL pin   -> Arduino Digital 3 or the dedicated SCL pin
//
// Arduino Mega:
// ----------------------
// Display:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//           SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//
// Arduino Due:
// ----------------------
// Display:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA1 (Digital 70) pin
//           SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL1 (Digital 71) pin
//
// The internal pull-up resistors will be activated when using the
// hardware I2C interfaces.
//
// You can connect the OLED display to any available pin but if you use
// any other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access
// to the pins used, and you will also have to use appropriate, external
// pull-up resistors on the data and clock signals.
//

// add support ssd1306_12832 by huaweiwx@sina.com 2016.11.08
// class OLED_12832 128x32    OLED_12864 128x64
//

#include <LED.h>
#include <OLED_I2C.h>

//OLED12864/32 
//OLED:   CS  RST  DC  D1(SDA)  D0(SCL)   VCC    GND
//STM32:  -   PA3  -    PA1      PA2     3.3/5V  0V
//ZION    -   23        49       50

//OLED_12832  myOLED(SDA, SCL, 8);   //use hwi2c
//OLED_12864  myOLED(SDA, SCL, 8);   //use hwi2c
//OLED_12832  myOLED(PC5, PC4, PC1); //use soft i2c  12832
//OLED_12864  myOLED(PC5,PC4,PC1);   //use soft i2c  12864

#define USE12864

#if defined(USE12864)
 OLED_12864  myOLED(PA1,PA2,PA3);   //use soft i2c  12864
#else 
 OLED_12832  myOLED(PA1,PA2,PA3);   //use soft i2c  12832
#endif 
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
  myOLED.printxy("HELLO MYDIGIT.CN!", CENTER, 0);
  myOLED.printxy("Internal TempSenser", CENTER,  8);
#if defined(USE12864)
    myOLED.printxy("012345678012345678!", CENTER, 34);
    myOLED.printxy("This is oled12864", CENTER,  42);
#endif
  myOLED.setFont(MediumNumbers);
}

void loop()
{
  Led.flash(500, 500, 1);
  myOLED.printNumF(8888, 2, CENTER, 16);
  myOLED.update();
}


