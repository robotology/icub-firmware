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
#include "stdio.h"

#include "EoCommon.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"
#include "math.h"
#include "EOtheServices_hid.h"

#include "EOMtheEMSappl.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMC4boards.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMC4boards_hid.h"

#if defined(EOTHESERVICES_disable_theMC4boards)

    // provide empty implementation, so that we dont need to change the caller of the API
    
    extern EOtheMC4boards* eo_mc4boards_Initialise(const eOmc4boards_config2_t *cfg2) 
    {   
        return NULL; 
    }

    extern eOresult_t eo_mc4boards_LoadShifts(EOtheMC4boards *p, eOmc_mc4shifts_t shifts)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mc4boards_LoadBroadcastFlags(EOtheMC4boards *p, uint16_t flags)
    {
        return eores_NOK_generic;
    }

    extern EOtheMC4boards* eo_mc4boards_GetHandle(void) 
    {   
        return NULL; 
    }

    extern eObool_t eo_mc4boards_AreThere(EOtheMC4boards *p)
    {
        return eobool_false;
    }

    extern eOresult_t eo_mc4boards_BroadcastStart(EOtheMC4boards *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mc4boards_BroadcastStop(EOtheMC4boards *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mc4boards_Config(EOtheMC4boards *p)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mc4boards_AcceptCANframe(EOtheMC4boards *p, eOcanframe_t *frame, eOcanport_t port, eOmc4boards_canframe_t cftype)
    {
        return eores_NOK_generic;
    }

    extern eOresult_t eo_mc4boards_Convert_encoderfactor_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_factor_t factor)
    {
        return eores_NOK_generic;
    }
    
    extern eOresult_t eo_mc4boards_Convert_encoderoffset_Set(EOtheMC4boards *p, uint8_t joint, eOmc4boards_conv_encoder_offset_t offset)
    {
        return eores_NOK_generic;
    }


    extern void eo_mc4boards_Convert_maxJointPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max)
    {
    }
    
    extern void eo_mc4boards_Convert_minJointPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min)
    {
    }
    
    extern icubCanProto_position_t eo_mc4boards_Convert_maxJointPos_Get(EOtheMC4boards *p, uint8_t joint)
    {
        return 0;
    }
    
    extern icubCanProto_position_t eo_mc4boards_Convert_minJointPos_Get(EOtheMC4boards *p, uint8_t joint)
    {
        return 0;
    }
    
    extern void eo_mc4boards_Convert_maxMotorPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max)
    {
    }
    
    extern void eo_mc4boards_Convert_minMotorPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min)
    {
    }
    
    extern icubCanProto_position_t eo_mc4boards_Convert_maxMotorPos_Get(EOtheMC4boards *p, uint8_t joint)
    {
        return 0;
    }
    
    extern icubCanProto_position_t eo_mc4boards_Convert_minMotorPos_Get(EOtheMC4boards *p, uint8_t joint)
    {
        return 0;
    }
    
    extern eOmeas_position_t eo_mc4boards_Convert_Position_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t pos)
    {
        return 0;
    }
    
    extern icubCanProto_position_t eo_mc4boards_Convert_Position_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_position_t pos)
    {
        return 0;
    }
    


    extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel, eOmc4boards_velocitycontext_t context)
    {
        return 0;
    }
    
    extern eOmeas_velocity_t eo_mc4boards_Convert_Velocity_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel)
    {
        return 0;
    }
    

    extern eOmeas_acceleration_t eo_mc4boards_Convert_Acceleration_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc)
    {
        return 0;
    }
    
    extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc)
    {
        return 0;
    }
    

    extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN_abs__NEW(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc)
    {
        return 0;
    }
    


    extern icubCanProto_stiffness_t eo_mc4boards_Convert_impedanceStiffness_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_stiffness_t stiff)
    {
        return 0;
    }
    
    extern eOmeas_stiffness_t eo_mc4boards_Convert_impedanceStiffness_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_stiffness_t s_stiff)
    {
        return 0;
    }
    

    extern icubCanProto_damping_t eo_mc4boards_Convert_impedanceDamping_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_damping_t i_damping)
    {
        return 0;
    }
    
    extern eOmeas_damping_t eo_mc4boards_Convert_impedanceDamping_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_damping_t s_damping)
    {
        return 0;
    }
    

    extern icubCanProto_torque_t eo_mc4boards_Convert_torque_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_torque_t i_torque)
    {
        return 0;
    }
    
    extern eOmeas_torque_t eo_mc4boards_Convert_torque_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_torque_t s_torque)
    {
        return 0;
    }
    


#elif !defined(EOTHESERVICES_disable_theMC4boards)

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
//static void send_diagnostic_debugmessage(eOerrmanErrorType_t type, eOerror_value_DEB_t value, uint8_t jointnum, uint16_t par16, uint64_t par64, const char* info);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
//Definition of constant values about torque conversion.
static const float MAX_TORQUE_LOWER_ARM_JOINTS = 2.0; //[Nm] . Max value of torque on lower arm joints.
static const float RESOLUTION_OF_TORQUE_VALUES =  32768.0;//the toruqe values in mc4CAN board are expressed on 16 bits with sign
static const float TORQUE_FACT_EMS2Nm = 1000000.0; //the ems use micro Nm


static float s_cliptoint32(float v)
{
    int64_t r = v;
    if(r > INT32_MAX)
    {
        r = INT32_MAX;
    }
    else if(r < INT32_MIN)
    {
        r = INT32_MIN;
    }
    
    return r;
}

static const eOmc4boards_config2_t s_eo_mc4boards_defaultconfig2 =
{   
    EO_INIT(.shifts)
    {
        EO_INIT(.velocity)                  8,
        EO_INIT(.estimJointVelocity)        5,
        EO_INIT(.estimJointAcceleration)    5,
        EO_INIT(.estimMotorVelocity)        1,
        EO_INIT(.estimMotorAcceleration)    1
    },
    EO_INIT(.broadcastflags)    (1<<eomc_mc4broadcast_position) | (1<<eomc_mc4broadcast_status) | (1<<eomc_mc4broadcast_print) | (1<<eomc_mc4broadcast_pidvalues)    
};

static EOtheMC4boards s_eo_themc4boards = 
{
    EO_INIT(.initted)           eobool_false,
    EO_INIT(.therearemc4s)      eobool_false,
    EO_INIT(.numofjomos)        0,
    EO_INIT(.configured)        eobool_false,
    EO_INIT(.command)           {0},    
    EO_INIT(.convencoder)       {0},
    EO_INIT(.config2)           {0},
    EO_INIT(.cansettings)       {0}
};

//static const char s_eobj_ownname[] = "EOtheMC4boards";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMC4boards* eo_mc4boards_Initialise(const eOmc4boards_config2_t *cfg2)
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
        uint8_t i;
        
        s_eo_themc4boards.therearemc4s = eobool_true;
        
        if(NULL == cfg2)
        {
            cfg2 = &s_eo_mc4boards_defaultconfig2;
        }
        
        memcpy(&s_eo_themc4boards.config2, cfg2, sizeof(eOmc4boards_config2_t));
        
        eo_mc4boards_LoadShifts(&s_eo_themc4boards, cfg2->shifts);
        eo_mc4boards_LoadBroadcastFlags(&s_eo_themc4boards, cfg2->broadcastflags);
        
        
        s_eo_themc4boards.numofjomos = eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);      
        
        s_eo_themc4boards.configured = eobool_false;
        
        for(i=0; i<s_eo_themc4boards.numofjomos; i++)
        {
            s_eo_themc4boards.convencoder[i].factor = 1;
            s_eo_themc4boards.convencoder[i].offset = 0;
        }
        
        for(i=0; i<s_eo_themc4boards.numofjomos; i++)
        {
            //The torque used in ems are expressed in microNm = 1Nm*10^-6
            //The can boards use RESOLUTION_OF_TORQUE_VALUES like max value of torque. 
            //Since on lower arm joints the max possible torque is 2Nm and we want the max precision possible
            //we use this conversion factor from Nm to torque_tick  RESOLUTION_OF_TORQUE_VALUES/MAX_TORQUE_LOWER_ARM_JOINTS
            //Therefore the conversion factor from microNm to torque_ticks is:
            s_eo_themc4boards.convtorque[i].factor = (RESOLUTION_OF_TORQUE_VALUES/MAX_TORQUE_LOWER_ARM_JOINTS)/TORQUE_FACT_EMS2Nm;
        }
        //char info[150];
        //snprintf(info, sizeof(info), "Set torque convFactor=%.7f", s_eo_themc4boards.convtorque[0].factor);
        //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, 0, 0, 0, info);
    }

    s_eo_themc4boards.initted = eobool_true;
    
    return(&s_eo_themc4boards);   
}


extern EOtheMC4boards* eo_mc4boards_GetHandle(void)
{
    return(eo_mc4boards_Initialise(NULL));
}


extern eObool_t eo_mc4boards_AreThere(EOtheMC4boards *p)
{
    return(s_eo_themc4boards.therearemc4s);
}


extern eOresult_t eo_mc4boards_LoadShifts(EOtheMC4boards *p, eOmc_mc4shifts_t shifts)
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
    
    s_eo_themc4boards.config2.shifts = shifts;
    
    
    s_eo_themc4boards.cansettings.velshift = shifts.velocity;
    s_eo_themc4boards.cansettings.estimshifts.estimShiftJointAcc = shifts.estimJointAcceleration;
    s_eo_themc4boards.cansettings.estimshifts.estimShiftJointVel = shifts.estimJointVelocity;
    s_eo_themc4boards.cansettings.estimshifts.estimShiftMotorAcc = shifts.estimMotorAcceleration;
    s_eo_themc4boards.cansettings.estimshifts.estimShiftMotorVel = shifts.estimMotorVelocity;
    
    return(eores_OK);       
}


extern eOresult_t eo_mc4boards_LoadBroadcastFlags(EOtheMC4boards *p, uint16_t flags)
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
    s_eo_themc4boards.config2.broadcastflags = flags;
    
    s_eo_themc4boards.cansettings.broadcastpolicy = 0;
    
    uint16_t tmp = flags & 0xffff;
    tmp >>= 1;
    s_eo_themc4boards.cansettings.broadcastpolicy =  tmp;

    // ....
    
    return(eores_OK);       
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
    s_eo_themc4boards.command.clas = eocanprot_msgclass_pollingMotorControl;    
    s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY;
    s_eo_themc4boards.command.value = &s_eo_themc4boards.cansettings.broadcastpolicy;    
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
    uint32_t silence = 0;
    s_eo_themc4boards.command.clas = eocanprot_msgclass_pollingMotorControl;    
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

extern void eo_mc4boards_Convert_maxJointPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max)
{
    p->convencoder[joint].max_mc4_joint_pos = max;
}

extern void eo_mc4boards_Convert_minJointPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min)
{
    p->convencoder[joint].min_mc4_joint_pos = min;
}

extern icubCanProto_position_t eo_mc4boards_Convert_maxJointPos_Get(EOtheMC4boards *p, uint8_t joint)
{
    return p->convencoder[joint].max_mc4_joint_pos;
}

extern icubCanProto_position_t eo_mc4boards_Convert_minJointPos_Get(EOtheMC4boards *p, uint8_t joint)
{
    return p->convencoder[joint].min_mc4_joint_pos;
}

extern void eo_mc4boards_Convert_maxMotorPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t max)
{
    p->convencoder[joint].max_mc4_motor_pos = max;
}

extern void eo_mc4boards_Convert_minMotorPos_Set(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t min)
{
    p->convencoder[joint].min_mc4_motor_pos = min;
}

extern icubCanProto_position_t eo_mc4boards_Convert_maxMotorPos_Get(EOtheMC4boards *p, uint8_t joint)
{
    return p->convencoder[joint].max_mc4_motor_pos;
}

extern icubCanProto_position_t eo_mc4boards_Convert_minMotorPos_Get(EOtheMC4boards *p, uint8_t joint)
{
    return p->convencoder[joint].min_mc4_motor_pos;
}

extern eOresult_t eo_mc4boards_Config(EOtheMC4boards *p)
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
    s_eo_themc4boards.command.clas = eocanprot_msgclass_pollingMotorControl;    
    uint8_t i = 0;

    for(i=0; i<s_eo_themc4boards.numofjomos; i++)
    {
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);
    
        // 1. velocity shift
        s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT;
        s_eo_themc4boards.command.value = &s_eo_themc4boards.cansettings.velshift;    
        // ok, now i send the value to the relevant address
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themc4boards.command, id32);

        // 2. speed estim shifts
        s_eo_themc4boards.command.type  = ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT;
        //s_eo_themc4boards.command.value = &estimshift;    
        s_eo_themc4boards.command.value = &s_eo_themc4boards.cansettings.estimshifts; 
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themc4boards.command, id32);     
    }
    
    s_eo_themc4boards.configured = eobool_true;
    
    return(eores_OK);       
}

extern eOresult_t eo_mc4boards_AcceptCANframe(EOtheMC4boards *p, eOcanframe_t *frame, eOcanport_t port, eOmc4boards_canframe_t cftype)
{
    return eores_OK;
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

// revised by accame&randazzo on 8oct15
extern eOmeas_position_t eo_mc4boards_Convert_Position_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_position_t pos)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    float temp = (((float)pos / s_eo_themc4boards.convencoder[joint].factor) - s_eo_themc4boards.convencoder[joint].offset);
    temp = s_cliptoint32(temp);
    
    return((eOmeas_position_t)temp);
}

// revised by accame&randazzo on 8oct15
extern icubCanProto_position_t eo_mc4boards_Convert_Position_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_position_t pos)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    float temp = (((float)pos + s_eo_themc4boards.convencoder[joint].offset) * s_eo_themc4boards.convencoder[joint].factor);
    temp = s_cliptoint32(temp);
    
    return((icubCanProto_position_t)temp); 
}


// revised by accame&randazzo on 8 oct 2015
extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel, eOmc4boards_velocitycontext_t context)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
     
    float temp = (float)vel * s_eo_themc4boards.convencoder[joint].factor;    // from icubdeg/sec to ticks/sec. it keeps the sign
    
    switch(context)
    {
        case eomc4_velocitycontext_toTICKS:
        {
            temp = temp;           
        } break;
        
        case eomc4_velocitycontext_toCAN_signed:
        {   // it is a normal conversion: we convert to ticks/ms and the we apply the shift fatcor
            temp /= 1000.0f;                                                    // from ticks/sec to ticks/ms
            temp *= (1 << s_eo_themc4boards.cansettings.velshift);   // apply the shift for canbus;
        } break;
        
        case eomc4_velocitycontext_toCAN_unsigned:
        {   // it is a normal conversion: we convert to ticks/ms and the we apply the shift fatcor
            temp /= 1000.0f;                                                    // from ticks/sec to ticks/ms
            temp *= (1 << s_eo_themc4boards.cansettings.velshift);   // apply the shift for canbus;
            temp = fabsf(temp);
        } break;        

        case eomc4_velocitycontext_toCAN_positionsetpoint:
        {   // it is a special case
            // revised accame&randazzo on 8oct15: it is ok to divide by 10 ... see function canBusMotionControl::setRefSpeedsraw()
            // the value is absolute because the direction of movement is given by the sign of position setpoint
            temp = fabsf(temp) / 10.0f;        
        } break;
        
        default:
        {
            temp = 0;
        } break;
        
    }
    
    temp = EO_CLIP_INT16(temp);
    
    return((icubCanProto_velocity_t)temp);    
}


// revised by accame&randazzo on 8 oct 2015
extern eOmeas_velocity_t eo_mc4boards_Convert_Velocity_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_velocity_t vel)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    float temp = 1000.0f * (float) vel;     // from ticks/ms to ticks/sec
    temp /= (1 << s_eo_themc4boards.cansettings.estimshifts.estimShiftJointVel); // now we divide by (1 << estim shift vel joint)
    temp /= s_eo_themc4boards.convencoder[joint].factor; // now we transform into icubdeg/sec


    temp = s_cliptoint32(temp);
    
    return((eOmeas_velocity_t) temp);
}



// 
// revised by accame&randazzo on 8 oct 2015: it is OK
//extern icubCanProto_velocity_t eo_mc4boards_Convert_Velocity_toCAN_OLD(EOtheMC4boards *p, uint8_t joint, eOmeas_velocity_t vel)
//{
//    if(joint >= s_eo_themc4boards.numofjomos)
//    {   
//        return(0);
//    }
//#if 0    
//    //in order to send velocity to mc4 like setpoint i need to convert it in encoderticks/ms and after shift in order to obtain a small value
//    int32_t tmp = vel * s_eo_themc4boards.convencoder[joint].factor;
//    tmp = tmp *(1 << s_eo_themc4boards.config2use.shiftvalues.velshift); //eo_measconv_hid_GetVelEstimShift(p, jId)); //here i can't use shift because i_vel can be negative.
//    tmp = tmp + 500;  //round to nearest integer
//    tmp = tmp/1000; //convert from sec to ms
//    return((icubCanProto_velocity_t)tmp);
//#else

//    #warning --> test it: we believe that we dont need the __fabs()
//    
//    float impVel = (float)vel * (s_eo_themc4boards.convencoder[joint].factor); // transform from icubdeg/sec to imp/sec
//    impVel /= (1000.0f); // transform from imp/sec to imp/ms
//    impVel *= (1 << s_eo_themc4boards.config2use.shiftvalues.velshift); // apply the shift for canbus
//    return((icubCanProto_velocity_t)impVel);
//   
//#endif    
//}



// revised by accame&randazzo on 8 oct 2015        
extern eOmeas_acceleration_t eo_mc4boards_Convert_Acceleration_fromCAN(EOtheMC4boards *p, uint8_t joint, icubCanProto_acceleration_t acc)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    // it is OK to divide by the 2^(estimShiftJointVel + estimShiftJointAcc). The MC4 shifts left by this sum of terms.
    float temp = 1000000.0f * (float)acc;   // from ticks/ms^2 to ticks/sec^2
    temp /= (1 << (s_eo_themc4boards.cansettings.estimshifts.estimShiftJointVel + s_eo_themc4boards.cansettings.estimshifts.estimShiftJointAcc)); // now we divide by (1 << estim shift vel + acc)
    temp /= s_eo_themc4boards.convencoder[joint].factor; // now we transform into icubdeg/sec^2

    temp = s_cliptoint32(temp);
    
    return((eOmeas_acceleration_t)temp);
}





// revised by accame&randazzo on 8 oct 2015
extern icubCanProto_acceleration_t eo_mc4boards_Convert_Acceleration_toCAN(EOtheMC4boards *p, uint8_t joint, eOmeas_acceleration_t acc)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }

    float temp = (float)acc * (s_eo_themc4boards.convencoder[joint].factor); // transform from icubdeg/sec^2 to ticks/sec^2
    temp /= (1000000.0f); // transform from ticks/sec^2 to ticks/ms^2
    temp *= (1 << s_eo_themc4boards.cansettings.velshift); // apply the shift for canbus  
    
    temp = EO_CLIP_INT16(temp);   
    
    return((icubCanProto_acceleration_t)temp);       
}

extern icubCanProto_stiffness_t eo_mc4boards_Convert_impedanceStiffness_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_stiffness_t stiff)
{
    
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    float factor = s_eo_themc4boards.convencoder[joint].factor;
    float trq_factor = s_eo_themc4boards.convtorque[joint].factor;
    
    icubCanProto_stiffness_t ret;
    float v = (stiff*trq_factor) / factor;
    if ( v >0 ) ret = (icubCanProto_stiffness_t)(v);
    else ret = -((icubCanProto_stiffness_t)(-v));
        
    return ret;    
}

extern eOmeas_stiffness_t eo_mc4boards_Convert_impedanceStiffness_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_stiffness_t stiff)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    float factor = s_eo_themc4boards.convencoder[joint].factor;
    float trq_factor = s_eo_themc4boards.convtorque[joint].factor;
    float f_stiff = stiff;
    
    float ret  = (f_stiff*factor)/trq_factor;  

    if(ret<0)
        ret = -ret;
    
    eOmeas_stiffness_t aux = (eOmeas_stiffness_t) ret;

    return(aux);
}

extern icubCanProto_damping_t eo_mc4boards_Convert_impedanceDamping_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_damping_t damping)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    
    float factor = s_eo_themc4boards.convencoder[joint].factor;
    float trq_factor = s_eo_themc4boards.convtorque[joint].factor;
    
    //Note: the damping is expressed in microNm/ideg/sec, while mc4 boards need torque_ticks/enc_tics/millisec
    icubCanProto_damping_t ret;
    float v = damping*trq_factor/factor*1000;
    if ( v >0 ) ret = (icubCanProto_damping_t)(v);
    else ret = -((icubCanProto_damping_t)(-v));
        
    return ret;    
}

extern eOmeas_damping_t       eo_mc4boards_Convert_impedanceDamping_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_damping_t damping)
{
    if(joint >= s_eo_themc4boards.numofjomos)
    {   
        return(0);
    }
    float factor = s_eo_themc4boards.convencoder[joint].factor;
    float trq_factor = s_eo_themc4boards.convtorque[joint].factor;
    float f_damping = damping;
    
    //Note: the mc4can boards use damping in torque_ticks/enc_tics/millisec, while ems use microNm/ideg/sec 
    float ret  = f_damping*factor/trq_factor/1000;  

    if(ret<0)
        ret = -ret;
    
    eOmeas_damping_t aux = (eOmeas_damping_t) ret;

    return(aux);
    
}

extern icubCanProto_torque_t eo_mc4boards_Convert_torque_I2S(EOtheMC4boards *p, uint8_t joint, eOmeas_torque_t torque)
{ 
    float trq_factor = s_eo_themc4boards.convtorque[joint].factor;
    float result = torque*trq_factor;
//    int8_t saturation = 0;
//    if(result>EO_INT16_MAX)
//        saturation = 1;
//    else if(result < -EO_INT16_MAX)
//        saturation = -1;
//    else
//        saturation = 0;
    
    icubCanProto_torque_t ret = EO_CLIP_INT16(result);
//    if(joint == 0)
//    {
//        char info[200];
//        snprintf(info, sizeof(info), "TRQ S=%d v=%d t=%.3f r=%.3f", saturation, ret, torque, result);
//        //snprintf(info, sizeof(info), "trq=%.3f res=%.3f", torque, result);
//        send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, joint, saturation, 0, info);
//    }
    
    return(ret);   
}

extern eOmeas_torque_t eo_mc4boards_Convert_torque_S2I(EOtheMC4boards *p, uint8_t joint, icubCanProto_torque_t torque)
{
        float trq_factor = s_eo_themc4boards.convtorque[joint].factor;

    return((eOmeas_torque_t)torque/trq_factor);
}


//static void send_diagnostic_debugmessage(eOerrmanErrorType_t type, eOerror_value_DEB_t value, uint8_t jointnum, uint16_t par16, uint64_t par64, const char* info)
//{
//    eOerrmanDescriptor_t errdes = {0};

//    errdes.code             = eoerror_code_get(eoerror_category_Debug, value);
//    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress    = jointnum;
//    errdes.par16            = par16;
//    errdes.par64            = par64;
//    eo_errman_Error(eo_errman_GetHandle(), type, info, NULL, &errdes);
//}


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
    eObrd_canlocation_t location = {0};
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


#endif // #elif !defined(EOTHESERVICES_disable_theMC4boards)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



