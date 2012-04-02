
@echo off

cls
echo == installing embobj ...


set TYPES=(core core-mee core-see core-nee core-vee env gpio gpio-mee ipnet ipnet-mee others prot utils)

FOR %%x in %TYPES% do call embobj.objs.%%x.bat %%x






