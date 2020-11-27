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

/* @file       EoProtocolAS_fun_ems4rd.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint as
    @author     valentina.gaggero@iit.it
    @date       06/11/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "hal.h"

#include "EoCommon.h"
#include "EOarray.h"

#include "EoAnalogSensors.h"


#include "EOMtheEMSappl.h"

#include "EOtheCANservice.h"


#include "EoProtocol.h"

#include "EoProtocolAS.h"


#include "EOtheEntities.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

#include "EOtheMAIS.h"
#include "EOtheSTRAIN.h"
#include "EOtheInertials2.h"
#include "EOtheInertials3.h"
#include "EOtheTemperatures.h"
#include "EOthePSC.h"
#include "EOthePOS.h"

#include "EOMtheEMStransceiver.h"

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


extern void eoprot_fun_UPDT_as_mais_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_mais_config_t *cfg = (eOas_mais_config_t*)rd->data;    
    eo_mais_Set(eo_mais_GetHandle(), cfg);
}


extern void eoprot_fun_UPDT_as_mais_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_maismode_t *maismode = (eOas_maismode_t*)rd->data;
    eo_mais_SetMode(eo_mais_GetHandle(), *maismode);
}


extern void eoprot_fun_UPDT_as_mais_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    uint8_t *datarate = (uint8_t*)rd->data;
    eo_mais_SetDataRate(eo_mais_GetHandle(), *datarate);
}


extern void eoprot_fun_UPDT_as_mais_config_resolution(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_maisresolution_t *resolution = (eOas_maisresolution_t*)rd->data;    
    eo_mais_SetResolution(eo_mais_GetHandle(), *resolution);  
}


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

extern void eoprot_fun_UPDT_as_inertial_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial_config_t *cfg = (eOas_inertial_config_t*)rd->data;    
    eo_inertials2_Config(eo_inertials2_GetHandle(), cfg);    
}


extern void eoprot_fun_UPDT_as_inertial_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_inertial_commands_t *cmd = (eOas_inertial_commands_t*)rd->data;  

    if(0 == cmd->enable)
    {
        eo_inertials2_Stop(eo_inertials2_GetHandle());    
    }
    else
    {
        eo_inertials2_Start(eo_inertials2_GetHandle());    
        eo_inertials2_Transmission(eo_inertials2_GetHandle(), eobool_true);
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



