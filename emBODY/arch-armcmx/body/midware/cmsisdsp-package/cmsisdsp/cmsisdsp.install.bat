
attrib -R ..\..\..\..\sys\mid\api\*.h
copy cmsisdsp-api\*.h ..\..\..\..\sys\mid\api
attrib +R ..\..\..\..\sys\mid\api\*.h

attrib -R ..\..\..\..\sys\mid\inc\*.*
copy cmsisdsp-inc\*.* ..\..\..\..\sys\mid\inc
attrib +R ..\..\..\..\sys\mid\inc\*.*

attrib -R ..\..\..\..\sys\mid\lib\*.lib
copy cmsisdsp-lib\cmsisdsp.cm3.lib ..\..\..\..\sys\mid\lib
copy cmsisdsp-lib\cmsisdsp.cm4.lib ..\..\..\..\sys\mid\lib
attrib +R ..\..\..\..\sys\mid\lib\*.lib

attrib -R ..\..\..\..\sys\mid\cfg\*.*
copy cmsisdsp-cfg\*.* ..\..\..\..\sys\mid\cfg
attrib +R ..\..\..\..\sys\mid\cfg\*.*






