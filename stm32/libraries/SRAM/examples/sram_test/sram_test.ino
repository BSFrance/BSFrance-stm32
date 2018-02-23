/**  sram_test.ino   sram(sdram) test for this board:
      DISCOVERY_F746NG
      DISCOVERY_F429ZI
      ARMFLY_F407ZG
      HASEE_III_F103ZE
      ILLUMINATI_F407ZG
      REDBULL_V2_F103ZE
      WAVESHARE_F746IG
   Allocate 256k ram, and write to Serial the results
*/
#include <Sram.h>

#define led LED_BUILTIN
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
}

uint32_t size = SRAM_LENTH / 2;   /* define in bsp.h*/;
void loop() {
  Serial.print("SRAM SIZE: 0x");
  Serial.println(size * 2, HEX);
  uint32_t start = millis();

  uint32_t sum = 0;
  uint32_t sum_check = 0;
  uint16_t data;
  for (int i = 0; i < size; i++) {
    data = i & 0xffff;
    sum += i & 0xffff;
    BSP_SRAM_WriteData(i * 2, &data, 1) ; //uint16_data, addr*2
  }

  Serial.print("Write time: ");
  Serial.print(millis() - start);
  Serial.println(" ms");
  start = millis();

  for (int i = 0; i < size; i++) {
    BSP_SRAM_ReadData(i * 2, &data, 1) ;
    sum_check += data;
  }

  Serial.print("Read time: ");
  Serial.print(millis() - start);
  Serial.println(" ms");

  Serial.print("Checksum should be:");
  Serial.println(sum);
  Serial.print("Checksum calculated:");
  Serial.println(sum_check);

  if (sum == sum_check) {
    Serial.print("SDRAM is working with buffer size: ");
    Serial.println(size);
  } else {
    Serial.println("SDRAM NOT WORKING !!!");
  }

  delay(5000);
}

