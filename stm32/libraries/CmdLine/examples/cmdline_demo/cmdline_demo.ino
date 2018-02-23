#include <cmdline.h>

#define LED LED_BUILTIN
char cmdline[256];
int ptr;
boolean stringComplete = false;  // whether the string is complete

void setup() {
  pinMode(LED, OUTPUT);     //LED D48
  Serial.begin(115200);
  Serial.println(F("cmdline lib demo"));
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

int Cmd_show(int argc, char *argv[])
{
  int i;
  Serial.print(F("argc="));
  Serial.println(argc);
  Serial.println(F("argv:"));

  for (i = 0; i < argc; i++)
  {
    Serial.print(argv[i]);
    Serial.print(F("  "));
  };
  return (0);
}

int Cmd_on(int argc, char *argv[])
{
  Serial.println(F("LED on"));
  digitalWrite(LED , LOW);    // turn on the LED
  return (0);
}

int Cmd_off(int argc, char *argv[])
{
  Serial.println(F("LED off"));
  digitalWrite(LED , HIGH);    // turn on the LED
  return (0);
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

tCmdLineEntry g_sCmdTable[] =
{
  { "help",   Cmd_help,      " : Display list of commands"} ,
  { "h",      Cmd_help,      "    : alias for help"} ,
  { "?",      Cmd_help,      "    : alias for help"     } ,
  { "show",   Cmd_show,      " : Show cmd args"     } ,
  { "s",      Cmd_show,      "    : alias for show"  },
  { "on",     Cmd_on,        "   : Turn LED on"  },
  { "off",    Cmd_off,       "  : Turn LED off"  },
  {  0, 0, 0 }
};
