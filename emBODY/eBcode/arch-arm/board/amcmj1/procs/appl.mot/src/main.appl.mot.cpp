
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_bldc_theApplication.h"

#include "embot_app_board_info.h"
#include "embot_app_board_mbd.h"

constexpr embot::app::bldc::theApplication::Config cfg 
{
    {embot::app::board::info::getCodePartition, embot::app::board::info::getCANgentCORE},
    {}, // systCfg: the default is typically OK 
    {}, // CommCfg: the default is typically OK
    {embot::app::board::mbd::Startup, embot::app::board::mbd::OnTick, 6*1024}  
};


    
int main(void)
{  
    embot::app::bldc::theApplication::getInstance().start(cfg); 
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


