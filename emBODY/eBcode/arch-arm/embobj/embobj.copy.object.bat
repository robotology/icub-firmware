@echo off
copy /Y     "%SOURCE_API%"\%1.h                       "%DEST_API%"    
copy /Y     "%SOURCE_HID%"\%1_hid.h                   "%DEST_HID%" 
copy /Y     "%SOURCE_SRC%"\%1.c                       "%DEST_SRC%" 




