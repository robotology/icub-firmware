@echo off
FOR /l %%i in (1,1,11) DO  (echo i'm compiling for board %%i
 echo #define EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD     %%i >..\..\app_cfg\extDefEmsBoardNum.h 
 c:\keil\uv4\uv4 -b ..\ems4rd-v01.uvproj -o .\script\log%%i.txt -t "ems4rd"
 echo ...done!
 move ..\..\bin\ems004.hex ..\..\bin\ems004_eb%%i.hex 
)

pause