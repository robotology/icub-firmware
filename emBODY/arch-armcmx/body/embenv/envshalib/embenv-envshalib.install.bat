
attrib -R ..\..\..\sys\env\api\*.h
copy /Y api\*.h ..\..\..\sys\env\api
attrib +R ..\..\..\sys\env\api\*.h

attrib -R ..\..\..\sys\env\src\*.c
copy /Y src\*.c ..\..\..\sys\env\src
attrib +R ..\..\..\sys\env\src\*.c




