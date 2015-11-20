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

#include "EOmcService.h"

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
static void s_eoprot_ep_mc_fun_MotorReactivationAttempt(uint8_t motor, uint32_t current_state);



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
            .filler =                {0}
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
            .filler =                {0}
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
            .filler =                {0}
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
            .filler02 =              {0}           
        },               
        .maxvelocityofjoint =        0,
        .motor_params =
        {
            .bemf_value =            0,
            .ktau_value =            0,
            .bemf_scale =            0,
            .ktau_scale =            0,
            .filler02 =              {0}
        },
        .velocitysetpointtimeout =   0,
        .tcfiltertype =              0,
        .jntEncoderType =            0,
        .filler04 =                  {0}
    },
    .status =                       
    {
        .basic =
        {
            .jnt_position =          0,
            .jnt_velocity =          0,
            .jnt_acceleration =      0,
            .jnt_torque =            0,
            .filler =                {0},
        },
        .ofpid =                     {0},
        .modes = 
        {
            .controlmodestatus =        eomc_controlmode_idle,
            .interactionmodestatus =    eOmc_interactionmode_stiff,
            .ismotiondone =             eobool_false,
            .filler =                   {0}
        }
    },
    .inputs =                        {0},
    .cmmnds =                       
    {
        .calibration =               {0},
        .setpoint =                  {0},
        .stoptrajectory =            0,
        .controlmode =               eomc_controlmode_cmd_switch_everything_off,
        .interactionmode =           eOmc_interactionmode_stiff,
        .filler =                    {0}        
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
            .filler =                {0}
        },
        .gearboxratio =              0,
        .rotorEncoderResolution =    0,
        .maxvelocityofmotor =        0,
        .maxcurrentofmotor =         0,
        .rotorIndexOffset =          0,
        .motorPoles =                0,
        .hasHallSensor =             eobool_false,
        .hasTempSensor =             eobool_false,
        .hasRotorEncoder =           eobool_false,
        .hasRotorEncoderIndex =      eobool_false,
        .rotorEncoderType =          0,
        .filler02 =                  {0},
        .limitsofrotor =
        {
            .max = 0,
            .min = 0
        }
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
    eOmc_joint_status_t *sta = (eOmc_joint_status_t*)eo_nv_RAM(nv);
    memcpy(sta, &joint_default_value.status, sizeof(eOmc_joint_status_t));
}

extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    // now we see if it is a mc4can or a 2foc or a mc4plus

    float rescaler_pos = 0;
    float rescaler_trq = 0;    


    // marco.accame on 16 apr 2015: so far i get the same function used for 2foc-based motion control. thus i keep all the comments about 2foc.
    
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


extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
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

extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
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


extern void eoprot_fun_UPDT_mc_joint_config_motor_params(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    eOmc_motor_params_t *params_ptr = (eOmc_motor_params_t*)rd->data;
    eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);    
    eo_emsController_SetMotorParams ((uint8_t)jxx, *params_ptr);
}


extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_impedance_t *impedance = (eOmc_impedance_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32); 
    eo_emsController_SetImpedance(jxx, impedance->stiffness, impedance->damping, impedance->offset);
}


extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_position_limits_t *limitsofjoint = (eOmeas_position_limits_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eo_emsController_SetPosMin(jxx, limitsofjoint->min);
    eo_emsController_SetPosMax(jxx, limitsofjoint->max);
}


extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t *time = (eOmeas_time_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

    eo_emsController_SetVelTimeout(jxx, *time);
}

/*
extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{   // 2foc or mc4plus or mc4can is equal
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    if(NULL == jstatus)
    {
        return; //error
    }
    
    //#warning marco.accame: better using cast to eOmc_motionmonitormode_t    
    //if(eomc_motionmonitormode_dontmonitor == *((eOenum08_t*)nv->ram))
    eOmc_motionmonitormode_t *motionmonitormode = (eOmc_motionmonitormode_t*)rd->data; 

    if(eomc_motionmonitormode_dontmonitor == *motionmonitormode)
    {
        jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }
}
*/

extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_setpoint_t *setpoint = (eOmc_setpoint_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_t *joint = eo_entities_GetJoint(eo_entities_GetHandle(), jxx);

    if(NULL == joint)
    {
        return; //error
    }

    joint->status.modes.ismotiondone = eobool_false;
    
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

extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eo_emsController_Stop(jxx);
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;



#warning -> see comment.
    // abilitare la perif pwm, mettere il emscontrl in controlmode che dipende dal joint e da calib type (chiedere a randaz), 
    // per il tipo 3 e': position...?
    
    // la StartCalibration deve esser edificata in modo da poter gestire anche altri tipi di calibrazione.
    // alessandro: aiutera'
    // usare joint index per identificare quali devono avere calibrazione 1,2,3...etc ?
    
    // commento: la calib tipo 3 ha solo offset (in xml e' il calibration3. il calibration2 etc non viene usato).
    // per calib 3: viene usato solo il calibration3.
    
    uint32_t state = eo_mcserv_GetMotorFaultMask(eo_mcserv_GetHandle(),jxx);
    if ((eo_mcserv_AreMotorsExtFaulted(eo_mcserv_GetHandle())) || (state & MOTOR_EXTERNAL_FAULT)) //or motors still faulted OR state (and so PWM) still need to be enabled
    {
        s_eoprot_ep_mc_fun_MotorReactivationAttempt(jxx, state);
    }

    //check for the type of calibration required

    if(calibrator->type == eomc_calibration_type6_mais_mc4plus)
    {
        // calibration for joint controlled with MAIS
        
        //params for the calib still need to be discussed
        /*
        eo_emsController_SetAxisCalibrationZero (jxx, calibrator->params.type6.calibrationZero);
        // the StartCalib for type6 should:
        // 1 - (set the params of position, velocity) and maxencoder
        // 2 - SetTrajectory with final position and velocity (p1,p2)
         eo_emsController_StartCalibration_type6 (jxx,
                                                 calibrator->params.type6.position,
                                                 calibrator->params.type6.velocity,
                                                 calibrator->params.type6.maxencoder);
        */
    }
    
    if(calibrator->type == eomc_calibration_type5_hard_stops_mc4plus)
    {
        // calibration for joint with incremental encoders
        eo_emsController_SetAxisCalibrationZero (jxx, calibrator->params.type5.calibrationZero);
        eo_emsController_StartCalibration_type5 (jxx,
                                                 calibrator->params.type5.pwmlimit,
                                                 calibrator->params.type5.final_pos);
    }
    else if(calibrator->type == eomc_calibration_type3_abs_sens_digital)
    {
        // calibration for joint with abs encoders
        eo_emsController_SetAxisCalibrationZero (jxx, calibrator->params.type3.calibrationZero);
        eo_emsController_StartCalibration_type3 (jxx, 
                                                calibrator->params.type3.position, 
                                                calibrator->params.type3.velocity,
                                                calibrator->params.type3.offset);
    }                             
    // davide:
    /*
     probably I shouldn't set the control mode in here...
     1) with the StartCalibration it's set to eomc_controlmode_cmd_calib
     2) after the calibration there's a control inside the axis controller --> if all the coupled joints are calibrated, then the control mode is set to position
    
    //eo_emsController_SetControlModeGroupJoints(jxx, (eOmc_controlmode_command_t) eomc_controlmode_cmd_position);
    */
}



extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_controlmode_command_t *controlmode = (eOmc_controlmode_command_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);

    //if this joint was in external fault or the state (and so PWM) is not updated, reenable it
    uint32_t state = eo_mcserv_GetMotorFaultMask(eo_mcserv_GetHandle(),jxx);
    if ((eo_mcserv_AreMotorsExtFaulted(eo_mcserv_GetHandle())) || (state & MOTOR_EXTERNAL_FAULT))
    {
       s_eoprot_ep_mc_fun_MotorReactivationAttempt(jxx, state);
    }
        
    eo_emsController_SetControlModeGroupJoints(jxx, (eOmc_controlmode_command_t)(*controlmode));       
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_interactionmode_t* interaction = (eOmc_interactionmode_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eo_emsController_SetInteractionModeGroupJoints(jxx, *interaction);
}



extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_torque_t *torque = (eOmeas_torque_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
    eo_emsController_ReadTorque(jxx, *torque);
}

extern void eoprot_fun_UPDT_mc_motor_config_gearboxratio(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    int32_t *gbx_ptr = (int32_t*)rd->data;
    eo_emsController_SetGearboxRatio(jxx, *gbx_ptr);
}

extern void eoprot_fun_UPDT_mc_motor_config_rotorencoder(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    int32_t *gbx_ptr = (int32_t*)rd->data;
    eo_emsController_SetRotorEncoder(jxx, *gbx_ptr);
}

#if defined(EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK)
    #warning --> EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK is defined, thus we are not using eo_emsController_set_Jacobian() etc
#else
extern void eoprot_fun_UPDT_mc_controller_config_jointcoupling(const EOnv* nv, const eOropdescriptor_t* rd)
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
#endif


// -- entity motor

extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv)
{
    eOmc_motor_config_t *cfg = (eOmc_motor_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &motor_default_value.config, sizeof(eOmc_motor_config_t));
}

extern void eoprot_fun_INIT_mc_motor_status(const EOnv* nv)
{
    eOmc_motor_status_t *sta = (eOmc_motor_status_t*)eo_nv_RAM(nv);
    memcpy(sta, &motor_default_value.status, sizeof(eOmc_motor_status_t));
}



#warning --> see comments

// finaora le mc4 normali non hanno controllo di corrente.
// propsota: per ora ignorare i comandi di: pid-corrente, limte corrente, 

extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_motor_config_t *cfg_ptr = (eOmc_motor_config_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);

    //set rotor encoder sign
    eo_emsController_SetRotorEncoderSign((uint8_t)mxx, (int32_t)cfg_ptr->rotorEncoderResolution);
    
    cfg_ptr = cfg_ptr;
    #warning -> in here the 2foc-based control does config the can board with ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID, ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY and ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT. what about mc4plus?
}

extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    eOmc_PID_t *pid_ptr = (eOmc_PID_t*)rd->data;
    
    pid_ptr = pid_ptr;
    #warning -> in here the 2foc-based control does config the can board with ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID etc. what about mc4plus?
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
    eOmeas_current_t *curr_ptr = (eOmeas_current_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    

    curr_ptr = curr_ptr;
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor() i have removed messages sent to CAN. how do we do that for mc4plus ???   
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eoprot_ep_mc_fun_MotorReactivationAttempt(uint8_t motor, uint32_t current_state)
{
     eo_mcserv_EnableMotor(eo_mcserv_GetHandle(), motor);
     eo_mcserv_EnableFaultDetection(eo_mcserv_GetHandle());
     
     //simulate the CANframe used by 2FOC to signal the status
     //uint8_t fault_mask[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; //clear all the faults
     uint64_t fault_mask = (((uint64_t)(current_state & ~MOTOR_EXTERNAL_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
    
     eo_mcserv_SetMotorFaultMask(eo_mcserv_GetHandle(), motor, (uint8_t*)&fault_mask);
     
     //reports to emscontroller the changed mask
     eo_emsController_CheckFaults();
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



