@echo off

:: here are core files
set SOURCE_API=.\embobj-api
set SOURCE_SRC=.\embobj-src
set SOURCE_HID=.\embobj-src

:: here are the final dirs
set D_API=api
set D_HID=src
set D_SRC=src


set TYP=others
set LST=(EOMdataContainer EOMfifoProdCons EOMtheEntitiesEnv EOvectorMutex)

echo. -- copying "%TYP%" files

set DEST_API=..\..\..\..\any-arch\sys\embobj\"%TYP%"\"%D_API%"
set DEST_HID=..\..\..\..\any-arch\sys\embobj\"%TYP%"\"%D_HID%"
set DEST_SRC=..\..\..\..\any-arch\sys\embobj\"%TYP%"\"%D_SRC%"


del /F /Q "%DEST_API%"\* 
del /F /Q "%DEST_HID%"\*
del /F /Q "%DEST_SRC%"\*  

FOR %%x in %LST% do call embobj.copy.object.bat %%x

attrib +R   "%DEST_API%"\*    
attrib +R   "%DEST_HID%"\*
attrib +R   "%DEST_SRC%"\*  
