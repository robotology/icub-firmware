IF "%~1"=="" GOTO endparse
mkdir rep
copy ..\appl\v1\bin\mc4plus_eb%1.hex rep
copy rep\mc4plus_eb%1.hex tools\target.hex 
C:\Keil_v5\UV4\UV4 -f tools\burn_target.uvprojx -o ..\rep\eApplication_mc4plus_burn_report.txt -t stm32f4ulproburn
del /F tools\*.bak
del /F tools\*.dep
del /F tools\*.plg
del /F tools\*.uvgui*
del /F tools\*.htm
del /F tools\target.hex
GOTO quit
:endparse
ECHO missing the number of board
:quit
