/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
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

#ifndef _EOCURRENTSWATCHDOG_HID_H_
#define _EOCURRENTSWATCHDOG_HID_H_


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOCurrentsWatchdog.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


struct EOCurrentsWatchdog_hid
{
    eOcurrents_watchdog_cfg_t   cfg;
    uint8_t                     numberofmotors;
    float*                      filter_reg;
    eObool_t                    initted;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



