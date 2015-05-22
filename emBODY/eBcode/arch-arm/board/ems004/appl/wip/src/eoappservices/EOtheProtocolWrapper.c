/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "eOcommon.h"
#include "EOtheErrorManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheProtocolWrapper.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheProtocolWrapper_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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

static EOtheProtocolWrapper s_eo_theprotocolwrapper = 
{
    .initted        = eobool_false,
    .joints         = {NULL},
    .motors         = {NULL},
    .skins          = {NULL},
    .strains        = {NULL},
    .maises         = {NULL}
};

//static const char s_eobj_ownname[] = "EOtheProtocolWrapper";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheProtocolWrapper* eo_protocolwrapper_Initialise(void)
{
    
    if(eobool_true == s_eo_theprotocolwrapper.initted)
    {
        return(&s_eo_theprotocolwrapper);
    }
    
    uint8_t i=0;
    uint8_t max = 0;
    
    // joints
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    if(max>eoprotwrap_max_joints) max = eoprotwrap_max_joints;
    for(i=0; i<max; i++)
    {
        s_eo_theprotocolwrapper.joints[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)i);
    }
    
    // motors
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
    if(max>eoprotwrap_max_motors) max = eoprotwrap_max_motors;
    for(i=0; i<max; i++)
    {
        s_eo_theprotocolwrapper.motors[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, (eOprotIndex_t)i);
    } 
    
    // skins
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
    if(max>eoprotwrap_max_skins) max = eoprotwrap_max_skins;
    for(i=0; i<max; i++)
    {
        s_eo_theprotocolwrapper.skins[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin, (eOprotIndex_t)i);
    }  
    
    // strains
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
    if(max>eoprotwrap_max_strains) max = eoprotwrap_max_strains;
    for(i=0; i<max; i++)
    {
        s_eo_theprotocolwrapper.strains[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)i);
    }  
    
    // maises
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
    if(max>eoprotwrap_max_maises) max = eoprotwrap_max_maises;
    for(i=0; i<max; i++)
    {
        s_eo_theprotocolwrapper.maises[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)i);
    }    
    
    s_eo_theprotocolwrapper.initted = eobool_true;   
    
    return(&s_eo_theprotocolwrapper);
}


extern EOtheProtocolWrapper* eo_protocolwrapper_GetHandle(void)
{
    return(eo_protocolwrapper_Initialise());
}

extern eOmc_joint_t * eo_protocolwrapper_GetJoint(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_joints)
    {
        return(NULL);
    }

    return(s_eo_theprotocolwrapper.joints[id]);
}

extern eOmc_joint_config_t * eo_protocolwrapper_GetJointConfig(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOmc_joint_config_t *ret = NULL;    
    eOmc_joint_t *jo = eo_protocolwrapper_GetJoint(p, id);
    
    if(NULL != jo)
    {
        ret = &(jo->config);
    }

    return(ret);
}


extern eOmc_joint_status_t * eo_protocolwrapper_GetJointStatus(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOmc_joint_status_t *ret = NULL;    
    eOmc_joint_t *jo = eo_protocolwrapper_GetJoint(p, id);
    
    if(NULL != jo)
    {
        ret = &(jo->status);
    }

    return(ret);    
}


extern eOmc_motor_t * eo_protocolwrapper_GetMotor(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_motors)
    {
        return(NULL);
    }
   
    return(s_eo_theprotocolwrapper.motors[id]);
}

extern eOmc_motor_status_t * eo_protocolwrapper_GetMotorStatus(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOmc_motor_status_t *ret = NULL;    
    eOmc_motor_t *mo = eo_protocolwrapper_GetMotor(p, id);
    
    if(NULL != mo)
    {
        ret = &(mo->status);
    }

    return(ret);
}

extern eOas_mais_t * eo_protocolwrapper_GetMais(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_maises)
    {
        return(NULL);
    }

    return(s_eo_theprotocolwrapper.maises[id]);
}

extern eOas_mais_config_t * eo_protocolwrapper_GetMaisConfig(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOas_mais_config_t *ret = NULL;    
    eOas_mais_t *ma = eo_protocolwrapper_GetMais(p, id);
    
    if(NULL != ma)
    {
        ret = &(ma->config);
    }

    return(ret);
}


extern eOas_mais_status_t * eo_protocolwrapper_GetMaisStatus(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOas_mais_status_t *ret = NULL;    
    eOas_mais_t *ma = eo_protocolwrapper_GetMais(p, id);
    
    if(NULL != ma)
    {
        ret = &(ma->status);
    }

    return(ret);
}


extern eOas_strain_t * eo_protocolwrapper_GetStrain(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_strains)
    {
        return(NULL);
    }

    return(s_eo_theprotocolwrapper.strains[id]);
}

extern eOas_strain_config_t * eo_protocolwrapper_GetStrainConfig(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOas_strain_config_t *ret = NULL;    
    eOas_strain_t *st = eo_protocolwrapper_GetStrain(p, id);
    
    if(NULL != st)
    {
        ret = &(st->config);
    }

    return(ret);
}

extern eOas_strain_status_t * eo_protocolwrapper_GetStrainStatus(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOas_strain_status_t *ret = NULL;    
    eOas_strain_t *st = eo_protocolwrapper_GetStrain(p, id);
    
    if(NULL != st)
    {
        ret = &(st->status);
    }

    return(ret);
}


extern eOsk_skin_t * eo_protocolwrapper_GetSkin(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_skins)
    {
        return(NULL);
    }

    return(s_eo_theprotocolwrapper.skins[id]);
}

extern eOsk_status_t * eo_protocolwrapper_GetSkinStatus(EOtheProtocolWrapper *p, eOprotIndex_t id)
{
    eOsk_status_t *ret = NULL;    
    eOsk_skin_t *sk = eo_protocolwrapper_GetSkin(p, id);
    
    if(NULL != sk)
    {
        ret = &(sk->status);
    }

    return(ret);
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



