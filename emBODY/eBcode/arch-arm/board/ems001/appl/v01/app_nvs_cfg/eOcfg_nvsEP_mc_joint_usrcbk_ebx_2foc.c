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

/* @file       eOcfg_nvsEP_mc_usrcbk_ebx.c
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
#include "EOnv_hid.h"

#include "EOMotionControl.h"


//application
#include "EOtheEMSapplBody.h"
#include "EOappTheDataBase.h"
#include "EOicubCanProto_specifications.h"
#include "EOappMeasuresConverter.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mngmnt_usr_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#include "EOMtheEMSapplCfg_hid.h"
// #define VERIFY_ROP_SETIMPEDANCE
// #define VERIFY_ROP_SETPOSITIONRAW


#if defined(VERIFY_ROP_SETIMPEDANCE) | defined(VERIFY_ROP_SETPOSITIONRAW)
    #include "EOtheEMSapplDiagnostics.h"
    #include "rxtools.h"
    #define MAXJ 4
    
    #if   defined(EOMTHEEMSAPPLCFG_USE_EB5)
        #define VERIFY_ROP_SETPOINT_EB5
    #endif
#endif


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

#if defined(VERIFY_ROP_SETIMPEDANCE)
    const uint32_t maxtimegap_impedance = 25; // 25 ms expressed in milli-sec
    static rxtools_rec_status_t status_rop_impedance[MAXJ] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
#endif
    
    
#if defined(VERIFY_ROP_SETPOSITIONRAW)
    const uint32_t maxtimegap_setpositionraw = 25; // 25 ms expressed in milli-sec
    static rxtools_rec_status_t status_rop_setpositionraw[MAXJ] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
#endif



    
    
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
    eOmc_joint_config_t             *cfg = (eOmc_joint_config_t*)nv->ram;
    memcpy(cfg, &joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t             *cfg = (eOmc_joint_status_t*)nv->ram;
    memcpy(cfg, &joint_default_value.status, sizeof(eOmc_joint_status_t));
}


//joint-update
            
extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t              res;
    eOmc_jointId_t          jxx = eoprot_ID2index(rd->id32);
    float                   rescaler_pos;
    float                   rescaler_trq;
    eOmc_joint_status_t     *jstatus_ptr = NULL;
    eOmc_joint_config_t     *cfg = (eOmc_joint_config_t*)nv->ram;

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
 

    
    // 1) set pid position 
    rescaler_pos = 1.0f/(float)(1<<cfg->pidposition.scale);
    eo_emsController_SetPosPid(jxx, cfg->pidposition.kp*rescaler_pos, 
	                                cfg->pidposition.kd*rescaler_pos, 
	                                cfg->pidposition.ki*rescaler_pos, 
	                                cfg->pidposition.limitonintegral,
                                    cfg->pidposition.limitonoutput,
	                                cfg->pidposition.offset);

    // 2) set torque pid    
    rescaler_trq = 1.0f/(float)(1<<cfg->pidtorque.scale);
    eo_emsController_SetTrqPid(jxx, cfg->pidtorque.kp*rescaler_trq, 
		                            cfg->pidtorque.kd*rescaler_trq, 
                                    cfg->pidtorque.ki*rescaler_trq,
                                    cfg->pidtorque.limitonintegral,
                                    cfg->pidtorque.limitonoutput, 
                                    cfg->pidtorque.offset);

    eo_emsController_SetEncSign((uint16_t)jxx, (int32_t)cfg->encoderconversionfactor);

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
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }

    if(eomc_motionmonitormode_dontmonitor == cfg->motionmonitormode)
    {
        jstatus_ptr->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus_ptr->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_setpointnotreachedyet;
    }

}


extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_PID_t      *pid_ptr = (eOmc_PID_t*)nv->ram;
    eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);
    float           rescaler = 1.0f/(float)(1<<pid_ptr->scale);
	
    eo_emsController_SetPosPid(jxx, pid_ptr->kp*rescaler, 
	                                pid_ptr->kd*rescaler, 
	                                pid_ptr->ki*rescaler,
                                    pid_ptr->limitonintegral,
                                    pid_ptr->limitonoutput, 
	                                pid_ptr->offset);
}

extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_PID_t      *pid_ptr = (eOmc_PID_t*)nv->ram;
    eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);
    float           rescaler = 1.0f/(float)(1<<pid_ptr->scale);

    eo_emsController_SetTrqPid(jxx, pid_ptr->kp*rescaler, 
	                                pid_ptr->kd*rescaler, 
	                                pid_ptr->ki*rescaler, 
                                    pid_ptr->limitonintegral,
                                    pid_ptr->limitonoutput, 
	                                pid_ptr->offset);
}

extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
   eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32); 
#if defined(VERIFY_ROP_SETIMPEDANCE)   
    
    if(jxx < MAXJ)
    {
        rxtools_tx_inrop_t txinrop = { 0xffffffff, 0xffffffff};
        txinrop.txtime = (EOK_uint64dummy == time) ? (0xffffffff) : (time / 1000);
        txinrop.txprog = sign;
        rxtools_results_t results = {0, 0, 0};     
        //reset impedence set point info
        eo_dgn_rxchecksepoints.impedence[jxx].deltaprognumber = INT32_MAX;
        eo_dgn_rxchecksepoints.impedence[jxx].deltarxtime = UINT32_MAX;
        
        int32_t ret = rxtools_verify_reception(&status_rop_impedance[jxx], &txinrop, maxtimegap_impedance, &results);
        
        if(-1 == ret)
        {   // error: eval retflags
            if(rxtools_flag_error_prognum == (rxtools_flag_error_prognum & results.flags))
            {
                // to do: an error in rop sequence number. there is a gap of results.deltaprognumber
              
                eo_dgn_rxchecksepoints.impedence[jxx].deltaprognumber = results.deltaprognumber;
            }
            if(rxtools_flag_error_rxtime == (rxtools_flag_error_rxtime & results.flags))
            {
                // to do: an error in timing: there was more than maxtimegap and it was results.deltarxtime
                eo_dgn_rxchecksepoints.impedence[jxx].deltarxtime = results.deltarxtime; 
            }  
            
            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_rxcheckSetpoints, 1000);
            eo_theEMSdgn_resetSetpoints(eo_theEMSdgn_GetHandle());
        }
    }
    
#endif  


    eOmc_impedance_t *cfg = (eOmc_impedance_t*)nv->ram;   
    eo_emsController_SetImpedance(jxx, cfg->stiffness, cfg->damping, cfg->offset);
}

extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t                 jxx = eoprot_ID2index(rd->id32);
    eOmeas_position_limits_t       *limit_ptr = (eOmeas_position_limits_t*)nv->ram;

    eo_emsController_SetPosMin(jxx, limit_ptr->min);
    eo_emsController_SetPosMax(jxx, limit_ptr->max);
}




extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t                           *time_ptr = (eOmeas_time_t*)nv->ram;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);

    eo_emsController_SetVelTimeout(jxx, *time_ptr);
}




extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    /*NOTE: this function is equal to mc4 fucntion.*/
    eOresult_t              res;
    eOmc_joint_status_t     *jstatus_ptr;
    eOmc_jointId_t          jxx = eoprot_ID2index(rd->id32);
    
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }

    if(eomc_motionmonitormode_dontmonitor == *((eOenum08_t*)nv->ram))
    {
        jstatus_ptr->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }
}

// #include "../checksetpoint/callback_file.h"
// //extern uint8_t callback_of_setpoint(int32_t data, uint8_t joint);
// extern uint8_t callback_of_setpointV2(int32_t data, uint8_t joint);
// extern uint8_t callback_of_setpoint_all_joints(verify_pair_t pair, uint8_t joint);
extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOmc_setpoint_t                         *setPoint = (eOmc_setpoint_t*)nv->ram;
    eOmc_joint_config_t                     *jconfig_ptr;
    eOmc_joint_status_t                     *jstatus_ptr;


    //set monitor status = notreachedyet if monitormode is forever
    res = eo_appTheDB_GetJointConfigPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jconfig_ptr);
    if(eores_OK != res)
    {
        return; //i should never be here
    }
    
    if(eomc_motionmonitormode_forever == jconfig_ptr->motionmonitormode)
    {
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
        if(eores_OK != res)
        {
            return; //i should never be here
        }
        
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == jstatus_ptr->basic.motionmonitorstatus)
        {
            jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
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
            
#if defined (VERIFY_ROP_SETPOINT_EB5)            
    if(jxx < MAXJ)
    {
        rxtools_tx_inrop_t txinrop = { 0xffffffff, 0xffffffff};
        txinrop.txtime = (EOK_uint64dummy == time) ? (0xffffffff) : (time / 1000);
        txinrop.txprog = sign;
        rxtools_results_t results = {0, 0, 0};        
        
        eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = INT32_MAX;
        eo_dgn_rxchecksepoints.position[jxx].deltarxtime = UINT32_MAX;
        
        int32_t ret = rxtools_verify_reception(&status_rop_setpositionraw[jxx], &txinrop, maxtimegap_setpositionraw, &results);
        
        
        if(-1 == ret)
        {   // error: eval retflags
            if(rxtools_flag_error_prognum == (rxtools_flag_error_prognum & results.flags))
            {
                // to do: an error in rop sequence number. there is a gap of results.deltaprognumber
              
                eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = results.deltaprognumber;
            }
            if(rxtools_flag_error_rxtime == (rxtools_flag_error_rxtime & results.flags))
            {
                // to do: an error in timing: there was more than maxtimegap and it was results.deltarxtime
                eo_dgn_rxchecksepoints.position[jxx].deltarxtime = results.deltarxtime; 
            }  
            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_rxcheckSetpoints, 1000);
            eo_theEMSdgn_resetSetpoints(eo_theEMSdgn_GetHandle());
        }
    }            
            
#endif            
            
            
            
            eo_emsController_SetPosRef(jxx, setPoint->to.position.value, setPoint->to.position.withvelocity);
        }break;
        
        case eomc_setpoint_positionraw:
        {
            
#if (defined(VERIFY_ROP_SETPOSITIONRAW)  & (!defined(VERIFY_ROP_SETPOINT_EB5)))
    
    if(jxx < MAXJ)
    {
        rxtools_tx_inrop_t txinrop = { 0xffffffff, 0xffffffff};
        txinrop.txtime = (EOK_uint64dummy == time) ? (0xffffffff) : (time / 1000);
        txinrop.txprog = sign;
        rxtools_results_t results = {0, 0, 0};        
        int32_t ret = rxtools_verify_reception(&status_rop_setpositionraw[jxx], &txinrop, maxtimegap_setpositionraw, &results);
        
        eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = INT32_MAX;
        eo_dgn_rxchecksepoints.position[jxx].deltarxtime = UINT32_MAX;

        
        if(-1 == ret)
        {   // error: eval retflags
            if(rxtools_flag_error_prognum == (rxtools_flag_error_prognum & results.flags))
            {
                // to do: an error in rop sequence number. there is a gap of results.deltaprognumber
              
                eo_dgn_rxchecksepoints.position[jxx].deltaprognumber = results.deltaprognumber;
            }
            if(rxtools_flag_error_rxtime == (rxtools_flag_error_rxtime & results.flags))
            {
                // to do: an error in timing: there was more than maxtimegap and it was results.deltarxtime
                eo_dgn_rxchecksepoints.position[jxx].deltarxtime = results.deltarxtime; 
            }  
            eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_rxcheckSetpoints, 1000);
            eo_theEMSdgn_resetSetpoints(eo_theEMSdgn_GetHandle());
        }
    }
    
#endif              
            
            eo_emsController_SetPosRaw(jxx, setPoint->to.position.value);
        }break;
        
        case eomc_setpoint_velocity:
        {
            eo_emsController_SetVelRef(jxx, setPoint->to.velocity.value, setPoint->to.velocity.withacceleration);    
        }break;

        case eomc_setpoint_torque:
        {
            eo_emsController_SetTrqRef(jxx, setPoint->to.torque.value);
        }break;

        case eomc_setpoint_current:
        {             
            eo_emsController_SetOutput(jxx, setPoint->to.current.value);
        }break;

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
    eOresult_t                              res;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    icubCanProto_controlmode_t              controlmode_2foc = icubCanProto_controlmode_openloop;
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
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
    


    eo_emsController_StartCalibration(jxx, 
                                      calibrator->params.type3.position, 
                                      calibrator->params.type3.velocity,
                                      calibrator->params.type3.offset);
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_controlmode_command_t *controlmode_ptr = (eOmc_controlmode_command_t*)nv->ram;
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
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
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
                eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &pid_open_loop);
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
                eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
                eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
            
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
                eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
            }
        }
        
    }
    */
    
    // 2) set control mode to ems controller
    eo_emsController_SetControlMode(jxx, (eOmc_controlmode_command_t)(*controlmode_ptr), eobool_true);       
}


// __ALE__
extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    eo_emsController_ReadTorque(jxx, *(eOmeas_torque_t*)nv->ram);
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



