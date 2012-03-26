
attrib -R ..\..\..\..\sys\abs\api\*.h
copy /Y hal-api\*.h ..\..\..\..\sys\abs\api
attrib +R ..\..\..\..\sys\abs\api\*.h

attrib -R ..\..\..\..\sys\abs\lib\*.lib
copy /Y hal-lib\hal.stm32f1.mcbstm32c.lib ..\..\..\..\sys\abs\lib
copy /Y hal-lib\hal.stm32f1.mcbstm32c.slim.lib ..\..\..\..\sys\abs\lib
copy /Y hal-lib\hal.stm32f1.ems001.lib ..\..\..\..\sys\abs\lib
copy /Y hal-lib\hal.stm32f1.ems001.slim.lib ..\..\..\..\sys\abs\lib
copy /Y hal-lib\hal.stellaris.lib ..\..\..\..\sys\abs\lib
attrib +R ..\..\..\..\sys\abs\lib\*.lib

attrib -R ..\..\..\..\sys\abs\src\*.s
copy /Y hal-stg\*.s ..\..\..\..\sys\abs\src
attrib +R ..\..\..\..\sys\abs\src\*.s

attrib -R ..\..\..\..\sys\abs\cfg\*.*
copy /Y hal-cfg\*.* ..\..\..\..\sys\abs\cfg
attrib +R ..\..\..\..\sys\abs\cfg\*.*






