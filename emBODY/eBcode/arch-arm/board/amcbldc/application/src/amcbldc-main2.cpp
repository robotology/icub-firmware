
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_bldc_theApplication.h"

#include "embot_app_board_amcbldc_info.h"
#include "embot_app_board_amcbldc_mbd.h"

constexpr embot::app::bldc::theApplication::Config cfg 
{
    {embot::app::board::amcbldc::info::getCodePartition, embot::app::board::amcbldc::info::getCANgentCORE},
    {}, // systCfg: the default is typically OK 
    {}, // CommCfg: the default is typically OK
    {embot::app::board::amcbldc::mbd::Startup, embot::app::board::amcbldc::mbd::OnTick}  // CtrlCfg: the default stack is typically OK
};
    

int main(void)
{
    embot::app::bldc::theApplication::getInstance().start(cfg); 
}



//#include "embot_app_amcbldc_theApplication.h"

//constexpr embot::prot::can::applicationInfo applInfo 
//{   
//    embot::prot::can::versionOfAPPLICATION {2, 0, 1},    
//    embot::prot::can::versionOfCANPROTOCOL {2, 0}    
//};

//constexpr embot::hw::CAN canBus {embot::hw::CAN::one};

// the amcbldc board needs class theCANagentCORE derived from embot::app::application::CANagentCORE
// The class theCANagentCORE is OK for the applications of all the boards w/ bootloader underneath
// and w/ a FLASH based storage.
// It allows: the synch of application info in RW storage, the standard management of core CAN messages 
// such as change of address, FW update etc.
// But this class theCANagentCORE is not OK for the amc2c which needs a different implementation because
// it will not have a bootloader underneath nor a RW storage, so it will behave differently.

//#include "embot_app_application_theCANagentCORE.h"

//embot::app::application::CANagentCORE* amcbldcGetCANgentCORE()
//{
//    static bool initted {false};
//    if(!initted)
//    {
//        embot::app::application::theCANagentCORE::getInstance().initialise({applInfo, canBus});
//        initted = true;
//    }
//    return &embot::app::application::theCANagentCORE::getInstance();
//}

// the amcbldc board needs this MC specialization, the amc2c may need a different one. let's see.
//#include "embot_app_amcbldc_MBD.h"

//constexpr embot::app::amcbldc::theApplication::Config cfg 
//{
//    {embot::hw::flash::Partition::ID::application, amcbldcGetCANgentCORE},
//    {}, // systCfg: the default is typically OK 
//    {}, // CommCfg: the default is typically OK
//    {embot::app::amcbldc::mbd::Startup, embot::app::amcbldc::mbd::OnTick}  // CtrlCfg: the default stack is typically OK
//};
//    

//int main(void)
//{
//    embot::app::amcbldc::theApplication::getInstance().start(cfg); 
//}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


