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
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"



#include "EOMtheEMSappl.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMC4boards.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMC4boards_hid.h"


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

static eObool_t s_eo_mc4boards_foundone(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOmc4boards_config_t s_eo_mc4boards_defaultconfig =
{
    .broadcastpolicy    =
    {
        .values   = 
        { 
            ICUBCANPROTO_PER_MC_MSG__POSITION, 
            ICUBCANPROTO_PER_MC_MSG__STATUS, 
            ICUBCANPROTO_PER_MC_MSG__PRINT, 
            ICUBCANPROTO_PER_MC_MSG__PID_VAL        
        }   
    },
    .shiftvalues        =
    {
        .velshift                           = 8,            // it is jointVelocityShift
        .estimshifts  =
        {
            .estimShiftJointVel             = 8,            // always 8, with: can ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT and measureconverter            
            .estimShiftJointAcc             = 5,            // on can with ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT it tx 0, on measureconverter eo_measconv_hid_GetAccEstimShift() is it used 5
                                                            // this latter is used only on eocanprotMCperiodic_parser_PER_MC_MSG__VELOCITY() VERY STRANGE: VERIFY
            .estimShiftMotorVel             = 0,            // on can ... it is tx 0
            .estimShiftMotorAcc             = 0             // on can ... it is tx 0       
        }
    }
};

static EOtheMC4boards s_eo_themc4boards = 
{
    .initted            = eobool_false,
    .therearemc4s       = eobool_false,
    .numofjomos         = 0,
    .config             = {0},
    .config2use         = {0},
    .command            = {0},
    .configured         = eobool_false,
    .convencoder        = {0}
};

//static const char s_eobj_ownname[] = "EOtheMC4boards";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMC4boards* eo_mc4boards_Initialise(const eOmc4boards_config_t *cfg)
{
    if(eobool_true == s_eo_themc4boards.initted)
    {
        return(&s_eo_themc4boards);
    }
    
    if(0 == eo_entities_NumOfJoints(eo_entities_GetHandle()))
    {
        s_eo_themc4boards.therearemc4s = eobool_false;
    }
    else if(eobool_false == s_eo_mc4boards_foundone())
    {
        s_eo_themc4boards.therearemc4s = eobool_false;
    }
    else
    {
        s_eo_themc4boards.therearemc4s = eobool_true;
        
        if(NULL == cfg)
        {
            cfg = &s_eo_mc4boards_defaultconfig;
        }
        
        memcpy(&s_eo_themc4boards.config, cfg, sizeof(eOmc4boards_config_t));
        
        // init .config2use
        // shiftvalues are not changed
        memset(&s_eo_themc4boards.config2use, 0, sizeof(eOmc4boards_config_t));
        // shiftvalues are not changed
        memcpy(&s_eo_themc4boards.config2use.shiftvalues, &s_eo_themc4boards.config.shiftvalues, sizeof(eOmc4boards_shiftvalues_t));
        // however we transform broadcast policies into flags
        uint8_t i;
        for(i=0; i<eoemc4boards_broadcastpolicylistsize; i++)
        {
            if(0 == s_eo_themc4boards.config.broadcastpolicy.values[i])
            {
                continue;
            }
            
            if(s_eo_themc4boards.config.broadcastpolicy.values[i] < 9)
            {
                s_eo_themc4boards.config2use.broadcastpolicy.values[0] |= (1 <<(s_eo_themc4boards.config.broadcastpolicy.values[i]-1));
            }
            else if(s_eo_themc4boards.config.broadcastpolicy.values[i] < 17)
            {
                s_eo_themc4boards.config2use.broadcastpolicy.values[1] |= (1<<(s_eo_themc4boards.config.broadcastpolicy.values[i]-1));
            }        
        }
        
        s_eo_themc4boards.numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);      
        
        s_eo_themc4boards.configured = eobool_false;
        
        for(i=0; i<s_eo_themc4boards.numofjomos; i++)
        {
            s_eo_themc4boards.convencoder[i].factor = 1;
            s_eo_themc4boards.convencoder[i].offset = 0;
        }
    }

    s_eo_themc4boards.initted = eobool_true;
    
    return(&s_eo_themc4boards);   
}


extern EOtheMC4boards* eo_mc4boards_GetHandle(void)
{
    return(eo_mc4boards_Initialise(NULL));
}


extern eOresult_t eo_mc4boards_BroadcastStart(EOtheMC4boards *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themc4boards.therearemc4s)
    {   // nothing to do because we dont have a mc4 board
        return(eores_OK);
    }
    
       
    // now, i do things. 
    
    // set broadcast policy to what in configuration
    s_eo_themc4boards.command.class = eocanprot_msgclass_pollingMotorControl;    
    s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
    s_eo_themc4boards.command.value = &s_eo_themc4boards.config2use.broadcastpolicy;    
    uint8_t i = 0;
    for(i=0; i<s_eo_themc4boards.numofjomos; i++)
    {
        // ok, now i send the value to the relevant address
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themc4boards.command, id32);
    }
    
    return(eores_OK);       
}



extern eOresult_t eo_mc4boards_BroadcastStop(EOtheMC4boards *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themc4boards.therearemc4s)
    {   // nothing to do because we dont have a mc4 board
        return(eores_OK);
    }
          
    // now, i do things. 
    
    // set broadcast policy to silence
    eOmc4boards_broadcastpolicy_t silence = {0};
    s_eo_themc4boards.command.class = eocanprot_msgclass_pollingMotorControl;    
    s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
    s_eo_themc4boards.command.value = &silence;    
    uint8_t i = 0;
    for(i=0; i<s_eo_themc4boards.numofjomos; i++)
    {
        // ok, now i send the value to the relevant address
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themc4boards.command, id32);
    }
    
    return(eores_OK);       
}

extern eOresult_t eo_mc4boards_ConfigShiftValues(EOtheMC4boards *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themc4boards.therearemc4s)
    {   // nothing to do because we dont have a mc4 board
        return(eores_OK);
    }

    if(eobool_true == s_eo_themc4boards.configured)
    {   // nothing to do because already configured
        return(eores_OK);
    }    

          
    // now, i do things. 
    
    // send ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT and ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT
    s_eo_themc4boards.command.class = eocanprot_msgclass_pollingMotorControl;    
    uint8_t i = 0;
    //icubCanProto_estimShift_t estimshift = {0};
    //estimshift.estimShiftJointVel = s_eo_themc4boards.config2use.shiftvalues.estimshifts.estimShiftJointVel;
    // all others are zero .... very strange, and what about estimShiftJointAcc ???
    for(i=0; i<s_eo_themc4boards.numofjomos; i++)
    {
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
    
        s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
        s_eo_themc4boards.command.value = &s_eo_themc4boards.config2use.shiftvalues.velshift;    
        // ok, now i send the value to the relevant address
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themc4boards.command, id32);

        s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;
        //s_eo_themc4boards.command.value = &estimshift;    
        s_eo_themc4boards.command.value = &s_eo_themc4boards.config2use.shiftvalues.estimshifts; 
        // ok, now i send the value to the relevant address
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themc4boards.command, id32);               
    }
    
    s_eo_themc4boards.configured = eobool_true;
    
    return(eores_OK);       
}


extern eOresult_t eo_mc4boards_Convert_encoderfactor_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_factor_t factor)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themc4boards.therearemc4s)
    {   // nothing to do because we dont have a mc4 board
        return(eores_NOK_generic);
    }

    if(joint >= s_eo_themc4boards.numofjomos)
    {   // nothing to do because dont have that joint
        return(eores_NOK_generic);
    }

    s_eo_themc4boards.convencoder[joint].factor = factor;

    return(eores_OK);    
}

extern eOresult_t eo_mc4boards_Convert_encoderoffset_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_offset_t offset)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == s_eo_themc4boards.therearemc4s)
    {   // nothing to do because we dont have a mc4 board
        return(eores_NOK_generic);
    }

    if(joint >= s_eo_themc4boards.numofjomos)
    {   // nothing to do because dont have that joint
        return(eores_NOK_generic);
    }

    s_eo_themc4boards.convencoder[joint].offset = offset;

    return(eores_OK);  

}

extern eOmeas_position_t eo_mc4boards_Convert_Position_E2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t pos)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return((eOmeas_position_t)((pos / s_eo_themc4boards.convencoder[joint].factor) - s_eo_themc4boards.convencoder[joint].offset));
}

extern icubCanProto_position_t eo_mc4boards_Convert_Position_I2E(EOtheMC4boards *p, uint8_t joint, eOmeas_position_t pos)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return((icubCanProto_position_t)((pos + s_eo_themc4boards.convencoder[joint].offset) * s_eo_themc4boards.convencoder[joint].factor)); 
}


extern eOmeas_velocity_t eo_mc4boards_Convert_Velocity_E2I_abs(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return((eOmeas_velocity_t)(vel/__fabs(s_eo_themc4boards.convencoder[joint].factor)));    
}

extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_I2E_abs(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }

     
    //NEW VERSION:
    /*the velocity is dived by 10, because the reuslt of followiong moltiplication
    (vel * __fabs(s_eo_themc4boards.convencoder[joint].factor))
    can be bigger then 32767 (max value of int16)
    so the result is divieded by 10.
    Note thet velocity is used to get the needed time to reach the setpoint, so in mc4 fw it is enogth to moltiply by 100 ensted of 1000 (1ms)
    This operation was already done by CanBusMotionControl
    */

    int32_t temp = (vel * __fabs(s_eo_themc4boards.convencoder[joint].factor));
    return((icubCanProto_velocity_t)(temp/10));    
}


extern eOmeas_velocity_t eo_mc4boards_Convert_Velocity_E2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return(vel/s_eo_themc4boards.convencoder[joint].factor);
}

extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_I2E(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    //in order to send velocity to mc4 like setpoint i need to convert it in encoderticks/ms and after shift in order to obtain a small value
    int32_t tmp = vel * s_eo_themc4boards.convencoder[joint].factor;
    tmp = tmp *(1 << s_eo_themc4boards.config2use.shiftvalues.estimshifts.estimShiftJointVel); //eo_measconv_hid_GetVelEstimShift(p, jId)); //here i can't use shift because i_vel can be negative.
    tmp = tmp + 500;  //round to nearest integer
    tmp = tmp/1000; //convert from sec to ms
    return((icubCanProto_velocity_t)tmp);
}


extern eOmeas_acceleration_t       eo_mc4boards_Convert_Acceleration_E2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return((eOmeas_acceleration_t)acc / s_eo_themc4boards.convencoder[joint].factor);
}

extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_I2E(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return((icubCanProto_acceleration_t)(acc * s_eo_themc4boards.convencoder[joint].factor));    
}

extern eOmeas_acceleration_t       eo_mc4boards_Convert_Acceleration_E2I_abs(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    return((eOmeas_acceleration_t)(acc / __fabs(s_eo_themc4boards.convencoder[joint].factor)));
}

extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_I2E_abs(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    int32_t tmp = acc << s_eo_themc4boards.config2use.shiftvalues.estimshifts.estimShiftJointVel;
    tmp = tmp * __fabs(s_eo_themc4boards.convencoder[joint].factor);
    tmp = tmp + 500000; //round to nearest integer
    tmp = tmp/1000000; // conver from sec^2 to millsec^2 
    return((icubCanProto_acceleration_t)tmp);    
}


extern icubCanProto_stiffness_t eo_mc4boards_Convert_impedanceStiffness_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_stiffness_t stiff)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    float factor = s_eo_themc4boards.convencoder[joint].factor;
    int32_t tmpstiff = (int32_t)stiff;
    
    if(stiff > INT32_MAX)
    {
        tmpstiff = INT32_MAX;
    }
    else
    {
        tmpstiff = (int32_t)stiff;
    }
    //arriva espresso in icubdegree e devo trasformarlo nelle tacche di encoder.
    //inoltre lo divido per mille perche' su robot interface e' moltiplicato per 1000 per avere piu' precisione possibile
    //return((icubCanProto_stiffness_t)(((i_stiff / factor)) /1000.0) );
    
    return((icubCanProto_stiffness_t)(((tmpstiff / factor)) /1000.0f) );    
}

extern eOmeas_stiffness_t       eo_mc4boards_Convert_impedanceStiffness_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_stiffness_t stiff)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    eOmeas_stiffness_t aux = stiff;
    
    return(aux*s_eo_themc4boards.convencoder[joint].factor*1000);    
}

extern icubCanProto_damping_t eo_mc4boards_Convert_impedanceDamping_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_damping_t damping)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    int32_t tmpdamping = (int32_t)damping;;
    if(damping > INT32_MAX)
    {
        tmpdamping = INT32_MAX;
    }
    else
    {
        tmpdamping = (int32_t)damping;
    }
    
    //arriva espresso in icubdegree e devo trasformarlo nelle tacche di encoder.
    //qui non divido per 1000 perche' devo estrimerlo al millisec.
    return((icubCanProto_stiffness_t)((tmpdamping / s_eo_themc4boards.convencoder[joint].factor)) );    
}

extern eOmeas_damping_t       eo_mc4boards_Convert_impedanceDamping_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_damping_t damping)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    eOmeas_damping_t aux = damping;

    return(aux*s_eo_themc4boards.convencoder[joint].factor);    
}

extern icubCanProto_torque_t eo_mc4boards_Convert_torque_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_torque_t torque)
{
//    if(joint >= s_eo_themc4boards.numofjomos)
//    {   
//        return(0);
//    }
    
    return(torque);   
}

extern eOmeas_torque_t       eo_mc4boards_Convert_torque_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_torque_t torque)
{
//    if(joint >= s_eo_themc4boards.numofjomos)
//    {   
//        return(0);
//    }
    return((eOmeas_torque_t)torque);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// -- in here .... so that all things related to MAIS are in a unique place




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eo_mc4boards_foundone(void)
{
    // simply search for the board of teh first joint. if an mc4 then ...
    eObool_t found = eobool_false;

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 0, 0);        
    eOcanmap_location_t location = {0};
    eObrd_cantype_t boardtype = eobrd_cantype_unknown; 
    if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, &boardtype))
    {
        if(eobrd_cantype_mc4 == boardtype)
        {
            found = eobool_true;
        }
    }  

    return(found);
}        
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



