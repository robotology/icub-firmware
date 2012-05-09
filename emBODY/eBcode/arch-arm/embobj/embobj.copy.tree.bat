@echo off
xcopy /Y     "%SOURCE_API%"\*.*                    "%DEST_API%"    
xcopy /Y     "%SOURCE_SRC%"\*.*                    "%DEST_SRC%" 
::xcopy /Y     "%SOURCE_HID%"\%1.*                    "%DEST_HID%" 



