/*
   SLLogicLogger - Simple logic analyser for Stellaris Launchpad
   SLLogicLogger supports the SUMP protocol over UART. The UART is mapped
   through the debug/flash controller on the Stellaris Launchpad to a virtual
   com port. It samples signals on PORTB with samplerate 10 Mhz, buffersize
   16kByte, sampling starts at any change on PORTB[0..7].

   PB0 and PB1 are limited to 3.6 V! All other pins are 5 V tolerant.
   The idea for this simple analyser comes from http://jjmz.free.fr/?p=148

   Description of the SUMP protocol:
   - http://www.sump.org/projects/analyzer/protocol/
   - http://dangerousprototypes.com/docs/The_Logic_Sniffer%27s_extended_SUMP_protocol

   A multiplatform client which supports the SUMP protocol is OLS:
   http://www.lxtreme.nl/ols/
   To install device support for OLS, copy ols.profile* file into the
   ols/plugins folder.

   Copyright (C) 2012 Thomas Fischl <tfischl@gmx.de> http://www.fischl.de
   Last update: 2012-12-24

   for gd32f103ret6 120M f_cpu samples signals on PORTA(0..7) with samplerate 23 Mhz, buffersize
   60kByte. by huaweiwx@sina.com 2016.6.12
   add samplerate 10M 3M 1M 300K 100K and trigger option  2016.6.15
*/

#ifdef STM32F1
# include "stm32f1xx_ll_cortex.h"
# define DEVICE  "GD32Fv0"
#elif defined(STM32F4)
# include "stm32f4xx_ll_cortex.h"
# define DEVICE  "F407v0"
#define MAX_SAMPLERATE 40000000
#endif

#define VERSION "0.13"

//boards
#define LED  LED_BUILTIN  //LED_BUILTIN

// buffer size
#define MAX_CAPTURE_SIZE 60*1024

uint8_t pin[8] = {PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7};

// SUMP command defintions
#define SUMP_RESET 0x00
#define SUMP_ARM   0x01
#define SUMP_QUERY 0x02
#define SUMP_SELF_TEST 0x03
#define SUMP_GET_METADATA 0x04

#define SUMP_XON   0x11
#define SUMP_XOFF  0x13
#define SUMP_TRIGGER_MASK   0xC0
#define SUMP_TRIGGER_VALUES 0xC1
#define SUMP_TRIGGER_CONFIG 0xC2

/* Most flags (except RLE) are ignored. */
#define SUMP_SET_DIVIDER 0x80
#define SUMP_SET_READ_DELAY_COUNT 0x81
#define SUMP_SET_FLAGS 0x82
#define SUMP_SET_RLE 0x0100

//vars
uint8_t        cmdByte = 0;
uint8_t   trigger_mask = 0;
uint8_t trigger_values = 0;
uint8_t    cmdBytes[5];
uint32_t       divider = 2;
uint8_t       useMicro = 0;
uint8_t     rleEnabled = false;
uint16_t     readCount = MAX_CAPTURE_SIZE;
uint16_t    delayCount = 0;
uint32_t tus;
// sampling buffer
uint8_t  logicdata[65535]  __attribute__((at(0x10000000)));
volatile uint32_t delays;

// send 32bit unsigned integer as SUMP metadata
void sump_sendmeta_uint32(uint8_t cmd, uint32_t i) {
  Serial.write(cmd);
  Serial.write((uint8_t)((i >> 24) & 0xff));
  Serial.write((uint8_t)((i >> 16) & 0xff));
  Serial.write((uint8_t)((i >> 8)  & 0xff));
  Serial.write((uint8_t)(i & 0xff));
}

// send 8bit unsigned integer as SUMP metadata
void sump_sendmeta_uint8(uint8_t cmd, uint8_t i) {
  Serial.write(cmd);
  Serial.write(i);
}

/*
   Extended SUMP commands are 5 bytes.  A command byte followed by 4 bytes
   of options. We already read the command byte, this gets the remaining
   4 bytes of the command.
   If we're debugging we save the received commands in a debug buffer.
   We need to make sure we don't overrun the debug buffer.
*/
void getCmd() {
  for (uint8_t i = 0; i < 4; i++) {
    while (! Serial.available());
     cmdBytes[i] = Serial.read();
  }
}

//100ns delay
//static inline __always_inline void delay100ns(volatile uint32_t nsk) {
//  for (; nsk != 0; nsk--);
//}

/*
   This function calculates what delay we need for the specific sample rate.
   The dividers are based on SUMP's 100Mhz clock.
   For example, a 1MHz sample rate has a divider of 99 (0x63 in the command
   byte).
   rate = clock / (divider + 1)
   rate = 100,000,000 / (99 + 1)
   result is 1,000,000 saying we want a 1MHz sample rate.
   We calculate our inter sample delay from the divider and the delay between
   samples gives us the sample rate per second.
   So for 1MHz, delay = (99 + 1) / 100 which gives us a 1 microsecond delay.
   For 500KHz, delay = (199 + 1) / 100 which gives us a 2 microsecond delay.
*/


void setupDelay() {
  if (divider < 9) { //9+1 = 10M
    useMicro = 0;
  }
  else {            //  <10M
    useMicro = 1;
  }
  switch (divider) {
    case 999:  //100k
      delays = 295;
      break;
    case 332:  //300k
      delays = 95;
      break;
    case 99: // 100/(99+1) = 1M
      delays = 25;
      break;
    case 32: // 3M
      delays = 5;
      break;
  }
}

// wait for trigger and do sampling
void dosampling() {
  uint32_t i = 0x1ffffff;  // 10s count
  uint8_t* ptr = &logicdata[MAX_CAPTURE_SIZE];
  volatile uint32_t  *port = portInputRegister(GPIOE);
  uint32_t start;
  uint8_t v1 = *port;
  uint8_t v0 = *port;

/*
  // get current gpio state an wait for change or timeout
  v1 = *port;
  if (trigger_mask) {
    while ((trigger_values ^ v1) & trigger_mask)
      v1 = *port;
  } else {
    while ((i-- != 0) && (v1 == v0))
    {
      v1 = *port;
    }
  }
*/
  i = MAX_CAPTURE_SIZE;
  digitalWrite(LED, LOW);  //for lowplus check
  start = micros();
  switch (divider) {
    case 1:
    case 2:
    case 3:   //100m/23m -1 =3
      //      systick_disable(); //1521us
      do {
        *ptr-- = *port; //1
        *ptr-- = *port; //2
        *ptr-- = *port; //3
        *ptr-- = *port; //4
        *ptr-- = *port; //5
        *ptr-- = *port; //6
        *ptr-- = *port; //7
        *ptr-- = *port; //8
        *ptr-- = *port; //9
        *ptr-- = *port; //10
        *ptr-- = *port; //1
        *ptr-- = *port; //2
        *ptr-- = *port; //3
        *ptr-- = *port; //4
        *ptr-- = *port; //5
        *ptr-- = *port; //6
        *ptr-- = *port; //7
        *ptr-- = *port; //8
        *ptr-- = *port; //9
        *ptr-- = *port; //20
        *ptr-- = *port; //1
        *ptr-- = *port; //2
        *ptr-- = *port; //3
        *ptr-- = *port; //4
        *ptr-- = *port; //5
        *ptr-- = *port; //6
        *ptr-- = *port; //7
        *ptr-- = *port; //8
        *ptr-- = *port; //9
        *ptr-- = *port; //30
        *ptr-- = *port; //1
        *ptr-- = *port; //2

        /*         logicdata[--i] = *port; //2
                 logicdata[--i] = *port; //3
                 logicdata[--i] = *port; //4
                 logicdata[--i] = *port; //5
                 logicdata[--i] = *port; //6
                 logicdata[--i] = *port; //7
                 logicdata[--i] = *port; //8
                 logicdata[--i] = *port; //9
                 logicdata[--i] = *port; //10
                 logicdata[--i] = *port; //1
                 logicdata[--i] = *port; //2
                 logicdata[--i] = *port; //3
                 logicdata[--i] = *port; //4
                 logicdata[--i] = *port; //5
                 logicdata[--i] = *port; //6*/
        i -= 32;
      } while (i > 0); //60/20=3*1024 cicules speed 40M
      //      systick_enable();
      break;
    case 9: // 100/(9+1) = 10M
      do {
        asm volatile("nop");
        asm volatile("nop");
        logicdata[i--] = *port;  //1
      } while (i);
      break;
    default:
      do {
        asm volatile("nop");
		for (; delays != 0; delays --);  /* delay100ns(delays);*/
        logicdata[i--] = *port;  //1
      } while (i);
      break;
  }
  tus = micros() - start;
  digitalWrite(LED, HIGH);

  // send it over uart
  i = MAX_CAPTURE_SIZE;
  //  Serial.write(v0);
  Serial.write(v1);
  do {
    Serial.write(logicdata[i--]); //output unsigned char 8bit data
  } while (i);
}

//init
void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  for (uint32_t i = 0; i < 8; i++) pinMode(pin[i], INPUT); //for sample input
  for (uint32_t j = 0; j < 8; j++) { //ready blink 3 times
   digitalToggle(LED);
    delay(250);
  }
}

// main routine
void loop () {
  if (Serial.available()) {
    // get the new byte:
    cmdByte = Serial.read();
    switch (cmdByte) {
      case SUMP_RESET: //0x00
      case SUMP_SELF_TEST: //0x03
        break;
      case SUMP_ARM:   //0x01
      case '1':
        dosampling();
        break;
      case SUMP_QUERY: //0x02
        //        Serial.print("1ALS");
        Serial.write('1');
        Serial.write('A');
        Serial.write('L');
        Serial.write('S');
        break;
      case SUMP_GET_METADATA: //0x04
        // device type GD32F103R

        Serial.write((uint8_t)0x01);
        Serial.print(DEVICE);
        /*        Serial.write('G');
                Serial.write('D');
                Serial.write('3');
                Serial.write('2');
                Serial.write('F');
                Serial.write('v');
                Serial.write('0');*/
        Serial.write((uint8_t)0x00);

        /*  firmware version  FPGA 0x02 pic 0x03*/
        Serial.write((uint8_t)0x02);
        Serial.print(VERSION);  //  "0.13"
        Serial.write((uint8_t)0x00);

        // amount of sample memory available (bytes)
        sump_sendmeta_uint32(0x21, MAX_CAPTURE_SIZE+2);

        // maximum sample rate (hz)
        sump_sendmeta_uint32(0x23, MAX_SAMPLERATE);

        // number of usable probes (short)
        sump_sendmeta_uint8(0x40, 0x08);

        // protocol version (short)
        sump_sendmeta_uint8(0x41, 0x02);

        //0x25 Capability Flags 0x0000001F
        //Bit 0 - Basic Trigger available.
        //Bit 1 - Advanced Trigger available.
        //Bit 2 - RLE Encoding available (see flag register).
        //Bit 3 - Extra RLE Encoding modes available (see flag register).
        //Bit 4 - State capture mode available (see flag register)
        //Bit 5 - Finish Now command available.
        //Bit 6 - Query Input Data command available.
        //Bit 7 - Query Capture State command available.
        //Bit 8 - Return Capture Data command, and manual capture mode available (see flag register).
        //Bits 31 - 9 - reserved
        //sump_sendmeta_uint32(0x25, 0x00000003);

        // end of meta data
        Serial.write((uint8_t)0x00);
        break;

      // long commands.. consume bytes from uart
      case SUMP_TRIGGER_MASK:  //0xc0
      case 0xC4:
      case 0xC8:
      case 0xCC:
        getCmd();
        trigger_mask = cmdBytes[0];
        break;
      case SUMP_TRIGGER_VALUES:  //0xc1
      case 0xC5:
      case 0xC9:
      case 0xCD:
        /*
             trigger_values can be used directly as the value of each bit
           defines whether we're looking for it to be high or low.
        */
        getCmd();
        trigger_values = cmdBytes[0];
        break;
      case SUMP_TRIGGER_CONFIG://c2
      case 0xC6:
      case 0xCA:
      case 0xCE:
        /* read the rest of the command bytes, but ignore them. */
        getCmd();
        break;
      case SUMP_SET_DIVIDER:   //0x80
        /*
             the shifting needs to be done on the 32bit unsigned long variable
           so that << 16 doesn't end up as zero.
        */
        getCmd();
        divider = cmdBytes[2];
        divider = divider << 8;
        divider += cmdBytes[1];
        divider = divider << 8;
        divider += cmdBytes[0];
        setupDelay();
        break;
      case SUMP_SET_READ_DELAY_COUNT://0x81:
        /*
             this just sets up how many samples there should be before
           and after the trigger fires.  The readCount is total samples
           to return and delayCount number of samples after the trigger.
           this sets the buffer splits like 0/100, 25/75, 50/50
           for example if readCount == delayCount then we should
           return all samples starting from the trigger point.
           if delayCount < readCount we return (readCount - delayCount) of
           samples from before the trigger fired.
        */
        getCmd();
        readCount = 4 * (((cmdBytes[1] << 8) | cmdBytes[0]) + 1);
        if (readCount > MAX_CAPTURE_SIZE)
          readCount = MAX_CAPTURE_SIZE;
        delayCount = 4 * (((cmdBytes[3] << 8) | cmdBytes[2]) + 1);
        if (delayCount > MAX_CAPTURE_SIZE)
          delayCount = MAX_CAPTURE_SIZE;
        break;
      case SUMP_SET_FLAGS: //0x82
        /* read the rest of the command bytes and check if RLE is enabled. */
        getCmd();
        rleEnabled = ((cmdBytes[1] & 0x40) != 0);  /*B0100 0000*/
        break;

      case '?':  //debug dump vars
        Serial.println(DEVICE);
        Serial.print("divider: 0x");  Serial.println(divider, HEX);
        Serial.print("delays ");      Serial.println(delays, DEC);
        Serial.print("trigger_mask  :");  Serial.println(trigger_mask, BIN);
        Serial.print("trigger_values:");  Serial.println(trigger_values, BIN);

        Serial.print("readCount: ");   Serial.println(readCount, DEC);
        Serial.print("delayCount: ");  Serial.println(delayCount, DEC);
        Serial.print("useMicro: ");    Serial.println(useMicro, DEC);
        Serial.print("rleEnabled: ");  Serial.println(rleEnabled, DEC);
        Serial.print("sample time us="); Serial.println(tus, DEC);
        break;
      default:
        break;
    }
  }
}


