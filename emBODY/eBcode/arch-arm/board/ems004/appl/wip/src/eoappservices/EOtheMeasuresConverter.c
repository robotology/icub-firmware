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

/* @file       EOtheMeasuresConverter.c
    @brief     This file implements measures converter.
    @author    valentina.gaggero@iit.it
    @date      08/24/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
//#include "math.h"

#include "EOtheMemoryPool.h"
//#include "EOappTheDataBase.h"
#include "EOtheErrorManager.h"

#include "EoProtocol.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOtheMeasuresConverter.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMeasuresConverter_hid.h"


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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_measconv_TableCreateAndInit(EOtheMeasuresConverter *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMeasuresConverter s_eo_appmeas_singleton =
{
    .cfg                                = {0},
    .jointVelocityShift                 = 0,
    .jointVelocityEstimationShift       = 0,
    .jointAccEstimationShift            = 0,
    .totalnumofjoint                    = 0,
    .jntEncConvDataList                 = NULL          
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOtheMeasuresConverter* eo_measconv_Initialise(eOmeasconv_cfg_t *cfg)
{
    eOresult_t res;

    if(NULL != s_eo_appmeas_singleton.jntEncConvDataList)
    {
        return(&s_eo_appmeas_singleton);
    }
     
    if(NULL == cfg)
    {
        return(NULL);
    }
    

    //save in obj its configuration
    memcpy(&(s_eo_appmeas_singleton.cfg), cfg, sizeof(eOmeasconv_cfg_t));
        
    s_eo_appmeas_singleton.jointVelocityShift = cfg->jointVelocityShift;
    s_eo_appmeas_singleton.jointVelocityEstimationShift = cfg->jointVelocityEstimationShift;
    s_eo_appmeas_singleton.jointAccEstimationShift = cfg->jointAccEstimationShift;

    res = s_eo_measconv_TableCreateAndInit(&s_eo_appmeas_singleton);

    if(eores_OK != res)
    {
        return(NULL);
    }
    
    return(&s_eo_appmeas_singleton);
}


extern EOtheMeasuresConverter* eo_measconv_GetHandle(void)
{
    if(NULL == s_eo_appmeas_singleton.jntEncConvDataList)
    {
        return(NULL);
    }
    return(&s_eo_appmeas_singleton);      
}

extern eOresult_t eo_measconv_SetJntEncoderConversionFactor(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeasconv_encConversionFactor_t convfactor)
{
    p->jntEncConvDataList[jId].factor = convfactor;

    return(eores_OK);
}

extern eOresult_t eo_measconv_SetJntEncoderConversionOffset(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeasconv_encConversionOffset_t convoffset)
{
    p->jntEncConvDataList[jId].offset = convoffset;
    
    return(eores_OK);
}

//#if 0

//__weak extern eOmeas_position_t eo_measconv_jntPosition_E2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_position_t e_pos)
//{  
//    return((eOmeas_position_t)e_pos);
//}

//__weak extern icubCanProto_position_t eo_measconv_jntPosition_I2E(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_position_t i_pos)
//{
//    return((icubCanProto_position_t)i_pos);
//}

//__weak extern eOmeas_velocity_t eo_measconv_jntVelocity_E2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_velocity_t e_vel)
//{
//    return((eOmeas_velocity_t)e_vel);
//}

//__weak extern eOmeas_velocity_t eo_measconv_jntVelocity_E2I_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_velocity_t e_vel)
//{
//    return((eOmeas_velocity_t)e_vel);
//}


//__weak extern icubCanProto_velocity_t eo_measconv_jntVelocity_I2E(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
//{
//    return((icubCanProto_velocity_t)i_vel);
//}


//__weak extern icubCanProto_velocity_t eo_measconv_jntVelocity_I2E_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
//{

//    return((icubCanProto_velocity_t)(i_vel * __fabs(eo_measconv_hid_GetEncConv_factor(p, jId))));
//}
//// __weak extern icubCanProto_velocity_t eo_measconv_jntVelocity_I2E_forSetVelRefMC4(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
//// {
////     return((icubCanProto_velocity_t)i_vel);
//// }


//__weak extern eOmeas_acceleration_t eo_measconv_jntAcceleration_E2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_acceleration_t e_acc)
//{ 
//    return((eOmeas_acceleration_t)e_acc);
//}


//__weak extern eOmeas_acceleration_t eo_measconv_jntAcceleration_E2I_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_acceleration_t e_acc)
//{
//    return((eOmeas_acceleration_t)e_acc);
//}
//__weak extern icubCanProto_acceleration_t eo_measconv_jntAcceleration_I2E(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
//{
//    return((icubCanProto_acceleration_t)i_acc);
//}

//__weak extern icubCanProto_acceleration_t eo_measconv_jntAcceleration_I2E_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
//{
//    return((icubCanProto_acceleration_t)i_acc);
//}
//__weak extern icubCanProto_stiffness_t eo_measconv_impedenceStiffness_I2S(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_stiffness_t i_stiff)
//{ 
//    return((icubCanProto_stiffness_t)i_stiff);
//}


//__weak extern eOmeas_stiffness_t eo_measconv_impedenceStiffness_S2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_stiffness_t s_stiff)
//{
//    return((eOmeas_stiffness_t)s_stiff);
//}


//__weak extern icubCanProto_damping_t eo_measconv_impedenceDamping_I2S(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_damping_t i_damping)
//{
//    return((icubCanProto_damping_t)i_damping);
//}

//__weak extern eOmeas_damping_t eo_measconv_impedenceDamping_S2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_damping_t s_damping)
//{
//    return((eOmeas_damping_t)s_damping);
//}


//__weak extern icubCanProto_torque_t eo_measconv_torque_I2S(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_torque_t i_torque)
//{
//    return((icubCanProto_torque_t)i_torque);
//}


//__weak extern eOmeas_torque_t eo_measconv_torque_S2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_torque_t s_torque)
//{
//     
//    return((eOmeas_torque_t)s_torque);
//}

//#endif


extern eOmeas_torque_t eo_measconv_torque_S2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_torque_t s_torque)
{    
    return((eOmeas_torque_t)s_torque);
}


extern eOmeas_position_t eo_measconv_jntPosition_E2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_position_t e_pos)
{    
    return((eOmeas_position_t)((e_pos / eo_measconv_hid_GetEncConv_factor(p, jId)) - eo_measconv_hid_GetEncConv_offset(p, jId)));
}

extern icubCanProto_position_t eo_measconv_jntPosition_I2E(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_position_t i_pos)
{
    return((icubCanProto_position_t)((i_pos + eo_measconv_hid_GetEncConv_offset(p, jId)) * eo_measconv_hid_GetEncConv_factor(p, jId))); 
}

extern eOmeas_velocity_t eo_measconv_jntVelocity_E2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_velocity_t e_vel)
{
    return(e_vel/eo_measconv_hid_GetEncConv_factor(p, jId));
}

extern eOmeas_velocity_t eo_measconv_jntVelocity_E2I_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_velocity_t e_vel)
{
    return((eOmeas_velocity_t)(e_vel/__fabs(eo_measconv_hid_GetEncConv_factor(p, jId))));
}


extern icubCanProto_velocity_t eo_measconv_jntVelocity_I2E(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
    int32_t tmp;
     
    //in order to send velocity to mc4 like setpoint i need to convert it in encoderticks/ms and after shift in order to obtain a small value
    tmp = i_vel * eo_measconv_hid_GetEncConv_factor(p, jId);
    tmp = tmp *(1 << eo_measconv_hid_GetVelEstimShift(p, jId)); //here i can't use shift because i_vel can be negative.
    tmp = tmp + 500;  //round to nearest integer
    tmp = tmp/1000; //convert from sec to ms
    return((icubCanProto_velocity_t)tmp);
}

extern icubCanProto_velocity_t eo_measconv_jntVelocity_I2E_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
    int32_t temp;
      
    //NEW VERSION:
    /*the velocity is dived by 10, because the reuslt of followiong moltiplication
    (i_vel * __fabs(eo_measconv_hid_GetEncConv_factor(p, jId)))
    can be bigger then 32767 (max value of int16)
    so thet result is divieded by 10.
    Note thet velocity is used to get the needed time to reach the setpoint, so in mc4 fw it is enogth to moltiply by 100 ensted of 1000 (1ms)
    This operation was already done by CanBusMotionControl
    */

    temp = (i_vel * __fabs(eo_measconv_hid_GetEncConv_factor(p, jId)));
    return((icubCanProto_velocity_t)(temp/10));
}

extern eOmeas_acceleration_t eo_measconv_jntAcceleration_E2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_acceleration_t e_acc)
{   
    return((eOmeas_acceleration_t)e_acc /eo_measconv_hid_GetEncConv_factor(p, jId));
}

extern eOmeas_acceleration_t eo_measconv_jntAcceleration_E2I_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_acceleration_t e_acc)
{
    return((eOmeas_acceleration_t)(e_acc / __fabs(eo_measconv_hid_GetEncConv_factor(p, jId))));
}

extern icubCanProto_acceleration_t eo_measconv_jntAcceleration_I2E(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
{
    return((icubCanProto_acceleration_t)(i_acc * eo_measconv_hid_GetEncConv_factor(p, jId)));
}

extern icubCanProto_acceleration_t eo_measconv_jntAcceleration_I2E_abs(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
{
    int32_t tmp = i_acc << eo_measconv_hid_GetVelEstimShift(p, jId);
    tmp = tmp * __fabs(eo_measconv_hid_GetEncConv_factor(p, jId));
    tmp = tmp + 500000; //round to nearest integer
    tmp = tmp/1000000; // conver from sec^2 to millsec^2 
    return((icubCanProto_acceleration_t)tmp);
}

 extern icubCanProto_torque_t eo_measconv_torque_I2S(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_torque_t i_torque)
 {
     return(i_torque );
 }


extern icubCanProto_stiffness_t eo_measconv_impedenceStiffness_I2S(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_stiffness_t i_stiff)
{
//     char *str_err;
 //   char str_err[200];
    float factor = eo_measconv_hid_GetEncConv_factor(p, jId);
    int32_t stiff;
 //   eOmeas_stiffness_t prova = 18000;
    
//     if(factor<0)
//     {
//         factor = -factor;
//     }
    
    if(i_stiff > INT32_MAX)
    {
        stiff = INT32_MAX;
    }
    else
    {
        stiff = (int32_t)i_stiff;
    }
    //arriva espresso in icubdegree e devo trasformarlo nelle tacche di encoder.
    //inoltre lo divido per mille perche' su robot interface e' moltiplicato per 1000 per avere piu' precisione possibile
    //return((icubCanProto_stiffness_t)(((i_stiff / factor)) /1000.0) );
    
    return((icubCanProto_stiffness_t)(((stiff / factor)) /1000.0f) );
}

extern eOmeas_stiffness_t eo_measconv_impedenceStiffness_S2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_stiffness_t s_stiff)
{
    eOmeas_stiffness_t aux = s_stiff;
    
    return(aux*eo_measconv_hid_GetEncConv_factor(p, jId)*1000);
}


extern icubCanProto_damping_t eo_measconv_impedenceDamping_I2S(EOtheMeasuresConverter *p, eOmc_jointId_t jId, eOmeas_damping_t i_damping)
{
    int32_t damping;
    if(i_damping > INT32_MAX)
    {
        damping = INT32_MAX;
    }
    else
    {
        damping = (int32_t)i_damping;
    }
    
    //arriva espresso in icubdegree e devo trasformarlo nelle tacche di encoder.
    //qui non divido per 1000 perche' devo estrimerlo al millisec.
    return((icubCanProto_stiffness_t)((damping / eo_measconv_hid_GetEncConv_factor(p, jId))) );
}


extern eOmeas_damping_t eo_measconv_impedenceDamping_S2I(EOtheMeasuresConverter *p, eOmc_jointId_t jId, icubCanProto_damping_t s_damping)
{
    eOmeas_damping_t aux = s_damping;

    return(aux*eo_measconv_hid_GetEncConv_factor(p, jId));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_measconv_TableCreateAndInit(EOtheMeasuresConverter *p)
{
    uint8_t i;

    /* 1) Create table */
//    p->totalnumofjoint = eo_appTheDB_GetNumberOfConnectedJoints(eo_appTheDB_GetHandle());
    p->totalnumofjoint = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    
    if(0 == p->totalnumofjoint)
    {
        return(eores_NOK_generic);
    }
    
    p->jntEncConvDataList = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOmeasconv_encoderConversionData_t), p->totalnumofjoint);

    /* 2) Init table */
    for(i=0; i<p->totalnumofjoint; i++)
    {
        p->jntEncConvDataList[i].factor = eo_measconv_hid_jntEncoderConvFactor_defaultValue;
        p->jntEncConvDataList[i].offset = eo_measconv_hid_jntEncoderConvOffset_defaultValue;
    }
    
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



