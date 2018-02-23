# BSFrance Arduino core for STM32 boards

This core is will allow you to use all BSFrance boards based on STM32 MCUs with the Arduino ecosystem, it implements all standard Arduino core function as well as many extras. The core also allows you to use standard CMSIS and STM32 HAL function drectly in your arduino sketch which can be very handy for optimisation purposes or for fast porting of existing code and drivers.

# How to install

At first drivers for the usb-serial (VCP, Virtual Com Port) and DFU (Device Firmware Upgrade) have to be installed. This applies to Windows only, Mac OSX and Linux users wont have to install drivers (in most cases).

  - Plug the board to USB, press the reset button twice within 0.5 second to set the board in DFU mode.
  - Launch Zadig installer, the board will be recognized as STM32 BOOTLOADER, install the driver.
  - Press the reset button once so the board restart in normal mode (run) and install the STMicro USB serial driver.

You can now proceed with the Arduino core installation, to do so, download it and unzip this repository to Documents / Arduino / hardware folder. Note : Documents / Arduino is the default location, if you use another location principle is the same. If you dont have any Hardware folder in Documents / Arduino create it and drop the BSFrance repository in it. If you already have a BSFrance folder you can drop the STM32 folder in it.

Launch Arduino IDE, select BSFrance LoRaMx boards in the Tool / board menu, select your variant in the Tools / Specific board menu, finally select the correct serial port in the Tool / port menu. Your board is now ready to use with Arduino IDE.

### Virtual serial port

The LoRaMx boards are providing a serial port via the native USB interface, this serial port can be used for debug or communication purpose, it is also used by the core during upload (for resetting the baord into DFU mode automatically).

### DFU

The DFU or Device Firmare Upgrade is a standard widely used to provide simple firmware upgrade on many devices and baords. On LoRaMx boards it provides automatic upload of user code via USB from Arduino IDE with no need for user action or external programmer.
All our STM32 boards are fitted with an extremely compact DFU capable firmware (4K, smallest on the market), some variants are also using the internal STM32 DFU bootlaoder, effectively using 0 octet of user flash.

### Manual DFU mode

We implemented a failsafe mecanism to enter in DFU mode manually, this mode can be set by pressing the reset button twice within 0.5 second (like a double clic), the board will then enter and stay in DFU mode until a firmware is uploaded or reset button is pressed (once).

This mecanism is very important for any board with native USB serial because if user code locks-up the main loop then serial port wont be served anymore and Arduino IDE will lose the ability to reset board into DFU mode for automatic uploading.
So in such case the DFU mode has to be set manually, this is done by pressing the reset button twice within 0.5 second (like a double clic), uploading can then be done normally in Arduino IDE with the upload button.

Note : when set to manual DFU mode the serial port wont be visible in Tools / port menu anymore, this is normal behavior and wont prevent uploading. The serial port cannot coexist with the DFU port due to OS limitations, during the uplaod process, the existing serial port is used by Arduino to reset the board into DFU mode, both mode are never active at the same time. When the board is set to manual DFU mode Arduino will detect it is already in DFU mode and will upload directly.
