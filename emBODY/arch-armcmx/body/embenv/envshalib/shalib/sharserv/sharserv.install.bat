
attrib -R ..\..\..\..\..\sys\env\api\eEsharedServices.h
copy /Y ..\..\api\eEsharedServices.h ..\..\..\..\..\sys\env\api
attrib +R ..\..\..\..\..\sys\env\api\eEsharedServices.h

attrib -R ..\..\..\..\..\sys\env\lib\sharSERV.*.obj
copy /Y lib\sharSERV.*.obj ..\..\..\..\..\sys\env\lib
attrib +R ..\..\..\..\..\sys\env\lib\sharSERV.*.obj

attrib -R ...\.\..\..\..\sys\env\bin\sharSERV.*.burnflash.bat
copy /Y bin\sharSERV.*.burnflash.bat ..\..\..\..\..\sys\env\bin
attrib +R ..\..\..\..\..\sys\env\bin\sharSERV.*.burnflash.bat

attrib -R ...\.\..\..\..\sys\env\bin\sharSERV.*.burnflash.uvproj
copy /Y bin\sharSERV.*.burnflash.uvproj ..\..\..\..\..\sys\env\bin
attrib +R ..\..\..\..\..\sys\env\bin\sharSERV.*.burnflash.uvproj

attrib -R ...\.\..\..\..\sys\env\bin\sharSERV.*.burnflash.uvopt
copy /Y bin\sharSERV.*.burnflash.uvopt ..\..\..\..\..\sys\env\bin
attrib +R ..\..\..\..\..\sys\env\bin\sharSERV.*.burnflash.uvopt

attrib -R ..\..\..\..\..\sys\env\bin\sharSERV.*.hex
copy /Y bin\sharSERV.*.hex ..\..\..\..\..\sys\env\bin
attrib +R ..\..\..\..\..\sys\env\bin\sharSERV.*.hex
