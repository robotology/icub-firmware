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

typedef struct
{
    eOabstime_t                 timeofstart;
    uint8_t                     discoverynumretries;
    eObool_t                    forcestop;
    eObool_t                    searching;
    eObool_t                    tickingenabled;    
} eOcandiscovery_searchstatus_t;


struct EOtheCANdiscovery2_hid
{
    eObool_t                        initted;
    eOcandiscovery_cfg_t            config;
    EOtimer                         *discoverytimer;    
    uint8_t                         discoverymaxretries;
    eOcandiscovery_searchstatus_t   searchstatus;
    eOcandiscovery_target_t         target;
    eOcandiscovery_detection_t      detection;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



