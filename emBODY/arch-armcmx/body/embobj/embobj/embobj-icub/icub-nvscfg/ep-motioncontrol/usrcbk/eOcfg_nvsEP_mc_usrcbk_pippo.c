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

/* @file       eOcfg_nvsEP_mc_usrcbk_pippo.c
    @brief     This file keeps examples for ems / pc104 of the user-defined functions used for all endpoints in mc
    @author     marco.accame@iit.it
    @date       05/02/2012
**/

#warning --> read this very important comment on the different use of group or general callbacks
// very important comment:
// the EOtheAgent calls a default callback which is specific of a given _usr group (upperarm, lowerarm, torso, upperleg, lowerleg).
// inside this callback it is possible to understand which endpoint is (left or right) by evaluating argument ep.
// the default group callback then calls the default motioncontrol callback. the same mechanism is used to understand which is the relevant 
// endpoint.
// the two default callbacks do nothing else.
// the user can override one of the two by simply defining a new function with the same name and declaration.
// if the more specific group callback is redefined, then the more general is not called anymore for those groups which found this overriden.
// RULE TO USE: override only the general or the only the group ones. If the genral ones are overridden it is possible to write more general
//              code inside the ems.

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "eOcfg_nvsEP_mc_hid.h"
#include "EOnv_hid.h"

//#include "eOcfg_nvsEP_mc_leg_con.h"
//#include "eOcfg_nvsEP_mc_upperleg_con.h"
//#include "eOcfg_nvsEP_mc_lowerleg_con.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------



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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// this function is common to all mc endpoint. if we define this one and we DO NOT redefine eo_cfg_nvsEP_mc_XXXX_usr_hid_UPDT_Jxx_jcmmnds__calibration,
// where XXXX = [lowerleg, upperleg, lowerarm, upperarm, torso], then the orginal eo_cfg_nvsEP_mc_XXXX_usr_hid_UPDT_Jxx_jcmmnds__calibration() calls 
// the following eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration() when the nv is changed by a rop.
// by redefining ONLY the more general eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(), every ems could use the very same code irrespectively of the
// motion control endpoints whcih has loaded.

// unfortunately, as eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Jxx_jcmmnds__calibration() and eo_cfg_nvsEP_mc_upperleg_usr_hid_UPDT_Jxx_jcmmnds__calibration()
// have been re-defined in files eOcfg_nvsEP_mc_lowerleg_usrcbk_pippo.c and eOcfg_nvsEP_mc_upperleg_usrcbk_pippo.c, ...
// the following eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration() is effectively called only on mc endpoints of arm and torso.

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;
    
    
    jxx = jxx;                                              // the joint number 
    theOwnershipIsLocal = theOwnershipIsLocal;              // always eobool_true in ems
    ep = ep; 
    ip = ip;                                                // the ip is EO_COMMON_IPV4ADDR_LOCALHOST if owneship is local    
    
    
    // the ems processes this callback when it receives a command from the pc104. in nv->loc the ems retrieves the value to use
    eOmc_calibrator_t *jcal = (eOmc_calibrator_t*)nv->loc;
    
    // use jcal
    jcal = jcal;
    
}

typedef struct
{
    uint32_t dummy1;
    uint32_t dummy2;
} MYmotionController;

static void motioncontroller_setpoint(MYmotionController *m, eOmc_setpoint_t *setp)
{
    switch(setp->type)
    {
        case eomc_setpoint_position:    
        {
            m->dummy1 = setp->to.position.value;
            m->dummy2 = setp->to.position.withvelocity;
        } break;
        
        default:
        {
            
        } break;
    }
}

static MYmotionController themotioncontrollers[3];

// the following function is called (nad NOT the more specific one) when a setpoint is received in all the mc endpoints of teh whole body.
// this function could be used to make the c code more generic inside the ems.
extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;
    
    
    jxx = jxx;                                              // the joint number 
    theOwnershipIsLocal = theOwnershipIsLocal;              // always eobool_true in ems
    ep = ep; 
    ip = ip;                                                // the ip is EO_COMMON_IPV4ADDR_LOCALHOST if owneship is local    
    
    
    // the ems processes this callback when it receives a command from the pc104. in nv->loc the ems retrieves the value to use
    eOmc_setpoint_t *jsetpoint = (eOmc_setpoint_t*)nv->loc;
    
    // use jsetpoint
    switch(jsetpoint->type)
    {
        case eomc_setpoint_position:    
        {
            // use with the position motionController on joint number jxx (from 0 to ....)
            eOmeas_position_t pos = jsetpoint->to.position.value;
            eOmeas_velocity_t vel = jsetpoint->to.position.withvelocity;
            pos = pos;
            vel  = vel;
            motioncontroller_setpoint(&themotioncontrollers[jxx], jsetpoint);
        } break;
    
    
    } 
    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------












// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




