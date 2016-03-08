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

#include "EoMotionControl.h"



#include "EOtheEntities.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANservice.h"


#include "EOtheBOARDtransceiver.h"


#include "EOtheMotionController.h"



// mc4-case
#include "EOproxy.h"
#include "EOtheMC4boards.h"
#include "EOtheVirtualStrain.h"

// foc-case
#include "Controller.h"

// mc4lus-case
#include "EOCurrentsWatchdog.h"


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

// -- common functions

static eOmotioncontroller_mode_t s_motorcontrol_getmode(void);


// -- foc-based functions


// -- mc4plus-based functions

// TODOALE manage this stuff
//static void s_eoprot_ep_mc_fun_MotorReactivationAttempt(uint8_t motor, uint32_t current_state);


// -- mc4can-based functions

static void s_onpid(const EOnv* nv, const eOropdescriptor_t* rd, pid_type_t type);
static eOresult_t s_translate_eOmcControlMode2icubCanProtoControlMode(eOmc_controlmode_command_t eomc_controlmode, eOprotIndex_t jId,
                                                                      icubCanProto_controlmode_t *icubcanProto_controlmode);



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


// -- entity joint


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    // now we see if it is a mc4can or a 2foc or a mc4plus    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
        
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_joint(jxx, cfg);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        // first of all: set conversion factor
//        EOtheMeasuresConverter* mc4boards = eo_measconv_GetHandle();
//        eo_measconv_SetJntEncoderConversionFactor(mc4boards, jxx, (eOmeasconv_encConversionFactor_t)eo_common_Q17_14_to_float(cfg->encoderconversionfactor));
//        eo_measconv_SetJntEncoderConversionOffset(mc4boards, jxx, (eOmeasconv_encConversionOffset_t)eo_common_Q17_14_to_float(cfg->encoderconversionoffset));
        EOtheMC4boards *mc4boards = eo_mc4boards_GetHandle();
//        eo_mc4boards_Convert_encoderfactor_Set(mc4boards, jxx, (eOmc4boards_conv_encoder_factor_t)eo_common_Q17_14_to_float(cfg->DEPRECATED_encoderconversionfactor));
//        eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, (eOmc4boards_conv_encoder_offset_t)eo_common_Q17_14_to_float(cfg->DEPRECATED_encoderconversionoffset));
        eo_mc4boards_Convert_encoderfactor_Set(mc4boards, jxx, (float)cfg->jntEncoderResolution/65535.0f);
        eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, 0); //->>> moved to the calibrators.
      
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
        
        // marco.accame TODO: (tag FIX_CALIB_MC4) manage max and min limits as in new_fw_ems ...
        // 4) set max/min limits... they will be used during calibration phase

        eo_mc4boards_Convert_minJointPos_Set(mc4boards, jxx, cfg->limitsofjoint.min);
        eo_mc4boards_Convert_maxJointPos_Set(mc4boards, jxx, cfg->limitsofjoint.max);
 
        // 5) set vel timeout
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
        command.value = &cfg->velocitysetpointtimeout;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);     
        

        // 6) set impedance 
        icubCanProto_impedance_t impedence_icubCanProtValues = {0};  
//        impedence_icubCanProtValues.stiffness = eo_measconv_impedenceStiffness_I2S(mc4boards, jxx, cfg->impedance.stiffness);
//        impedence_icubCanProtValues.damping = eo_mc4boards_Convert_impedanceDamping_I2S(mc4boards, jxx, cfg->impedance.damping);
//        impedence_icubCanProtValues.offset = eo_mc4boards_Convert_torque_I2S(mc4boards, jxx, cfg->impedance.offset);

        impedence_icubCanProtValues.stiffness = eo_mc4boards_Convert_impedanceStiffness_I2S(eo_mc4boards_GetHandle(), jxx, cfg->impedance.stiffness);
        impedence_icubCanProtValues.damping = eo_mc4boards_Convert_impedanceDamping_I2S(eo_mc4boards_GetHandle(), jxx, cfg->impedance.damping);
        impedence_icubCanProtValues.offset = eo_mc4boards_Convert_torque_I2S(eo_mc4boards_GetHandle(), jxx, cfg->impedance.offset);
        
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
        command.value = &impedence_icubCanProtValues;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
        
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
        command.value = &impedence_icubCanProtValues.offset;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);   


        // max velocity of joint. it MUST be positive
        icubCanProto_velocity_t vel_ticks = eo_mc4boards_Convert_Velocity_toCAN(eo_mc4boards_GetHandle(), jxx, cfg->maxvelocityofjoint, eomc4_velocitycontext_toCAN_unsigned); 
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
        command.value = &vel_ticks; 
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);         
    }

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
    float rescaler = 1.0f/(float)(1<<pid->scale);

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_pos_pid(jxx, pid);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        s_onpid(nv, rd, pid_typePOS);
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
    float rescaler = 1.0f/(float)(1<<pid->scale);

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_trq_pid(jxx, pid);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        s_onpid(nv, rd, pid_typeTOR);
    }
}



// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config_motor_params(const EOnv* nv, const eOropdescriptor_t* rd) 
{   // not for mc4can
    eOmc_motor_params_t *mparams = (eOmc_motor_params_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);        

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_motor_friction(jxx, mparams);
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_impedance_t *impedance = (eOmc_impedance_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_joint_impedance(jxx, impedance);
    } // mark
    else if(eo_motcon_mode_mc4 == mcmode)
    {   
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;

        if(eo_ropcode_set == rd->ropcode)
        {    
            EOtheMC4boards* mc4boards = eo_mc4boards_GetHandle();
            icubCanProto_impedance_t impedence_icubCanProtValues = {0};  
            impedence_icubCanProtValues.stiffness = eo_mc4boards_Convert_impedanceStiffness_I2S(mc4boards, jxx, impedance->stiffness);
            impedence_icubCanProtValues.damping = eo_mc4boards_Convert_impedanceDamping_I2S(mc4boards, jxx, impedance->damping);
            impedence_icubCanProtValues.offset = eo_mc4boards_Convert_torque_I2S(mc4boards, jxx, impedance->offset);

            
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
            command.value = &impedence_icubCanProtValues;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
            
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
            command.value = &impedence_icubCanProtValues.offset;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

            return;
        }
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
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_position_limits_t *limitsofjoint = (eOmeas_position_limits_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_joint_pos_limits(jxx, limitsofjoint->min, limitsofjoint->max);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    

        if(eo_ropcode_set == rd->ropcode)
        {    
            icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(eo_mc4boards_GetHandle(), jxx, limitsofjoint->min);
            icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(eo_mc4boards_GetHandle(), jxx, limitsofjoint->max);

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
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t *time = (eOmeas_time_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_joint_vel_ref_timeout(jxx, *time);
    } // marker
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
        command.value = time;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_status_core_modes_ismotiondone(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
   
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        // do nothing
    } // marker
    else if(eo_motcon_mode_mc4 == mcmode)
    {          
        if(eo_ropcode_ask == rd->ropcode)
        {   
            // must send a get motion done request to the mc4 board and then ... activate the proxy for the reply to robotInterface            
            eOcanprot_command_t command = {0};
            command.class = eocanprot_msgclass_pollingMotorControl;
            command.type = ICUBCANPROTO_POL_MC_CMD__MOTION_DONE;
            command.value = NULL; 
        
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
            param->p08_1 = 1;       // we expect one can frame
            param->p08_2 = 0;       // and we havent received any yet
              
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
                      
            return;
        }
    
    }    

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_setpoint_t *setpoint = (eOmc_setpoint_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_t *joint = eo_entities_GetJoint(eo_entities_GetHandle(), jxx);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

    if(NULL == joint)
    {
        return; // error... maybe robotInterface sends a command to a joint with index higher than supported
    } 
       

    joint->status.core.modes.ismotiondone = eobool_false;

    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
        switch(setpoint->type)
        { 
            case eomc_setpoint_position:
            {
                if(MController_set_joint_pos_ref(jxx, setpoint->to.position.value, setpoint->to.position.withvelocity))
                {
                    joint->status.target.trgt_position = setpoint->to.position.value;
                }
            } break;
            
            case eomc_setpoint_positionraw:
            {
                if(MController_set_joint_pos_raw(jxx, setpoint->to.positionraw.value))
                {
                    joint->status.target.trgt_positionraw = setpoint->to.positionraw.value;
                }
            } break;
            
            case eomc_setpoint_velocity:
            {
                if(MController_set_joint_vel_ref(jxx, setpoint->to.velocity.value, setpoint->to.velocity.withacceleration))
                {
                    joint->status.target.trgt_velocity = setpoint->to.velocity.value;
                }    
            } break;

            case eomc_setpoint_torque:
            {
                if(MController_set_joint_trq_ref(jxx, setpoint->to.torque.value))
                {
                    joint->status.target.trgt_torque = setpoint->to.torque.value;
                }

            } break;

            case eomc_setpoint_openloop:
            {
                if(MController_set_joint_out_ref(jxx, setpoint->to.openloop.value))
                {
                    joint->status.target.trgt_openloop = setpoint->to.openloop.value;
                }
            } break;

            default:
            {
                
            } break;
        }
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        EOtheMC4boards *mc4boards = eo_mc4boards_GetHandle();
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = 0;
        command.value = NULL;


        // marco.accame: i put them in here and not inside the case {} because of scope of the variables ...
        icubCanProto_setpoint_position_t setpoint_pos = {0};
        icubCanProto_setpoint_velocity_t setpoint_vel = {0};
        icubCanProto_setpoint_torque_t setpoint_torque = {0};
        icubCanProto_setpoint_openloop_t setpoint_openloop = {0};
        icubCanProto_position_t pos = 0;
        
        command.type  = 0;
        
        switch(setpoint->type)
        {
            case eomc_setpoint_position:
            {                
                setpoint_pos.value = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, setpoint->to.position.value);
                setpoint_pos.withvelocity = eo_mc4boards_Convert_Velocity_toCAN(mc4boards, jxx, setpoint->to.position.withvelocity, eomc4_velocitycontext_toCAN_positionsetpoint);
                
                command.type  = ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE; 
                command.value =  &setpoint_pos;

                joint->status.target.trgt_position = setpoint->to.position.value;
            } break;

            case eomc_setpoint_velocity:
            {   
                setpoint_vel.value = eo_mc4boards_Convert_Velocity_toCAN(mc4boards, jxx, setpoint->to.velocity.value, eomc4_velocitycontext_toCAN_signed);
                setpoint_vel.withacceleration = eo_mc4boards_Convert_Acceleration_toCAN(mc4boards, jxx, setpoint->to.velocity.withacceleration);                          
                // acceleration in velocity setpoint must be positive, as it is the velocity which has the sign. moreover: it must be higher that 1 tick/ms^2
                if (setpoint_vel.withacceleration < 1)
                {
                    setpoint_vel.withacceleration = 1;
                } 
                
                command.type  = ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE; 
                command.value =  &setpoint_vel;   
                
                joint->status.target.trgt_velocity = setpoint->to.velocity.value;
//#undef DEBUG_SETPOINT_VELOCITY
//#if defined(DEBUG_SETPOINT_VELOCITY) 
//                
//                eOerrmanDescriptor_t errdes = {0};
//                static eOmeas_acceleration_t prevAcc = -1;
//                static eOmeas_velocity_t prevVel = -1;
//                
//                eObool_t print = eobool_false;
//                
//                if((prevAcc != setpoint->to.velocity.withacceleration) || (prevVel != setpoint->to.velocity.value))
//                {
//                    print = eobool_true;   
//                }
//                prevAcc = setpoint->to.velocity.withacceleration;
//                prevVel = setpoint->to.velocity.value;
//                
//                if(eobool_true == print)
//                {
//                    // original
//                    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
//                    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//                    errdes.sourceaddress    = jxx;
//                    errdes.par16            = 0xfff1;
//                    errdes.par64            = (setpoint->to.velocity.value) | ((uint64_t)(setpoint->to.velocity.withacceleration)<<32);
//                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &errdes);
//                }  
//    
//                if(eobool_true == print)
//                {
//                    // conversion
//                    errdes.code             = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
//                    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//                    errdes.sourceaddress    = jxx;
//                    errdes.par16            = 0xfff2;
//                    errdes.par64            = (setpoint_vel.value) | ((uint64_t)(setpoint_vel.withacceleration)<<32);
//                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &errdes);  
//                }                    
//                
//#endif //defined(DEBUG_SETPOINT_VELOCITY) 
                                               
            } break;

            case eomc_setpoint_torque:
            {                
                setpoint_torque.value = eo_mc4boards_Convert_torque_I2S(mc4boards,jxx, setpoint->to.torque.value);
                  
                command.type  = ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE; 
                command.value =  &setpoint_torque;         
                
                joint->status.target.trgt_torque = setpoint->to.torque.value;
            } break;

            case eomc_setpoint_openloop:
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
                               
                setpoint_openloop.value = setpoint->to.openloop.value;

                command.type  = ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS; 
                command.value =  &setpoint_openloop;   

                 joint->status.target.trgt_openloop = setpoint->to.openloop.value;
            } break;

            case eomc_setpoint_positionraw:
            {    
                pos = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, setpoint->to.positionraw.value);
                command.type  = ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION; 
                command.value =  &pos;  

                joint->status.target.trgt_positionraw = setpoint->to.positionraw.value;
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

               
        if(eomc_setpoint_openloop == setpoint->type)
        {
            // since mc4 does not send periodic messages with openloop reference values
            // i need to ask it direct to mc4
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS; 
            command.value =  NULL;  
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
        }
        
    }   // marker-end-of-case-mc4-can

} // f-marker-end


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        MController_stop_joint(jxx);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY;
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
    }

} // fmarker-end


// f-marker-begin
static void send_diagnostic_debugmessage(eOerrmanErrorType_t type, eOerror_value_DEB_t value, uint8_t jointnum, uint16_t par16, uint64_t par64, const char* info)
{
    eOerrmanDescriptor_t errdes = {0};

    errdes.code             = eoerror_code_get(eoerror_category_Debug, value);
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = jointnum;
    errdes.par16            = par16;
    errdes.par64            = par64;
    eo_errman_Error(eo_errman_GetHandle(), type, info, NULL, &errdes);
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;    
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_calibrate(jxx, calibrator);        
    } 
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        EOtheMC4boards *mc4boards = eo_mc4boards_GetHandle();
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
    //            iCubCanProtCalibrator.params.type0.velocity = eo_measconv_jntVelocity_toCAN(mc4boards, jxx, calibrator->params.type0.velocity);           
                iCubCanProtCalibrator.params.type0.velocity = calibrator->params.type0.velocity;
                eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type0.calibrationZero);
                //the following block is repeated beatween all calibration types... it should be revised.
                {
                   icubCanProto_position_t tmp_minpos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_maxpos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_minpos);
                   icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_maxpos);
                   if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                       minpos_icubCanProtValue = maxpos_icubCanProtValue;
                       maxpos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &minpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &maxpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                }
                found = eobool_true;
            } break;
                
            case eomc_calibration_type1_abs_sens_analog:
            {
    //             icubCanProto_position_t pos = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, calibrator->params.type1.position);
    //             /*sice pos param is a word of 16 bits i must check min and max*/
    //             if((pos < INT16_MIN)||(pos>INT16_MAX))
    //             {
    //                 return;
    //             }
    //             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
    //             iCubCanProtCalibrator.params.type1.velocity = eo_measconv_jntVelocity_toCAN(mc4boards, jxx, calibrator->params.type1.velocity);
                iCubCanProtCalibrator.params.type1.position = calibrator->params.type1.position; 
                iCubCanProtCalibrator.params.type1.velocity =  calibrator->params.type1.velocity;
                eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type1.calibrationZero);
                //the following block is repeated beatween all calibration types... it should be revised.
                {
                   icubCanProto_position_t tmp_minpos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_maxpos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_minpos);
                   icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_maxpos);
                   if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                       minpos_icubCanProtValue = maxpos_icubCanProtValue;
                       maxpos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &minpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &maxpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                }
                found = eobool_true; 
            } break;

            case eomc_calibration_type2_hard_stops_diff:
            {
                iCubCanProtCalibrator.params.type2.pwmlimit = calibrator->params.type2.pwmlimit;
    //            iCubCanProtCalibrator.params.type2.velocity = eo_measconv_jntVelocity_toCAN(mc4boards, jxx, calibrator->params.type2.velocity);           
                iCubCanProtCalibrator.params.type2.velocity = calibrator->params.type2.velocity;
                eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type2.calibrationZero);
                //the following block is repeated beatween all calibration types... it should be revised.
                {
                   icubCanProto_position_t tmp_minpos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_maxpos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_minpos);
                   icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_maxpos);
                   if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                       minpos_icubCanProtValue = maxpos_icubCanProtValue;
                       maxpos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &minpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &maxpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                }
                found = eobool_true;             
            } break;

            case eomc_calibration_type3_abs_sens_digital:
            {
    //              icubCanProto_position_t pos = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, calibrator->params.type3.position);
    //             /*sice pos param is a word of 16 bits i must check min and max*/
    //             if((pos < INT16_MIN)||(pos>INT16_MAX))
    //             {
    //                 return;
    //             }
    //             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
    //             iCubCanProtCalibrator.params.type3.velocity = eo_measconv_jntVelocity_toCAN(mc4boards, jxx, calibrator->params.type3.velocity);           
                iCubCanProtCalibrator.params.type1.position = calibrator->params.type3.position; 
                iCubCanProtCalibrator.params.type3.velocity = calibrator->params.type3.velocity;            
                iCubCanProtCalibrator.params.type3.offset = calibrator->params.type3.offset;
                eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type3.calibrationZero);
                //the following block is repeated beatween all calibration types... it should be revised.
                {
                   icubCanProto_position_t tmp_minpos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_maxpos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_minpos);
                   icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_maxpos);
                   if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                       minpos_icubCanProtValue = maxpos_icubCanProtValue;
                       maxpos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &minpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &maxpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                }
                found = eobool_true; 
            } break;

            case eomc_calibration_type6_mais:
            {
                float computed_zero=0;
                float computed_encoder=0;
              
                //compute the encoder conversion
                {
                   char info [50];
                  
                   icubCanProto_position_t tmp_min_joint_pos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_max_joint_pos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   //icubCanProto_position_t tmp_min_motor_pos = eo_mc4boards_Convert_minMotorPos_Get(mc4boards, jxx);
                   //icubCanProto_position_t tmp_max_motor_pos = eo_mc4boards_Convert_maxMotorPos_Get(mc4boards, jxx);
                  
                   if (calibrator->params.type6.current==1)
                   {
                       iCubCanProtCalibrator.params.type6.position= calibrator->params.type6.vmax;
                   }
                   else if (calibrator->params.type6.current==-1)
                   {
                       iCubCanProtCalibrator.params.type6.position= calibrator->params.type6.vmin;
                   }
                   else
                   {
                       sprintf(info,"error type6.current=%d",calibrator->params.type6.current);
                       send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, jxx, 0, 0, info);
                       iCubCanProtCalibrator.params.type6.position=0;
                   }
             
                   //sprintf(info,"type6 pos%d curr%d %d %d",iCubCanProtCalibrator.params.type6.position,calibrator->params.type6.current,tmp_max_motor_pos,tmp_min_motor_pos);
                   //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, jxx, 0, 0, info);
                   iCubCanProtCalibrator.params.type6.velocity = calibrator->params.type6.velocity;
                   
                   computed_encoder =  (float)(calibrator->params.type6.vmax - calibrator->params.type6.vmin) / (float)(tmp_max_joint_pos - tmp_min_joint_pos);
                               
                   //sprintf(info,"vmin %d vmax %d enc %f",calibrator->params.type6.vmin, calibrator->params.type6.vmax,computed_encoder);
                   //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, jxx, 0, 0, info);
                   //sprintf(info,"calicom %d",iCubCanProtCalibrator.params.type6.velocity);
                   //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, jxx, 0, 0, info);
                  
                   computed_zero    =  - (float)(tmp_min_joint_pos) + ((float)(calibrator->params.type6.vmin) / computed_encoder);

                   //sprintf(info,"min %d max %d zero %f",tmp_min_motor_pos, tmp_max_motor_pos, computed_zero);
                   //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag00, jxx, 0, 0, info);
                  
                   eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type6.calibrationZero+computed_zero);
                   eo_mc4boards_Convert_encoderfactor_Set(mc4boards, jxx, computed_encoder);
                  
                   icubCanProto_position_t min_joint_pos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_min_joint_pos);
                   icubCanProto_position_t max_joint_pos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_max_joint_pos);
                   if(max_joint_pos_icubCanProtValue < min_joint_pos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = min_joint_pos_icubCanProtValue;
                       min_joint_pos_icubCanProtValue = max_joint_pos_icubCanProtValue;
                       max_joint_pos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &min_joint_pos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &max_joint_pos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32);
                }
                found = eobool_true; 
            } break;
            
            case eomc_calibration_type7_hall_sensor:
            {
                float computed_zero=0;
                float computed_encoder=0;
              
                iCubCanProtCalibrator.params.type7.position = calibrator->params.type7.position;
                iCubCanProtCalibrator.params.type7.velocity = calibrator->params.type7.velocity;
                iCubCanProtCalibrator.params.type7.reserved = 0; //fixed to zero
              
                //compute the encoder conversion
                {
                   //char info [50];
                  
                   icubCanProto_position_t tmp_min_joint_pos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_max_joint_pos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_min_motor_pos = eo_mc4boards_Convert_minMotorPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_max_motor_pos = eo_mc4boards_Convert_maxMotorPos_Get(mc4boards, jxx);
                   computed_encoder =  (float)(calibrator->params.type7.vmax - calibrator->params.type7.vmin) / (float)(tmp_max_joint_pos - tmp_min_joint_pos);
                               
                   //sprintf(info,"vmin %d vmax %d enc %f",calibrator->params.type7.vmin, calibrator->params.type7.vmax,computed_encoder);
                   //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag02, jxx, 0, 0, info);
                  
                   computed_zero    =  - (float)(tmp_min_joint_pos) + ((float)(calibrator->params.type7.vmin) / computed_encoder);

                   //sprintf(info,"min %d max %d zero %f",tmp_min_motor_pos, tmp_max_motor_pos, computed_zero);
                   //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag03, jxx, 0, 0, info);
                  
                   eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type7.calibrationZero+computed_zero);
                   eo_mc4boards_Convert_encoderfactor_Set(mc4boards, jxx, computed_encoder);
                  
                   icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_min_joint_pos);
                   icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_max_joint_pos);
                   if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                       minpos_icubCanProtValue = maxpos_icubCanProtValue;
                       maxpos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &minpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &maxpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32);
                }
                found = eobool_true; 
            } break;
              
            case eomc_calibration_type4_abs_and_incremental:
            {
    //             icubCanProto_position_t pos = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, calibrator->params.type4.position);
    //             //here position is in int16_t ==> so i must verify if pos is out of int16_t range
    //             if((pos > INT16_MAX) || (pos < INT16_MIN))
    //             {
    //                 return;
    //                 #warning VALE --> how to manage this error???
    //             }
    //             iCubCanProtCalibrator.params.type4.position = (icubCanProto_position4calib_t)pos; 
    //            iCubCanProtCalibrator.params.type4.velocity = eo_measconv_jntVelocity_toCAN(mc4boards, jxx, calibrator->params.type4.velocity);            
                iCubCanProtCalibrator.params.type4.position = calibrator->params.type4.position;
                iCubCanProtCalibrator.params.type4.velocity = calibrator->params.type4.velocity;
                iCubCanProtCalibrator.params.type4.maxencoder = calibrator->params.type4.maxencoder;
                eo_mc4boards_Convert_encoderoffset_Set(mc4boards, jxx, calibrator->params.type4.calibrationZero);
                //the following block is repeated beatween all calibration types... it should be revised.
                {
                   icubCanProto_position_t tmp_minpos = eo_mc4boards_Convert_minJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t tmp_maxpos = eo_mc4boards_Convert_maxJointPos_Get(mc4boards, jxx);
                   icubCanProto_position_t minpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_minpos);
                   icubCanProto_position_t maxpos_icubCanProtValue = eo_mc4boards_Convert_Position_toCAN(mc4boards, jxx, tmp_maxpos);
                   if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
                   {
                       //swap min and max
                       icubCanProto_position_t pos_icubCanProtValue = minpos_icubCanProtValue;
                       minpos_icubCanProtValue = maxpos_icubCanProtValue;
                       maxpos_icubCanProtValue = pos_icubCanProtValue;
                   }
                   eOcanprot_command_t command_limit = {0};
                   command_limit.class = eocanprot_msgclass_pollingMotorControl;
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
                   command_limit.value = &minpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32); 
                  
                   command_limit.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
                   command_limit.value = &maxpos_icubCanProtValue;
                   eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command_limit, rd->id32);
                }
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


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_controlmode_command_t *controlmode = (eOmc_controlmode_command_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_set_control_mode(jxx, (eOmc_controlmode_command_t)(*controlmode));        
    }    
    else if(eo_motcon_mode_mc4 == mcmode)
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


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_interactionmode_t* interaction = (eOmc_interactionmode_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_set_interaction_mode(jxx, *interaction);
    } // marker
    else if(eo_motcon_mode_mc4 == mcmode)
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


// f-marker-begin
extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_torque_t *torque = (eOmeas_torque_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_update_joint_torque_fbk(jxx, *torque);
    } // marker
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        eOmeas_torque_t trq = EO_CLIP_INT16(*torque);
        icubCanProto_torque_t icub_torque = trq + 0x8000;
        eo_virtualstrain_SetTorque(eo_virtualstrain_GetHandle(), jxx, icub_torque);        
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_gearboxratio(const EOnv* nv, const eOropdescriptor_t* rd)
{   // not for mc4can
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    int32_t *gbxratio = (int32_t*)rd->data;

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        MController_config_motor_gearbox_ratio(jxx, *gbxratio);
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_rotorencoder(const EOnv* nv, const eOropdescriptor_t* rd)
{   // not for mc4can

    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    int32_t *rotenc = (int32_t*)rd->data;    
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        empty_fake_MController_config_motor_encoder(jxx, *rotenc);
    }
}

#if defined(EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK)
#warning INFO: EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK is defined, thus we are not using eo_emsController_set_Jacobian() etc
#else
// f-marker-begin
extern void eoprot_fun_UPDT_mc_controller_config_jointcoupling(const EOnv* nv, const eOropdescriptor_t* rd)
{   // not for mc4can

    // TODOALE
    
    eOmc_jointcouplingmatrix_t *mat = (eOmc_jointcouplingmatrix_t*)rd->data;    

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc = mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
   
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
}
#endif


// -- entity motor


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    eOmc_motor_config_t *mconfig = (eOmc_motor_config_t*)rd->data;
    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    
    #warning marco.accame: the code for ems and mc4plus is sligthly different .... what it correct?
        
    if(eo_motcon_mode_foc == mcmode)
    {
        MController_config_motor(mxx, mconfig);
        // TODOALE
        //mconfig->pwmLimit = eo_emsController_GetActuationLimit(mxx);
        return;           
    }
    else if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))   
    {
        MController_config_motor(mxx, mconfig);
        eo_currents_watchdog_UpdateCurrentLimits( eo_currents_watchdog_GetHandle(), mxx);
        // If pwmLimit is bigger than hardwhere limit, emsController uses hardwarelimit. 
        // Therefore I need to update netvar with the limit used in emsController.
        // TODOALE
        //mconfig->pwmLimit = eo_emsController_GetActuationLimit(mxx);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        //char info [50];
        EOtheMC4boards *mc4boards = eo_mc4boards_GetHandle();
      
        eo_mc4boards_Convert_minMotorPos_Set(mc4boards, mxx, mconfig->limitsofrotor.min);
        eo_mc4boards_Convert_maxMotorPos_Set(mc4boards, mxx, mconfig->limitsofrotor.max);
      
    // marco.accame on 8oct2015: removed because it is wrong. message ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY must be used with max velocity of JOINT !!!    
    //    // set max velocity      
    //    icubCanProto_velocity_t vel_icubCanProtValue = eo_mc4boards_Convert_Velocity_toCAN(eo_mc4boards_GetHandle(), mxx, mconfig->maxvelocityofmotor);           
    //    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    //    command.value = &vel_icubCanProtValue;
    //    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 

        // set current limit  
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
        command.value = &mconfig->currentLimits.overloadCurrent ;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
      
        // set max motor encoder limit
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MAX_MOTOR_POS;
        command.value = &mconfig->limitsofrotor.max;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
        
        // set min motor encoder limit
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_MIN_MOTOR_POS;
        command.value = &mconfig->limitsofrotor.min;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32); 
        
        //set pwmlimit
        icubCanProto_pwm_t icubcan_pwm = mconfig->pwmLimit; //eOmeas_pwm_t and icubCanProto_pwm_t are defined like int16
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_PWM_LIMIT;
        command.value = &icubcan_pwm;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
        
        //sprintf(info,"motomax %d motomin %d",mconfig->limitsofrotor.max,mconfig->limitsofrotor.min);
        //send_diagnostic_debugmessage(eo_errortype_debug, eoerror_value_DEB_tag01, mxx, 0, 0, info);
    }

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if(eo_motcon_mode_foc == mcmode)
    {
        //TODOALE
        return;
    }
    else if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        // TODOALE
        return;
    }
    else if(eo_motcon_mode_mc4 == mcmode)
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

}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_currentlimits(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);

    eOmc_current_limits_params_t *currentLimits = (eOmc_current_limits_params_t*)rd->data;
    eOmeas_current_t curr = currentLimits->overloadCurrent;

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    // foc-base and mc4based should send overloadCurrent to can-boards


    if(eo_motcon_mode_foc == mcmode)
    {
        //  send the can message to relevant board
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
        command.value = &curr;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
        // TODOALE
        #warning TODO: ALE should add in here a function for updating maxcurrent of motor value inside the controller
    }
    else if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eo_currents_watchdog_UpdateCurrentLimits( eo_currents_watchdog_GetHandle(), mxx);
        // now for the case of mc4plus, the watchdog updates the current limits inside ems controller. 
        // we should re-evaluate the situation after refactoring of mc-controller
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        // just send the can message to relevant board
       //  send the can message to relevant board
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;
        command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
        command.value = &curr;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
    }
}


// f-marker-begin
extern void eoprot_fun_UPDT_mc_motor_config_pwmlimit(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t mxx = eoprot_ID2index(rd->id32);
    eOmeas_pwm_t *pwm_limit = (eOmeas_pwm_t *)rd->data;

    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        *pwm_limit = MController_config_motor_pwm_limit(mxx, *pwm_limit);
    }
    else if(eo_motcon_mode_mc4 == mcmode)
    {
        if(eo_ropcode_set == rd->ropcode)
        {    
            icubCanProto_pwm_t icubcan_pwm = *pwm_limit; //eOmeas_pwm_t and icubCanProto_pwm_t are defined like int16
        
            eOcanprot_command_t command = {0};
            command.class = eocanprot_msgclass_pollingMotorControl;
            command.type  = ICUBCANPROTO_POL_MC_CMD__SET_PWM_LIMIT;
            command.value = &icubcan_pwm;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
        }
        else if(eo_ropcode_ask == rd->ropcode)
        {      
            EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
            eOproxy_params_t *param = eo_proxy_Params_Get(proxy, rd->id32);
            eOcanprot_command_t command = {0};
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
            param->p08_1 = 1;       // we expect one can frames
            param->p08_2 = 0;       // and we havent received any yet
                   
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_PWM_LIMIT;
            command.value = NULL;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);        
        }     
    }

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// -- common functions

static eOmotioncontroller_mode_t s_motorcontrol_getmode(void)
{
    return(eo_motioncontrol_GetMode(eo_motioncontrol_GetHandle()));
}


// -- 2foc-based functions


// -- mc4plus-based funtions



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
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



