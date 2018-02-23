/**  spiflash.ino  spiflash test for this board:
      DISCOVERY_F746NG
      DISCOVERY_F429ZI
      ARMFLY_F407ZG
      HASEE_III_F103ZE
      ILLUMINATI_F407ZG
      REDBULL_V2_F103ZE
      WAVESHARE_F746IG
*/
#include <SpiFlash.h>

#define led1 LED_BUILTIN

/* usage: 1 SPIFLASH  myFlash;        use spiconfig file define parameter
          2 SPIFLASH  myFlash(SPI1);  use SPI1
          3 SPIFLASH  myFlash(MOSI,MISO, SCK);  use SPI pindef
 * */
SPIFLASH  myFlash;
void setup() {
  pinMode(led1, OUTPUT);
  Serial.begin(115200);
  while (!Serial.available()); //wait for usb_serial input a char; 

  Serial.println("SPIFLASH ID:");
  delay(100);
  //  myFlash.begin(PA4); //PA4 as  spiflash cs_pin
  myFlash.begin();      //default spiflash cs_pin
  uint32_t id = myFlash.readID();
  Serial << "DeviceID:" << _HEX(id) <<"\n";
  Serial << "capacity:" << myFlash.capacity()/1024 <<" k\n";

  uint8_t buf[256];
  myFlash.read(buf, 0, 256);
  for (uint16_t i = 0; i < 256; i++) {
    Serial << _HEX(buf[i]) << "  ";
  }
  Serial.println("\nread ok!");
}

void loop() {
  digitalToggle(led1);
  delay(1000);
}


