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

#include "EOtheEntities.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEntities_hid.h"


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

static EOtheEntities s_eo_theentities = 
{
    .initted        = eobool_false,
    .joints         = {NULL},
    .motors         = {NULL},
    .skins          = {NULL},
    .strains        = {NULL},
    .maises         = {NULL},
    .numofjoints    = 0,
    .numofmotors    = 0,
    .numofskins     = 0,
    .numofstrains   = 0,
    .numofmaises    = 0
};

//static const char s_eobj_ownname[] = "EOtheEntities";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheEntities* eo_entities_Initialise(void)
{
    
    if(eobool_true == s_eo_theentities.initted)
    {
        return(&s_eo_theentities);
    }
    
    eo_entities_Refresh(&s_eo_theentities);
    
    s_eo_theentities.initted = eobool_true;   
    
    return(&s_eo_theentities);
}


extern EOtheEntities* eo_entities_GetHandle(void)
{
    return(eo_entities_Initialise());
}

extern eOresult_t eo_entities_Refresh(EOtheEntities *p)
{
    uint8_t i=0;
    uint8_t max = 0;
    
    memset(s_eo_theentities.joints, 0, sizeof(s_eo_theentities.joints));
    memset(s_eo_theentities.motors, 0, sizeof(s_eo_theentities.motors));
    memset(s_eo_theentities.skins, 0, sizeof(s_eo_theentities.skins));
    memset(s_eo_theentities.strains, 0, sizeof(s_eo_theentities.strains));
    memset(s_eo_theentities.maises, 0, sizeof(s_eo_theentities.maises));
    
    // joints
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    if(max>eoprotwrap_max_joints) max = eoprotwrap_max_joints;
    s_eo_theentities.numofjoints = max;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.joints[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)i);
    }
    
    // motors
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
    s_eo_theentities.numofmotors = max;
    if(max>eoprotwrap_max_motors) max = eoprotwrap_max_motors;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.motors[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, (eOprotIndex_t)i);
    } 
    
    // skins
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
    s_eo_theentities.numofskins = max;
    if(max>eoprotwrap_max_skins) max = eoprotwrap_max_skins;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.skins[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin, (eOprotIndex_t)i);
    }  
    
    // strains
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
    s_eo_theentities.numofstrains = max;
    if(max>eoprotwrap_max_strains) max = eoprotwrap_max_strains;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.strains[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)i);
    }  
    
    // maises
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
    s_eo_theentities.numofmaises = max;
    if(max>eoprotwrap_max_maises) max = eoprotwrap_max_maises;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.maises[i] = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)i);
    }    
    
    return(eores_OK);
}

extern uint8_t eo_entities_NumOfJoints(EOtheEntities *p)
{
    return(s_eo_theentities.numofjoints);    
}
    
extern eOmc_joint_t * eo_entities_GetJoint(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_joints)
    {
        return(NULL);
    }

    return(s_eo_theentities.joints[id]);
}

extern eOmc_joint_config_t * eo_entities_GetJointConfig(EOtheEntities *p, eOprotIndex_t id)
{
    eOmc_joint_config_t *ret = NULL;    
    eOmc_joint_t *jo = eo_entities_GetJoint(p, id);
    
    if(NULL != jo)
    {
        ret = &(jo->config);
    }

    return(ret);
}


extern eOmc_joint_status_t * eo_entities_GetJointStatus(EOtheEntities *p, eOprotIndex_t id)
{
    eOmc_joint_status_t *ret = NULL;    
    eOmc_joint_t *jo = eo_entities_GetJoint(p, id);
    
    if(NULL != jo)
    {
        ret = &(jo->status);
    }

    return(ret);    
}


extern uint8_t eo_entities_NumOfMotors(EOtheEntities *p)
{
    return(s_eo_theentities.numofmotors);    
}

extern eOmc_motor_t * eo_entities_GetMotor(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_motors)
    {
        return(NULL);
    }
   
    return(s_eo_theentities.motors[id]);
}

extern eOmc_motor_status_t * eo_entities_GetMotorStatus(EOtheEntities *p, eOprotIndex_t id)
{
    eOmc_motor_status_t *ret = NULL;    
    eOmc_motor_t *mo = eo_entities_GetMotor(p, id);
    
    if(NULL != mo)
    {
        ret = &(mo->status);
    }

    return(ret);
}


extern uint8_t eo_entities_NumOfMaises(EOtheEntities *p)
{
    return(s_eo_theentities.numofmaises);    
}

extern eOas_mais_t * eo_entities_GetMais(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_maises)
    {
        return(NULL);
    }

    return(s_eo_theentities.maises[id]);
}

extern eOas_mais_config_t * eo_entities_GetMaisConfig(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_mais_config_t *ret = NULL;    
    eOas_mais_t *ma = eo_entities_GetMais(p, id);
    
    if(NULL != ma)
    {
        ret = &(ma->config);
    }

    return(ret);
}


extern eOas_mais_status_t * eo_entities_GetMaisStatus(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_mais_status_t *ret = NULL;    
    eOas_mais_t *ma = eo_entities_GetMais(p, id);
    
    if(NULL != ma)
    {
        ret = &(ma->status);
    }

    return(ret);
}

extern uint8_t eo_entities_NumOfStrains(EOtheEntities *p)
{
    return(s_eo_theentities.numofstrains);    
}

extern eOas_strain_t * eo_entities_GetStrain(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_strains)
    {
        return(NULL);
    }

    return(s_eo_theentities.strains[id]);
}

extern eOas_strain_config_t * eo_entities_GetStrainConfig(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_strain_config_t *ret = NULL;    
    eOas_strain_t *st = eo_entities_GetStrain(p, id);
    
    if(NULL != st)
    {
        ret = &(st->config);
    }

    return(ret);
}

extern eOas_strain_status_t * eo_entities_GetStrainStatus(EOtheEntities *p, eOprotIndex_t id)
{
    // don't do any control on p, as ... it is a dummy object
    eOas_strain_status_t *ret = NULL;    
    eOas_strain_t *st = eo_entities_GetStrain(p, id);
    
    if(NULL != st)
    {
        ret = &(st->status);
    }

    return(ret);
}


extern uint8_t eo_entities_NumOfSkins(EOtheEntities *p)
{
    return(s_eo_theentities.numofskins);    
}

extern eOsk_skin_t * eo_entities_GetSkin(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= eoprotwrap_max_skins)
    {
        return(NULL);
    }

    return(s_eo_theentities.skins[id]);
}

extern eOsk_status_t * eo_entities_GetSkinStatus(EOtheEntities *p, eOprotIndex_t id)
{
    eOsk_status_t *ret = NULL;    
    eOsk_skin_t *sk = eo_entities_GetSkin(p, id);
    
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



