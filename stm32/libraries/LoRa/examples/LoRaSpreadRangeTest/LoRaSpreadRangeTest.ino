#include <SPI.h>              // include libraries
#include <LoRa.h>

// uncomment the section corresponding to your board
// BSFrance 2017 contact@bsrance.fr 

//  //LoR32u4 433MHz V1.0 (white board)
//  #define SCK     15
//  #define MISO    14
//  #define MOSI    16
//  #define SS      1
//  #define RST     4
//  #define DI0     7
//  #define BAND    433E6 
//  #define PABOOST true

//  //LoR32u4 433MHz V1.2 (white board)
//  #define SCK     15
//  #define MISO    14
//  #define MOSI    16
//  #define SS      8
//  #define RST     4
//  #define DI0     7
//  #define BAND    433E6 
//  #define PABOOST true 

//LoR32u4II 868MHz or 915MHz (black board)
#define SCK     15
#define MISO    14
#define MOSI    16
#define SS      8
#define RST     4
#define DI0     7
#define BAND    868E6  // 915E6
#define PABOOST true 

//// LoRaGateway module B 
//#define SCK        14
//#define MISO       12
//#define MOSI       13
//#define SS         D4 //GPIO16
//#define RST        15
//#define DI0        0
//#define BAND       868E6 
//#define PABOOST    false

//// LoRaM0 ports
//#define SCK     PB11
//#define MISO    PB10
//#define MOSI    PA7
//#define SS      8
//#define RST     4
//#define DI0     3
//#define BAND    868E6 
//#define PABOOST    false

//// LoRaM3-OLED ports
//#define SCK     PA5
//#define MISO    PA6
//#define MOSI    PA7
//#define SS      PA4
//#define RST     PA2
//#define DI0     PA3
//#define BAND    868E6 
//#define PABOOST    false

//// LoRa32s OLED
//#define SCK     5    
//#define MISO    19   
//#define MOSI    27   
//#define SS      18   
//#define RST     17   
//#define DI0     21  
//#define BAND      868E6
//#define PABOOST   false

////MiniDK = RA8S
//#define SCK      14 //D5
//#define MISO     12 //D6
//#define MOSI     13 //D7
//#define SS       0  //D3
//#define RST      3  //D4
//#define DI0      8
//#define BAND     868E6 
//#define PABOOST  false

byte msgCount = 0;            // count of outgoing messages
int interval = 2000;          // interval between sends
long lastSendTime = 0;        // time of last packet send
int rssi,snr;

void setup() {
  Serial.begin(9600);                   // initialize serial
  //while (!Serial);
  //SPI.begin(SCK,MISO,MOSI,SS);
  Serial.println("LoRa Duplex - Set spreading factor");
  LoRa.setPins(SS,RST,DI0); // set CS, reset, IRQ pin
 pinMode(13, OUTPUT);
  if (!LoRa.begin(BAND,PABOOST)) {            
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  LoRa.setSpreadingFactor(8);           // ranges from 6-12,default 7 see API docs
  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    //String message = "       ";   // send a message
    char buff[10];
    sprintf(buff, "%-5d%-5d", rssi,snr);
    //message += rssi;
    //message += " ";
    //message += snr;
    //message += msgCount;
    sendMessage(buff);
    //Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    interval = 3000;//random(2000) + 1000;    // 2-3 seconds
    //msgCount++;
  }
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  
  if (packetSize == 0) return;          
  String incoming = "";
  digitalWrite(13, HIGH);
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  Serial.print(incoming);
  Serial.print("\t"); 
  rssi = LoRa.packetRssi();
  snr = LoRa.packetSnr();
  Serial.print(String(rssi));
  Serial.print("  ");
  Serial.println(String(snr));
  digitalWrite(13, LOW);
}

