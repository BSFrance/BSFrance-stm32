#include <DS18B20.h>
#include "bsp.h"
// OneWire DS18S20 Temperature Example
//Armfly_X2 使用PB3 编译生成时需要设置调试SWD释放 PB3

DS18B20Class ds;
void setup(void) {
  Serial.begin(115200);
  ds.setPin(DS18B20_DQ_PIN);
  if(ds.Init())
    for(;;){
      Serial << "ds18b20 not found\n" ;
	  delay(2000);
	}

}

void loop(void) {
  Serial << "Temperature" << ds.getTemp() <<"\n";
  delay(1000);
}
