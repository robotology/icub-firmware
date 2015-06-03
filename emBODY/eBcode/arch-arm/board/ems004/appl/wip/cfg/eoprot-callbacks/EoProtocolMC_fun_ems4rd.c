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

/* @file       EoProtocolMC_fun_ems4rd.c
    @brief      This file contains the callback functions used for handling mc eth messages
    @author     marco.accame@iit.it
    @date       05/28/2015
**/




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv.h"

#include "EOMotionControl.h"


#include "EOtheEMSapplBody.h"
#include "EOtheMeasuresConverter.h"

#include "EOtheEntities.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANservice.h"


#ifdef USE_PROTO_PROXY
#include "EOproxy.h"
#include "EOtheBOARDtransceiver.h"
#endif

#include "EOtheVirtualStrain.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// remove this if you want to use function eoprot_fun_UPDT_mc_controller_config_jointcoupling() as defined inside this file
#define EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef enum 
{ 
    pid_typePOS = 0,
    pid_typeTOR = 1
} pid_type_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_motorcontrol_is2foc_based(void);


// mc4can-based 
static void s_onpid(const EOnv* nv, const eOropdescriptor_t* rd, pid_type_t type);

static eOresult_t s_translate_eOmcControlMode2icubCanProtoControlMode(eOmc_controlmode_command_t eomc_controlmode, eOprotIndex_t jId,
                                                                      icubCanProto_controlmode_t *icubcanProto_controlmode);




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

const eOmc_joint_t joint_default_value =
{
    .config =             
    {
        .pidposition =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        },
        .pidvelocity =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        },
        .pidtorque =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        }, 
        .limitsofjoint =
        {
            .min =                   0,
            .max =                   0
        },
        .impedance =
        {
            .stiffness =             0,
            .damping =               0,
            .offset =                0,
            .filler02 =              {0xf1, 0xf2}           
        },        
        
        .velocitysetpointtimeout =   0,
       
        .motionmonitormode =         eomc_motionmonitormode_dontmonitor,
        .filler01 =                  0xe0,
        .encoderconversionfactor =   EOUTIL_EMULFLOAT32_ONE,
        .encoderconversionoffset =   EOUTIL_EMULFLOAT32_ZERO,
        .motor_params =
        {
            .bemf_value =            0,
            .ktau_value =            0,
            .bemf_scale =            0,
            .ktau_scale =            0,
            .filler02 =              {0xf1, 0xf2}
        },
        .tcfiltertype =              0,
        .filler03 =                  {0xf1, 0xf2, 0xf3}
    },
    .status =                       
    {
        .basic =
        {
            .jnt_position =          0,
            .jnt_velocity =          0,
            .jnt_acceleration =      0,
            .jnt_torque =            0,
            .motionmonitorstatus =   eomc_motionmonitorstatus_notmonitored,
            .controlmodestatus =     eomc_controlmode_idle,
        },
        .ofpid =                     {0},
        .interactionmodestatus =     eomc_imodeval_stiff,
        .chamaleon03 =               {0} //{0xd1, 0xd2, 0xd3}
    },
    .inputs =                        {0},
    .cmmnds =                       
	{
		.calibration =               {0},
		.setpoint =                  {0},
		.stoptrajectory =            0,
		.controlmode =				 eomc_controlmode_cmd_switch_everything_off,
        .interactionmode =           eomc_imodeval_stiff,
        .filler01 =                  0        
	}
}; 

const eOmc_motor_t motor_default_value =
{
    .config =             
    {
        .pidcurrent =
        {
            .kp =                    0,
            .ki =                    0,
            .kd =                    0,
            .limitonintegral =       0,
            .limitonoutput =         0,
            .scale =                 0,
            .offset =                0,
            .kff =                   0,
            .stiction_up_val =       0,
            .stiction_down_val =     0,
            .filler =                {0xf1, 0xf2, 0xf3}
        },
        .gearboxratio =              0,
        .rotorencoder =              0,
        .maxvelocityofmotor =        0,
        .maxcurrentofmotor =         0,
        .filler02 =                  {0}
    },
    .status =                       {0}
}; 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// -- entity joint

extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t *cfg = (eOmc_joint_status_t*)eo_nv_RAM(nv);
    memcpy(cfg, &joint_default_value.status, sizeof(eOmc_joint_status_t));
}

extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    // now we see if it is a mc4can or a 2foc
    
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        float rescaler_pos = 0;
        float rescaler_trq = 0;

        //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
        //(currently no pid velocity is sent to 2foc)
     

        
        // 1) set pid position 
        rescaler_pos = 1.0f/(float)(1<<cfg->pidposition.scale);
        eo_emsController_SetPosPid(jxx, cfg->pidposition.kp*rescaler_pos, 
                                        cfg->pidposition.kd*rescaler_pos, 
                                        cfg->pidposition.ki*rescaler_pos, 
                                        cfg->pidposition.limitonintegral,
                                        cfg->pidposition.limitonoutput,
                                        cfg->pidposition.offset,
                                        cfg->pidposition.stiction_up_val*rescaler_pos, 
                                        cfg->pidposition.stiction_down_val*rescaler_pos);

        // 2) set torque pid    
        rescaler_trq = 1.0f/(float)(1<<cfg->pidtorque.scale);
        eo_emsController_SetTrqPid(jxx, cfg->pidtorque.kp*rescaler_trq, 
                                        cfg->pidtorque.kd*rescaler_trq, 
                                        cfg->pidtorque.ki*rescaler_trq,
                                        cfg->pidtorque.limitonintegral,
                                        cfg->pidtorque.limitonoutput, 
                                        cfg->pidtorque.offset,
                                        cfg->pidtorque.kff*rescaler_trq,
                                        cfg->pidtorque.stiction_up_val*rescaler_trq, 
                                        cfg->pidtorque.stiction_down_val*rescaler_trq);

        eo_emsController_SetAbsEncoderSign((uint8_t)jxx, (int32_t)cfg->encoderconversionfactor);
        
        eo_emsController_SetMotorParams((uint8_t)jxx, cfg->motor_params);
        
        eo_emsController_SetTcFilterType((uint8_t)jxx, (uint8_t) cfg->tcfiltertype);

        // 3) set velocity pid:    to be implemented
       
        // 4) set min position    
        eo_emsController_SetPosMin(jxx, cfg->limitsofjoint.min);
            
        // 5) set max position
        eo_emsController_SetPosMax(jxx, cfg->limitsofjoint.max);

        // 6) set vel timeout        
        eo_emsController_SetVelTimeout(jxx, cfg->velocitysetpointtimeout);
            
        // 7) set impedance 
        eo_emsController_SetImpedance(jxx, cfg->impedance.stiffness, cfg->impedance.damping, cfg->impedance.offset);
        
    }
    else
    {
        // first of all: set conversion factor
        EOtheMeasuresConverter* appMeasConv = eo_measconv_GetHandle();
        eo_measconv_SetJntEncoderConversionFactor(appMeasConv, jxx, (eOmeasconv_encConversionFactor_t)eo_common_Q17_14_to_float(cfg->encoderconversionfactor));
        eo_measconv_SetJntEncoderConversionOffset(appMeasConv, jxx, (eOmeasconv_encConversionOffset_t)eo_common_Q17_14_to_float(cfg->encoderconversionoffset));


        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;
        
        // 1) send pid position 
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_POS_PID;
        command.value = &cfg->pidposition;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
        
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS;
        command.value = &cfg->pidposition;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 


        // 2) send torque pid
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID;
        command.value = &cfg->pidtorque;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
        
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS;
        command.value = &cfg->pidtorque;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);     
        
        // 3) send velocity pid: currently is not send: neither MC4 nor 2foc use pid velocity.

        // 4) set limits
        icubCanProto_position_t minpos_icubCanProtValue = eo_measconv_jntPosition_I2E(appMeasConv, jxx, cfg->limitsofjoint.min);
        icubCanProto_position_t maxpos_icubCanProtValue = eo_measconv_jntPosition_I2E(appMeasConv, jxx, cfg->limitsofjoint.max);

        if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
        {
            //swap min and max
            icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
            minpos_icubCanProtValue = maxpos_icubCanProtValue;
            maxpos_icubCanProtValue = pos_icubCanProtValue;
        }
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
        command.value = &minpos_icubCanProtValue;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
        
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
        command.value = &maxpos_icubCanProtValue;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
        

        // 5) set vel timeout
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
        command.value = &cfg->velocitysetpointtimeout;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);     
        

        // 6) set impedance 
        icubCanProto_impedance_t impedence_icubCanProtValues = {0};  
        impedence_icubCanProtValues.stiffness = eo_measconv_impedenceStiffness_I2S(appMeasConv, jxx, cfg->impedance.stiffness);
        impedence_icubCanProtValues.damping = eo_measconv_impedenceDamping_I2S(appMeasConv, jxx, cfg->impedance.damping);
        impedence_icubCanProtValues.offset = eo_measconv_torque_I2S(appMeasConv, jxx, cfg->impedance.offset);

        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
        command.value = &impedence_icubCanProtValues;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
        
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
        command.value = &impedence_icubCanProtValues.offset;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);         
    }
    
    // 8) set monitormode status
    
    if(eomc_motionmonitormode_dontmonitor == cfg->motionmonitormode)
    {
        jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }

}


extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        eOmc_PID_t *pid_ptr = (eOmc_PID_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        float rescaler = 1.0f/(float)(1<<pid_ptr->scale);

        eo_emsController_SetPosPid(jxx, pid_ptr->kp*rescaler,   
                                        pid_ptr->kd*rescaler, 
                                        pid_ptr->ki*rescaler,
                                        pid_ptr->limitonintegral,
                                        pid_ptr->limitonoutput, 
                                        pid_ptr->offset,
                                        pid_ptr->stiction_up_val*rescaler,
                                        pid_ptr->stiction_down_val*rescaler);
    }
    else
    {
        s_onpid(nv, rd, pid_typePOS);
    }
}

extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        eOmc_PID_t *pid_ptr = (eOmc_PID_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        float rescaler = 1.0f/(float)(1<<pid_ptr->scale);
        
        eo_emsController_SetTrqPid(jxx, pid_ptr->kp*rescaler, 
                                        pid_ptr->kd*rescaler, 
                                        pid_ptr->ki*rescaler,
                                        pid_ptr->limitonintegral,
                                        pid_ptr->limitonoutput, 
                                        pid_ptr->offset,
                                        pid_ptr->kff*rescaler,
                                        pid_ptr->stiction_up_val*rescaler,
                                        pid_ptr->stiction_down_val*rescaler);
    }
    else
    {
        s_onpid(nv, rd, pid_typeTOR);
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_motor_params(const EOnv* nv, const eOropdescriptor_t* rd) 
{   // only 2foc
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        eOmc_motor_params_t *params_ptr = (eOmc_motor_params_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);        
        eo_emsController_SetMotorParams ((uint8_t)jxx, *params_ptr);
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_impedance_t *impedance = (eOmc_impedance_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        eo_emsController_SetImpedance(jxx, impedance->stiffness, impedance->damping, impedance->offset);
    }
    else // mc4can
    {   
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;

        if(eo_ropcode_set == rd->ropcode)
        {    
            EOtheMeasuresConverter* appMeasConv = eo_measconv_GetHandle();
            icubCanProto_impedance_t impedence_icubCanProtValues = {0};  
            impedence_icubCanProtValues.stiffness = eo_measconv_impedenceStiffness_I2S(appMeasConv, jxx, impedance->stiffness);
            impedence_icubCanProtValues.damping = eo_measconv_impedenceDamping_I2S(appMeasConv, jxx, impedance->damping);
            impedence_icubCanProtValues.offset = eo_measconv_torque_I2S(appMeasConv, jxx, impedance->offset);

            
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
            command.value = &impedence_icubCanProtValues;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
            
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
            command.value = &impedence_icubCanProtValues.offset;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

            return;
        }
    #ifdef USE_PROTO_PROXY
        else if(eo_ropcode_ask == rd->ropcode)
        {      
            EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
            eOproxy_params_t *param = eo_proxy_Params_Get(proxy, rd->id32);
            if(NULL == param)
            {
                eOerrmanDescriptor_t errdes = {0};
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_forward_callback_fails);
                errdes.par16            = 0; 
                errdes.par64            = ((uint64_t)rd->signature << 32) | (rd->id32); 
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
                return;
            }
            param->p08_1 = 2;       // we expect two can frames
            param->p08_2 = 0;       // and we havent received any yet
                   
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS;
            command.value = NULL;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
            
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET;
            command.value = NULL;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);         
            
            return;
        }
    #endif         
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_position_limits_t *limitsofjoint = (eOmeas_position_limits_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        eo_emsController_SetPosMin(jxx, limitsofjoint->min);
        eo_emsController_SetPosMax(jxx, limitsofjoint->max);
    }
    else // mc4can
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    

        if(eo_ropcode_set == rd->ropcode)
        {    
            icubCanProto_position_t minpos_icubCanProtValue = eo_measconv_jntPosition_I2E(eo_measconv_GetHandle(), jxx, limitsofjoint->min);
            icubCanProto_position_t maxpos_icubCanProtValue = eo_measconv_jntPosition_I2E(eo_measconv_GetHandle(), jxx, limitsofjoint->max);

            if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
            {
                //swap min and max
                icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                minpos_icubCanProtValue = maxpos_icubCanProtValue;
                maxpos_icubCanProtValue = pos_icubCanProtValue;
            }
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
            command.value = &minpos_icubCanProtValue;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
            
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
            command.value = &maxpos_icubCanProtValue;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
            
            return;
        }
    #ifdef USE_PROTO_PROXY
        else if(eo_ropcode_ask == rd->ropcode)
        {     
            EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
            eOproxy_params_t *param = eo_proxy_Params_Get(proxy, rd->id32);
            if(NULL == param)
            {
                eOerrmanDescriptor_t errdes = {0};
                errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress    = 0;
                errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_forward_callback_fails);
                errdes.par16            = 0; 
                errdes.par64            = ((uint64_t)rd->signature << 32) | (rd->id32); 
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
                return;
            }
            param->p08_1 = 2;       // we expect two can frames
            param->p08_2 = 0;       // and we havent received any yet
            
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION;
            command.value = NULL;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
            
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION;
            command.value = NULL;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);         
            
            return;        

        }
    #endif        
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t *time = (eOmeas_time_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

    if(eobool_true == s_motorcontrol_is2foc_based())
    {    
        eo_emsController_SetVelTimeout(jxx, *time);
    }
    else // mc4can
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
        command.value = time;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{   // 2foc or mc4can is equal
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    if(NULL == jstatus)
    {
        return; //error
    }
    
    //#warning marco.accame: better using cast to eOmc_motionmonitormode_t
    if(eomc_motionmonitormode_dontmonitor == *((eOenum08_t*)rd->data))
    {
        jstatus->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }
}

extern void eoprot_fun_UPDT_mc_joint_config_encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd)
{   // mc4can only     
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        return;
    }
    
    eOresult_t res = eores_NOK_generic;    
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOutil_emulfloat32_t *encfactor = (eOutil_emulfloat32_t*)rd->data;

    res = eo_measconv_SetJntEncoderConversionFactor(eo_measconv_GetHandle(), jxx, (eOmeasconv_encConversionFactor_t)eo_common_Q17_14_to_float(*encfactor));
    if(eores_OK != res)
    {
        return; //error 
    }
}

extern void eoprot_fun_UPDT_mc_joint_config_encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd)
{   // mc4can only     
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        return;
    }
    
    eOresult_t res = eores_NOK_generic;     
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOutil_emulfloat32_t *encoffset = (eOutil_emulfloat32_t*)rd->data;

    res = eo_measconv_SetJntEncoderConversionOffset(eo_measconv_GetHandle(), jxx, (eOmeasconv_encConversionOffset_t)eo_common_Q17_14_to_float(*encoffset));
    if(eores_OK != res)
    {
        return; //error 
    }
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_setpoint_t *setpoint = (eOmc_setpoint_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_t *joint = eo_entities_GetJoint(eo_entities_GetHandle(), jxx);

    if(NULL == joint)
    {
        return; //error
    } 
       
    //#warning -> marco.accame: cast to proper type
    if(eomc_motionmonitormode_forever == joint->config.motionmonitormode)
    {       
        //#warning --> marco.accame: cast to proper type
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == joint->status.basic.motionmonitorstatus)
        {
            joint->status.basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
        }
    }


    if(eobool_true == s_motorcontrol_is2foc_based())   
    {        
        switch(setpoint->type)
        { 
            case eomc_setpoint_position:
            {
                eo_emsController_SetPosRef(jxx, setpoint->to.position.value, setpoint->to.position.withvelocity);
            } break;
            
            case eomc_setpoint_positionraw:
            {
                eo_emsController_SetPosRaw(jxx, setpoint->to.position.value);
            } break;
            
            case eomc_setpoint_velocity:
            {
                eo_emsController_SetVelRef(jxx, setpoint->to.velocity.value, setpoint->to.velocity.withacceleration);    
            } break;

            case eomc_setpoint_torque:
            {
                eo_emsController_SetTrqRef(jxx, setpoint->to.torque.value);
            } break;

            case eomc_setpoint_current:
            {
                eo_emsController_SetOutput(jxx, setpoint->to.current.value);
            } break;

            default:
            {
                
            } break;
        }
    }
    else // mc4can
    {
        EOtheMeasuresConverter *appMeasConv = eo_measconv_GetHandle();
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = 0;
        command.value = NULL;


        // marco.accame: i put them in here and not inside the case {} because of scope of the variables ...
        icubCanProto_setpoint_position_t setpoint_pos = {0};
        icubCanProto_setpoint_velocity_t setpoint_vel = {0};
        icubCanProto_setpoint_torque_t setpoint_torque = {0};
        icubCanProto_setpoint_current_t setpoint_current = {0};
        icubCanProto_position_t pos = 0;
        
        command.type  = 0;
        
        switch(setpoint->type)
        {
            case eomc_setpoint_position:
            {                
                setpoint_pos.value = eo_measconv_jntPosition_I2E(appMeasConv, jxx, setpoint->to.position.value);
                //reference velocity of position set point must be always >0, so here absolute func is used.
                setpoint_pos.withvelocity = eo_measconv_jntVelocity_I2E_abs(appMeasConv, jxx, setpoint->to.position.withvelocity);
                
                command.type  = ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE; 
                command.value =  &setpoint_pos; 
            } break;

            case eomc_setpoint_velocity:
            {   
                setpoint_vel.withacceleration = eo_measconv_jntAcceleration_I2E_abs(appMeasConv, jxx, setpoint->to.velocity.withacceleration);           
                setpoint_vel.value = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, setpoint->to.velocity.value);
                
                if (setpoint_vel.withacceleration < 1)
                {
                    setpoint_vel.withacceleration = 1;
                }            

                command.type  = ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE; 
                command.value =  &setpoint_vel;                                  
            } break;

            case eomc_setpoint_torque:
            {                
                setpoint_torque.value = eo_measconv_torque_I2S(appMeasConv,jxx, setpoint->to.torque.value);
                  
                command.type  = ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE; 
                command.value =  &setpoint_torque;                  
            } break;

            case eomc_setpoint_current:
            { 
                // marco.accame on 27 feb 2015.               
                // when embObjMotionControl::setRefOutputRaw() sends a set<setpoint, (current, value)>,
                // here is what it is done: we send it to mc4 with a openloop params command ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS.
                // then the reading back is done with embObjMotionControl::getRefOutputRaw() which
                // just reads its buffered value of joint.status.ofpid.positionreference which in this case 
                // must contain the value of the openloop current as received and accepted by the mc4 
                // board. the mc4 board could refuse to accept the value if for instance is not in openloop mode.
                // moreover: the mc4 board does not normally broadcast this value, thus we must ask it
                // in an explicit way with a further CAN message of type ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS
                // at this point, in the tx phase the two CAN commands (set and then get) are sent and the
                // reply of teh get is received at 1 ms from now in the next rx phase (hopefully).
                // at this point the rx handler of the openloop param fills the value of joint.status.ofpid.positionreference
                // which is broadcasted up to robotInterface about 1.8 ms after now.
                // ... i can smell danger of a race condition if a high level user calls setRefOutputRaw() and then getRefOutputRaw()
                // without any wait time in between.
                // ... moreover: the getRefOutputRaw() returns the value buffered in EMS and asked to mc4 the last time a setRefOutputRaw()
                // was called.
                // anyway: for now it works fine and having asked around to people in the lab the thing is not worrying them
                // a solution: getRefOutputRaw() does not read the joint.status but directly
                // send a ask<eoprot_tag_mc_joint_status_ofpid_positionreference> WHICH IS A NEW TAG TO BE ADDED.
                // then we use the proxy so that this request is sent to the mc4 which gives the value and then the value
                // is sent up with a say<eoprot_tag_mc_joint_status_ofpid_positionreference, value> 
                // in such a way the value is always correct   
                               
                setpoint_current.value = setpoint->to.current.value;

                command.type  = ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS; 
                command.value =  &setpoint_current;                  
            } break;

            case eomc_setpoint_positionraw:
            {    
                pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, setpoint->to.position.value);
                command.type  = ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION; 
                command.value =  &pos;                   
            } break;
            
            default:
            {
                command.type  = 0;
            } break;
        }
        
        if(0 == command.type)
        {
            return;
        }

        // send msg
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

               
        if(eomc_setpoint_current == setpoint->type)
        {
            // since mc4 does not send periodic messages with openloop reference values
            // i need to ask it direct to mc4
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS; 
            command.value =  NULL;  
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
        }
        
    }
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    if(eobool_true == s_motorcontrol_is2foc_based()) 
    {
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        eo_emsController_Stop(jxx);
    }
    else // mc4can
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY;
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
    }
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;    
    
    
    if(eobool_true == s_motorcontrol_is2foc_based()) 
    {
        
        // must send something to can: 
        // ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD + ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN + ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE
        
        #ifdef EXPERIMENTAL_SPEED_CONTROL
        icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_velocity;
        #else
        icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_openloop;
        #endif
        
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;
        
        // first one:
        command.type  = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

        // second one:
        command.type  = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

        // third one:
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
        command.value = &controlmode_2foc;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    

        eo_emsController_StartCalibration(jxx, 
                                          calibrator->params.type3.position, 
                                          calibrator->params.type3.velocity,
                                          calibrator->params.type3.offset);

    }
    else // mc4can
    {
        
        icubCanProto_calibrator_t iCubCanProtCalibrator = {.type = icubCanProto_calibration_type0_hard_stops}; // all the rest is 0
        iCubCanProtCalibrator.type = (icubCanProto_calibration_type_t)calibrator->type;
        
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER;
        command.value = &iCubCanProtCalibrator;   
              
        eObool_t found = eobool_false;
        
        switch(calibrator->type)
        {
            case eomc_calibration_type0_hard_stops:
            {
                iCubCanProtCalibrator.params.type0.pwmlimit = calibrator->params.type0.pwmlimit;
    //            iCubCanProtCalibrator.params.type0.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type0.velocity);           
                iCubCanProtCalibrator.params.type0.velocity = calibrator->params.type0.velocity;  
                found = eobool_true;            
            } break;
                
            case eomc_calibration_type1_abs_sens_analog:
            {
    //             icubCanProto_position_t pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, calibrator->params.type1.position);
    //             /*sice pos param is a word of 16 bits i must check min and max*/
    //             if((pos < INT16_MIN)||(pos>INT16_MAX))
    //             {
    //                 return;
    //             }
    //             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
    //             iCubCanProtCalibrator.params.type1.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type1.velocity);
                iCubCanProtCalibrator.params.type1.position = calibrator->params.type1.position; 
                iCubCanProtCalibrator.params.type1.velocity =  calibrator->params.type1.velocity;
                found = eobool_true; 
            } break;

            case eomc_calibration_type2_hard_stops_diff:
            {
                iCubCanProtCalibrator.params.type2.pwmlimit = calibrator->params.type2.pwmlimit;
    //            iCubCanProtCalibrator.params.type2.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type2.velocity);           
                iCubCanProtCalibrator.params.type2.velocity = calibrator->params.type2.velocity;   
                found = eobool_true;             
            } break;

            case eomc_calibration_type3_abs_sens_digital:
            {
    //              icubCanProto_position_t pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, calibrator->params.type3.position);
    //             /*sice pos param is a word of 16 bits i must check min and max*/
    //             if((pos < INT16_MIN)||(pos>INT16_MAX))
    //             {
    //                 return;
    //             }
    //             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
    //             iCubCanProtCalibrator.params.type3.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type3.velocity);           
                iCubCanProtCalibrator.params.type1.position = calibrator->params.type3.position; 
                iCubCanProtCalibrator.params.type3.velocity = calibrator->params.type3.velocity;            
                iCubCanProtCalibrator.params.type3.offset = calibrator->params.type3.offset;
                found = eobool_true; 
            } break;

            case eomc_calibration_type4_abs_and_incremental:
            {
    //             icubCanProto_position_t pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, calibrator->params.type4.position);
    //             //here position is in int16_t ==> so i must verify if pos is out of int16_t range
    //             if((pos > INT16_MAX) || (pos < INT16_MIN))
    //             {
    //                 return;
    //                 #warning VALE --> how to manage this error???
    //             }
    //             iCubCanProtCalibrator.params.type4.position = (icubCanProto_position4calib_t)pos; 
    //            iCubCanProtCalibrator.params.type4.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type4.velocity);            
                iCubCanProtCalibrator.params.type4.position = calibrator->params.type4.position;
                iCubCanProtCalibrator.params.type4.velocity = calibrator->params.type4.velocity;
                iCubCanProtCalibrator.params.type4.maxencoder = calibrator->params.type4.maxencoder;
                found = eobool_true; 
            } break;
            
            default:
            {
                found = eobool_false; 
            } break;        
        }
        
        if(eobool_false == found)
        {
            return;
        }

        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
        
    }

}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_controlmode_command_t *controlmode = (eOmc_controlmode_command_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
      
    if(eobool_true == s_motorcontrol_is2foc_based()) 
    {       
        eo_emsController_SetControlModeGroupJoints(jxx, (eOmc_controlmode_command_t)(*controlmode));    
    }
    else // mc4can
    {
        icubCanProto_controlmode_t icubcanProto_controlmode = icubCanProto_controlmode_idle;
        if(eores_OK != s_translate_eOmcControlMode2icubCanProtoControlMode(*controlmode, jxx, &icubcanProto_controlmode))
        {
            return;
        }        
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
        command.value = &icubcanProto_controlmode;   
                   
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
    }    
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_interactionmode_t* interaction = (eOmc_interactionmode_t*)rd->data;
    
    if(eobool_true == s_motorcontrol_is2foc_based()) 
    {
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);    
        eo_emsController_SetInteractionModeGroupJoints(jxx, *interaction);
    }
    else
    {
        icubCanProto_interactionmode_t icub_interactionmode = icubCanProto_interactionmode_unknownError;
        
        if(eOmc_interactionmode_stiff == *interaction)
        {
            icub_interactionmode = icubCanProto_interactionmode_stiff;
        }
        else if(eOmc_interactionmode_compliant == *interaction)
        {
            icub_interactionmode = icubCanProto_interactionmode_compliant;
        }
        else
        {
            return;
        }    
        
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE;
        command.value = &icub_interactionmode;   
                  
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
    }
}



extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_torque_t *torque = (eOmeas_torque_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    if(eobool_true == s_motorcontrol_is2foc_based()) 
    {
        eo_emsController_ReadTorque(jxx, *torque);
    }
    else // mc4can
    {
        icubCanProto_torque_t icub_torque = *torque + 0x8000;
        eo_virtualstrain_SetTorque(eo_virtualstrain_GetHandle(), jxx, icub_torque);        
    }
}

extern void eoprot_fun_UPDT_mc_motor_config_gearboxratio(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    // 2foc only
    if(eobool_false == s_motorcontrol_is2foc_based()) 
    {
        return;
    }
    
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    int32_t *gbx_ptr = (int32_t*)rd->data;
    eo_emsController_SetGearboxRatio(jxx, *gbx_ptr);
}

extern void eoprot_fun_UPDT_mc_motor_config_rotorencoder(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // 2foc only
    if(eobool_false == s_motorcontrol_is2foc_based()) 
    {
        return;
    }
    
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    int32_t *gbx_ptr = (int32_t*)rd->data;
    eo_emsController_SetRotorEncoder(jxx, *gbx_ptr);
}

#if defined(EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK)
    #warning --> EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK is defined, thus we are not using eo_emsController_set_Jacobian() etc
#else
extern void eoprot_fun_UPDT_mc_controller_config_jointcoupling(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    // 2foc only
    if(eobool_false == s_motorcontrol_is2foc_based()) 
    {
        return;
    }
    
    eOmc_jointcouplingmatrix_t *mat = (eOmc_jointcouplingmatrix_t*)rd->data;
    
    /*
    float Ji[4][4];
    
    for (int i=0; i<4; ++i)
    {
        for (int j=0; j<4; ++j)
        {
            Ji[i][j]=(float)((*mat)[i][j])/16384.0f;
        }
    }
    */
    
    eo_emsController_set_Jacobian(*mat);
        
    #warning --> marco.accame: put in here the debug messages for jointcoupling (and then remove them)
        
    eOerrmanDescriptor_t errdes = {0};
    errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    errdes.param                = 0;
    errdes.sourcedevice         = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress        = 0;  
    //char *str = NULL;
    char str[eomn_info_status_extra_sizeof] = {0};
 
    for (int i=0; i<4; ++i)
    {
        snprintf(str, sizeof(str), "r%d: %f %f %f %f", i, Ji[i][0], Ji[i][1], Ji[i][2], Ji[i][3]);             
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);    
    }   
}
#endif


// -- entity motor

extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv)
{
    eOmc_motor_config_t *cfg = (eOmc_motor_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &motor_default_value.config, sizeof(eOmc_motor_config_t));
}

extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv)
{
    eOmc_motor_status_t *cfg = (eOmc_motor_status_t*)eo_nv_RAM(nv);
    memcpy(cfg, &motor_default_value.status, sizeof(eOmc_motor_status_t));
}


extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    // must send some can frames. however, in here i need the type of attached board: 1foc or mc4.
    // there are many modes to do this, but in wait of the EOmcManager object we just use a s_motorcontrol_is2foc_based() function
    
    eOmc_motor_config_t *cfg_ptr = (eOmc_motor_config_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    
    // in here i assume that all the mc boards are either 1foc or mc4
    if(eobool_true == s_motorcontrol_is2foc_based())
    {
        // send current pid
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
        command.value = &cfg_ptr->pidcurrent;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

        // send current pid limits
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
        command.value = &cfg_ptr->pidcurrent;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);             
    }
    
    // set max velocity      
    icubCanProto_velocity_t vel_icubCanProtValue = eo_measconv_jntVelocity_I2E(eo_measconv_GetHandle(), mxx, cfg_ptr->maxvelocityofmotor);           
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    command.value = &vel_icubCanProtValue;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

    // set current limit  
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    command.value = &cfg_ptr->maxcurrentofmotor;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

}

extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
       
    // send current pid
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    command.value = pid;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);

    // send current pid limits
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
    command.value = pid;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);             

}


extern void eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_velocity_t *vel = (eOmeas_velocity_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    
    // set max velocity  
    icubCanProto_velocity_t vel_icubCanProtValue = eo_measconv_jntVelocity_I2E(eo_measconv_GetHandle(), mxx, *vel);           
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    command.value = &vel_icubCanProtValue;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

}


extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    eOmeas_current_t *curr = (eOmeas_current_t*)rd->data;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;

    // set current limit  
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    command.value = curr;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);     
  
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// in the future, there will be 2foc, mc4can, and mc4plus
static eObool_t s_motorcontrol_is2foc_based(void)
{
    static eOmn_appl_runMode_t apprunmode = applrunMode__default;
    static eObool_t is2focbased = eobool_false; 
    if(applrunMode__default == apprunmode)
    {
        apprunmode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
        is2focbased = (applrunMode__2foc == apprunmode) ? (eobool_true) : (eobool_false);
    }
    return(is2focbased);
}


// -- 2foc-based functions



// -- mc4can-based functions

static eOresult_t s_translate_eOmcControlMode2icubCanProtoControlMode(eOmc_controlmode_command_t eomc_controlmode, eOprotIndex_t jId,
                                                                      icubCanProto_controlmode_t *icubcanProto_controlmode)
{
    eOresult_t res = eores_OK;
    
    switch(eomc_controlmode)
    {
        case eomc_controlmode_cmd_position:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_position;
        } break;
        case eomc_controlmode_cmd_velocity:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_velocity;
        } break;
        case eomc_controlmode_cmd_torque:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_torque;
        } break;
        case eomc_controlmode_cmd_impedance_pos:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_impedance_pos;
        } break;
        case eomc_controlmode_cmd_impedance_vel:
        {
             *icubcanProto_controlmode = icubCanProto_controlmode_impedance_vel;
        } break;
        case eomc_controlmode_cmd_current:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_current;
        } break;
        case eomc_controlmode_cmd_openloop:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_openloop;
        } break;
        case eomc_controlmode_cmd_switch_everything_off:
        {
            res = eores_NOK_generic;
        } break;
        case eomc_controlmode_cmd_force_idle:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_forceIdle;
        } break;
        case eomc_controlmode_cmd_mixed:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_mixed;
        } break;
        case eomc_controlmode_cmd_direct:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_direct;
        } break;
        case eomc_controlmode_cmd_idle:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_idle;
        } break;
        default:
        {
            res = eores_NOK_generic;
        } break;
            
    }
    return(res);
}


// type is: 0 pos, 1 torque
static void s_onpid(const EOnv* nv, const eOropdescriptor_t* rd, pid_type_t type)
{
    static const uint8_t cmd_set_pid[2] = {ICUBCANPROTO_POL_MC_CMD__SET_POS_PID, ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID};
    static const uint8_t cmd_set_pidlimits[2] = {ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS, ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS};
    static const uint8_t cmd_get_pid[2] = {ICUBCANPROTO_POL_MC_CMD__GET_POS_PID, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID};
    static const uint8_t cmd_get_pidlimits[2] = {ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS};
        
    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;    
    
    // if set, we just send two commands
    if(eo_ropcode_set == rd->ropcode)
    {
        command.type  = cmd_set_pid[type]; // ICUBCANPROTO_POL_MC_CMD__SET_POS_PID or ..
        command.value = pid;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
        
        command.type  = cmd_set_pidlimits[type]; // ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS or ..
        command.value = pid;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
        
        return;
    }
    
#ifdef USE_PROTO_PROXY
    // if ask and we have the proxy, we ask to can about two values
    else if(eo_ropcode_ask == rd->ropcode)
    {
       
        EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
        eOproxy_params_t *param = eo_proxy_Params_Get(proxy, rd->id32);
        if(NULL == param)
        {
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
            errdes.sourceaddress    = 0;
            errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_forward_callback_fails);
            errdes.par16            = 0; 
            errdes.par64            = ((uint64_t)rd->signature << 32) | (rd->id32); 
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
            return;
        }
        param->p08_1 = 2;       // we expect two can frames
        param->p08_2 = 0;       // and we havent received any yet

        command.type  = cmd_get_pid[type]; // ICUBCANPROTO_POL_MC_CMD__GET_POS_PID or ..
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
        
        command.type  = cmd_get_pidlimits[type]; // ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS or ..
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
        
        return;
    }
#endif//USE_PROTO_PROXY    
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



