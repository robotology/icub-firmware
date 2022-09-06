
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Davide Tomé
 * email:   davide.tome@iit.it
*/

#include "embot_app_skeleton_os_bootloader.h"


#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_can.h"

#include <stdlib.h>
#include <stdbool.h>
#include "embot_hw_can.h"

#include "tests.h"



namespace tests{	
const uint8_t  Firmware_vers = 3;
const uint8_t  Revision_vers = 0;
const uint8_t  Build_number  = 0;
    
    

		void Send2Can(uint8_t status, uint8_t d1, uint8_t d2, uint8_t d3, 
	uint8_t d4, uint8_t d5, uint8_t d6,uint8_t d7)	{
		
			embot::hw::can::Frame canframe;
						
			canframe.id = 1;
			canframe.size = 8;
			canframe.data[0] = 1;
			canframe.data[1] = 1;
			canframe.data[2] = 1;
			canframe.data[3] = 1;
			canframe.data[4] = 1;
			canframe.data[5] = 1;
			canframe.data[6] = 1;
			canframe.data[7] = 1;
		
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);

		}
	
    static void getFirmwareVersion(void* param)
    {
			embot::hw::can::Frame canframe;
						
			canframe.id = 1;
			canframe.size = 8;
			canframe.data[0] = Firmware_vers;
			canframe.data[1] = Revision_vers;
			canframe.data[2] = Build_number;
				
			embot::hw::can::put(embot::hw::CAN::one, {canframe.id, canframe.size, canframe.data});   
			embot::hw::can::transmit(embot::hw::CAN::one);			
			
    }


}
