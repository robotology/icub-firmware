
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_skeleton_os_updaterofbootloader.h"

// --------------------------------------------------------------------------------------------------------------------
// application info

constexpr embot::app::theCANboardInfo::applicationInfo appInfo 
{ 
    embot::prot::can::versionOfAPPLICATION {2, 7, 222}, // keep build = 222 so that we recognise it
    embot::prot::can::versionOfCANPROTOCOL {2, 0} 
};

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 
    embot::app::skeleton::os::updaterofbootloader::run(appInfo);
    for(;;);    
}
   
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


