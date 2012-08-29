
attrib -R ..\..\..\..\sys\mid\api\*.h
copy stm32f1-api\*.h ..\..\..\..\sys\mid\api
attrib +R ..\..\..\..\sys\mid\api\*.h

attrib -R ..\..\..\..\sys\mid\inc\*.h
copy stm32f1-inc\*.h ..\..\..\..\sys\mid\inc
attrib +R ..\..\..\..\sys\mid\inc\*.h

attrib -R ..\..\..\..\sys\mid\lib\*.lib
copy stm32f1-lib\stm32f1.lib ..\..\..\..\sys\mid\lib
attrib +R ..\..\..\..\sys\mid\lib\*.lib




