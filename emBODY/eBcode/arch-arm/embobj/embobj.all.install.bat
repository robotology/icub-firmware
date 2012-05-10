
@echo off

cls
echo == installing embobj ...

set DEST_ROOT=..\..\..\..\any-arch\sys\embobj


set TYPES=(core core-mee core-see core-nee core-vee env gpio gpio-mee ipnet ipnet-mee others prot utils)

set OTHERS=(icub icub-nvscfg-as icub-nvscfg-mn icub-nvscfg-mc icub-nvscfg-sk icub-nvscfg-eps)

FOR %%x in %TYPES% do call embobj.objs.%%x.bat %%x


FOR %%x in %OTHERS% do call embobj.%%x.bat %%x





