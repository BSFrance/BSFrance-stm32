/*

 Serial Port usage and naming example.
 
 Most of the STM32 microcontrollers have more than one serial port.

 The STM32GENERIC Arduino framwork allows you to access this ports.

 The naming is

 Serial       = SerialUSB  this is equal to SerialUSB the virtual serial USB port
 SerialUART1               physical serial interface   
 SerialUART2               physical serial interface
 ...                       ...

 The SerialUARTx are in the data sheets sometimes called USARTx.
 Check the datasheed of your on board microcontroller to which pins this USARTS are connected.
 
 ! Do it in advance before programming the routine to avoid software induced short cuts !
 
 April 2017, ChrisMicro
 
 */

//#define Serial SerialUART1 
//#define Serial SerialUART2

void setup() 
{
  Serial.begin(115200);
  while (!Serial.available()); //wait for usb_serial input a char; 
  Serial.println("Serial Port Example");
  Serial.println("just counting up");
}

int Counter=0;

void loop() 
{
  Serial.println(Counter); 
  Counter++;
  delay(1000);
}

