/* USB Mass Storage/MSC_CDC Composite drive based on Spi serial flash or stm32 internal flash,
   Select USB: Mass Storage or MSC_CDC Composite from the menu
   EXPERIMENTAL, for testing purposes.
   When writing, use eject drive, or the filesystem may get corrupted. In that case, use:
     chkdsk <drive> /f
   Connect to USB. Upload files. Eject drive, Disconnect. Reconnect. Files should be there.
   Create a config.txt in, and write 1/2/3 into it. The leds will light up accordingly

   if Select Serial Communication:SerialUSB from the menu, output message by USB CDC
   ---------------------------------------------------------------------------------------
   huaweiwx@sina.com 2017.9.15
   default:  512k
   ---------------------------------------------------------------------------------------
*/

#define SPIFLASHDISK_SIZE    2040       /*user define, max 2040k*/
#include <SpiFlashDisk.h>
#include <LED.h>

void setup() {
  SPIFLASH.begin(); //need while usb unconnected
  Serial.begin(115200);
  Led.Init();
  while (!Serial.available()) {
    Led.flash(500, 500, 1);
  }
  Serial << "SpiFlash vdisk: " << SPIFLASHDISK_SIZE << "k\n";
  
  // SdFat interface, so we can access it from the microcontroller
  fat.begin();
}

void loop() {
  // Needed, or SdFat will not know about changes from USB drive
  fat.cacheClear();
  File f = fat.open("config.txt", FILE_READ);
  if (!f) {
    Led.flash(10, 990, 1);
    Serial << "config.txt not found!\n";
    return;
  }

  char c = f.read();
  Serial.print("c= ");
  Serial.println(c);
  switch (c) {
    case '1':
      Led.flash(500, 500, 1);
      break;
    case '2':
      Led.flash(250, 250, 2);
      break;
    case '3':
      Led.flash(165,165, 3);
      break;
    default:
      Led.flash(110, 110, 4);
      break;
  }
  f.close();
}
