mkdir rep
copy ..\env\eLoader\bin\eLoader_ems004.hex rep
copy rep\eLoader_ems004.hex tools\target.hex 
c:\keil\uv4\uv4 -f tools\burn_target.uvproj -o rep\eLoader_ems004_burn_report.txt -t ems4ulproburn
del -Y tools\*.bak
del -Y tools\*.dep
del -Y tools\*.plg
del -Y tools\*.uvgui*
del -Y tools\target.hex
