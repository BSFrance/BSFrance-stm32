/* USB Mass Storage/MSC_CDC Composite drive based on internal flash,
   Select USB: Mass Storage or MSC_CDC Composite from the menu
   EXPERIMENTAL, for testing purposes.
   When writing, use eject drive, or the filesystem may get corrupted. In that case, use:
     chkdsk <drive> /f
   Connect to USB. Upload files. Eject drive, Disconnect. Reconnect. Files should be there.
   Create a config.txt in, and write 1/2/3 into it. The leds will light up accordingly   
   if Select Serial Communication:SerialUSB from the menu, output message by USB CDC
   ---------------------------------------------------------------------------------------
   modify for f1/4  by huaweiwx@sina.com
   default FLASHDISK_SIZE:   F103x8/xB  64    xC/xD/xE 256
                             F4xx   xE  256         xG 512 
   ---------------------------------------------------------------------------------------
*/
#define FLASHDISK_SIZE     64   /*user define, unused default defined*/   
#include <InternalFlashDisk.h>
#include <LED.h>

void setup() {
  Serial.begin(115200);
  Led.Init();
  while (!Serial.available()) {
    Led.flash(500, 500, 1);
  }
  
  Serial << "InternalFlash vdisk: " << FLASHDISK_SIZE << "k\n";
  
// SdFat interface:FlashBlockFat fat defined in InternalFlashDisk.h, so we can access it from the microcontroller
  fat.begin();
}

void loop() {
  // Needed, or SdFat will not know about changes from USB drive
  fat.cacheClear();
  File f = fat.open("config.txt", FILE_READ);
  if (!f) {
    Led.flash(500, 500, 1);
    Serial << "config.txt not found!\n";
    return;
  }

  char c = f.read();
  Serial.print("c= ");
  Serial.println(c);
  switch (c) {
    case '1':
      Led.flash(10, 990, 1);
      break;
    case '2':
      Led.flash(10, 490, 2);
      break;
    case '3':
      Led.flash(10, 323, 3);
      break;
    default:
      Led.flash(250, 250, 2);
      break;
  }
  f.close();
}
