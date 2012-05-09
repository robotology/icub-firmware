@echo off

:: here are core files
set SOURCE_API=.\embobj-icub\icub\api
set SOURCE_SRC=.\embobj-icub\icub\api
set SOURCE_HID=.\embobj-icub\icub\api

:: here are the final dirs
set D_API=api
set D_HID=src
set D_SRC=src


set TYP=icub
set LST=(EoBoards EoMeasures EoMotionControl EoSensors EoSkin EoUtilities)

echo. -- copying "%TYP%" files

set DEST_API="%DEST_ROOT%"\"%TYP%"\"%D_API%"
set DEST_HID="%DEST_ROOT%"\"%TYP%"\"%D_HID%"
set DEST_SRC="%DEST_ROOT%"\"%TYP%"\"%D_SRC%"


del /F /Q "%DEST_API%"\* 
del /F /Q "%DEST_HID%"\*
del /F /Q "%DEST_SRC%"\*  

FOR %%x in %LST% do call embobj.copy.object.bat %%x

attrib +R   "%DEST_API%"\*    
attrib +R   "%DEST_HID%"\*
attrib +R   "%DEST_SRC%"\*  
