// 
//    FILE: dht11.h
// VERSION: 0.4.1
// PURPOSE: DHT11 Temperature & Humidity Sensor library for Arduino
// LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
//
// DATASHEET: http://www.micro4you.com/files/sensor/DHT11.pdf
//
//     URL: http://playground.arduino.cc/Main/DHT11Lib
//
// HISTORY:
// George Hadjikyriacou - Original version
// see dht.cpp file
// 

#ifndef _DHT11_h_
#define _DHT11_h_

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define DHT11LIB_VERSION "0.4.1"

#define DHTLIB_OK				0
#define DHTLIB_ERROR_CHECKSUM	-1
#define DHTLIB_ERROR_TIMEOUT	-2

typedef struct {
	int  pin;
	int  humidity;
	int  temperature;
} DHT11_TypeDef;


class DHT11
{
public:
    DHT11(int pin=0xff);
	void Init(void);
    int  read(void);
	DHT11_TypeDef *pdata =&sDht11;
  private:
    DHT11_TypeDef  sDht11;
};
#endif
//
// END OF FILE
//
