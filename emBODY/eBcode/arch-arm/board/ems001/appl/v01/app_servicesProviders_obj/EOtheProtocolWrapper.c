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
    .initted                 = eobool_false
};

//static const char s_eobj_ownname[] = "EOtheProtocolWrapper";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheProtocolWrapper* eo_protocolwrapper_Initialise(void)
{
    s_eo_theprotocolwrapper.initted = eobool_true;    
    return(&s_eo_theprotocolwrapper);
}


extern EOtheProtocolWrapper* eo_protocolwrapper_GetHandle(void)
{
    return(eo_protocolwrapper_Initialise());
}


extern eOmc_joint_config_t * eo_protocolwrapper_GetJointConfig(EOtheProtocolWrapper *p, eOmc_jointId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOmc_joint_config_t *ret = NULL;    
    eOmc_joint_t *jo = (eOmc_joint_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)id);
    
    if(NULL != jo)
    {
        ret = &(jo->config);
    }

    return(ret);
}


extern eOmc_joint_status_t * eo_protocolwrapper_GetJointStatus(EOtheProtocolWrapper *p, eOmc_jointId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOmc_joint_status_t *ret = NULL;    
    eOmc_joint_t *jo = (eOmc_joint_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)id);
    
    if(NULL != jo)
    {
        ret = &(jo->status);
    }

    return(ret);
}

extern eOmc_motor_status_t * eo_protocolwrapper_GetMotorStatus(EOtheProtocolWrapper *p, eOmc_motorId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOmc_motor_status_t *ret = NULL;    
    eOmc_motor_t *mo = (eOmc_motor_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, (eOprotIndex_t)id);
    
    if(NULL != mo)
    {
        ret = &(mo->status);
    }

    return(ret);
}


extern eOas_mais_config_t * eo_protocolwrapper_GetMaisConfig(EOtheProtocolWrapper *p, eOas_maisId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOas_mais_config_t *ret = NULL;    
    eOas_mais_t *ma = (eOas_mais_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)id);
    
    if(NULL != ma)
    {
        ret = &(ma->config);
    }

    return(ret);
}


extern eOas_mais_status_t * eo_protocolwrapper_GetMaisStatus(EOtheProtocolWrapper *p, eOas_maisId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOas_mais_status_t *ret = NULL;    
    eOas_mais_t *ma = (eOas_mais_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)id);
    
    if(NULL != ma)
    {
        ret = &(ma->status);
    }

    return(ret);
}


extern eOas_strain_config_t * eo_protocolwrapper_GetStrainConfig(EOtheProtocolWrapper *p, eOas_strainId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOas_strain_config_t *ret = NULL;    
    eOas_strain_t *st = (eOas_strain_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)id);
    
    if(NULL != st)
    {
        ret = &(st->config);
    }

    return(ret);
}

extern eOas_strain_status_t * eo_protocolwrapper_GetStrainStatus(EOtheProtocolWrapper *p, eOas_strainId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOas_strain_status_t *ret = NULL;    
    eOas_strain_t *st = (eOas_strain_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)id);
    
    if(NULL != st)
    {
        ret = &(st->status);
    }

    return(ret);
}

extern EOarray_of_10canframes * eo_protocolwrapper_GetSkinStatusArray(EOtheProtocolWrapper *p, eOsk_skinId_t id)
{
    // don't do any control on p, as ... it is a dummy object
    EOarray_of_10canframes *ret = NULL;    
    eOsk_skin_t *sk = (eOsk_skin_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eosk_entity_skin, (eOprotIndex_t)id);
    
    if(NULL != sk)
    {
        ret = &(sk->status.arrayof10canframes);
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



