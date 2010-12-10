@ECHO off
cls
:start
ECHO.
ECHO This script let you install library.
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
copy /-Y ..\2BLL\output\library_2bll.lib ..\..\motorControllerDsp56f807\2BLL\lib\
goto end

:2blldc_board
copy /-Y ..\2BLLDC\output\library_2blldc.lib ..\..\motorControllerDsp56f807\2BLLDC\lib\
goto end



:4dc_board
copy /-Y ..\4DC\output\library_4dc.lib ..\..\motorControllerDsp56f807\4DC\lib\
goto end


:all_board
copy /-Y ..\2BLL\output\library_2bll.lib ..\..\motorControllerDsp56f807\2BLL\lib\
copy /-Y ..\2BLLDC\output\library_2blldc.lib ..\..\motorControllerDsp56f807\2BLLDC\lib\
copy /-Y ..\4DC\output\library_4dc.lib ..\..\motorControllerDsp56f807\4DC\lib\
goto end


:end
pause