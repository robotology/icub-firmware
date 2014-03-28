IF "%~1"=="" GOTO endparse
mkdir rep
copy ..\appl\v1\bin\ems004_eb%1.hex rep
copy rep\ems004_eb%1.hex tools\target.hex 
c:\keil\uv4\uv4 -f tools\burn_target.uvproj -o .\rep\eUpdater_ems004.burn.report.txt -t ems4ulproburn
del -Y tools\*.bak
del -Y tools\*.dep
del -Y tools\*.plg
del -Y tools\*.uvgui*
del -Y tools\target.hex
GOTO quit
:endparse
ECHO missing the number of board
:quit
