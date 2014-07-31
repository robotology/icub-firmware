Folder structure:
	- reference ==> contains the 2foc application project developed by edl group. (currently only reference to edl's svn revision)
	- proj      ==> contains the project files of 2foc application used on robot.
	- app       ==> contains the source code taken form edl project and modified to adapt to robot's constraints and needs
	- app-added ==> contains new souce code files
	- doc       ==> documentation 

Compilation option:
Some compilation options are added in UserParams.h

	- SYNC_2FOC_TO_EMS: with this option the 2foc sends can msg only when ems has sent a "set_desired_current" 
	  or "ICUBPROTO_PERIODICCMD_EMSTO2FOC_DESIRED_CURRENT" message. (for more info about last msg see CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS option).
	  Note that 2foc fw sends status message always every 1 second even if ems doesn't send msg. 

	- NO_FAULT: this option is usefull in debug mode in order to disable check for fault pin during start up.
 
	- CAN_STATUS_MSG_PERIOD_SYNC_WITH_EMS: in this mode 2foc is enabled to parse perdioidc message of ems containg 4 desired current (one for each 2foc)
