
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BOARD_AMC2C_INFO_H_
#define __EMBOT_APP_BOARD_AMC2C_INFO_H_


#include "embot_core.h"

#include "embot_app_bldc_theApplication.h"
#include "embot_app_application_CANagentCORE.h"
#include "embot_app_eth_theICCserviceROP.h"

namespace embot::app::board::amc2c::info {
        
    embot::hw::FLASHpartitionID getCodePartition();    
    embot::app::application::CANagentCORE* getCANgentCORE();
    
    bool OnROPrx(const embot::app::eth::icc::ItemROP &rxrop, embot::app::eth::icc::ItemROP &reply);
    
} // embot::app::board::amc2c::info {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
