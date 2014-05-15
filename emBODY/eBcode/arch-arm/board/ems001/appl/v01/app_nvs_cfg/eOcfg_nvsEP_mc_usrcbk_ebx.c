/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       eOcfg_nvsEP_mc_usrcbk_ebx.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mc
    @author     valentina.gaggero@iit.it
    @date       05/04/2012
**/





// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv_hid.h"

#include "EOMotionControl.h"
//#include "eOcfg_nvsEP_mc.h"

// #include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
// #include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"


//application
#include "EOMtheEMSapplCfg_cfg.h" // to see EOMTHEEMSAPPLCFG_*
#include "EOtheEMSapplBody.h"
#include "EOappTheDataBase.h"
#include "EOicubCanProto_specifications.h"
#include "EOappMeasuresConverter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mngmnt_usr_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
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

const eOmc_joint_t joint_default_value =
{
    EO_INIT(.config)             
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
        EO_INIT(.limitsofjoint)
        {
            EO_INIT(.min)                   0,
            EO_INIT(.max)                   0
        },
        EO_INIT(.impedance)
        {
            EO_INIT(.stiffness)             0,
            EO_INIT(.damping)               0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler02)              {0xf1, 0xf2}           
        },        
        
        EO_INIT(.velocitysetpointtimeout)   0,
       
        EO_INIT(.motionmonitormode)         eomc_motionmonitormode_dontmonitor,
        EO_INIT(.filler01)                 0xe0,
        EO_INIT(.encoderconversionfactor)   EOUTIL_EMULFLOAT32_ONE,
        EO_INIT(.encoderconversionoffset)   EOUTIL_EMULFLOAT32_ZERO,
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.basic)
        {
            EO_INIT(.position)              0,
            EO_INIT(.velocity)              0,       
            EO_INIT(.acceleration)          0,     
            EO_INIT(.torque)                0,
            EO_INIT(.motionmonitorstatus)   eomc_motionmonitorstatus_notmonitored,
            EO_INIT(.controlmodestatus)      eomc_controlmode_idle,
        },
        EO_INIT(.ofpid)                     {0},
        EO_INIT(.interactionmodestatus)     eomc_imodeval_stiff,
        EO_INIT(.chamaleon03)               {0} //{0xd1, 0xd2, 0xd3}
    },
    EO_INIT(.inputs)                       {0},
    EO_INIT(.cmmnds)                       
	{
		EO_INIT(.calibration)               {0},
		EO_INIT(.setpoint)                  {0},
		EO_INIT(.stoptrajectory)            0,
		EO_INIT(.controlmode)				eomc_controlmode_cmd_switch_everything_off,
        EO_INIT(.interactionmode)           eomc_imodeval_stiff,
        EO_INIT(.filler01)                  0        
	}
}; 



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
#if  defined(EOMTHEEMSAPPLCFG_USE_EB1)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c"
 #elif   defined(EOMTHEEMSAPPLCFG_USE_EB2)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_mc4.c"  
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB3)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB4)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_mc4.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c" 
#elif  defined(EOMTHEEMSAPPLCFG_USE_EB6)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB8)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c"
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB9)
      #include    "eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c"
#else
    #warning --> no eth proto user callback defined!!!
#endif

//this file is equal for all ems except eb10 and eb11
#if !defined(EOMTHEEMSAPPLCFG_USE_EB10) & !defined(EOMTHEEMSAPPLCFG_USE_EB11)
    #include    "eOcfg_nvsEP_mc_motor_usrcbk_ebx.c"
#endif
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



