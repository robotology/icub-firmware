/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOCANMESGWATCHDOG_HID_H_
#define _EOCANMESGWATCHDOG_HID_H_


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOCanMsgWatchdog.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum
{
    wd_st_uninitilized  = 0,
    wd_st_configured    = 1,
    wd_st_started       = 2
} eO_canmsg_watchdog_state_t;


struct EOCanMsgWatchdog_hid
{
    eOcanmsg_watchdog_cfg_t    cfg;
    eO_canmsg_watchdog_state_t state;
    eOabstime_t starttime;
    
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




