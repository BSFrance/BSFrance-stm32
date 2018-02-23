@echo off

echo Installing Maple DFU driver...
"%~dp0wdi-simple" --vid 0xDF11 --pid 0x0483 --type 0 --name "Maple DFU" --dest "%~dp0maple-dfu"
echo.

echo Installing Maple Serial driver...
"%~dp0wdi-simple" --vid 0xDF11 --pid 0x0483 --type 3 --name "Maple Serial" --dest "%~dp0maple-serial"
echo.

pause
