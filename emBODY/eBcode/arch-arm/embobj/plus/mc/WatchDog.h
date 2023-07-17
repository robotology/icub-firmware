/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#ifndef MC_WATCHDOG___
#define MC_WATCHDOG___


#include "EoCommon.h"

#include "EOemsControllerCfg.h"


#define USE_FLOAT_CTRL_UNITS

typedef struct // WatchDog
{
    uint32_t timer;
    uint32_t base_time;
} WatchDog;

extern void WatchDog_init(WatchDog* o);
extern void WatchDog_new(uint8_t n);
extern void WatchDog_set_base_time_msec(WatchDog* o, uint32_t base_time_msec);
extern void WatchDog_rearm(WatchDog* o);
extern void WatchDog_rearm_from(WatchDog* o, uint32_t from);
extern void Watchdog_disarm(WatchDog* o);
extern BOOL WatchDog_check_expired(WatchDog* o);


 
#endif  // include-guard


