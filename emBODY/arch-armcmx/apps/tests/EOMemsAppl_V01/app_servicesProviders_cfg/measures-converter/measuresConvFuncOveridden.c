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
extern eOmeas_position_t eo_appMeasConv_jntPosition_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t e_pos)
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
    
    return((eOmeas_position_t)((e_pos / eo_appMeasConv_hid_GetEncConv_factor(p, jId)) - eo_appMeasConv_hid_GetEncConv_offset(p, jId)));
}

extern eOicubCanProto_position_t eo_appMeasConv_jntPosition_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_position_t i_pos)
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
    return((eOicubCanProto_position_t)((i_pos + eo_appMeasConv_hid_GetEncConv_offset(p, jId)) * eo_appMeasConv_hid_GetEncConv_factor(p, jId))); 
}

extern eOmeas_velocity_t eo_appMeasConv_jntVelocity_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel)
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
    return(e_vel/eo_appMeasConv_hid_GetEncConv_factor(p, jId));
}

extern eOmeas_velocity_t eo_appMeasConv_jntVelocity_E2I_abs(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel)
{
//    uint32_t vel_factor;
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
    return((eOmeas_velocity_t)(e_vel/__fabs(eo_appMeasConv_hid_GetEncConv_factor(p, jId))));
}


extern eOicubCanProto_velocity_t eo_appMeasConv_jntVelocity_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
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
    
//    return(((eOicubCanProto_velocity_t)((i_vel * eo_appMeasConv_hid_GetEncConv_factor(p, jId))/1000))<<eo_appMeasConv_hid_GetVelShift(p, jId));
    return((eOicubCanProto_velocity_t)(i_vel * eo_appMeasConv_hid_GetEncConv_factor(p, jId)));
}

extern eOicubCanProto_velocity_t eo_appMeasConv_jntVelocity_I2E_abs(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
    int32_t temp;
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
    

    //return((eOicubCanProto_velocity_t)(i_vel * __fabs(eo_appMeasConv_hid_GetEncConv_factor(p, jId))));
            
    //NEW VERSION:
    /*the velocity is dived by 10, because the seuslt of followiong moltiplication
    (i_vel * __fabs(eo_appMeasConv_hid_GetEncConv_factor(p, jId)))
    can be bigger then 32767 (max value of int16)
    so thet result is divieded by 10.
    Note thet velocity is used to get the needed time to reach the setpoint, so in mc4 fw it is enogth to moltiply by 100 ensted of 1000 (1ms)
    */

    temp = (i_vel * __fabs(eo_appMeasConv_hid_GetEncConv_factor(p, jId)));
    return((eOicubCanProto_velocity_t)(temp/10));
}

extern eOmeas_acceleration_t eo_appMeasConv_jntAcceleration_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc)
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
    
    //return(((e_acc *1000000)>>(eo_appMeasConv_hid_GetVelEstimShift(p, jId)+eo_appMeasConv_hid_GetAccEstimShift(p, jId)))/eo_appMeasConv_hid_GetEncConv_factor(p, jId));
    return((eOmeas_acceleration_t)e_acc /eo_appMeasConv_hid_GetEncConv_factor(p, jId));
}

extern eOmeas_acceleration_t eo_appMeasConv_jntAcceleration_E2I_abs(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc)
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
    
    //return(((e_acc *1000000)>>(eo_appMeasConv_hid_GetVelEstimShift(p, jId)+eo_appMeasConv_hid_GetAccEstimShift(p, jId)))/eo_appMeasConv_hid_GetEncConv_factor(p, jId));
    return((eOmeas_acceleration_t)(e_acc / __fabs(eo_appMeasConv_hid_GetEncConv_factor(p, jId))));
}

extern eOicubCanProto_acceleration_t eo_appMeasConv_jntAcceleration_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
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
   
//    return(((eOicubCanProto_acceleration_t)(i_acc * eo_appMeasConv_hid_GetEncConv_factor(p, jId)))<<eo_appMeasConv_hid_GetVelShift(p, jId));
    return((eOicubCanProto_acceleration_t)(i_acc * eo_appMeasConv_hid_GetEncConv_factor(p, jId)));
}

extern eOicubCanProto_acceleration_t eo_appMeasConv_jntAcceleration_I2E_abs(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
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
   
//    return(((eOicubCanProto_acceleration_t)(i_acc * eo_appMeasConv_hid_GetEncConv_factor(p, jId)))<<eo_appMeasConv_hid_GetVelShift(p, jId));
    return((eOicubCanProto_acceleration_t)(i_acc * __fabs(eo_appMeasConv_hid_GetEncConv_factor(p, jId))));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




