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
#include "EOappTheDataBase.h" 


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
static eOresult_t s_eo_appMeasConv_getShiftValuesFromDB(EOappMeasConv *p);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappMeasConv* eo_appMeasConv_New(eOappMeasConv_cfg_t *cfg)
{
    eOresult_t res;
    EOappMeasConv *retptr = NULL;

    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOappMeasConv), 1);

    //save in obj its configuration
    memcpy(&(retptr->cfg), cfg, sizeof(eOappMeasConv_cfg_t));
    
    //the db contains shift values, but here i save a mirror copy in order to make conversion function faster.
    res = s_eo_appMeasConv_getShiftValuesFromDB(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }

    res = s_eo_appMeasConv_TableCareateAndInit(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    return(retptr);
}

extern eOresult_t eo_appMeasConv_SetJntEncoderConversionFactor(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_encConversionFactor_t convfactor)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
    
    p->jntEncConvDataList[jId].factor = convfactor;

    return(eores_OK);
}

extern eOresult_t eo_appMeasConv_SetJntEncoderConversionOffset(EOappMeasConv *p, eOmc_jointId_t jId, eOappMeasConv_encConversionOffset_t convoffset)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
    
    p->jntEncConvDataList[jId].offset = convoffset;
    
    return(eores_OK);
}


__weak extern eOmeas_position_t eo_appMeasConv_jntPosition_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t e_pos)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif     
    return((eOmeas_position_t)e_pos);
}

__weak extern eOicubCanProto_position_t eo_appMeasConv_jntPosition_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_position_t i_pos)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOicubCanProto_position_t)i_pos);
}

__weak extern eOmeas_velocity_t eo_appMeasConv_jntVelocity_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOmeas_velocity_t)e_vel);
}


__weak extern eOicubCanProto_velocity_t eo_appMeasConv_jntVelocity_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif

    return((eOicubCanProto_velocity_t)i_vel);
}

__weak extern eOicubCanProto_velocity_t eo_appMeasConv_jntVelocity_I2E_forSetVelRefMC4(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif

    return((eOicubCanProto_velocity_t)i_vel);
}


__weak extern eOmeas_acceleration_t eo_appMeasConv_jntAcceleration_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc)
{
#ifdef _APPMEASCONV_SAFE_    
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOmeas_acceleration_t)e_acc);
}

__weak extern eOicubCanProto_acceleration_t eo_appMeasConv_jntAcceleration_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOicubCanProto_acceleration_t)i_acc);
}


__weak extern eOicubCanProto_stiffness_t eo_appMeasConv_impedenceStiffness_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_stiffness_t i_stiff)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif

     
    return((eOicubCanProto_stiffness_t)i_stiff);
}


__weak extern eOmeas_stiffness_t eo_appMeasConv_impedenceStiffness_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_stiffness_t s_stiff)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOmeas_stiffness_t)s_stiff);
}


__weak extern eOicubCanProto_damping_t eo_appMeasConv_impedenceDamping_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_damping_t i_damping)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif

    return((eOicubCanProto_damping_t)i_damping);
}

__weak extern eOmeas_damping_t eo_appMeasConv_impedenceDamping_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_damping_t s_damping)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOmeas_damping_t)s_damping);
}


__weak extern eOicubCanProto_torque_t eo_appMeasConv_torque_I2S(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_torque_t i_torque)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
    
    return((eOicubCanProto_torque_t)i_torque);
}


__weak extern eOmeas_torque_t eo_appMeasConv_torque_S2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_torque_t s_torque)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
     
    return((eOmeas_torque_t)s_torque);
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

    /* 1) Create table */
    p->totalnumofjoint = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle());
    if(0 == p->totalnumofjoint)
    {
        return(eores_NOK_generic);
    }
    p->jntEncConvDataList = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOappMeasConv_encoderConversionData_t), p->totalnumofjoint);

    /* 2) Init table */
    for(i=0; i<p->totalnumofjoint; i++)
    {
        p->jntEncConvDataList[i].factor = eo_appMeasConv_hid_jntEncoderConvFactor_defaultValue;
        p->jntEncConvDataList[i].offset = eo_appMeasConv_hid_jntEncoderConvOffset_defaultValue;
    }
    
    return(eores_OK);
}


static eOresult_t s_eo_appMeasConv_getShiftValuesFromDB(EOappMeasConv *p)
{
    eOresult_t res;
    eOappTheDB_jointShiftValues_t *shiftval_ptr;
    
    res = eo_appTheDB_GetShiftValuesOfJointPtr(eo_appTheDB_GetHandle(), 0, &shiftval_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    memcpy(&p->shiftvalues, shiftval_ptr, sizeof(eOappTheDB_jointShiftValues_t));
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



