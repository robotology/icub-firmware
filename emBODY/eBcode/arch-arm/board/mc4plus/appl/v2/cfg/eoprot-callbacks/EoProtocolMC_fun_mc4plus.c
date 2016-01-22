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

/* @file       EoProtocolMC_fun_mc4plus.c
    @brief      This file keeps the redefinition of protocol mc callbacks for mc4plus boards
    @author     marco.accame@iit.it
    @date       16 april 2015
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


#include "EOtheEntities.h"
#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheMotionController.h"

#include "EOemsController.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMC.h"


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOmotioncontroller_mode_t s_motorcontrol_getmode(void);

static void s_eoprot_ep_mc_fun_MotorReactivationAttempt(uint8_t motor, uint32_t current_state);



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


extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
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

        //eo_emsController_SetAbsEncoderSign((uint8_t)jxx, (int32_t)cfg->DEPRECATED_encoderconversionfactor);
        eo_emsController_SetAbsEncoderSign((uint8_t)jxx, (int32_t)cfg->jntEncoderResolution);
        
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

}


extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOmc_PID_t      *pid_ptr = (eOmc_PID_t*)rd->data;
        eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);
        float           rescaler = 1.0f/(float)(1<<pid_ptr->scale);

        eo_emsController_SetPosPid(jxx, pid_ptr->kp*rescaler,   
                                        pid_ptr->kd*rescaler, 
                                        pid_ptr->ki*rescaler,
                                        pid_ptr->limitonintegral,
                                        pid_ptr->limitonoutput, 
                                        pid_ptr->offset,
                                        pid_ptr->stiction_up_val*rescaler,
                                        pid_ptr->stiction_down_val*rescaler);
    }
}

extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOmc_PID_t      *pid_ptr = (eOmc_PID_t*)rd->data;
        eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);
        float           rescaler = 1.0f/(float)(1<<pid_ptr->scale);
        
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
}


extern void eoprot_fun_UPDT_mc_joint_config_motor_params(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOmc_motor_params_t *params_ptr = (eOmc_motor_params_t*)rd->data;
        eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);    
        eo_emsController_SetMotorParams ((uint8_t)jxx, *params_ptr);
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {    
        eOmc_impedance_t *impedance = (eOmc_impedance_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32); 
        eo_emsController_SetImpedance(jxx, impedance->stiffness, impedance->damping, impedance->offset);
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {    
        eOmeas_position_limits_t *limitsofjoint = (eOmeas_position_limits_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        
        eo_emsController_SetPosMin(jxx, limitsofjoint->min);
        eo_emsController_SetPosMax(jxx, limitsofjoint->max);
    }
}


extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {    
        eOmeas_time_t *time = (eOmeas_time_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

        eo_emsController_SetVelTimeout(jxx, *time);
    }
}


extern void eoprot_fun_UPDT_mc_joint_status_modes_ismotiondone(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
   
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        // do nothing
    }   
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_setpoint_t *setpoint = (eOmc_setpoint_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_t *joint = eo_entities_GetJoint(eo_entities_GetHandle(), jxx);
    
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

    if(NULL == joint)
    {
        return; //error
    }

    joint->status.modes.ismotiondone = eobool_false;
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        
        switch(setpoint->type)
        { 
            case eomc_setpoint_position:
            {
                eo_emsController_SetPosRef(jxx, setpoint->to.position.value, setpoint->to.position.withvelocity);
            } break;
            
            case eomc_setpoint_positionraw:
            { 
                eo_emsController_SetPosRaw(jxx, setpoint->to.positionraw.value);
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
    
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        eo_emsController_Stop(jxx);
    }
    
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;

        uint32_t state = eo_motioncontrol_extra_GetMotorFaultMask(eo_motioncontrol_GetHandle(), jxx);
        if((eobool_true == eo_motioncontrol_extra_AreMotorsExtFaulted(eo_motioncontrol_GetHandle())) || (state & MOTOR_EXTERNAL_FAULT)) // or motors still faulted OR state (and so PWM) still need to be enabled
        {
            s_eoprot_ep_mc_fun_MotorReactivationAttempt(jxx, state);
        }

        eo_emsController_StartCalibration(jxx, (eOmc_calibration_type_t)calibrator->type, calibrator->params.any);   
    }    
}



extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOmc_controlmode_command_t *controlmode = (eOmc_controlmode_command_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
       
        uint32_t state = eo_motioncontrol_extra_GetMotorFaultMask(eo_motioncontrol_GetHandle(), jxx);
        if((eobool_true == eo_motioncontrol_extra_AreMotorsExtFaulted(eo_motioncontrol_GetHandle())) || (state & MOTOR_EXTERNAL_FAULT))
        {
            s_eoprot_ep_mc_fun_MotorReactivationAttempt(jxx, state);
        }        
            
        eo_emsController_SetControlModeGroupJoints(jxx, (eOmc_controlmode_command_t)(*controlmode));   
    }    
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
        eOmc_interactionmode_t* interaction = (eOmc_interactionmode_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        
        eo_emsController_SetInteractionModeGroupJoints(jxx, *interaction);
    }
}



extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOmeas_torque_t *torque = (eOmeas_torque_t*)rd->data;
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        
        eo_emsController_ReadTorque(jxx, *torque);
    }
}

extern void eoprot_fun_UPDT_mc_motor_config_gearboxratio(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {    
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        int32_t *gbx_ptr = (int32_t*)rd->data;
        eo_emsController_SetGearboxRatio(jxx, *gbx_ptr);
    }
}

extern void eoprot_fun_UPDT_mc_motor_config_rotorencoder(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
        eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
        int32_t *gbx_ptr = (int32_t*)rd->data;
        eo_emsController_SetRotorEncoder(jxx, *gbx_ptr);
    }
}

#if defined(EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK)
    #warning --> EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK is defined, thus we are not using eo_emsController_set_Jacobian() etc
#else
extern void eoprot_fun_UPDT_mc_controller_config_jointcoupling(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
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
}
#endif


// -- entity motor




#warning --> see comments

// finora le mc4 normali non hanno controllo di corrente.
// propsota: per ora ignorare i comandi di: pid-corrente, limte corrente, 

extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))   
    {    
        eOmc_motor_config_t *cfg_ptr = (eOmc_motor_config_t*)rd->data;
        eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);

        //set rotor encoder sign
        eo_emsController_SetRotorEncoderSign((uint8_t)mxx, (int32_t)cfg_ptr->rotorEncoderResolution);
        
        cfg_ptr = cfg_ptr;
        #warning -> TBD: in here the 2foc-based control does config the can board with ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID, ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY and ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT. what about mc4plus?
    }
}

extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {        
        eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
        eOmc_PID_t *pid_ptr = (eOmc_PID_t*)rd->data;
        
        pid_ptr = pid_ptr;
        #warning -> TBD: in here the 2foc-based control does config the can board with ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID etc. what about mc4plus?
    }
}


extern void eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_velocity_t *vel_ptr = (eOmeas_velocity_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    

    vel_ptr = vel_ptr;
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor() i have removed messages sent to CAN. how do we do that for mc4plus ???
}


extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // now we see if it is a mc4can or a 2foc or a mc4plus
    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();
    
    if((eo_motcon_mode_mc4plus == mcmode) || (eo_motcon_mode_mc4plusmais == mcmode))
    {
        eOmeas_current_t *curr_ptr = (eOmeas_current_t*)rd->data;
        eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
        

        curr_ptr = curr_ptr;
        #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor() i have removed messages sent to CAN. how do we do that for mc4plus ???   
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOmotioncontroller_mode_t s_motorcontrol_getmode(void)
{
    return(eo_motioncontrol_GetMode(eo_motioncontrol_GetHandle()));
}


static void s_eoprot_ep_mc_fun_MotorReactivationAttempt(uint8_t motor, uint32_t current_state)
{
    eo_motioncontrol_extra_MotorEnable(eo_motioncontrol_GetHandle(), motor);
    eo_motioncontrol_extra_FaultDetectionEnable(eo_motioncontrol_GetHandle());

    // simulate the CANframe used by 2FOC to signal the status
    uint64_t fault_mask = (((uint64_t)(current_state & ~MOTOR_EXTERNAL_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
    eo_motioncontrol_extra_SetMotorFaultMask(eo_motioncontrol_GetHandle(), motor, (uint8_t*)&fault_mask);

    // reports to emscontroller the changed mask
    eo_emsController_CheckFaults();
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



