# BSFrance Arduino core for STM32 boards

This core will allow you to use BSFrance AVR based boards with the Arduino ecosystem, it ibsed on STM32 HAL.

# How to install

At first drivers for the usb-serial (VCP, Virtual Com Port) and DFU (Device Firmware Upgrade) have to be installed. This applies to Windows only, Mac OSX and Linux users wont have to install drivers (in most cases).

  - Plug the board to USB, press the reset button twice within 0.5 second to set the board in DFU mode.
  - Launch [Zadig installer](https://github.com/BSFrance/BSFrance-stm32/raw/master/stm32/tools/win/drivers/zadig-2.3.exe), the board will be recognized as STM32 BOOTLOADER, install the driver.
  - Press the reset button once so the board restart in normal mode (run) and install the [STM32 Virtual COM Port driver](https://github.com/BSFrance/BSFrance-stm32/raw/master/stm32/tools/win/drivers/Virtual%20Com%20port%20driver%20V1.4.0.msi).

You can now proceed with the Arduino core installation :
  - download the [core repository](https://github.com/BSFrance/BSFrance-stm32/archive/master.zip) and unzip it to Documents / Arduino / hardware folder (you might also want to rename it BSFrance-stm32).

*Note : Documents / Arduino is the default location used by Arduino IDE, if you use another location the principle is the same. If there is no Hardware folder in Documents / Arduino create it and move the BSFrance-stm32 repository inside.*

  - Launch Arduino IDE, select BSFrance LoRaMx boards in the Tool / board menu, select your variant in the Tools / Specific board menu, finally select the correct serial port in the Tool / port menu. Your board is now ready to use with Arduino IDE.

### USB serial port

The LoRaMx boards are providing a serial port via the native USB interface, this serial port can be used for debug or communication purpose, it is also used by Arduino to provide automatic upload.

### DFU

The DFU or Device Firmare Upgrade is a standard widely used to provide simple firmware upgrade on many devices and baords. On LoRaMx boards it provides automatic upload of user code via USB from Arduino IDE with no need for user action or external programmer.
Our STM32 boards are fitted with an extremely compact DFU capable firmware (4K, smallest on the market), some variants are also using the internal STM32 DFU bootlaoder, effectively using 0 octet of user flash.
Note : all boards with the 4K bootloader will blink the white LED fast when set to DFU mode, all boards using the internal bootlader (STM32F3xx, STM32F4xx, STML4xx) won't blink white LED when set to DFU mode.


### Manual DFU mode

We implemented a failsafe mecanism to enter in DFU mode manually, by pressing the reset button twice within 0.5 second (like a double clic) the board will enter and stay in DFU mode until a firmware is uploaded or reset button is pressed (once).

This mecanism is very important for any board with native USB serial because if user code locks-up the main loop then serial port wont be served anymore and Arduino IDE will lose the ability to reset board into DFU mode for automatic uploading.
So in such case the DFU mode has to be set manually by pressing the reset button twice within 0.5 second, uploading can then be done in Arduino IDE with the upload button as usual.

Note : when set to manual DFU mode the serial port wont be visible in Tools / port menu anymore, this is normal behavior and wont prevent uploading. The serial port cannot coexist with the DFU port due to OS limitations, during the uplaod process, the existing serial port is used by Arduino to reset the board into DFU mode, both mode are never active at the same time. When the board is set to manual DFU mode Arduino will detect it is already in DFU mode and will upload directly.

### SWD (STlink)

Alternatively STLink debugger can be used for uploading, make sure to select STLink in the Tools / upload method menu. Connect your STLink to the SWD interface (3V3 GND DIO CLK), there is no need to connect the RST line, reset is issued in software.
