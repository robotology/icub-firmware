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

#if !defined(__cplusplus)
    #error this Watchdog.c file must be compiled in C++
#endif

// - API
#include "WatchDog.h"  

// - HIDDEN DATA
#include "WatchDog_hid.h"  

// - dependencies    
#include "EoCommon.h"
#include "EOemsControllerCfg.h"



void WatchDog_init(WatchDog* o)
{
    o->min_timer = 1000000000;
    o->timer = 0;
    o->base_time = 0;
}

void WatchDog_new(uint8_t n)
{
    WatchDog* o = NEW(WatchDog, n);
    
    for (int i=0; i<n; ++i)
    {
        WatchDog_init(o+i);
    }
}

void WatchDog_set_base_time_msec(WatchDog* o, uint32_t base_time_msec)
{
    o->base_time = (base_time_msec*(uint32_t)CTRL_LOOP_FREQUENCY)/1000;
    o->min_timer = o->base_time;
    o->timer = o->base_time;
}

void WatchDog_rearm(WatchDog* o)
{
    if (o->timer < o->min_timer)
    {
        o->min_timer = o->timer;
    }
        
    o->timer = o->base_time;
}

void WatchDog_rearm_from(WatchDog* o, uint32_t from)
{
    if (o->timer < o->min_timer)
    {
        o->min_timer = o->timer;
    }
    
    o->timer = from;
}

void Watchdog_disarm(WatchDog* o)
{
    o->timer = 0;
}

BOOL WatchDog_check_expired(WatchDog* o)
{
    if (!o->base_time) return FALSE;
    
    if (o->timer)
    {
        --o->timer;
        
        return FALSE;
    }
    
    return TRUE;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

