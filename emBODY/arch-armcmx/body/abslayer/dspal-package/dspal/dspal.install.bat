
attrib -R ..\..\..\..\sys\abs\api\dspal*.h
copy /Y dspal-api\dspal*.h ..\..\..\..\sys\abs\api
attrib +R ..\..\..\..\sys\abs\api\dspal*.h

attrib -R ..\..\..\..\sys\abs\lib\dspal.cm3.lib
copy /Y dspal-lib\dspal.cm3.lib ..\..\..\..\sys\abs\lib
attrib +R ..\..\..\..\sys\abs\lib\dspal.cm3.lib

attrib -R ..\..\..\..\sys\abs\cfg\dspal_*.*
copy /Y dspal-cfg\*.* ..\..\..\..\sys\abs\cfg
attrib +R ..\..\..\..\sys\abs\cfg\dspal_*.*






