

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOarray.h"

#include "EoAnalogSensors.h"

#include "EOtheCANservice.h"
#include "EoProtocol.h"
#include "EoProtocolAS.h"


//#include "EOtheEntities.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

#if defined(USE_EMBOT_theServices)
#include "embot_app_eth_theServices.h"
#else
#include "EOtheServices.h"
#endif

#if defined(USE_EMBOT_theHandler)
#include "theApplication_config.h"
#include "theHandler_Config.h"
#include "embot_app_eth_theHandler.h"
#else
#include "EOMtheEMSappl.h"
#include "EOMtheEMSapplCfg.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void eoprot_fun_UPDT_as_strain_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_strain_config_t *cfg = (eOas_strain_config_t*)rd->data;
    eo_strain_Set(eo_strain_GetHandle(), cfg);       
}

extern void eoprot_fun_UPDT_as_strain_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_strainmode_t *mode = (eOas_strainmode_t*)rd->data;
    eo_strain_SetMode(eo_strain_GetHandle(), *mode);
    
}

extern void eoprot_fun_UPDT_as_strain_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint8_t *datarate = (uint8_t*)rd->data;
    eo_strain_SetDataRate(eo_strain_GetHandle(), *datarate);  
}


extern void eoprot_fun_UPDT_as_strain_config_signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eObool_t *signaloncefullscale = (eObool_t*)rd->data;
    if(eobool_true == *signaloncefullscale)
    {
        eo_strain_GetFullScale(eo_strain_GetHandle(), NULL);
    }
}


extern void eoprot_fun_UPDT_as_inertial3_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial3_config_t *cfg = (eOas_inertial3_config_t*)rd->data;    
    eo_inertials3_Config(eo_inertials3_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_inertial3_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial3_commands_t *cmd = (eOas_inertial3_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_inertials3_Stop(eo_inertials3_GetHandle());    
    }
    else
    {
        eo_inertials3_Start(eo_inertials3_GetHandle());    
        eo_inertials3_Transmission(eo_inertials3_GetHandle(), eobool_true);
    }
}


extern void eoprot_fun_UPDT_as_temperature_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_temperature_config_t *cfg = (eOas_temperature_config_t*)rd->data;    
    eo_temperatures_Config(eo_temperatures_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_temperature_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_temperature_commands_t *cmd = (eOas_temperature_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_temperatures_Stop(eo_temperatures_GetHandle());    
    }
    else
    {
        eo_temperatures_Start(eo_temperatures_GetHandle());    
        eo_temperatures_Transmission(eo_temperatures_GetHandle(), eobool_true);
    }
}


extern void eoprot_fun_UPDT_as_psc_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_psc_config_t *cfg = (eOas_psc_config_t*)rd->data;    
    eo_psc_Config(eo_psc_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_psc_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_psc_commands_t *cmd = (eOas_psc_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_psc_Stop(eo_psc_GetHandle());    
    }
    else
    {
        eo_psc_Start(eo_psc_GetHandle());    
        eo_psc_Transmission(eo_psc_GetHandle(), eobool_true);
    }
}


extern void eoprot_fun_UPDT_as_pos_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_pos_config_t *cfg = (eOas_pos_config_t*)rd->data;    
    eo_pos_Config(eo_pos_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_pos_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_pos_commands_t *cmd = (eOas_pos_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_pos_Stop(eo_pos_GetHandle());    
    }
    else
    {
        eo_pos_Start(eo_pos_GetHandle());    
        eo_pos_Transmission(eo_pos_GetHandle(), eobool_true);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



