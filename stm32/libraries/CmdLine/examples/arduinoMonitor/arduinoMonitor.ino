#include <cmdline.h>

char cmdline[256];
int ptr;
boolean stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(115200);
  Serial.println(F("Arduino Monitor Demo V0.90"));
  ptr = 0;
  Serial.print(F(">"));
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
#if defined(ARDUINO_ARCH_STM32)||defined(ARDUINO_ARCH_HALMX)
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
  { "?",      Cmd_help,      "    : alias for help"     } ,
  { "pinMode", Cmd_pinMode,  ": Set pin mode(exp. pm 58 1)" } ,
  { "pm",     Cmd_pinMode,  "   : alias for pinMode" } ,
  { "dw",     Cmd_digitalWrite,    "   : Write port(exp. dw 13 1)" } ,
  { "dr",     Cmd_digitalRead,    "   : Read port in(exp. dr 1)"  },
  { "aw",     Cmd_analogWrite,  "   : Analog  out (exp. aw 58 255)" } ,
  { "ar",     Cmd_analogRead,   "   : Analog in (exp. ar 0)"},
  {  0, 0, 0 }
};
