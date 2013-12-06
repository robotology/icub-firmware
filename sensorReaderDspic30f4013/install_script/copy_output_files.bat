@ECHO off
cls
:start
ECHO.
ECHO 1. Mais
ECHO 2. Strain
ECHO 3. ALL_BOARD
set choice=
set /p choice=Type of board.
if '%choice%'=='1' goto mais
if '%choice%'=='2' goto strain
if '%choice%'=='3' goto all_board
ECHO "%choice%" is not valid please try again
ECHO.
goto start

:mais
set MY_BOARD=mais
ECHO selected board is %MY_BOARD%
goto cpy_build

:strain
set MY_BOARD=strain
ECHO %MY_BOARD%
goto cpy_build


:all_board
set MY_BOARD=ALL
ECHO selected board is %MY_BOARD%
goto cpy_build



:cpy_build
if '%MY_BOARD%' == 'ALL' goto copy_all_build else
ECHO copio da %MY_BOARD% in build
copy /Y ..\%MY_BOARD%\output\*.hex ..\..\build\
goto end



:copy_all_build
ECHO copio da mais a build
copy /Y ..\mais\output\*.hex ..\..\build\
ECHO.
ECHO copio da \strain a build
copy /Y ..\strain\output\*.hex ..\..\build\
ECHO.
goto end


:end
pause