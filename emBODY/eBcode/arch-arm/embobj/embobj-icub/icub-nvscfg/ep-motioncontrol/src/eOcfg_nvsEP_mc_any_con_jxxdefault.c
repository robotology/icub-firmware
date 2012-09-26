/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       eOcfg_nvsEP_mc_any_con_jxxdefault.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       04/06/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoMotionControl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
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


const eOmc_joint_t eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue =
{
    EO_INIT(.jconfig)             
    {
        EO_INIT(.pidposition)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.pidvelocity)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.pidtorque)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler03)              {0xf1, 0xf2, 0xf3}
        }, 
        EO_INIT(.impedance)
        {
            EO_INIT(.stiffness)             0,
            EO_INIT(.damping)               0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler02)              {0xf1, 0xf2}           
        },        
        EO_INIT(.minpositionofjoint)        0,
        EO_INIT(.maxpositionofjoint)        0,
        EO_INIT(.velocitysetpointtimeout)   0,
        EO_INIT(.holder01FFU00)             0xe0,
        EO_INIT(.motionmonitormode)         eomc_motionmonitormode_dontmonitor,
        EO_INIT(.des02FORjstatuschamaleon04)   {{EOUTIL_CHAMELEON_DESCRIPTOR_SIZE_NONE,EOUTIL_CHAMELEON_DESCRIPTOR_IDEN_NONE}, {EOUTIL_CHAMELEON_DESCRIPTOR_SIZE_NONE, EOUTIL_CHAMELEON_DESCRIPTOR_IDEN_NONE}},////{EOUTIL_CHAMELEON_DESCRIPTOR_NONE, EOUTIL_CHAMELEON_DESCRIPTOR_NONE},
        EO_INIT(.encoderconversionfactor)   EOUTIL_EMULFLOAT32_ONE,
        EO_INIT(.encoderconversionoffset)   EOUTIL_EMULFLOAT32_ZERO,
        EO_INIT(.holder01FFU01)             0xe1,
        EO_INIT(.holder01FFU02)             0xe2,
        EO_INIT(.holder02FFU03)             0xe3e3,
        EO_INIT(.holder02FFU04)             0xe4e3
    },
    EO_INIT(.jstatus)                       {0},
    EO_INIT(.jinputs)                       {0},
    EO_INIT(.jcmmnds)                       
	{
		EO_INIT(.calibration)               {0},
		EO_INIT(.setpoint)                  {0},
		EO_INIT(.stoptrajectory)            0,
		EO_INIT(.controlmode)				eomc_controlmode_switch_everything_off
	}
}; 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
