mkdir rep
c:\keil\uv4\uv4 -f sharSERV.mcbstm32c.burnflash.uvproj -orep/sharSERV.mcbstm32c.report.txt
del -Y *.bak
del -Y *.dep
del -Y *.plg
del -Y *.uvgui*
