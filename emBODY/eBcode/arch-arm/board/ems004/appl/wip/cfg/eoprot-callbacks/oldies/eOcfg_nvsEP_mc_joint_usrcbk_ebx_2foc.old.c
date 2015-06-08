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

/* @file       eOcfg_nvsEP_mc_joint_usrcbk_ebx_2foc.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mc
    @author     valentina.gaggero@iit.it
    @date       05/04/2012
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


//application
#include "EOtheEMSapplBody.h"
//#include "EOappTheDataBase.h"
//#include "EOicubCanProto_specifications.h"
#include "EOtheMeasuresConverter.h"

#include "EOtheEntities.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANservice.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

// remove this if you want to use function eoprot_fun_UPDT_mc_controller_config_jointcoupling() as defined inside this file
#define EOMOTIONCONTROL_DONTREDEFINE_JOINTCOUPLING_CALLBACK

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSapplCfg_hid.h"

// #define VERIFY_ROP_SETIMPEDANCE
// #define VERIFY_ROP_SETPOSITIONRAW


//#if defined(VERIFY_ROP_SETIMPEDANCE) | defined(VERIFY_ROP_SETPOSITIONRAW)
//    #include "EOtheEMSapplDiagnostics.h"
//    #include "rxtools.h"
//    #define MAXJ 4
//    
//    #if   defined(EOMTHEEMSAPPLCFG_USE_EB5)
//        #define VERIFY_ROP_SETPOINT_EB5
//    #endif
//#endif


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

//#if defined(VERIFY_ROP_SETIMPEDANCE)
//    const uint32_t maxtimegap_impedance = 25; // 25 ms expressed in milli-sec
//    static rxtools_rec_status_t status_rop_impedance[MAXJ] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
//#endif
    
    
//#if defined(VERIFY_ROP_SETPOSITIONRAW)
//    const uint32_t maxtimegap_setpositionraw = 25; // 25 ms expressed in milli-sec
//    static rxtools_rec_status_t status_rop_setpositionraw[MAXJ] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
//#endif



    
    
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
/* TAG_ALE*/

//joint-init
                    
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


//joint-update
            
extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    //eOresult_t              res;
    eOmc_jointId_t          jxx = eoprot_ID2index(rd->id32);
    float                   rescaler_pos;
    float                   rescaler_trq;
    eOmc_joint_status_t     *jstatus = NULL;
    eOmc_joint_config_t     *cfg = (eOmc_joint_config_t*)rd->data;

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
    jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
    if(NULL == jstatus)
    {
        return; //error
    }    
    
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
//   eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32); 
//#if defined(VERIFY_ROP_SETIMPEDANCE)   
//    
//    if(jxx < MAXJ)
//    {
//        rxtools_tx_inrop_t txinrop = { 0xffffffff, 0xffffffff};
//        txinrop.txtime = (EOK_uint64dummy == time) ? (0xffffffff) : (time / 1000);
//        txinrop.txprog = sign;
//        rxtools_results_t results = {0, 0, 0};     
//        //reset impedence set point info
//        eo_dgn_rxchecksepoints.impedence[jxx].deltaprognumber = INT32_MAX;
//        eo_dgn_rxchecksepoints.impedence[jxx].deltarxtime = UINT32_MAX;
//        
//        int32_t ret = rxtools_verify_reception(&status_rop_impedance[jxx], &txinrop, maxtimegap_impedance, &results);
//        
//        if(-1 == ret)
//        {   // error: eval retflags
//            if(rxtools_flag_error_prognum == (rxtools_flag_error_prognum & results.flags))
//            {
//                // to do: an error in rop sequence number. there is a gap of results.deltaprognumber
//              
//                eo_dgn_rxchecksepoints.impedence[jxx].deltaprognumber = results.deltaprognumber;
//            }
//            if(rxtools_flag_error_rxtime == (rxtools_flag_error_rxtime & results.flags))
//            {
//                // to do: an error in timing: there was more than maxtimegap and it was results.deltarxtime
//                eo_dgn_rxchecksepoints.impedence[jxx].deltarxtime = results.deltarxtime; 
//            }  
//            
//            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_rxcheckSetpoints, 1000);
//            eo_theEMSdgn_resetSetpoints(eo_theEMSdgn_GetHandle());
//        }
//    }
//    
//#endif  


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
    /*NOTE: this function is equal to mc4 fucntion.*/
//    eOresult_t              res;
    eOmc_joint_status_t     *jstatus = NULL;
    eOmc_jointId_t          jxx = eoprot_ID2index(rd->id32);
    
    jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
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

// #include "../checksetpoint/callback_file.h"
// //extern uint8_t callback_of_setpoint(int32_t data, uint8_t joint);
// extern uint8_t callback_of_setpointV2(int32_t data, uint8_t joint);
// extern uint8_t callback_of_setpoint_all_joints(verify_pair_t pair, uint8_t joint);

extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOmc_setpoint_t                         *setPoint = (eOmc_setpoint_t*)rd->data;
    eOmc_joint_config_t                     *jconfig = NULL;
    eOmc_joint_status_t                     *jstatus = NULL;


    //set monitor status = notreachedyet if monitormode is forever
    jconfig = eo_entities_GetJointConfig(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
    if(NULL == jconfig)
    {
        return; //error
    }  
    
    //#warning -> marco.accame: cast to proper type
    if(eomc_motionmonitormode_forever == jconfig->motionmonitormode)
    {
        jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
        if(NULL == jstatus)
        {
            return; //error
        }
        
        //#warning --> marco.accame: cast to proper type
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == jstatus->basic.motionmonitorstatus)
        {
            jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
        }
    }
    
    switch (setPoint->type)
    { 
        case eomc_setpoint_position:
        {
//             #if defined(EOMTHEEMSAPPLCFG_USE_EB8)  || defined(EOMTHEEMSAPPLCFG_USE_EB6)
//             verify_pair_t pair;
//             pair.pos = setPoint->to.position.value;
//             pair.vel = setPoint->to.position.withvelocity;
//             callback_of_setpoint_all_joints(pair, jxx);
//             
//             //callback_of_setpoint(setPoint->to.position.value, jxx);
//             //callback_of_setpointV2(setPoint->to.position.value, jxx);
//             #endif
//            
//#if defined (VERIFY_ROP_SETPOINT_EB5)            
//    if(jxx < MAXJ)
//    {
//        rxtools_tx_inrop_t txinrop = { 0xffffffff, 0xffffffff};
//        txinrop.txtime = (EOK_uint64dummy == time) ? (0xffffffff) : (time / 1000);
//        txinrop.txprog = sign;
//        rxtools_results_t results = {0, 0, 0};        
//        
//        eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = INT32_MAX;
//        eo_dgn_rxchecksepoints.position[jxx].deltarxtime = UINT32_MAX;
//        
//        int32_t ret = rxtools_verify_reception(&status_rop_setpositionraw[jxx], &txinrop, maxtimegap_setpositionraw, &results);
//        
//        
//        if(-1 == ret)
//        {   // error: eval retflags
//            if(rxtools_flag_error_prognum == (rxtools_flag_error_prognum & results.flags))
//            {
//                // to do: an error in rop sequence number. there is a gap of results.deltaprognumber
//              
//                eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = results.deltaprognumber;
//            }
//            if(rxtools_flag_error_rxtime == (rxtools_flag_error_rxtime & results.flags))
//            {
//                // to do: an error in timing: there was more than maxtimegap and it was results.deltarxtime
//                eo_dgn_rxchecksepoints.position[jxx].deltarxtime = results.deltarxtime; 
//            }  
//            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_rxcheckSetpoints, 1000);
//            eo_theEMSdgn_resetSetpoints(eo_theEMSdgn_GetHandle());
//        }
//    }            
//            
//#endif            
            
            
            
            eo_emsController_SetPosRef(jxx, setPoint->to.position.value, setPoint->to.position.withvelocity);
        } break;
        
        case eomc_setpoint_positionraw:
        {
            
//#if (defined(VERIFY_ROP_SETPOSITIONRAW)  & (!defined(VERIFY_ROP_SETPOINT_EB5)))
//   
//    if(jxx < MAXJ)
//    {
//        rxtools_tx_inrop_t txinrop = { 0xffffffff, 0xffffffff};
//        txinrop.txtime = (EOK_uint64dummy == time) ? (0xffffffff) : (time / 1000);
//        txinrop.txprog = sign;
//        rxtools_results_t results = {0, 0, 0};        
//        int32_t ret = rxtools_verify_reception(&status_rop_setpositionraw[jxx], &txinrop, maxtimegap_setpositionraw, &results);
//        
//        eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = INT32_MAX;
//        eo_dgn_rxchecksepoints.position[jxx].deltarxtime = UINT32_MAX;
//
//        
//        if(-1 == ret)
//        {   // error: eval retflags
//            if(rxtools_flag_error_prognum == (rxtools_flag_error_prognum & results.flags))
//            {
//                // to do: an error in rop sequence number. there is a gap of results.deltaprognumber
//              
//                eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = results.deltaprognumber;
//            }
//            if(rxtools_flag_error_rxtime == (rxtools_flag_error_rxtime & results.flags))
//            {
//                // to do: an error in timing: there was more than maxtimegap and it was results.deltarxtime
//                eo_dgn_rxchecksepoints.position[jxx].deltarxtime = results.deltarxtime; 
//            }  
//            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_rxcheckSetpoints, 1000);
//            eo_theEMSdgn_resetSetpoints(eo_theEMSdgn_GetHandle());
//        }
//    }
//    
//#endif              
            
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
            return;
        }
    }
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eo_emsController_Stop(jxx);
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{
#if 1
    
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)rd->data;    

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
                                      
#else

    eOresult_t                              res;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)rd->data;
    
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
    


    eo_emsController_StartCalibration(jxx, 
                                      calibrator->params.type3.position, 
                                      calibrator->params.type3.velocity,
                                      calibrator->params.type3.offset);
    
#endif
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_controlmode_command_t *controlmode_ptr = (eOmc_controlmode_command_t*)rd->data;
    eOmc_jointId_t             jxx = eoprot_ID2index(rd->id32);
    
    /*
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_controlmode_command_t              controlmode_2foc = eomc_controlmode_cmd_openloop; //eomc_controlmode_cmd_current;
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());



    // 1) send control mode value to 2foc
    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);

    switch(*controlmode_ptr)
    {
        case eomc_controlmode_cmd_switch_everything_off:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
            eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);

            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
            eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
        }break;
        
        default: //2foc control mode is always current
        {
            if (eo_emsController_GetControlMode(jxx) == eomc_controlmode_idle)
            {
                eOmc_PID_t pid_open_loop;
                pid_open_loop.kp = 0x0A00;
                pid_open_loop.kd = 0;
                pid_open_loop.ki = 0;
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
                eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &pid_open_loop);
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
                eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
                eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
            
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
                eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
            }
        }
        
    }
    */
    
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

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



