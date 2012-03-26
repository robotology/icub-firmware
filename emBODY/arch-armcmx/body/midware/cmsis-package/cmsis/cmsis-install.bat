
attrib -R ..\..\..\..\sys\mid\api\*.h
copy cmsis-api\*.h ..\..\..\..\sys\mid\api
attrib +R ..\..\..\..\sys\mid\api\*.h

attrib -R ..\..\..\..\sys\mid\inc\*.h
copy cmsis-inc\core\*.h ..\..\..\..\sys\mid\inc
copy cmsis-inc\dev-stm32\*.h ..\..\..\..\sys\mid\inc
copy cmsis-inc\dev-lm3s\*.h ..\..\..\..\sys\mid\inc
attrib +R ..\..\..\..\sys\mid\inc\*.h


attrib -R ..\..\..\..\sys\mid\src\*.c
copy cmsis-src\core\*.c ..\..\..\..\sys\mid\src
copy cmsis-src\dev-stm32\*.c ..\..\..\..\sys\mid\src
copy cmsis-src\dev-lm3s\*.c ..\..\..\..\sys\mid\src
attrib +R ..\..\..\..\sys\mid\src\*.c

attrib -R ..\..\..\..\sys\mid\cfg\*.*
copy cmsis-cfg\stm32-armcc\*.* ..\..\..\..\sys\mid\cfg
copy cmsis-cfg\lm3s-armcc\*.* ..\..\..\..\sys\mid\cfg
attrib +R ..\..\..\..\sys\mid\cfg\*.*




