
attrib -R ..\..\..\..\sys\abs\api\osal*.h
copy /Y osal-api\osal*.h ..\..\..\..\sys\abs\api
attrib +R ..\..\..\..\sys\abs\api\osal*.h

attrib -R ..\..\..\..\sys\abs\lib\*.lib
copy /Y osal-lib\osal.open.cm3.lib ..\..\..\..\sys\abs\lib
copy /Y osal-lib\osal.open.cm3.dbg.lib ..\..\..\..\sys\abs\lib
copy /Y osal-lib\osal.open.cm4.lib ..\..\..\..\sys\abs\lib
copy /Y osal-lib\osal.open.cm4.dbg.lib ..\..\..\..\sys\abs\lib
attrib +R ..\..\..\..\sys\abs\lib\*.lib

attrib -R ..\..\..\..\sys\abs\cfg\*.*
copy /Y osal-cfg\*.* ..\..\..\..\sys\abs\cfg
attrib +R ..\..\..\..\sys\abs\cfg\*.*






