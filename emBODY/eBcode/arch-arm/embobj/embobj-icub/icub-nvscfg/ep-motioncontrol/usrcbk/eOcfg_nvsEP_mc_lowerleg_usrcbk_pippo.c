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

/* @file       eOcfg_nvsEP_mc_lowerleg_usrcbk_pippo.c
    @brief     This file keeps examples for ems / pc104 of the user-defined functions used for endpoint mc_lowerleg by
    @author     marco.accame@iit.it
    @date       05/02/2012
**/

#warning --> read this very important comment on the different use of th example in ems or pc104
// very important comment:
// the ems uses either the left or the right lowerleg endpoint, thus on ems it is clear that the nv->loc is relevant to 
// whcih joint.
// the pc104 has both the endpoints, even if loaded onto different nvscfg objects. however, the callback is the same for
// both of them. thus, the pc104 must discriminate versus the value of ep or of ip.


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "eOcfg_nvsEP_mc_lowerleg_usr_hid.h"

#include "eOcfg_nvsEP_mc_lowerleg_con.h"

#include "EOnv_hid.h"

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


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

extern void eo_cfg_nvsEP_mc_lowerleg_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    
    uint8_t j, m;
    eo_cfg_nvsEP_mc_lowerleg_t *lloc = (eo_cfg_nvsEP_mc_lowerleg_t*)loc;
    eo_cfg_nvsEP_mc_lowerleg_t *rrem = (eo_cfg_nvsEP_mc_lowerleg_t*)rem;    
 
    // in here we initailise the ram allocated by the EOnvscfg object: 
    // 1. at least put it at its default value. 
    // 2. if you need to initialise other peripherals or objects linked to the values do it.
    //    HOWEVER: initialisation of NVs is done on specific functions 
    
    
    // 1. assign default values to loc ..
    
    if(eobool_true == theOwnershipIsLocal)
    {   // the caller is the EOtheAgent when operates on a locally owned NV. Or more simply: by the ems board
    
        // initialise only the local memory.
        
        if(NULL == loc) 
        {
            return;
        }
        for(j=0; j<jointLowerLeg_TOTALnumber; j++)
        {
            memcpy(&lloc->joints[j], eo_cfg_nvsEP_mc_lowerleg_joint_defaultvalue, sizeof(eOmc_joint_t)); 
        }
        for(m=0; m<motorLowerLeg_TOTALnumber; m++)
        {
            memcpy(&lloc->motors[m], eo_cfg_nvsEP_mc_lowerleg_motor_defaultvalue, sizeof(eOmc_motor_t)); 
        } 
        
        
    } 
    else
    {   // the caller is the EOtheAgent when operates on a remotely owned NV. Or more simply: by the pc104
        
        // we can initialise both local and remote memory
        rrem = rrem;
        lloc = lloc;
    
    }
        
    
}



extern void eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Jxx_jconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t  jxx = (eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t) xx;
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;
    
    
    jxx = jxx;                                              // the joint number
    theOwnershipIsLocal = theOwnershipIsLocal;              // always eobool_true in ems
    ep = ep;                                                // the ep value helps discriminate if it is left or right (endpoint_mc_leftlowerleg or endpoint_mc_rightlowerleg)
    ip = ip;                                                // the ip is EO_COMMON_IPV4ADDR_LOCALHOST if owneship is local
    
    
    // the ems uses nv->loc to copy the config of the joint into relevant data structures ...
    eOmc_joint_config_t *jcfg = (eOmc_joint_config_t*)nv->loc;
    jcfg = jcfg;
    
    // the pc104 does not normally process this function. it does it only if it has received a sig or a say. in such a case it uses the nv->rem to use for what it wants
}


extern void eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Mxx_mconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_cfg_nvsEP_mc_lowerleg_con_motorNumber_t  mxx = (eo_cfg_nvsEP_mc_lowerleg_con_motorNumber_t) xx;
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;
    
    
    mxx = mxx;                                              // the motor number
    theOwnershipIsLocal = theOwnershipIsLocal;              // always eobool_true in ems
    ep = ep;                                                // the ep value helps discriminate if it is left or right (endpoint_mc_leftlowerleg or endpoint_mc_rightlowerleg)
    ip = ip;                                                // the ip is EO_COMMON_IPV4ADDR_LOCALHOST if owneship is local

    // the ems uses nv->loc to copy the config of the motor into relevant data structures ...
    eOmc_motor_config_t *mcfg = (eOmc_motor_config_t*)nv->loc;
    mcfg = mcfg;
    
    
    // the pc104 does not normally process this function. it does it only if it has received a sig or a say. in such a case it uses the nv->rem to use for what it wants    
}

extern void eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Jxx_jstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t  jxx = (eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t) xx;
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;
    
    
    jxx = jxx;                                              // the joint number
    theOwnershipIsLocal = theOwnershipIsLocal;              // always eobool_true in ems
    ep = ep;                                                // the ep value helps discriminate if it is left or right (endpoint_mc_leftlowerleg or endpoint_mc_rightlowerleg)
    ip = ip;                                                // the ip is the one of the sender of the rop (if owneship is remote)
    
    // the ems does not process this callback, as it it a ro variable which it can signalled to the pc104
    
    // the pc104 processes this callback when it receives a signalling from the ems. it uses the ep and the ip to understand who sent the message.
    // only two ems (one in left and one in right leg) can have sent this message. 
    eOmc_joint_status_t *jsta = (eOmc_joint_status_t*)nv->rem;
    jsta = jsta;
    
    if(endpoint_mc_leftlowerleg == ep)
    {
        // its ems eb6
    }
    else if(endpoint_mc_rightlowerleg == ep)
    {
        // it is ems eb8
    }
    
}


extern void eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Jxx_jcmmnds__calibration(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t  jxx = (eo_cfg_nvsEP_mc_lowerleg_con_jointNumber_t) xx;
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;
    
    
    jxx = jxx;                                              // the joint number
    theOwnershipIsLocal = theOwnershipIsLocal;              // always eobool_true in ems
    ep = ep; 
    ip = ip;                                                // the ip is EO_COMMON_IPV4ADDR_LOCALHOST if owneship is local    
    
    
    // the ems processes this callback when it receives a command from the pc104. in nv->loc the ems retrieves the value to use
    eOmc_calibrator_t *jcal = (eOmc_calibrator_t*)nv->loc;
    
    switch(jcal->type)
    {
        case eomc_calibration_type0_hard_stops:
        {
            // use jcal->params.type0
            int16_t             pwlimit     = jcal->params.type0.pwmlimit;
            eOmeas_velocity_t   velocity    = jcal->params.type0.velocity;
            
            pwlimit = pwlimit;
            velocity = velocity;
        } break;
        
        default:
        {        
        } break;   
    }
    
}


#if 0
// this function is common to all mc endpoint. if we define this one and we DO NOT redefine eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Jxx_jcmmnds__calibration,
// then the orginal eo_cfg_nvsEP_mc_lowerleg_usr_hid_UPDT_Jxx_jcmmnds__calibration() calls the following
extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOnvEP_t ep = nv->ep;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    ep = ep;
}
#endif






// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



