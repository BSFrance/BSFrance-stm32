/**  nor_test_arduino.ino   arduino style nor_test tested for this board:
      HASEE_III_F103ZE
      REDBULL_V2_F103ZE
   Allocate 256k ram, and write to Serial the results
*/

#include <NorFlash.h>
#include "bsp.h"

#define led1 LED_BUILTIN
NORFLASH myNOR;
void setup() {
  NOR_IDTypeDef  nor_id;
  nor_id.Manufacturer_Code=0;
  pinMode(led1, OUTPUT);
  Serial.begin(115200);
  Serial.println("NOR FLASH ID:");
  delay(100);
  uint8_t status;
  status = myNOR.Init();
 if (status != NOR_STATUS_OK) Serial.println("init err");
//  myNOR.ReturnToReadMode();
  myNOR.readID(&nor_id);
  myNOR.returnToReadMode();

 
  Serial.print(nor_id.Manufacturer_Code, HEX);
  Serial.print(" ");
  Serial.print(nor_id.Device_Code1, HEX);
  Serial.print(" ");
  Serial.print(nor_id.Device_Code2, HEX);
  Serial.print(" ");
  Serial.println(nor_id.Device_Code3, HEX);

  if ( ( nor_id.Manufacturer_Code == 0x00BF ) && (nor_id.Device_Code1 == 0x2782))
  {
    Serial.println("Type = 39VF160");
  }
  else if ( ( nor_id.Manufacturer_Code == 0x00BF ) && (nor_id.Device_Code1 == 0x234B))
  {
    Serial.println("Type = 39VF1601");
  }
  else if ( ( nor_id.Manufacturer_Code == 0x0001 ) && (nor_id.Device_Code1 == 0x227E) &&
       ( nor_id.Device_Code2 == 0x2212 ) && ( nor_id.Device_Code3 == 0x2200 ) )
  {
    Serial.println("Type = AM29LV128ML");
  }
  else if ( ( nor_id.Manufacturer_Code == 0x0001 ) && ( nor_id.Device_Code1 == 0x227E ) &&
            ( nor_id.Device_Code2 == 0x2221 ) && ( nor_id.Device_Code3 == 0x2201 ) )
  {
    Serial.println("Type = S29GL128P");
  }
  else if ( ( nor_id.Manufacturer_Code == 0x0020 ) && ( nor_id.Device_Code1 == 0x227E ) &&
            ( nor_id.Device_Code2 == 0x2220 ) && ( nor_id.Device_Code3 == 0x2200 ) )
  {
    Serial.println("Type = M29DW128F");
  }
  else
  {
    Serial.println("Type = Unknow");
  }

  uint16_t data;
  uint32_t len = 16;

  for (uint16_t i = 0; i < len; i++) {
    myNOR.readData(i, &data, 1) ;
    Serial.print(data, HEX);
    Serial.print(" ");
  }
  Serial.println("\nread ok!\n");
  pinMode(led1, OUTPUT);
}

void loop() {
  digitalToggle(led1);
  delay(1000);
}

