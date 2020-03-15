
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_SKELETON_OS_BOOTLOADER_H_
#define _EMBOT_APP_SKELETON_OS_BOOTLOADER_H_

#include "embot_core.h"

#include "embot_app_theCANboardInfo.h"

namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
       
    [[noreturn]] void run(const embot::app::theCANboardInfo::bootloaderInfo &info);

       
}}}}}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
