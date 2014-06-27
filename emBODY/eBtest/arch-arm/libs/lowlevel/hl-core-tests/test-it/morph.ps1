
(Get-Content core-cmsis.uvprojx) | 
Foreach-Object {$_ -replace "-DEO_ROBOT_DEFS", "-DEO_ROBOT_NAME=iCubGenova04 -DEO_ROBOT_BOARD_NAME=ems4rd -DEO_ROBOT_BOARD_NUMBER=1"} | 
Set-Content core-cmsis-modified.uvprojx

(Get-Content core-cmsis.uvoptx) | 
Set-Content core-cmsis-modified.uvoptx