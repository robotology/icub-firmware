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
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOappMeasuresConverter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappMeasuresConverter_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
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
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
extern eOresult_t eo_appMeasConv_jntPosition_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t e_pos, eOmeas_position_t *i_pos_ptr)
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
    
    *i_pos_ptr = ((e_pos / eo_appMeasConv_hid_GetEncPropRatio(p, jId)) - eo_appMeasConv_hid_GetEncZero(p, jId));
     
    return(eores_OK);
}

extern eOresult_t eo_appMeasConv_jntPosition_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t *e_pos_ptr, eOmeas_position_t i_pos)
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
    *e_pos_ptr = ((i_pos + eo_appMeasConv_hid_GetEncZero(p, jId)) * eo_appMeasConv_hid_GetEncPropRatio(p, jId)); 
     
    return(eores_OK);
}

extern eOresult_t eo_appMeasConv_jntVelocity_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel, eOmeas_velocity_t *i_vel_ptr)
{
    uint32_t vel_factor;
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
    vel_factor = (1 << eo_appMeasConv_hid_GetVelEstimFactor(p, jId));
    
    //*1000 : msec-->sec
    *i_vel_ptr = (((e_vel*1000)/vel_factor)/eo_appMeasConv_hid_GetEncPropRatio(p, jId));
     
    return(eores_OK);
}


extern eOresult_t eo_appMeasConv_jntVelocity_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t *e_vel_ptr, eOmeas_velocity_t i_vel)
{
     uint32_t vel_factor;
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
    vel_factor = (1 << eo_appMeasConv_hid_GetVelEstimFactor(p, jId));    
    *e_vel_ptr = (((i_vel * eo_appMeasConv_hid_GetEncPropRatio(p, jId)) /1000) *vel_factor);
     
    return(eores_OK);
}

extern eOresult_t eo_appMeasConv_jntAcceleration_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc, eOmeas_velocity_t *i_acc_ptr)
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
    
    #warning VALE --> is correct acceleration conversion??
    *i_acc_ptr = e_acc; 
     
    return(eores_OK);
}

extern eOresult_t eo_appMeasConv_jntAcceleration_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t *e_acc_ptr, eOmeas_velocity_t i_acc)
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
    #warning VALE--> chiedi info: e'' necessario usare acc_shift???
    *e_acc_ptr = (i_acc/(1000*1000)); 
     
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




