mkdir rep
copy ..\env\eLoader\bin\eLoader_ems4rd.hex rep
copy rep\eLoader_ems4rd.hex tools\target.hex 
C:\Keil_v5\UV4\UV4 -f tools\burn_target.uvprojx -o ..\rep\eLoader_ems4rd_burn_report.txt -t ems4ulproburn
del /F tools\*.bak
del /F tools\*.dep
del /F tools\*.plg
del /F tools\*.uvgui*
del /F tools\*.htm
del /F tools\target.hex
