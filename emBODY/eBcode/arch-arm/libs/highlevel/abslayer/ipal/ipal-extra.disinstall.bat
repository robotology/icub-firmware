
    
@echo off

cls
echo == Initiating system instance variables...
echo. -- Setting the variables...

:: Here you need to make some changes to suit your system.
set SOURCE=./ipal-extra
set SVN=C:\Program Files\TortoiseSVN\bin
set URL=https://svn.icub.iit.local/repos/soft/embed/embedded/arm/embody/body/abslayer/ipal-package/ipal/ipal-extra

:: Unless you want to modify the script, this is enough.

echo. -- deleting ...

rmdir /S "%SOURCE%"
DEL /F .\ipal-lib\ipal.*.lib
DEL /F ..\..\..\..\sys\abs\lib\ipal.*.lib

echo. ++ Done.
    
    
    
    





