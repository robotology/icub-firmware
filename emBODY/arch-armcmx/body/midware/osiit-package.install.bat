
    
@echo off

cls
echo == Initiating system instance variables...
echo. -- Setting the variables...

:: Here you need to make some changes to suit your system.
:: https://svn.cognitivehumanoids.eu/sdl/embedded/arm/embody/body/midware/osiit-package version 960
set SOURCE=./osiit-package
set SVN=C:\Program Files\TortoiseSVN\bin
set URL=https://svn.icub.iit.local/repos/sdl/embedded/arm/embody/body/midware/osiit-package

:: Unless you want to modify the script, this is enough.

echo. %SOURCE%
echo. %SVN%
echo. %URL%
echo. ++ Done setting variables.
echo.
echo == Updating source from SVN
echo. -- Running update...
"%SVN%\TortoiseProc.exe" /command:checkout /path:"%SOURCE%" /url:"%URL%" /closeonend:1
echo. ++ Done.

echo. -- Cleaning up...
set SOURCE=
set SVN=
echo. ++ Done.
    
    
    
    





