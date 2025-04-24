
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_bldc_theApplication.h"

#include "embot_app_board_amc2c_info.h"
#include "embot_app_board_amc2c_mbd.h"

constexpr embot::app::bldc::theApplication::Config cfg 
{
    {embot::app::board::amc2c::info::getCodePartition, embot::app::board::amc2c::info::getCANgentCORE},
    {}, // systCfg: the default is typically OK 
    {}, // CommCfg: the default is typically OK
    {embot::app::board::amc2c::mbd::Startup, embot::app::board::amc2c::mbd::OnTick}  // CtrlCfg: the default stack is typically OK
};
    

int main(void)
{
    embot::app::bldc::theApplication::getInstance().start(cfg);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


