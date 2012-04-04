mkdir rep
c:\keil\uv4\uv4 -f eraseeepromflash.ems001.burnflash.uvproj -orep/eraseeepromflash.ems001.report.txt
del -Y *.bak
del -Y *.dep
del -Y *.plg
del -Y *.uvgui*
