
attrib -R ..\..\..\..\sys\abs\api\ipal*.h
copy /Y ipal-api\ipal*.h ..\..\..\..\sys\abs\api
attrib +R ..\..\..\..\sys\abs\api\ipal*.h

attrib -R ..\..\..\..\sys\abs\lib\ipal.*.lib
copy /Y ipal-lib\ipal*.lib ..\..\..\..\sys\abs\lib
attrib +R ..\..\..\..\sys\abs\lib\ipal.*.lib

attrib -R ..\..\..\..\sys\abs\cfg\ipal_*.*
copy /Y ipal-cfg\*.* ..\..\..\..\sys\abs\cfg
attrib +R ..\..\..\..\sys\abs\cfg\ipal_*.*






