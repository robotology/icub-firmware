/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_eth_theApplication.h"
 
#include "embot_hw_bsp_amc.h"

int main(void)
{
    // w/ default value we dont specialise anything. so we need to start to cm4 by hand later on
    embot::hw::bsp::amc::set({});
    embot::app::eth::theApplication::getInstance().start({});
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

