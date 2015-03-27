@echo off
FOR /l %%i in (1,1,9) DO  (echo i'm compiling for board %%i
 echo #define EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD     %%i >..\..\app_cfg\extDefEmsBoardNum.h 
 c:\keil\uv4\uv4 -b ..\ems001-v01.uvproj -o .\script\log%%i.txt -t "ems001"
 echo ...done!
 move ..\..\bin\ems001.hex ..\..\bin\ems001_eb%%i.hex 
)

pause