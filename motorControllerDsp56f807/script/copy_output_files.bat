@ECHO off
cls
:start
ECHO.
ECHO 1. 2BLL
ECHO 2. 2BLLDC
ECHO 3. 4DC
ECHO 4. ALL_BOARD
set choice=
set /p choice=Type of board.
if '%choice%'=='1' goto 2bll_board
if '%choice%'=='2' goto 2blldc_board
if '%choice%'=='3' goto 4dc_board
if '%choice%'=='4' goto all_board
ECHO "%choice%" is not valid please try again
ECHO.
goto start

:2bll_board
set MY_BOARD=2BLL
ECHO selected board is %MY_BOARD%
goto choose_dest

:2blldc_board
set MY_BOARD=2BLLDC
ECHO %MY_BOARD%
goto choose_dest

:4dc_board
set MY_BOARD=4DC
ECHO %MY_BOARD%
goto choose_dest

:all_board
set MY_BOARD=ALL
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

goto end


:cpy_build
if '%MY_BOARD%' == 'ALL' goto copy_all_build else
ECHO copio da %MY_BOARD% in build
copy /Y ..\%MY_BOARD%\output\*.out.S ..\..\build\
goto end


:cpy_build_test
ECHO copio da %MY_BOARD% in test
copy /Y  ..\%MY_BOARD%\output\*.out.S ..\..\build_testing\
goto end


:copy_all_build
ECHO copio da 2BLL a build
copy /Y ..\2BLL\output\*.out.S ..\..\build\
ECHO.
ECHO copio da \2BLLDC a build
copy /Y ..\2BLLDC\output\*.out.S ..\..\build\
ECHO.
ECHO copio da \4DC a build
copy /Y ..\4DC\output\*.out.S ..\..\build\
goto end


:end
pause