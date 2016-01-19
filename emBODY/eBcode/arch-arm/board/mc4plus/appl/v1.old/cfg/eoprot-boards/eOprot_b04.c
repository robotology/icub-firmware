/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       eOprot_b04.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"


#include "EOconstvector_hid.h"


#include "EoProtocolAS.h"
#include "EoProtocolMC.h"
#include "EoProtocolMN.h"
#include "EoProtocolSK.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eOprot_b04.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

EO_VERIFYproposition(eoprot_b04_gasdfe, eoprot_boards_maxnumberof > eoprot_b04_boardnumber);



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static uint16_t s_eoprot_b04_ep2index(void* p, eOnvEP8_t ep);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOnvset_EPcfg_t s_eoprot_b04_theEPcfgs[] =
{  
    {   // management
        EO_INIT(.endpoint)                          eoprot_endpoint_management,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b04_management_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_mn     
    },        

    {   // motion-control         
        EO_INIT(.endpoint)                          eoprot_endpoint_motioncontrol,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b04_motioncontrol_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_mc      
    },
 
    {   // analog-sensors         
        EO_INIT(.endpoint)                          eoprot_endpoint_analogsensors,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b04_analogsensors_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_as   
    },

    {   // skin         
        EO_INIT(.endpoint)                          eoprot_endpoint_skin,
        EO_INIT(.dummy)                             0,
        EO_INIT(.epram_sizeof)                      sizeof(eOprot_b04_skin_t),
        EO_INIT(.fptr_ram_initialise)               eoprot_fun_INITIALISE_sk         
    }     
    
};  EO_VERIFYsizeof(s_eoprot_b04_theEPcfgs, sizeof(eOnvset_EPcfg_t)*(eoprot_b04_endpoints_numberof));



static const EOconstvector s_eoprot_b04_constvectofEPcfg = 
{
    EO_INIT(.size)                  sizeof(s_eoprot_b04_theEPcfgs)/sizeof(const eOnvset_EPcfg_t),
    EO_INIT(.item_size)             sizeof(eOnvset_EPcfg_t),
    EO_INIT(.item_array_data)       s_eoprot_b04_theEPcfgs
};



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

const eOnvset_DEVcfg_t eoprot_b04_nvsetDEVcfg =
{
    EO_INIT(.boardnum)                  eoprot_b04_boardnumber,
    EO_INIT(.dummy)                     {0, 0, 0},
    EO_INIT(.param)                     NULL,
    EO_INIT(.fptr_device_initialise)    eoprot_b04_Initialise,     
    EO_INIT(.vectorof_epcfg)            &s_eoprot_b04_constvectofEPcfg,
    EO_INIT(.fptr_ep2indexofepcfg)      s_eoprot_b04_ep2index
};



const uint8_t eoprot_b04_mn_entities_numberofeach[eomn_entities_numberof] = 
{ 
    eoprot_b04_mn_comms_numberof, 
    eoprot_b04_mn_appls_numberof,
    eoprot_b04_mn_infos_numberof
};

const uint8_t eoprot_b04_mc_entities_numberofeach[eomc_entities_numberof] = 
{ 
    eoprot_b04_mc_joints_numberof, 
    eoprot_b04_mc_motors_numberof, 
    eoprot_b04_mc_controllers_numberof
};

const uint8_t eoprot_b04_as_entities_numberofeach[eoas_entities_numberof] = 
{ 
    eoprot_b04_as_strains_numberof, 
    eoprot_b04_as_maises_numberof,
    eoprot_b04_as_extorque_numberof
};

const uint8_t eoprot_b04_sk_entities_numberofeach[eosk_entities_numberof] = 
{ 
    eoprot_b04_sk_skins_numberof
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eoprot_b04_Initialise(void* p, eObool_t islocal)
{
    // must initialise the mc, the mn, the ...
    
    eoprot_config_endpoint_entities(eoprot_b04_boardnumber, eoprot_endpoint_management, eoprot_b04_mn_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b04_boardnumber, eoprot_endpoint_motioncontrol, eoprot_b04_mc_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b04_boardnumber, eoprot_endpoint_analogsensors, eoprot_b04_as_entities_numberofeach);
    eoprot_config_endpoint_entities(eoprot_b04_boardnumber, eoprot_endpoint_skin, eoprot_b04_sk_entities_numberofeach);
    
    
    if(eobool_true == islocal)
    {
        eoprot_config_board_local(eoprot_b04_boardnumber);
        eoprot_config_proxied_variables(eoprot_b04_boardnumber, eoprot_b04_isvariableproxied);
    }
    
    return(eores_OK);
}

/*NOTE: proxied variable can be used only in EMS4 because of memory missing in ems001 */
#ifdef USE_PROTO_PROXY
extern eObool_t eoprot_b04_isvariableproxied(eOnvID32_t id)
{    
    //currently proxied net vars are regarding joints only.
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id);
    if(eoprot_endpoint_motioncontrol != ep)
    {
        return(eobool_false);
    }
    
    eOprotEntity_t ent = eoprot_ID2entity(id);
    if(eoprot_entity_mc_joint != ent)
    {
        return(eobool_false);
    }
    
    eOprotTag_t tag = eoprot_ID2tag(id);
    
    switch(tag)
    {
        //VALE get velocity pid not implemented!!!
        case eoprot_tag_mc_joint_config_pidposition:
        // case eoprot_tag_mc_joint_config_pidvelocity:     // marco.accame on 03mar15: the pidvelocity propagation to mc4 is is not implemented, thus i must remove from proxy.
        case eoprot_tag_mc_joint_config_pidtorque:
        case eoprot_tag_mc_joint_config_limitsofjoint:
        case eoprot_tag_mc_joint_config_impedance:
        case eoprot_tag_mc_joint_cmmnds_setpoint:           // marco.accame on 03mar15: the setpoint should not be asked, thus why in here? i may just remove the handler so that no reply is obtained if wrongly used
        {
            return(eobool_true);
        }
        
        default:
        {
            return(eobool_false);
        };
     };
}
#else
extern eObool_t eoprot_b04_isvariableproxied(eOnvID32_t id)
{
    return(eobool_false);
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


EO_VERIFYproposition(s_eoprot_b04_mn_val, 0 == eoprot_endpoint_management);
EO_VERIFYproposition(s_eoprot_b04_mc_val, 1 == eoprot_endpoint_motioncontrol);
EO_VERIFYproposition(s_eoprot_b04_as_val, 2 == eoprot_endpoint_analogsensors);
EO_VERIFYproposition(s_eoprot_b04_sk_val, 3 == eoprot_endpoint_skin);

static uint16_t s_eoprot_b04_ep2index(void* p, eOnvEP8_t ep)
{    
    if(ep < eoprot_b04_endpoints_numberof)
    {
        return(ep);
    }
    return(EOK_uint16dummy);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



