mkdir rep
c:\keil\uv4\uv4 -f eraseeepromflash.mcbstm32c.burnflash.uvproj -orep/eraseeepromflash.mcbstm32c.report.txt
del -Y *.bak
del -Y *.dep
del -Y *.plg
del -Y *.uvgui*
