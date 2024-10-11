
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BOARD_AMCFOC_1CM7_INFO_H_
#define __EMBOT_APP_BOARD_AMCFOC_1CM7_INFO_H_


#include "embot_core.h"

#include "embot_app_bldc_theApplication.h"
#include "embot_app_application_CANagentCORE.h"

namespace embot::app::board::amcfoc::cm7::info {
        
    embot::hw::FLASHpartitionID getCodePartition();    
    embot::app::application::CANagentCORE* getCANgentCORE();
    
} // embot::app::board::amcfoc::cm7::info {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
