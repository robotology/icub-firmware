@echo off

:: here are core files
set SOURCE_API=.\embobj-icub\icub-nvscfg\cfg-eps-ebx
set SOURCE_SRC=.\embobj-icub\icub-nvscfg\cfg-eps-ebx
::set SOURCE_HID=.\embobj-icub\icub-nvscfg\ep-analogsensors\

:: here are the final dirs
set D_API=board-eps
set D_SRC=board-eps
set D_HID=board-eps

set TYP=icub-nvscfg
::set LST=(EoBoards EoMeasures EoMotionControl EoSensors EoSkin EoUtilities)

echo. -- copying "%TYP%" files

set DEST_API="%DEST_ROOT%"\"%TYP%"\"%D_API%"
set DEST_SRC="%DEST_ROOT%"\"%TYP%"\"%D_SRC%"
::set DEST_HID="%DEST_ROOT%"\"%TYP%"\"%D_HID%"

del /F /Q "%DEST_API%"\* 
del /F /Q "%DEST_SRC%"\*  
::del /F /Q "%DEST_HID%"\*

::FOR %%x in %LST% do call embobj.copy.tree.bat %%x
call embobj.copy.tree.bat

attrib +R   "%DEST_API%"\*    
attrib +R   "%DEST_SRC%"\*  
::attrib +R   "%DEST_HID%"\*