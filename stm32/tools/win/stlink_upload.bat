@echo off
rem: Note %~dp0 get path of this batch file
rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
%driverLetter%
cd %~dp0
rem: the two line below are needed to fix path issues with incorrect slashes before the bin file name
set str=%4
set str=%str:/=\%

set alt=%2

:FLASH
rem stlink\ST-LINK_CLI.exe -c SWD -P %str% %6 -Run %7
stlink\ST-LINK_CLI.exe -c SWD -P %str% -Run
goto EXIT


:TEXANE
rem: Using the open source texane-stlink instead of the proprietary STM stlink exe
rem: texane-stlink\st-flash.exe write %str% 0x8000000
goto EXIT

:JLINK
rem: Using the open source texane-stlink instead of the proprietary STM stlink exe
rem: texane-stlink\st-flash.exe write %str% 0x8000000
goto EXIT


:EXIT

