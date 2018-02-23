# BSFrance Arduino core for STM32 boards

This core is will allow you to use all BSFrance boards based on STM32 MCUs with the Arduino ecosystem, it implements all standard Arduino core function as well as many extras. The core also allows you to use standard CMSIS and STM32 HAL function drectly in your arduino sketch which can be very handy for optimisation purposes or for fast porting of existing code and drivers.

# How to install

At first drivers for the usb-serial (VCP, Virtual Com Port) and DFU (Device Firmware Upgrade) have to be installed. This applies to Windows only since Mac OSX and Linux users wont have to install drivers in most cases.

  - Plug the board to USB, press the reset button twice within 0.5 second to set the board in DFU mode.
  - Launch Zadig installer, the board will be recognized as STM32 BOOTLOADER, install the driver.
  - Press the reset button once so the board restart in normal (run) mode and install the STMicro USB serial driver.

You can now proceed with the ARduino core installation in Documents / Arduino / hardware folder.
If you dont have any Hardware folder in Documents/Arduino create it and drop the BSFrance folder in it. If you already have a BSFrance folder just drag the STM32 folderin it. Lauch Arduino IDE, select BSFrance LoRaMx boards in the Tool / board menu, select your variant in the Tools / Specific board menu, finally select the correct serial port in the Tool / port menu. Your board is now ready to use in Arduino IDE.

### Virtual serial port

The LoRaMx boards are providing a serial port via the native USB interface, this serial port can be used for debug or communication purpose, it is also used by the core during upload (for resetting the baord into DFU mode automatically).

### DFU

The DFU or Device Firmare Upgrade is a standard widely used to provide simple firmware upgrade on many devices and baords. On LoRaMx boards it provides automatic upload of user code via USB from Arduino IDE with no need for user action or external programmer.
All our boards are fitted with an extremely compact DFU capable firmware (4K, smallest on the market), some variants are directly using the internal STM32 DFU bootlaoder, effectively using 0 octet of user flash.

### Manual DFU mode

We implemented a failsafe mecanism to enter in DFU mode manually, it can be set by pressing the reset button twice within 0.5 second (like a double clic), the board will then enter ans stay in DFU mode until a firmware is uploaded or reset button is hit.

This mecanism is very important for any board with native USB serial because if user code locks-up then serial port is broken and Arduino IDE loses the ability to reset the board into DFU mode automatically for uploading.
So in such case the DFU mode has to be entered manually, this is done by pressing the reset button twice within 0.5 second (like a double clic), uploading can then be done normally in Arduino IDE.
Note : Manul DFU mode the serial port wont be visible in the Tools / port menu, this is normal and wont prevent uploading, here is why : the serial port is used by Arduino only to reset the board to DFU mode, if the DFU mode is already set Arduino can see it and start upload directly.
