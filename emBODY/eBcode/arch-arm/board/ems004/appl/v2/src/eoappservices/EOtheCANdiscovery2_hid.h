/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

#ifndef _EOTHECANDISCOVERY2_HID_H_
#define _EOTHECANDISCOVERY2_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheCANprotocol.h"
#include "EOtimer.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheCANdiscovery2.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOtheCANdiscovery2_hid
{
    eObool_t                initted;
    eo_candiscovery_cfg_t   config;
    EOtimer                 *discoverytimer;    
    uint8_t                 discoverymaxretries;
    uint8_t                 discoverynumretries;
    eObool_t                allfound;
    eObool_t                forcestop;
    eObool_t                searching;
    eObool_t                tickingenabled;
    eObool_t                atleastoneboardisincompatible;
    eo_candiscovery_set_t   setofboards;
    uint16_t                canreplies[2];
    uint16_t                canincompatibilities[2];
    eObrd_typeandversions_t candetected[2][15];

}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



