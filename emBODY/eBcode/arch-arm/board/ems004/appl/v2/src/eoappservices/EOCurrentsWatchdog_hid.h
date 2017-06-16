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

#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOCurrentsWatchdog.h"
#include "EoMotionControl.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    float                commulativeAverage;
    uint16_t             counter;
} eoCurrentWD_averageData_t;

struct EOCurrentsWatchdog_hid
{
    eOmc_motor_t**              themotors;
    uint8_t                     numberofmotors;
    //float*                    filter_reg;
    float*                      nominalCurrent2; //in this list nominalCurrent^2 for each motor are saved
    float*                      I2T_threshold; //=(peakCurrent^2 - nominalCurrent^2);
    eoCurrentWD_averageData_t*  avgCurrent;
    float*                      accomulatorEp; //Ep_t = (averageCurrent_t^2-nominalCurrent^2) + Ep_t-1, where Ep_t is Ep at time t
    eObool_t                    initted;
    eObool_t                    *motorinI2Tfault;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



