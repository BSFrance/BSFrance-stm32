/* disk_operate.ino cmdline demo for vdisk moniter
   USB Mass Storage/MSC_CDC Composite drive based on internal flash or spi serial flash,
   Select USB: Mass Storage or MSC_CDC Composite from the menu
   EXPERIMENTAL, for testing purposes.
   When writing, use eject drive, or the filesystem may get corrupted. In that case, use:
     chkdsk <drive> /f
   Connect to USB. Upload files. Eject drive, Disconnect. Reconnect. Files should be there.
   Create some file as config.txt in,   
   if Select Serial Communication:SerialUSB from the menu, output message by USB CDC
   
   ---------------------------------------------------------------------------------------
   modify for f1/4  by huaweiwx@sina.com
   default FLASHDISK_SIZE:   F103x8/xB  64    xC/xD/xE 256
                             F4xx   xE  256         xG 512 
   ---------------------------------------------------------------------------------------
*/
//use InternalFlashDisk/SpiFlashDisk/sd/mmc 
#define SPIFLASHDISK_SIZE    2040       /*user define, max 2040k*/
#include <SpiFlashDisk.h>

//#define FLASHDISK_SIZE     64   /*user define, unused default defined*/
//#include <InternalFlashDisk.h>


#include <cmdline.h>

#define LED LED_BUILTIN
char cmdline[256];
int ptr;
boolean stringComplete = false;  // whether the string is complete

// SdFat interface, so we can access it from the microcontroller
//extern FlashBlockFat fat;

void setup() {
  pinMode(LED, OUTPUT);     //LED D48
  Serial.begin(115200);
  while (!Serial.available()) {
    digitalToggle(LED);
    delay(500);
  }
  Serial.println(F("stm32 DOS demo"));
  ptr = 0;
  Serial.print(F(">"));
  fat.begin();
}

void loop() {
  int nStatus;
  if (stringComplete) {
    if (cmdline[0] > ' ') {
      nStatus = CmdLineProcess(cmdline);
      if (nStatus == CMDLINE_BAD_CMD)
      {
        Serial.println(F("Bad command!"));
      }
      else if (nStatus == CMDLINE_TOO_MANY_ARGS)
      {
        Serial.println(F("Too many arguments for command processor!"));
      }
      else if (nStatus != 0)
      {
        Serial.print(F("Command returned error code"));
        Serial.println(nStatus);
      }
    }
    ptr = 0;
    stringComplete = false;
    Serial.print(F("\r\n>"));
  }

#if defined(STM32GENERIC)
  serialEvent();
#endif
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    cmdline[ptr++] = inChar;
    cmdline[ptr] = '\0';
    if ((inChar == '\n') || (inChar == '\r')) {
      stringComplete = true;
    }
  }
}

//*****************************************************************************
// This function implements the "help" command.  It prints a simple list of the
// available commands with a brief description.
//*****************************************************************************
int Cmd_help(int argc, char *argv[])
{
  tCmdLineEntry *pEntry;
  pEntry = &g_sCmdTable[0];
  Serial.println(F("\r\nAvailable commands:\r\n------------------\n"));
  while (pEntry->pcCmd)
  {
    Serial.print( pEntry->pcCmd);
    Serial.println(pEntry->pcHelp);
    pEntry++;
  }
  return (0);
}

// Usage: dir
int Cmd_dir(int argc, char *argv[])
{
  fat.cacheClear();
  Serial << "dir:\n";
  fat.ls();
  return (0);
}

// This function implements the "dw" command. bit write to a pin output
// Usage: dw pin 0/1
int Cmd_digitalWrite(int argc, char *argv[])
{
  uint8_t pin, v;
  if (argc == 3) {
    pin = atoi(argv[1]);
    v = atoi(argv[2]);
    if (v) {
      digitalWrite(pin, HIGH);   // turn on/off the LED
    } else {
      digitalWrite(pin , LOW);
    };
  } else {
    Serial.println(F(" argc err!\r\n"));
  }
  return (0);
}

// This function implements the "dr" command. Bit pin read input
// Usage: br pin
int Cmd_digitalRead(int argc, char *argv[])
{
  if (argc == 2) {
    Serial.println(digitalRead(atoi(argv[1])));
  } else {
    Serial.println(F(" argc err!\r\n"));
  }
  return (0);
}

// This function implements the "aw" command. analog pin output
// Usage: aw pin byte
int Cmd_analogWrite(int argc, char *argv[])
{
  uint16_t pin;
  uint8_t  v;
  if (argc == 3) {
    pin = atoi(argv[1]);
    v = atoi(argv[2]);
    analogWrite(pin, v);
  } else {
    Serial.println(F(" argc err!\r\n"));
  }
  return (0);
}

// This function implements the "ar" command. analog pin read input
// Usage: ar pin
int Cmd_analogRead(int argc, char *argv[])
{
  uint8_t pin;
  if (argc == 2) {
    pin = atoi(argv[1]);
    Serial.println(analogRead(pin));
  } else {
    Serial.println(F(" argc err!\r\n"));
  }

  return (0);
}

// This function implements the "pm" command. Set a pin input/output mode
// Usage: pm pin mode[0/1/2]
int Cmd_pinMode(int argc, char *argv[])
{
  uint8_t pin, m;
  if (argc == 3) {
    pin = atoi(argv[1]);
    m = atoi(argv[2]);
    if ((m >= 0) && (m < 3)) {
      pinMode(pin , m);
    } else {
      Serial.println(F(" not this pin mode  err!\r\n"));
    };
  } else {
    Serial.println(F(" argc err!\r\n"));
  }
  return (0);
}

tCmdLineEntry g_sCmdTable[] =
{
  { "help",   Cmd_help,      " : Display list of commands"} ,
  { "h",      Cmd_help,      "    : alias for help"} ,
  { "?",      Cmd_help,      "    : alias for help"} ,
  { "dir",    Cmd_dir,       " : list vdisk files"} ,
  { "pinMode", Cmd_pinMode,  ": Set pin mode(exp. pinMode 58 1)"} ,
  { "pm",     Cmd_pinMode,  "   : alias for pinMode" } ,
  { "digitalWrite",Cmd_digitalWrite,    "   : digitalWrite(exp. dw 13 1)"},
  { "dw",     Cmd_digitalWrite,    "   : alias for" } ,
  { "digitalRead",Cmd_digitalRead,    "   : digitalRead (exp. dr 1)"},
  { "dr",     Cmd_digitalRead,    "   : alias for digitalRead"  },

  { "aw",     Cmd_analogWrite,  "   : analogWrite(exp. aw 58 255)"} ,
  { "ar",     Cmd_analogRead,   "   : analogRead(exp. ar 0)"},
  {  0, 0, 0 }
};
