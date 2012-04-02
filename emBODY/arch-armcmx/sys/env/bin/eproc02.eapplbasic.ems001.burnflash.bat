
mkdir rep
c:\keil\uv4\uv4 -f eproc02.eapplbasic.ems001.burnflash.uvproj -orep/eproc02.eapplbasic.ems001.report.txt
del -Y *.bak
del -Y *.dep
del -Y *.plg
del -Y *.uvgui*
