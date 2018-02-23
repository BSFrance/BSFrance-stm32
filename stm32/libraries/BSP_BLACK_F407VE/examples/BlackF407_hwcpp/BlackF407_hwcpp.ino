#include <StdPeriphLib_F4.h>
#include <BSP_V11_YL136_F4.h>

/*
   Stm32f4xx StdPeriphLib for arduino_maple  by huaweiwx@sina.com  2016.8.11
*/

#ifndef BOARD_F407V
//# error the hwcpptest sketch was for STM32F407_V11 board!
#endif

#define LED0  LED_BUILTIN
#define LED1  LED1_BUILTIN
SDCARD SDcard;

TFTLCD TFT;
TOUCH  TP;

#ifdef HAVE_24CXX  //HAVE_24CXX defined in boarddef.h  YL136 board not 24cxx 
EXTEEPROM EEPROM;
#endif

#ifdef  HAVE_SPIFLASH
SPIFLASH myspiflash;
#endif

#pragma GCC diagnostic ignored "-Wwrite-strings"
void setup() {
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
//  Serial1.begin(115200);

#ifdef HAVE_24CXX
  EEPROM.Init();
#endif

  disk_initialize(EX_FLASH);
  TFT.Init();
  //  NRF24L01_Init();
  //    sprintf((char*)lcd_id,"LCD ID:%04X",lcddev.id); //将LCD ID打印到lcd_id数组。
//  Serial1.print("LCD ID: ");
//  Serial1.println(TFT.devInfo()->id, HEX); //9320

  TP.Init();
#if 0     //触屏校正,运行一次，修改touch.h 校正参数
  TP.adjust();
#endif
  TFT.displayOn();
  TFT.setColor(RED);
  TFT.clear(WHITE);
#ifdef  ARDUINO_YL136VET6
  TFT.showString(20, 30, 200, 16, 16, "STM32F407VET6_YL136 TEST:");
#elif defined(ARDUINO_V11VET6)
  TFT.showString(20, 30, 200, 16, 16, "STM32F407VET6_V11 TEST:");
#else
  TFT.showString(20, 30, 200, 16, 16, "STM32F407VET6 TEST:");
#endif
  TFT.showString(20, 50, 200, 16, 16, "hello bbs.mydigit.cn");
  TFT.showString(20, 70, 200, 16, 16, "com1 output 115200bps");
  TFT.showString(20, 90, 200, 16, 16, "by huawei 04/08/2017");
  TFT.setColor(BLUE); //设置字体为蓝色
  if (SDcard.Init()) //检测不到SD卡
  {
    TFT.showString(20, 110, 200, 16, 16, "SD Card Error!");
//    Serial1.println("SD Card Error!");
  } else {
    TFT.showString(20, 110, 200, 16, 16, "SD Card  OK!");
    switch (SDCardInfo.CardType)
    {
      case SDIO_STD_CAPACITY_SD_CARD_V1_1:
        TFT.showString(20, 130, 200, 16, 16, "Card Type:SDSC V1.1");
        Serial1.println("Card Type:SDSC V1.1");
        break;
      case SDIO_STD_CAPACITY_SD_CARD_V2_0:
        TFT.showString(20, 130, 200, 16, 16, "Card Type:SDSC V2.0");
        Serial1.println("Card Type:SDSC V2.0");
        break;
      case SDIO_HIGH_CAPACITY_SD_CARD:
        TFT.showString(20, 130, 200, 16, 16, "Card Type:SDHC V2.0");
//        Serial1.println("Card Type:SDHC V2.0");
        break;
      case SDIO_MULTIMEDIA_CARD:
        TFT.showString(20, 130, 200, 16, 16, "Card Type:NNC Card");
//        Serial1.println("Card Type:MMC Card");
        break;
    }
    TFT.showString(20, 150, 200, 16, 16, "SD Card Size:     MB");
    TFT.showNum(20 + 13 * 8, 150, SDCardInfo.CardCapacity >> 20, 5, 16); //显示SD卡容量

//    Serial1.print("SD Card ok! ManufacturerID: 0x"); Serial1.println(SDCardInfo.SD_cid.ManufacturerID, HEX); //制造商ID
//    Serial1.print("Card RCA: "); Serial1.println(SDCardInfo.RCA, DEC);            //卡相对地址
//    Serial1.print("Card Capacity: "); Serial1.println((uint32_t)(SDCardInfo.CardCapacity >> 20), DEC ); //显示容量
//    Serial1.print("Card BlockSize: "); Serial1.println(SDCardInfo.CardBlockSize, DEC);  //显示块大小
  }

#ifdef  HAVE_SPIFLASH
  TFT.showString (20, 170, 200, 16, 16, "SPI FLASH ID:");
  TFT.showNum(20 + 15 * 8, 170, SpiFlash.chipType(), 5, 16);
//  Serial1.print("SPI FLASH ID=");
//  Serial1.println(SpiFlash.chipType(), HEX);
#endif

#ifdef HAVE_24CXX
  for (uint16_t i = 0; i < 256; i++) {
    Serial1.print(EEPROM.readOneByte(i), HEX);
    Serial1.print(" ");
  }
  Serial1.println();
#endif

  myAdcInit();
  Serial1.print("Temperature: ");
  Serial1.println(getTemperature(), DEC);
  TFT.showString(20, 190, 200, 16, 16, "Temperature:  00.00C");
}

// the loop function runs over and over again forever
void loop() {
  static uint16_t i = 0;
  uint16_t temp;
  temp = getTemperature();
  TFT.setColor( RED);
  TFT.showxNum(20 + 11 * 8,  190, temp / 100, 2, 16, 0); //Temperature
  TFT.showxNum(20 + 14 * 8,  190, temp % 100, 2, 16, 0); //Temperature
  TFT.showxNum(20 + 18 * 8,  190, i++,      5, 16, 0);
//  Serial1.println(temp, DEC);
#if 0      //触屏校正输出，写到touch.h
  Serial1 << "tp x:" << (TP.tpInfo()->xfac * 100000000) << " " << (TP.tpInfo()->xoff) << "\n";
  Serial1 << "tp y:" << (TP.tpInfo()->yfac * 100000000) << " " << (TP.tpInfo()->yoff) << "\n";
  Serial1 << "tp type:" << (TP.tpInfo()->touchtype) << "\n";
#endif
  digitalWrite(LED0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED1, LOW);   // turn the LED off by making the voltage LOW
  delay(200);              // wait for 0.2 second
  digitalWrite(LED0, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(800);              // wait for 0.8 second
}
