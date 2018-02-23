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

#include "bsp.h"    //defined SRAM_LENTH in this file

#define led LED_BUILTIN
void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(115200);
}
#ifndef SRAM_LENTH
#define SRAM_LENTH (512*1024)
#endif

uint32_t size = SRAM_LENTH - 8  /* malloc overhead*/;

extern "C" void setHeapAtSram(void);
void loop() {
  setHeapAtSram();
  Serial.println("===========");
  Serial.println("Allocating 1024kB buffer");
    uint8_t *data = (uint8_t*)malloc(size);
    if (data == NULL) {
        Serial.println("Malloc failed!");
        delay(2000);
        return;
    }

    Serial.print("Buffer address: ");
    Serial.println((uint32_t)data, HEX);

    uint32_t start = millis();

    uint32_t sum = 0;
    uint32_t sum_check = 0;

    for(int i=0; i<size; i++) {
        data[i] = i & 0xff;
        sum += i & 0xff;
    }

    Serial.print("Write time: ");
    Serial.print(millis() - start);
    Serial.println(" ms");
    start = millis();

    for(int i=0; i<size; i++) {
        sum_check += data[i];
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

    free(data);

    delay(5000);
}

