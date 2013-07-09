@ECHO off
cls
:start
ECHO.
ECHO 1. 6SG

set choice=
set /p choice=Type of board.
if '%choice%'=='1' goto 6SG


ECHO "%choice%" is not valid please try again
ECHO.
goto start

:6SG
set MY_BOARD=6sg-embobj-step0
ECHO selected board is %MY_BOARD%
goto choose_dest





:choose_dest
ECHO.
ECHO 1. build
ECHO 2. build_testing
set choice=
set /p choice=Type the destination folder.

if '%choice%'=='1' goto cpy_build
if '%choice%'=='2' goto :cpy_build_test
ECHO "%choice%" is not valid please try again
ECHO.
goto start

goto ends


:cpy_build
if '%MY_BOARD%' == 'ALL' goto copy_all_build else
ECHO copio da %MY_BOARD% in build
copy /Y ..\proj\output\*.hex ..\..\..\..\..\..\build\6sg.hex
goto end


:cpy_build_test
if '%MY_BOARD%' == 'ALL' goto copy_all_build_test else
ECHO copio da %MY_BOARD% in test
copy /Y  ..\output\*.hex ..\..\..\..\..\..\build_testing\
goto end



:end
pause