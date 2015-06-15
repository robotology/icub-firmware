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

/* @file       EoProtocolMC_fun_redefined.c
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
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const eOmc_joint_t s_joint_default_value =
{
    EO_INIT(.config)             
    {
        EO_INIT(.pidposition)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.kff)                   0,
            EO_INIT(.stiction_up_val)       0,
            EO_INIT(.stiction_down_val)     0,
            EO_INIT(.filler)                {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.pidvelocity)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.kff)                   0,
            EO_INIT(.stiction_up_val)       0,
            EO_INIT(.stiction_down_val)     0,
            EO_INIT(.filler)                {0xf1, 0xf2, 0xf3}
        },
        EO_INIT(.pidtorque)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.kff)                   0,
            EO_INIT(.stiction_up_val)       0,
            EO_INIT(.stiction_down_val)     0,
            EO_INIT(.filler)                {0xf1, 0xf2, 0xf3}
        }, 
        EO_INIT(.limitsofjoint)
        {
            EO_INIT(.min)                   0,
            EO_INIT(.max)                   0
        },
        EO_INIT(.impedance)
        {
            EO_INIT(.stiffness)             0,
            EO_INIT(.damping)               0,
            EO_INIT(.offset)                0,
            EO_INIT(.filler02)              {0xf1, 0xf2}           
        },        
        
        EO_INIT(.velocitysetpointtimeout)   0,
       
        EO_INIT(.motionmonitormode)         eomc_motionmonitormode_dontmonitor,
        EO_INIT(.filler01)                  0xe0,
        EO_INIT(.encoderconversionfactor)   EOUTIL_EMULFLOAT32_ONE,
        EO_INIT(.encoderconversionoffset)   EOUTIL_EMULFLOAT32_ZERO,
        EO_INIT(.motor_params)
        {
            EO_INIT(.bemf_value)            0,
            EO_INIT(.ktau_value)            0,
            EO_INIT(.bemf_scale)            0,
            EO_INIT(.ktau_scale)            0,
            EO_INIT(.filler02)              {0xf1, 0xf2}
        },
        EO_INIT(.tcfiltertype)              0,
        EO_INIT(.filler03)                  {0xf1, 0xf2, 0xf3}
    },
    EO_INIT(.status)                       
    {
        EO_INIT(.basic)
        {
            EO_INIT(.jnt_position)          0,
            EO_INIT(.jnt_velocity)          0,
            EO_INIT(.jnt_acceleration)      0,
            EO_INIT(.jnt_torque)            0,
            EO_INIT(.motionmonitorstatus)   eomc_motionmonitorstatus_notmonitored,
            EO_INIT(.controlmodestatus)     eomc_controlmode_idle,
        },
        EO_INIT(.ofpid)                     {0},
        EO_INIT(.interactionmodestatus)     eomc_imodeval_stiff,
        EO_INIT(.chamaleon03)               {0} 
    },
    EO_INIT(.inputs)                       {0},
    EO_INIT(.cmmnds)                       
	{
		EO_INIT(.calibration)               {0},
		EO_INIT(.setpoint)                  {0},
		EO_INIT(.stoptrajectory)            0,
		EO_INIT(.controlmode)				eomc_controlmode_cmd_switch_everything_off,
        EO_INIT(.interactionmode)           eomc_imodeval_stiff,
        EO_INIT(.filler01)                  0        
	}
}; 

static const eOmc_motor_t s_motor_default_value =
{
    EO_INIT(.config)             
    {
        EO_INIT(.pidcurrent)
        {
            EO_INIT(.kp)                    0,
            EO_INIT(.ki)                    0,
            EO_INIT(.kd)                    0,
            EO_INIT(.limitonintegral)       0,
            EO_INIT(.limitonoutput)         0,
            EO_INIT(.scale)                 0,
            EO_INIT(.offset)                0,
            EO_INIT(.kff)                   0,
            EO_INIT(.stiction_up_val)       0,
            EO_INIT(.stiction_down_val)     0,
            EO_INIT(.filler)                {0xf1, 0xf2, 0xf3}

        },
        EO_INIT(.gearboxratio)              0,
        EO_INIT(.rotorencoder)              0,
        EO_INIT(.maxvelocityofmotor)        0,
        EO_INIT(.maxcurrentofmotor)         0,
        EO_INIT(.filler02)                  {0}
    },
    EO_INIT(.status)                       {0}
}; 




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// ## joints: INIT

extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &s_joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t *sta = (eOmc_joint_status_t*)eo_nv_RAM(nv);
    memcpy(sta, &s_joint_default_value.status, sizeof(eOmc_joint_status_t));
}


// ## joints: UPDT

extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    float rescaler_pos = 0;
    float rescaler_trq = 0;    
    // marco.accame: thejoint must not be NULL but it is impossible that it is NULL inside this function. i decide not to verify it.
    #warning --> for a future quicker access to the joint memory, you could initialise all joints (motors) pointers inside an array (maybe local, maybe inside EOmcService.
    // in this way you avoid recomputing all the times the same pointer.
    eOmc_joint_t *thejoint = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jxx);
    eOmc_joint_status_t *jstatus = &thejoint->status;

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
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eOmc_impedance_t *cfg = (eOmc_impedance_t*)rd->data;   
    eo_emsController_SetImpedance(jxx, cfg->stiffness, cfg->damping, cfg->offset);
}

extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t                 jxx = eoprot_ID2index(rd->id32);
    eOmeas_position_limits_t       *limit_ptr = (eOmeas_position_limits_t*)rd->data;

    eo_emsController_SetPosMin(jxx, limit_ptr->min);
    eo_emsController_SetPosMax(jxx, limit_ptr->max);
}




extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t                           *time_ptr = (eOmeas_time_t*)rd->data;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);

    eo_emsController_SetVelTimeout(jxx, *time_ptr);
}



extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    /*NOTE: this function is equal to mc4 function.*/
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_t *thejoint = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jxx);
    eOmc_joint_status_t *jstatus = &thejoint->status;
    
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


extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eOmc_setpoint_t *setPoint = (eOmc_setpoint_t*)rd->data;
    
    eOmc_joint_t *thejoint = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jxx);
    eOmc_joint_status_t *jstatus = &thejoint->status; 
    eOmc_joint_config_t *jconfig = &thejoint->config;   


    // set monitor status = notreachedyet if monitormode is forever    
    if(eomc_motionmonitormode_forever == jconfig->motionmonitormode)
    {
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == jstatus->basic.motionmonitorstatus)
        {
            jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
        }
    }
    
    switch(setPoint->type)
    { 
        case eomc_setpoint_position:
        {
            eo_emsController_SetPosRef(jxx, setPoint->to.position.value, setPoint->to.position.withvelocity);
        } break;
        
        case eomc_setpoint_positionraw:
        { 
            eo_emsController_SetPosRaw(jxx, setPoint->to.position.value);
        } break;
        
        case eomc_setpoint_velocity:
        {
            eo_emsController_SetVelRef(jxx, setPoint->to.velocity.value, setPoint->to.velocity.withacceleration);    
        } break;

        case eomc_setpoint_torque:
        {
            eo_emsController_SetTrqRef(jxx, setPoint->to.torque.value);
        } break;

        case eomc_setpoint_current:
        {
            eo_emsController_SetOutput(jxx, setPoint->to.current.value);
        } break;

        default:
        {
        } break;
    }
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eo_emsController_Stop(jxx);
}



extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;


#if 1
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_joint_cmmnds_calibration() i have removed messages sent to CAN. how do we do that for mc4plus ???
#else    
    eOresult_t res = eores_NOK_generic;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc = {.emscanport = eOcanport1, .addr = 0, .indexinsidecanboard = eo_icubCanProto_jm_index_first};
    
    #ifdef EXPERIMENTAL_SPEED_CONTROL
    icubCanProto_controlmode_t              controlmode_2foc = icubCanProto_controlmode_velocity;
    #else
    icubCanProto_controlmode_t              controlmode_2foc = icubCanProto_controlmode_openloop;
    #endif
    
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    // 1) send current control mode and pwm ena to 2foc
    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }
    

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
    
#endif

#warning -> see comment.
    // abilitare la perif pwm, mettere il emscontrl in controlmode che dipende dal joint e da calib type (chiedere a randaz), 
    // per il tipo 3 e': position...?
    
    // la StartCalibration deve esser edificata in modo da poter gestire anche altri tipi di calibrazione.
    // alessandro: aiutera'
    // usare joint index per identificare quali devono avere calibrazione 1,2,3...etc ?
    
    // commento: la calib tipo 3 ha solo offset (in xml e' il calibration3. il calibration2 etc non viene usato).
    // per calib 3: viene usato solo il calibration3.
    
    //check for the type of calibration required
    
    #warning this should change to calibrationtype5, but in robotInterface this is still not handled. now it's type4 (because it also have 3 params) to test it
    if(calibrator->type == eomc_calibration_type4_abs_and_incremental)
    {
        //calibration for joint with incremental encoders
        eo_emsController_StartCalibration_type0 (jxx,
                                                 calibrator->params.any[0],
                                                 calibrator->params.any[1]);
    }
    else if(calibrator->type == eomc_calibration_type3_abs_sens_digital)
    {
        //calibration for joint with abs encoders
        eo_emsController_StartCalibration_type3(jxx, 
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
    eOmc_controlmode_command_t *controlmode_ptr = (eOmc_controlmode_command_t*)rd->data;
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
        
    // 2) set control mode to ems controller
    eo_emsController_SetControlModeGroupJoints(jxx, (eOmc_controlmode_command_t)(*controlmode_ptr));       
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_interactionmode_t* interaction = (eOmc_interactionmode_t*)rd->data;
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    
    eo_emsController_SetInteractionModeGroupJoints(jxx, *interaction);
}


// __ALE__
extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eo_emsController_ReadTorque(jxx, *(eOmeas_torque_t*)rd->data);
}

extern void eoprot_fun_UPDT_mc_motor_config_gearboxratio(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    int32_t      *gbx_ptr = (int32_t*)rd->data;
    eo_emsController_SetGearboxRatio(jxx, *gbx_ptr);
}

extern void eoprot_fun_UPDT_mc_motor_config_rotorencoder(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    int32_t      *gbx_ptr = (int32_t*)rd->data;
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


// ## motors: INIT


extern void eoprot_fun_INIT_mc_motor_config(const EOnv* nv)
{
    eOmc_motor_config_t *cfg = (eOmc_motor_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &s_motor_default_value.config, sizeof(eOmc_motor_config_t));
}

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(const EOnv* nv)
{
    eOmc_motor_status_t *sta = (eOmc_motor_status_t*)eo_nv_RAM(nv);
    memcpy(sta, &s_motor_default_value.status, sizeof(eOmc_motor_status_t));
}


// ## motors: UPDT

#warning --> see comments

// finaora le mc4 normali non hanno controllo di corrente.
// propsota: per ora ignorare i comandi di: pid-corrente, limte corrente, 

extern void eoprot_fun_UPDT_mc_motor_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);    
    eOmc_motor_config_t *cfg_ptr = (eOmc_motor_config_t*)rd->data;

#if 1
    cfg_ptr = cfg_ptr;
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config() i have removed messages sent to CAN. how do we do that for mc4plus ???
#else 

    eObrd_cantype_t boardType;
    icubCanProto_velocity_t vel_icubCanProtValue;
    
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    

    if(eobrd_cantype_1foc == boardType)
    {
        // 1) send current pid
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);

        // 2) send current pid limits
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->pidcurrent);
    }
    
    // 2) set max velocity  
    vel_icubCanProtValue = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, cfg_ptr->maxvelocityofmotor);           
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&vel_icubCanProtValue);

    // 3) set current limit  
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg_ptr->maxcurrentofmotor);

#endif

}

extern void eoprot_fun_UPDT_mc_motor_config_pidcurrent(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    eOmc_PID_t *pid_ptr = (eOmc_PID_t*)rd->data;

#if 1
    pid_ptr = pid_ptr;
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config_pidcurrent() i have removed messages sent to CAN. how do we do that for mc4plus ???
#else 

    
    eOresult_t                              res;    
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

	res = eo_appTheDB_GetMotorCanLocation(eo_appTheDB_GetHandle(), mxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

  	//set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);

    // send current pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

#endif    
}


extern void eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_velocity_t *vel_ptr = (eOmeas_velocity_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    
#if 1
    vel_ptr = vel_ptr;
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config_maxvelocityofmotor() i have removed messages sent to CAN. how do we do that for mc4plus ???
#else 
    
    icubCanProto_velocity_t                 vel_icubCanProtValue;
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());


    vel_icubCanProtValue = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, mxx, *vel_ptr);           
    eo_appCanSP_SendCmd2Motor(appCanSP_ptr, (eOmc_motorId_t)mxx, msgCmd, (void*)&vel_icubCanProtValue);
#endif
}


extern void eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_current_t *curr_ptr = (eOmeas_current_t*)rd->data;
    eOmc_motorId_t mxx = eoprot_ID2index(rd->id32);
    
#if 1
    curr_ptr = curr_ptr;
    #warning TBD: marco.accame -> in eoprot_fun_UPDT_mc_motor_config_maxcurrentofmotor() i have removed messages sent to CAN. how do we do that for mc4plus ???
#else     
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appCanSP_SendCmd2Motor(appCanSP_ptr, (eOmc_motorId_t)mxx, msgCmd, (void*)curr_ptr);
#endif    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



