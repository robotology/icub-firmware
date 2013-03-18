
    
@echo off

cls
echo == Initiating system instance variables...
echo. -- Setting the variables...

:: Here you need to make some changes to suit your system.
set SOURCE=./osiit-package
set SVN=C:\Program Files\TortoiseSVN\bin
set URL=https://svn.icub.iit.local/repos/soft/embed/embedded/arm/embody/body/abslayer/ipal-package/ipal/ipal-extra

:: Unless you want to modify the script, this is enough.

echo. -- deleting ...

rmdir /S "%SOURCE%"
DEL /F ..\..\sys\mid\lib\osiit*.lib
DEL /F ..\..\sys\mid\api\osiit*
DEL /F ..\..\sys\mid\cfg\osiit*
DEL /F ..\..\sys\mid\inc\osiit*
DEL /F ..\..\sys\mid\inc\rtl_*

echo. ++ Done.
    
    
    
    





