mkdir rep
copy ..\env\eLoader\bin\eLoader_mc4plus.hex rep
copy rep\eLoader_mc4plus.hex tools\target.hex 
C:\Keil_v5\UV4\UV4 -f tools\burn_target.uvprojx -o ..\rep\eLoader_mc4plus_burn_report.txt -t stm32f4ulproburn
del /F tools\*.bak
del /F tools\*.dep
del /F tools\*.plg
del /F tools\*.uvgui*
del /F tools\*.htm
del /F tools\target.hex
