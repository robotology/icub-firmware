/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _BOARDINFO_H_
#define _BOARDINFO_H_


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"


namespace env::dualcore::board::info {
    
    void set(uint64_t uniqueid);

    const eEboardInfo_t * get();

}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


