
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef MC_WATCHDOG_HID___
#define MC_WATCHDOG_HID___

#include "EoCommon.h"

struct WatchDog_hid
{
    uint32_t min_timer;
    uint32_t timer;
    uint32_t base_time;
};

 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
