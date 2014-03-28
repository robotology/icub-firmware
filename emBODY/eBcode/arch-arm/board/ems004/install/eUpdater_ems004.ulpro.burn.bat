mkdir rep
copy ..\env\eUpdater\bin\eUpdaterCANgtw_ems004_open.hex rep
copy rep\eUpdaterCANgtw_ems004_open.hex tools\target.hex 
c:\keil\uv4\uv4 -f tools\burn_target.uvproj -o rep\eUpdater_ems004_burn_report.txt -t ems4ulproburn
del -Y tools\*.bak
del -Y tools\*.dep
del -Y tools\*.plg
del -Y tools\*.uvgui*
del -Y tools\target.hex
