
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Davide Tom�
 * email:   davide.tome@iit.it
*/


#include "tests.h"

// dependencies
#include "embot_core.h"
#include "embot_os_theScheduler.h"

#include "embot_hw_can.h"
#include "embot_prot_can.h"
#include "embot_app_theLEDmanager.h"



namespace testplan { namespace tests {
    
		struct FirmwareInfo{
			static const uint8_t  Firmware_vers = 1;
			static const uint8_t  Revision_vers = 0;
			static const uint8_t  Build_number  = 0;
		};
			
	}
}



