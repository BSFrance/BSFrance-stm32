@Echo off
Rem: Note %~dp0 get path of this batch file
Rem: Need to change drive if My Documents is on a drive other than C:
set driverLetter=%~dp0
set driverLetter=%driverLetter:~0,2%
rem %driverLetter%
cd %~dp0

set str=%3

set str=%str:/=\%

set commport=%1

set commportnum=%commport:COM=%

if "%5" == ""   goto A
set s=%5
set g=%6
goto B

:A
echo default -S0x0800000 -g0x0800000
set s=0x8000000
set g=0x8000000

:B
if "%2"=="USERDEF"     goto USERDEF
if "%2"=="CH340"       goto CH340
if "%2"=="PL2303"      goto PL2303

if "%2"=="UNUSED"    goto UNUSED

goto UPLOAD

:CH340
echo for CH340  Rts_HiRst Dtr_HiBoot
stm32flash -b 115200 -w %str% %1 -S%s% -g%g% -i  1,-2,2,1 -n 40
goto EXIT

:PL2303
echo for PL2303 Rts_HiRst Dtr_HiBoot
stm32flash -b 115200 -w %str% %1 -S%s% -g%g% -i  1,-2,2,1:-1-2 -n 40
goto EXIT

:USERDEF
echo for user define  Rts/Dtr to reset&boot
stm32flash -b 115200 -w %str% %1 -S%s% -g%g% -i  2,-1,1,2 -n 40
goto EXIT


:UNUSED


:UPLOAD
echo for General Board Dtr/Rts unused
stm32flash -b 115200 -w %str% %1 -S%s% -g%g% -n 40

:EXIT