/*
 * Copyright (C) 2015 ICub Facility - Istituto Italiano di Tecnologia
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


#include "EOemsController.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

 #include "EOemsController_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eo_emsController_hid_userdef_config_motor(EOemsController* ctrl, uint8_t motor)
{
    // this is used by the EMS application to initialize the 2FOCs controller (the first time eo_emsController_SetControlMode is called)
    
    // for MC4plus: in case of calibration type 5 (with PWM applied till the hard stops), we need to enable the PWM for the motor... 
    
}


extern void eo_emsController_hid_userdef_set_motor_idle(EOemsController* ctrl, uint8_t motor)
{
    // this is used by the EMS application to set the 2FOC in idle state
    
    // for MC4plus: we could disable the specified motor
    
}

extern void eo_emsController_hid_userdef_force_motor_idle(EOemsController* ctrl, uint8_t motor)
{
    // this is used by the EMS application to set the 2FOC in force idle state
    
    // for MC4plus: we could disable the specified motor
}


extern void eo_emsController_hid_userdef_set_motor_running(EOemsController* ctrl, uint8_t motor)
{
    // this is used by the EMS application to set the 2FOC in running state

    // for MC4plus: we could enable the specified motor
    
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




