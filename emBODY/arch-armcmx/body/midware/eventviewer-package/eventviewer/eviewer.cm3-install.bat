
attrib -R ..\..\..\..\sys\mid\api\*.h
copy eventviewer-api\*.h ..\..\..\..\sys\mid\api
attrib +R ..\..\..\..\sys\mid\api\*.h

attrib -R ..\..\..\..\sys\mid\inc\*.*
copy eventviewer-inc\*.* ..\..\..\..\sys\mid\inc
attrib +R ..\..\..\..\sys\mid\inc\*.*

attrib -R ..\..\..\..\sys\mid\lib\*.lib
copy eventviewer-lib\eventviewer.cm3.lib ..\..\..\..\sys\mid\lib
attrib +R ..\..\..\..\sys\mid\lib\*.lib

attrib -R ..\..\..\..\sys\mid\cfg\*.*
copy eventviewer-cfg\eventviewer-stm32-cfg.ini ..\..\..\..\sys\mid\cfg
attrib +R ..\..\..\..\sys\mid\cfg\*.*






