
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_CODE_UPDATEROFBOOTLOADER_H_
#define _EMBOT_CODE_UPDATEROFBOOTLOADER_H_

#include "embot_core.h"

#include "embot_app_theCANboardInfo.h"

namespace embot { namespace code { namespace updaterofbootloader {
       
    [[noreturn]] void run(const embot::app::theCANboardInfo::applicationInfo &info);

       
} } } // namespace embot { namespace code { namespace updaterofbootloader



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
