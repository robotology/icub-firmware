@ECHO off
cls
:start
ECHO.
ECHO This script let you install library.
ECHO 1. MAIS
ECHO 2. STRAIN
ECHO 3. ALL
set choice=
set /p choice=Type of board.
if '%choice%'=='1' goto mais_board
if '%choice%'=='2' goto strain_board
if '%choice%'=='3' goto all_board
ECHO "%choice%" is not valid please try again
ECHO.
goto start

:mais_board
copy /-Y ..\output\DSPIC30f-HAL-MAIS.a ..\..\sensorReaderDSPic30f4013\common\libraries

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\common\*.h
copy /-Y ..\source_code\common\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\common
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\common\*.h

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\mais
copy /-Y ..\source_code\mais\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\mais
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\mais
goto end


:strain_board
copy /-Y ..\output\DSPIC30f-HAL-STRAIN.a ..\..\sensorReaderDSPic30f4013\common\libraries

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\common\*.h
copy /-Y ..\source_code\common\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\common
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\common\*.h

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\strain
copy /-Y ..\source_code\strain\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\strain
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\strain
goto end


:all_board
copy /-Y ..\output\DSPIC30f-HAL-MAIS.a ..\..\sensorReaderDSPic30f4013\common\libraries
copy /-Y ..\output\DSPIC30f-HAL-STRAIN.a ..\..\sensorReaderDSPic30f4013\common\libraries

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\common\*.h
copy /-Y ..\source_code\common\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\common
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\common\*.h

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\mais
copy /-Y ..\source_code\mais\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\mais
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\mais

attrib -r ..\..\sensorReaderDSPic30f4013\common\API\strain
copy /-Y ..\source_code\strain\include\*.h ..\..\sensorReaderDSPic30f4013\common\API\strain
attrib +r ..\..\sensorReaderDSPic30f4013\common\API\strain
goto end


:end
pause