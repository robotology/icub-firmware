mkdir rep
copy ..\env\eLoader\bin\eLoader_ems004.hex rep
copy rep\eLoader_ems004.hex tools\target.hex 
c:\keil\uv4\uv4 -f tools\tool-ems4rd-ulpro-burn-target.uvproj -orep\eLoader_ems004.burn.report.txt
del -Y tools\*.bak
del -Y tools\*.dep
del -Y tools\*.plg
del -Y tools\*.uvgui*
del -Y tools\target.hex
