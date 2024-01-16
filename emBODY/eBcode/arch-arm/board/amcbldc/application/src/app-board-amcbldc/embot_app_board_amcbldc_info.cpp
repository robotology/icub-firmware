
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_board_amcbldc_info.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_application_theCANagentCORE.h"
#include "embot_app_bldc_theApplication.h"


// --------------------------------------------------------------------------------------------------------------------
// - the configurable constants
// --------------------------------------------------------------------------------------------------------------------
    
namespace embot::app::board::amcbldc::info {
    
    constexpr embot::prot::can::applicationInfo applInfo 
    {   
        embot::prot::can::versionOfAPPLICATION {2, 0, 7},    
        embot::prot::can::versionOfCANPROTOCOL {2, 0}   
    };


    constexpr embot::hw::FLASHpartitionID codePartition 
    {
        embot::hw::FLASHpartitionID::application
    };

    constexpr embot::hw::CAN canBus 
    {
        embot::hw::CAN::one
    };

} // namespace embot::app::board::amcbldc::info {

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::board::amcbldc::info {
    
    embot::hw::FLASHpartitionID getCodePartition()
    {
        return codePartition;
    }  
      
    embot::app::application::CANagentCORE* getCANgentCORE()
    {
        static bool initted {false};
        if(!initted)
        {
            embot::app::application::theCANagentCORE::getInstance().initialise({applInfo, canBus});
            initted = true;
        }
        return &embot::app::application::theCANagentCORE::getInstance();
    }
    
} // namespace embot::app::board::amcbldc::info {


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



