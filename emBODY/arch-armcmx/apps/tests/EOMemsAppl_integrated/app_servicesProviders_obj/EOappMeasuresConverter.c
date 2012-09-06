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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       EOappMeasuresConverter.c
    @brief     This file implements measures converter.
    @author    valentina.gaggero@iit.it
    @date      08/24/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"

#include "EOtheMemoryPool.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappMeasuresConverter.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappMeasuresConverter_hid.h"


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
static eOresult_t s_eo_appMeasConv_TableCareateAndInit(EOappMeasConv *p);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappMeasConv* eo_appMeasConv_New(eOappMeasConv_cfg_t *cfg)
{
    EOappMeasConv *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOappMeasConv), 1);

    //save in obj its configuration
    memcpy(&(retptr->cfg), cfg, sizeof(eOappMeasConv_cfg_t));

    s_eo_appMeasConv_TableCareateAndInit(retptr);
    
    return(retptr);
}

extern eOresult_t eo_appMeasConv_SetJntEncoderPropRatio(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_propRatio_t propRatio)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    
    p->jntEncoderPropRatioList[jId].propRatio = propRatio;

    return(eores_OK);
}

extern eOresult_t eo_appMeasConv_SetJntEncoderZero(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_encoderZero_t zero)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    
    p->jntEncoderPropRatioList[jId].zero = zero;
    
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_jntPosition_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t e_pos, eOmeas_position_t *i_pos_ptr)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == i_pos_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *i_pos_ptr = e_pos; 
     
    return(eores_OK);
}

__weak extern eOresult_t eo_appMeasConv_jntPosition_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t *e_pos_ptr, eOmeas_position_t i_pos)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == e_pos_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *e_pos_ptr = i_pos; 
     
    return(eores_OK);
}

__weak extern eOresult_t eo_appMeasConv_jntVelocity_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel, eOmeas_velocity_t *i_vel_ptr)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == i_vel_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *i_vel_ptr = e_vel; 
     
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_jntVelocity_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t *e_vel_ptr, eOmeas_velocity_t i_vel)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == e_vel_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *e_vel_ptr = i_vel; 
     
    return(eores_OK);
}

__weak extern eOresult_t eo_appMeasConv_jntAcceleration_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc, eOmeas_velocity_t *i_acc_ptr)
{
#ifdef _APPMEASCONV_SAFE_    
    if((NULL == p) || (NULL == i_acc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *i_acc_ptr = e_acc; 
     
    return(eores_OK);
}

__weak extern eOresult_t eo_appMeasConv_jntAcceleration_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t *e_acc_ptr, eOmeas_velocity_t i_acc)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == e_acc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *e_acc_ptr = i_acc; 
     
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_impedenceStiffness_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_stiffness_t *s_stiff, eOmeas_stiffness_t i_stiff)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == s_stiff))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *s_stiff = i_stiff; 
     
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_impedenceStiffness_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_stiffness_t s_stiff, eOmeas_stiffness_t *i_stiff)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == i_stiff))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *i_stiff = s_stiff; 
     
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_impedenceDamping_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_damping_t *s_damping, eOmeas_damping_t i_damping)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == s_damping))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *s_damping = i_damping; 
     
    return(eores_OK);
}

__weak extern eOresult_t eo_appMeasConv_impedenceDamping_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_damping_t s_damping, eOmeas_damping_t *i_damping)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == i_damping))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *i_damping = s_damping; 
     
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_torque_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_torque_t *s_torque, eOmeas_torque_t i_torque)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == s_torque))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *s_torque = i_torque; 
     
    return(eores_OK);
}


__weak extern eOresult_t eo_appMeasConv_torque_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_torque_t s_torque, eOmeas_torque_t *i_torque)
{
#ifdef _APPMEASCONV_SAFE_
    if((NULL == p) || (NULL == i_torque))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= jointNumberMAX)
    {
        return(eores_NOK_nodata);
    }
#endif
    *i_torque = s_torque; 
     
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_appMeasConv_TableCareateAndInit(EOappMeasConv *p)
{
    uint8_t i;
    uint16_t jnt_num = jointNumberMAX;

//     /* 1) Create table */
//     jnt_num = eo_cfg_nvsEP_mc_joint_numbermax_Get(eOcfg_nvsEP_mc_endpoint_t ep);
//     p->jntEncoderPropRatioList = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOappMeasConv_encoderConversionData_t), jnt_num);

    /* 2) Init table */
    for(i=0; i<jnt_num; i++)
    {
        p->jntEncoderPropRatioList[i].propRatio = eOappMeasConv_hid_jntEncoderPropRatioDefaultValue;
        p->jntEncoderPropRatioList[i].zero = eOappMeasConv_hid_jntEncoderZeroDefaultValue;
    }
    
    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



