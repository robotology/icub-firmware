@echo off

:: here are core files
set SOURCE_API=.\embobj-icub\icub-ems
set SOURCE_SRC=.\embobj-icub\icub-ems
set SOURCE_HID=.\embobj-icub\icub-ems
set SOURCE_CFG=.\embobj-icub\icub-ems

:: here are the final dirs
set D_API=api
set D_HID=src
set D_SRC=src
set D_CFG=cfg

set TYP=icub-app
set LST=(eOcfg_sm_EMSappl EOMtheEMSappl EOMtheEMSconfigurator EOMtheEMSerror EOMtheEMSrunner EOMtheEMSsocket EOMtheEMStransceiver EOMtheEMSapplCfg EOMtheEMSdiscoverylistener EOMtheEMSdiscoverytransceiver)

echo. -- copying "%TYP%" files

set DEST_API="%DEST_ROOT%"\"%TYP%"\"%D_API%"
set DEST_HID="%DEST_ROOT%"\"%TYP%"\"%D_HID%"
set DEST_SRC="%DEST_ROOT%"\"%TYP%"\"%D_SRC%"
set DEST_CFG="%DEST_ROOT%"\"%TYP%"\"%D_CFG%"


del /F /Q "%DEST_API%"\* 
del /F /Q "%DEST_HID%"\*
del /F /Q "%DEST_SRC%"\*  

FOR %%x in %LST% do call embobj.copy.object.bat %%x

attrib +R   "%DEST_API%"\*    
attrib +R   "%DEST_HID%"\*
attrib +R   "%DEST_SRC%"\*  

::here is an exception
copy /Y     %SOURCE_CFG%\EOMtheEMSapplCfg_cfg.h         %DEST_CFG% 
copy /Y     %SOURCE_CFG%\overridden_configurator.c      %DEST_CFG% 
copy /Y     %SOURCE_CFG%\overridden_runner.c            %DEST_CFG%
copy /Y     %SOURCE_CFG%\overridden_error.c             %DEST_CFG% 
copy /Y     %SOURCE_CFG%\overridden_appl.c              %DEST_CFG% 
copy /Y     %SOURCE_CFG%\cfg-readme.txt                 %DEST_CFG% 


 