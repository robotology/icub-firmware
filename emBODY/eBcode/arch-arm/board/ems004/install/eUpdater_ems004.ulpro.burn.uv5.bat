mkdir rep
copy ..\env\eUpdater\bin\eUpdaterCANgtw_ems004_open.hex rep
copy rep\eUpdaterCANgtw_ems004_open.hex tools\target.hex 
C:\Keil_v5\UV4\UV4 -f tools\burn_target.uvprojx -o ..\rep\eUpdater_ems004_burn_report.txt -t ems4ulproburn
del /F tools\*.bak
del /F tools\*.dep
del /F tools\*.plg
del /F tools\*.uvgui*
del /F tools\*.htm
del /F tools\target.hex
