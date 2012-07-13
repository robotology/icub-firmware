/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo, Valentina Gaggero
 * email:   alessandro.scalzo@iit.it, valentina.gaggero@iit.it
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EOemsController.h"
#include "EOMappTheSysController.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "proto_test.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "proto_test.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define MC_START           1
#define MC_STOP            2
#define MC_SET_POS         3
#define MC_SET_POS_PID     4
#define MC_SET_POS_PID_LIM 5
#define MC_SET_VEL         6
#define MC_SET_VEL_PID     7
#define MC_SET_VEL_LIM     8
#define MC_SET_OFFSET      9
#define MC_CALIBRATE       10
#define MC_SET_LIMITS      11
#define MC_CONTROL_MODE    12


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern eOresult_t proto_test_parse(uint8_t *payload)
{

    switch(payload[0]) 
    {
        case MC_START:
        {
            eom_appTheSysController_Go2RunState(eom_appTheSysController_GetHandle());
        }
        break;

        case MC_STOP:
        {   
            eo_emsController_SetControlMode(3, CM_IDLE);
            eo_emsController_ResetPosPid(3);
               
//            pwm_out = 0; non l'ho aggiunta!!!   
        }
        break;

        case MC_CALIBRATE:
        {
            uint8_t joint = payload[1];
            int32_t gotopos = *(int32_t*)(payload+4);
            int32_t offset  = *(int32_t*)(payload+8);
            int32_t timeout = *(int32_t*)(payload+12);
            int32_t max_err = *(int32_t*)(payload+16);
            
            eo_emsController_StartCalibration(joint, gotopos, offset, timeout, max_err);
        }
        break;

        case MC_SET_LIMITS:
        {
            uint8_t joint = payload[1];
            int32_t pos_min = *(int32_t*)(payload+4);
            int32_t pos_max = *(int32_t*)(payload+8);
            int32_t vel_max = *(int32_t*)(payload+12);
            
            eo_emsController_SetLimits(joint, pos_min, pos_max, vel_max);
        }
        break;

        case MC_SET_POS:
        {
            uint8_t joint = payload[1];
            int32_t pos = *(int32_t*)(payload+4);
            uint32_t time_ms = *(uint32_t*)(payload+8);

            eo_emsController_SetPosRef(joint, pos, time_ms);
        }
        break;

        case MC_SET_VEL:
        {
            uint8_t joint = payload[1];
            int32_t vel = *(int32_t*)(payload+4);
            uint32_t time_ms = *(uint32_t*)(payload+8);

            eo_emsController_SetVelRef(joint, vel, time_ms);
        }
        break;

        case MC_SET_POS_PID:
        {
            uint8_t joint = payload[1];
            int32_t k  = *(int32_t*)(payload+4);
            int32_t kd = *(int32_t*)(payload+8);
            int32_t ki = *(int32_t*)(payload+12);
            uint8_t shift = *(uint8_t*)(payload+16);
            float rescaler = 1.0f/(float)(1<<shift);

            eo_emsController_SetPosPid(joint, k*rescaler, kd*rescaler, ki*rescaler);
        }
        break;

        case MC_SET_POS_PID_LIM:
        {
            uint8_t joint = payload[1];
            int16_t Ymax = *(int16_t*)(payload+4);
            int16_t Imax = *(int16_t*)(payload+8);

            eo_emsController_SetPosPidLimits(joint, Ymax, Imax);
        }
        break;

        case MC_SET_OFFSET:
        {
            uint8_t joint = payload[1];
            int16_t off = *(int16_t*)(payload+4);
            
            eo_emsController_SetOffset(joint, off);
        }
        break;

        case MC_CONTROL_MODE:
        {
            uint8_t joint = payload[1];
            uint8_t cmode = payload[2];

            eo_emsController_SetControlMode(joint, (control_mode_t)cmode);
        }
        break;
    }
    
    return(eores_OK);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



