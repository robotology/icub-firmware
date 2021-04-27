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
    
    
#define EOCURRENTSWATCHDOG_CORRECT_THE_INITIALISE_BUG  

// marco.accame on 09 mar 2021
// the function eo_currents_watchdog_Initialise() has a behaviour which is not 
// similar to the other embobj singletons. 
// it should be called only once at startup inside eo_motioncontrol_Initialise()
// and then never again. its role should be only to be sure that the module has
// the required resources. And also it should have a protection vs multiple calls
// to avoid damage due to repeated memory allocation.
// The module should then have a method such as eo_currents_watchdog_Activate() and 
// maybe also a eo_currents_watchdog_Deactivate() to be called inside 
// eo_motioncontrol_Activate() and eo_motioncontrol_Deactivate().
// Instead, there is only eo_currents_watchdog_Initialise() which allocates memory and 
// at the same time activates the module. And the function is w/out protection vs multiple
// calls. Also, eo_currents_watchdog_Initialise() is called inside eo_motioncontrol_Activate().
// As a result, the continuos calls of eo_currents_watchdog_Initialise() produce RAM erosion
// which eventually may lead to a fatal error.
// 
// with macro CORRECT_THE_INITIALISE_BUG we solve this problem.
// we maintain the usual API to change as less as possible but protect vs continuos
// allocations. we actually use static data.

  


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    float                commulativeAverage;
    uint16_t             counter;
} eoCurrentWD_averageData_t;

#if defined(EOCURRENTSWATCHDOG_CORRECT_THE_INITIALISE_BUG)

enum {maxmotors = 4};

struct EOCurrentsWatchdog_hid
{
    eOmc_motor_t*               themotors[maxmotors];
    uint8_t                     numberofmotors;
    //float*                    filter_reg;
    float                       nominalCurrent2[maxmotors]; //in this list nominalCurrent^2 for each motor are saved
    float                       I2T_threshold[maxmotors]; //=(peakCurrent^2 - nominalCurrent^2);
    eoCurrentWD_averageData_t   avgCurrent[maxmotors];
    float                       accomulatorEp[maxmotors]; //Ep_t = (averageCurrent_t^2-nominalCurrent^2) + Ep_t-1, where Ep_t is Ep at time t
    eObool_t                    initted;
    eObool_t                    motorinI2Tfault[maxmotors];
}; 
#else
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
#endif

// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



