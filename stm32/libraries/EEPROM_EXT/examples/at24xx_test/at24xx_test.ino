//  at24cxx_rw_wire.ino
//  huaweiwx@sina.com 2017.2.21
#include <exteeprom.h>
EXTEEPROM EEPROM(AT24C02);

#define LED  LED_BUILTIN

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  while (!Serial.available()) { //wait for usb_serial available;
    digitalToggle(LED);
    delay(1000);
  }

  //EEPROM.begin();    //defaul chip addr A2A1A0 = 000;
  //EEPROM.begin(0x03) //A2A1A0 =011
  if (EEPROM.begin())
    Serial << "eeprom init err!  dev:24C " << EEPROM.pdata->dev << "  at adr 0x" << _HEX(EEPROM.pdata->adr) << "\n";
  else {
    Serial << "eeprom init ok!  at adr 0x" << _HEX(EEPROM.pdata->adr) << " is AT24C";
    switch (EEPROM.pdata->dev){
      case AT24C01:
        Serial << "01";
        break;
      case AT24C02:
        Serial << "02";
        break;
      case AT24C04:
        Serial << "04";
        break;
      case AT24C08:
        Serial << "08";
        break;
      case AT24C16:
        Serial << "16";
        break;
      case AT24C32:
        Serial << "32";
        break;
      case AT24C64:
        Serial << "64";
        break;
      case AT24C128:
        Serial << "128";
        break;
      case AT24C256:
        Serial << "256";
        break;
      case AT24C512:
        Serial << "512";
        break;
    }
    Serial << "\n";
  }

 // write test
    for (uint16_t i = 0; i < 16; i++) {
    EEPROM.writeOneByte((uint16_t)i, (uint8_t)(16-i));
    }

  for (uint16_t i = 0; i < 16; i++) {
    for (uint16_t j = 0; j < 16; j++) {
      Serial << _HEX(EEPROM.readOneByte(i * 16 + j)) << " ";
    }
    Serial << "\n";
  }
}

void loop() {
  digitalToggle(LED);
  delay(1000);
  // Insert background code here
}

