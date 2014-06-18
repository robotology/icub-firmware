IF "%~1"=="" GOTO endparse
mkdir rep
copy ..\appl\v1\bin\ems004_eb%1.hex rep
copy rep\ems004_eb%1.hex tools\target.hex 
C:\Keil_v5\UV4\UV4 -f tools\burn_target.uvprojx -o ..\rep\eApplication_ems004_burn_report.txt -t ems4ulproburn
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
