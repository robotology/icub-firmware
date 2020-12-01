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

#include "EoCommon.h"
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
    EO_INIT(.initted        ) eobool_false,
    EO_INIT(.joints         ) {NULL},
    EO_INIT(.motors         ) {NULL},
    EO_INIT(.skins          ) {NULL},
    EO_INIT(.strains        ) {NULL},
    EO_INIT(.maises         ) {NULL},
    EO_INIT(.temperatures   ) {NULL},
    EO_INIT(.inertials      ) {NULL},
    EO_INIT(.inertials3     ) {NULL},
    EO_INIT(.pscs           ) {NULL},
    EO_INIT(.poss           ) {NULL},
    EO_INIT(.numofjoints    ) 0,
    EO_INIT(.numofmotors    ) 0,
    EO_INIT(.numofskins     ) 0,
    EO_INIT(.numofstrains   ) 0,
    EO_INIT(.numofmaises    ) 0,
    EO_INIT(.numoftemperatures) 0,
    EO_INIT(.numofinertials ) 0,
    EO_INIT(.numofinertials3 ) 0,
    EO_INIT(.numofpscs ) 0,
    EO_INIT(.numofposs ) 0
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
    
    eo_entities_Reset(&s_eo_theentities);
    
    s_eo_theentities.initted = eobool_true;   
    
    return(&s_eo_theentities);
}


extern EOtheEntities* eo_entities_GetHandle(void)
{
    return(eo_entities_Initialise());
}

extern eOresult_t eo_entities_Reset(EOtheEntities *p)
{
    uint8_t i=0;
    uint8_t max = 0;
    
    memset(s_eo_theentities.joints, 0, sizeof(s_eo_theentities.joints));
    memset(s_eo_theentities.motors, 0, sizeof(s_eo_theentities.motors));
    memset(s_eo_theentities.skins, 0, sizeof(s_eo_theentities.skins));
    memset(s_eo_theentities.strains, 0, sizeof(s_eo_theentities.strains));
    memset(s_eo_theentities.maises, 0, sizeof(s_eo_theentities.maises));
    memset(s_eo_theentities.temperatures, 0, sizeof(s_eo_theentities.temperatures));
    memset(s_eo_theentities.inertials, 0, sizeof(s_eo_theentities.inertials));
    memset(s_eo_theentities.inertials3, 0, sizeof(s_eo_theentities.inertials3));
    memset(s_eo_theentities.pscs, 0, sizeof(s_eo_theentities.pscs));
    
    // joints
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    if(max>eoprotwrap_max_joints) max = eoprotwrap_max_joints;
    s_eo_theentities.numofjoints = 0;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.joints[i] = (eOmc_joint_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)i);
    }
    
    // motors
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
    s_eo_theentities.numofmotors = 0;
    if(max>eoprotwrap_max_motors) max = eoprotwrap_max_motors;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.motors[i] = (eOmc_motor_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, (eOprotIndex_t)i);
    } 
    
    // skins
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin);
    s_eo_theentities.numofskins = 0;
    if(max>eoprotwrap_max_skins) max = eoprotwrap_max_skins;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.skins[i] = (eOsk_skin_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin, (eOprotIndex_t)i);
    }  
    
    // strains
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain);
    s_eo_theentities.numofstrains = 0;
    if(max>eoprotwrap_max_strains) max = eoprotwrap_max_strains;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.strains[i] = (eOas_strain_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)i);
    }  
    
    // maises
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais);
    s_eo_theentities.numofmaises = 0;
    if(max>eoprotwrap_max_maises) max = eoprotwrap_max_maises;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.maises[i] = (eOas_mais_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)i);
    }    

    // temperatures
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_temperature);
    s_eo_theentities.numoftemperatures = 0;
    if(max>eoprotwrap_max_temperatures) max = eoprotwrap_max_temperatures;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.temperatures[i] = (eOas_temperature_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_temperature, (eOprotIndex_t)i);
    } 
    
    
    // inertials
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_inertial);
    s_eo_theentities.numofinertials = 0;
    if(max>eoprotwrap_max_inertials) max = eoprotwrap_max_inertials;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.inertials[i] = (eOas_inertial_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_inertial, (eOprotIndex_t)i);
    } 

    // inertials3
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_inertial3);
    s_eo_theentities.numofinertials3 = 0;
    if(max>eoprotwrap_max_inertials3) max = eoprotwrap_max_inertials3;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.inertials3[i] = (eOas_inertial3_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_inertial3, (eOprotIndex_t)i);
    } 

    // psc
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_psc);
    s_eo_theentities.numofpscs = 0;
    if(max>eoprotwrap_max_pscs) max = eoprotwrap_max_pscs;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.pscs[i] = (eOas_psc_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_psc, (eOprotIndex_t)i);
    } 
    
    // pos
    max = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_pos);
    s_eo_theentities.numofposs = 0;
    if(max>eoprotwrap_max_poss) max = eoprotwrap_max_poss;
    for(i=0; i<max; i++)
    {
        s_eo_theentities.poss[i] = (eOas_pos_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_pos, (eOprotIndex_t)i);
    } 
    
    return(eores_OK);
}

extern eOresult_t eo_entities_SetNumOfJoints(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_joints)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofjoints = num;
    
    return(eores_OK);   
}


extern uint8_t eo_entities_NumOfJoints(EOtheEntities *p)
{
    return(s_eo_theentities.numofjoints);    
}
    
extern eOmc_joint_t * eo_entities_GetJoint(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofjoints)
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

extern eOresult_t eo_entities_SetNumOfMotors(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_motors)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofmotors = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfMotors(EOtheEntities *p)
{
    return(s_eo_theentities.numofmotors);    
}

extern eOmc_motor_t * eo_entities_GetMotor(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofmotors)
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


extern eOresult_t eo_entities_SetNumOfMaises(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_maises)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofmaises = num;
    
    return(eores_OK);   
}


extern uint8_t eo_entities_NumOfMaises(EOtheEntities *p)
{
    return(s_eo_theentities.numofmaises);    
}

extern eOas_mais_t * eo_entities_GetMais(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofmaises)
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


extern eOresult_t eo_entities_SetNumOfTemperatures(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_temperatures)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numoftemperatures = num;
    
    return(eores_OK);   
}


extern uint8_t eo_entities_NumOfTemperatures(EOtheEntities *p)
{
    return(s_eo_theentities.numoftemperatures);    
}

extern eOas_temperature_t * eo_entities_GetTemperature(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numoftemperatures)
    {
        return(NULL);
    }

    return(s_eo_theentities.temperatures[id]);
}

extern eOas_temperature_config_t * eo_entities_GetTemperatureConfig(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_temperature_config_t *ret = NULL;    
    eOas_temperature_t *te = eo_entities_GetTemperature(p, id);
    
    if(NULL != te)
    {
        ret = &(te->config);
    }

    return(ret);
}


extern eOas_temperature_status_t * eo_entities_GetTemperatureStatus(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_temperature_status_t *ret = NULL;    
    eOas_temperature_t *te = eo_entities_GetTemperature(p, id);
    
    if(NULL != te)
    {
        ret = &(te->status);
    }

    return(ret);
}


extern eOresult_t eo_entities_SetNumOfInertials(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_inertials)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofinertials = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfInertials(EOtheEntities *p)
{
    return(s_eo_theentities.numofinertials);    
}

extern eOas_inertial_t * eo_entities_GetInertial(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofinertials)
    {
        return(NULL);
    }

    return(s_eo_theentities.inertials[id]);
}

extern eOas_inertial_config_t * eo_entities_GetInertialConfig(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_inertial_config_t *ret = NULL;    
    eOas_inertial_t *in = eo_entities_GetInertial(p, id);
    
    if(NULL != in)
    {
        ret = &(in->config);
    }

    return(ret);
}


extern eOas_inertial_status_t * eo_entities_GetInertialStatus(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_inertial_status_t *ret = NULL;    
    eOas_inertial_t *in = eo_entities_GetInertial(p, id);
    
    if(NULL != in)
    {
        ret = &(in->status);
    }

    return(ret);
}


extern eOresult_t eo_entities_SetNumOfInertials3(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_inertials3)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofinertials3 = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfInertials3(EOtheEntities *p)
{
    return(s_eo_theentities.numofinertials3);    
}

extern eOas_inertial3_t * eo_entities_GetInertial3(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofinertials3)
    {
        return(NULL);
    }

    return(s_eo_theentities.inertials3[id]);
}

extern eOas_inertial3_config_t * eo_entities_GetInertial3Config(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_inertial3_config_t *ret = NULL;    
    eOas_inertial3_t *in = eo_entities_GetInertial3(p, id);
    
    if(NULL != in)
    {
        ret = &(in->config);
    }

    return(ret);
}


extern eOas_inertial3_status_t * eo_entities_GetInertial3Status(EOtheEntities *p, eOprotIndex_t id)
{
    eOas_inertial3_status_t *ret = NULL;    
    eOas_inertial3_t *in = eo_entities_GetInertial3(p, id);
    
    if(NULL != in)
    {
        ret = &(in->status);
    }

    return(ret);
}



extern eOresult_t eo_entities_SetNumOfPSCs(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_pscs)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofpscs = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfPSCs(EOtheEntities *p)
{
    return(s_eo_theentities.numofpscs);    
}

extern eOas_psc_t * eo_entities_GetPSC(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofpscs)
    {
        return(NULL);
    }

    return(s_eo_theentities.pscs[id]);
}


extern eOresult_t eo_entities_SetNumOfPOSs(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_poss)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofposs = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfPOSs(EOtheEntities *p)
{
    return(s_eo_theentities.numofposs);    
}

extern eOas_pos_t * eo_entities_GetPOS(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofposs)
    {
        return(NULL);
    }

    return(s_eo_theentities.poss[id]);
}

extern eOresult_t eo_entities_SetNumOfStrains(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_strains)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofstrains = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfStrains(EOtheEntities *p)
{
    return(s_eo_theentities.numofstrains);    
}

extern eOas_strain_t * eo_entities_GetStrain(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofstrains)
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


extern eOresult_t eo_entities_SetNumOfSkins(EOtheEntities *p, uint8_t num)
{
    if(num > eoprotwrap_max_skins)
    {
        return(eores_NOK_generic);
    }
    
    s_eo_theentities.numofskins = num;
    
    return(eores_OK);   
}

extern uint8_t eo_entities_NumOfSkins(EOtheEntities *p)
{
    return(s_eo_theentities.numofskins);    
}

extern eOsk_skin_t * eo_entities_GetSkin(EOtheEntities *p, eOprotIndex_t id)
{
    if(id >= s_eo_theentities.numofskins)
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



