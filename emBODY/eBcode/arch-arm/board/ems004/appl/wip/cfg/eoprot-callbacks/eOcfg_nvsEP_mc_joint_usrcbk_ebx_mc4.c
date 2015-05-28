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

/* @file       eOcfg_nvsEP_mc_joint_usrcbk_ebx_mc4.c
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

#include "EOMotionControl.h"

//application
#include "EOtheEMSapplBody.h"

#include "EOtheMeasuresConverter.h"
#include "EOtheMeasuresConverter_hid.h"

#include "EOtheEMSapplDiagnostics.h"

#include "EOtheEntities.h"

#ifdef USE_PROTO_PROXY
#include "EOMtheEMSbackdoor.h"
#include "EOproxy.h"
#include "EOtheBOARDtransceiver.h"
#endif


#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANservice.h"

#include "EOtheVirtualStrain.h"

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
// empty-section



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

static void s_onpid(const EOnv* nv, const eOropdescriptor_t* rd, pid_type_t type);

static eOresult_t s_translate_eOmcControlMode2icubCanProtoControlMode(eOmc_controlmode_command_t eomc_controlmode, eOmc_jointId_t jId,
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
/* TAG_ALE*/

//joint-init
extern void eoprot_fun_INIT_mc_joint_config(const EOnv* nv)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)eo_nv_RAM(nv);
    memcpy(cfg, &joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t *status = (eOmc_joint_status_t*)eo_nv_RAM(nv);
    memcpy(status, &joint_default_value.status, sizeof(eOmc_joint_status_t));
}


//joint-update
extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
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
    
    
    // 7) set monitormode status
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);

    if(eomc_motionmonitormode_dontmonitor == cfg->motionmonitormode)
    {
        jstatus->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_setpointnotreachedyet;
    }  
    
    
#if 0
    eOresult_t                              res;    
    eOmc_joint_config_t                     *cfg = (eOmc_joint_config_t*)rd->data;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_joint_status_t                     *jstatus = NULL;
    icubCanProto_position_t                 pos_icubCanProtValue, minpos_icubCanProtValue, maxpos_icubCanProtValue;
    icubCanProto_impedance_t                impedence_icubCanProtValues;
    eOicubCanProto_msgDestination_t         msgdest;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    /* Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv = eo_measconv_GetHandle(); 

    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }
    
    //first of all set conversion factor
    res = eo_measconv_SetJntEncoderConversionFactor(appMeasConv, jxx, (eOmeasconv_encConversionFactor_t)eo_common_Q17_14_to_float(cfg->encoderconversionfactor));
    if(eores_OK != res)
    {
        return;
    }
    res = eo_measconv_SetJntEncoderConversionOffset(appMeasConv, jxx, (eOmeasconv_encConversionOffset_t)eo_common_Q17_14_to_float(cfg->encoderconversionoffset));
    if(eores_OK != res)
    {
        return;
    }



    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    // 1) send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_POS_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidposition);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidposition);


    // 2) send torque pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidtorque);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidtorque);

    // 3) send velocity pid: currently is not send: neither MC4 nor 2foc use pid velocity.

    // 4) set limits
    minpos_icubCanProtValue = eo_measconv_jntPosition_I2E(appMeasConv, jxx, cfg->limitsofjoint.min);
    maxpos_icubCanProtValue = eo_measconv_jntPosition_I2E(appMeasConv, jxx, cfg->limitsofjoint.max);
    
    if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
    {
        //swap min and max
        pos_icubCanProtValue = minpos_icubCanProtValue;
        minpos_icubCanProtValue = maxpos_icubCanProtValue;
        maxpos_icubCanProtValue = pos_icubCanProtValue;
    }
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&minpos_icubCanProtValue);
 
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&maxpos_icubCanProtValue);

    // 5) set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->velocitysetpointtimeout);

    // 6) set impedance
    impedence_icubCanProtValues.stiffness = eo_measconv_impedenceStiffness_I2S(appMeasConv, jxx, cfg->impedance.stiffness);
    impedence_icubCanProtValues.damping = eo_measconv_impedenceDamping_I2S(appMeasConv, jxx, cfg->impedance.damping);
    impedence_icubCanProtValues.offset = eo_measconv_torque_I2S(appMeasConv, jxx, cfg->impedance.offset);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues.offset);
    
    // 7) set monitormode status
    jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
    if(NULL == jstatus)
    {
        return; //error
    }    

    if(eomc_motionmonitormode_dontmonitor == cfg->motionmonitormode)
    {
        jstatus->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_setpointnotreachedyet;
    }
#endif    
}


extern void eoprot_fun_UPDT_mc_joint_config_pidposition(const EOnv* nv, const eOropdescriptor_t* rd)
{
    s_onpid(nv, rd, pid_typePOS);
}

extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    s_onpid(nv, rd, pid_typeTOR);
}




extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_impedance_t *impedance = (eOmc_impedance_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
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

extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{

    eOmeas_position_limits_t *limitsofjoint = (eOmeas_position_limits_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    
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


extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t *time = (eOmeas_time_t*)rd->data;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;    
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
    command.value = time;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);    
    
//#if 0    
//    eOmeas_time_t                          *time_ptr = (eOmeas_time_t*)rd->data;
//    eOmc_jointId_t                         jxx = eoprot_ID2index(rd->id32);
//    eOicubCanProto_msgCommand_t            msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT
//    };
//
//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, (void*)time_ptr);  
//#endif //0    
}


extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
    
    if(NULL == jstatus)
    {
        return; //error
    }

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
{  
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
{ 
    eOresult_t res = eores_NOK_generic;     
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOutil_emulfloat32_t *encoffset = (eOutil_emulfloat32_t*)rd->data;

    res = eo_measconv_SetJntEncoderConversionOffset(eo_measconv_GetHandle(), jxx, (eOmeasconv_encConversionOffset_t)eo_common_Q17_14_to_float(*encoffset));
    if(eores_OK != res)
    {
        return; //error 
    }
}

//#warning TODO: the setpoint cannot be read, thus in here the reception of a eo_ropcode_ask is illegal. if the variable is proxied, and we receive an ask .... issue a diagnostics warning
extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{   
    eOmc_setpoint_t *setpoint = (eOmc_setpoint_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_t *joint = eo_entities_GetJoint(eo_entities_GetHandle(), jxx);
        
    EOtheMeasuresConverter *appMeasConv = eo_measconv_GetHandle();

    if(NULL == joint)
    {
        return; //error
    }   
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;    
    command.type  = 0;
    command.value = NULL;
    
    // 1) set monitor status = notreachedyet if monitormode is forever
    
    // only set i possible with a cmmnds-setpoint ... howver there is this control because the ask was sent in the past and ...
    if(eo_ropcode_set == rd->ropcode)
    {
        //#warning marco.accame: it would be better the following with cast to (eOmc_motionmonitormode_t)
        //if(eomc_motionmonitormode_forever == (eOmc_motionmonitormode_t)joint->config.motionmonitormode)
        if(eomc_motionmonitormode_forever == joint->config.motionmonitormode)        
        {           
            /* if monitorstatus values setpointreached means this is a new set point, 
            so i need to start to check is set point is reached because i'm in monitormode = forever */
            if(eomc_motionmonitorstatus_setpointisreached == joint->status.basic.motionmonitorstatus)
            //if(eomc_motionmonitorstatus_setpointisreached == (eOmc_motionmonitorstatus_t)joint->status.basic.motionmonitorstatus)
            {
                joint->status.basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
            }
        }
        
        // 2) prepare setpoint to send
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
        
        return;
    }    
    
    
//#if 0    
//    eOmc_setpoint_t                         *setPoint = (eOmc_setpoint_t*)rd->data;
//    void                                    *val_ptr = NULL;
//    eOmc_joint_status_t                     *jstatus = NULL;
//    eOmc_joint_config_t                     *jconfig = NULL;
//    icubCanProto_position_t                 pos;
//    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
//    EOappTheDB                              *db = eo_appTheDB_GetHandle();

//    eOicubCanProto_msgCommand_t            msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) 0
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
//    EOappMeasConv* appMeasConv = eo_measconv_GetHandle();

//    // 1) set monitor status = notreachedyet if monitormode is forever
//    jconfig = eo_entities_GetJointConfig(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
//    if(NULL == jconfig)
//    {
//        return; //error
//    }    
//    
//    if(eo_ropcode_set == rd->ropcode)
//    {
//        //#warning marco.accame: it would be better the following with cast to (eOmc_motionmonitormode_t)
//        //if(eomc_motionmonitormode_forever == (eOmc_motionmonitormode_t)jconfig->motionmonitormode)
//        if(eomc_motionmonitormode_forever == jconfig->motionmonitormode)        
//        {
//            jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
//            if(NULL == jstatus)
//            {
//                return; //error
//            }
//            
//            /* if monitorstatus values setpointreached means this is a new set point, 
//            so i need to start to check is set point is reached because i'm in monitormode = forever */
//            if(eomc_motionmonitorstatus_setpointisreached == jstatus->basic.motionmonitorstatus)
//            //if(eomc_motionmonitorstatus_setpointisreached == (eOmc_motionmonitorstatus_t)jstatus->basic.motionmonitorstatus)
//            {
//                jstatus->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
//            }
//        }
//        
//        // 2) prepare setpoint to send
//        switch(setPoint->type)
//        {
//            case eomc_setpoint_position:
//            {
//                icubCanProto_setpoint_position_t  setpoint_pos;
//                
//                setpoint_pos.value = eo_measconv_jntPosition_I2E(appMeasConv, jxx, setPoint->to.position.value);
//                //reference velocity of position set point must be always >0, so here absolute func is used.
//                setpoint_pos.withvelocity = eo_measconv_jntVelocity_I2E_abs(appMeasConv,jxx, setPoint->to.position.withvelocity);
//                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE; 
//                val_ptr =  &setpoint_pos; 
//            } break;

//            case eomc_setpoint_velocity:
//            {
//                icubCanProto_setpoint_velocity_t  setpoint_vel;
//                
//                setpoint_vel.withacceleration = eo_measconv_jntAcceleration_I2E_abs(appMeasConv, jxx, setPoint->to.velocity.withacceleration);           
//                setpoint_vel.value = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, setPoint->to.velocity.value);
//                
//                if (setpoint_vel.withacceleration < 1)
//                {
//                    setpoint_vel.withacceleration = 1;
//                }            
//                
//                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE;                 
//                val_ptr =  &setpoint_vel;   
//                
//            } break;

//            case eomc_setpoint_torque:
//            {
//                icubCanProto_setpoint_torque_t setpoint_torque;
//                
//                setpoint_torque.value = eo_measconv_torque_I2S(appMeasConv,jxx, setPoint->to.torque.value);
//                
//                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE;           
//                val_ptr =  &setpoint_torque;    
//            } break;

//            case eomc_setpoint_current:
//            { 
//                // marco.accame on 27 feb 2015.               
//                // when embObjMotionControl::setRefOutputRaw() sends a set<setpoint, (current, value)>,
//                // here is what it is done: we send it to mc4 with a openloop params command ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS.
//                // then the reading back is done with embObjMotionControl::getRefOutputRaw() which
//                // just reads its buffered value of joint.status.ofpid.positionreference which in this case 
//                // must contain the value of the openloop current as received and accepted by the mc4 
//                // board. the mc4 board could refuse to accept the value if for instance is not in openloop mode.
//                // moreover: the mc4 board does not normally broadcast this value, thus we must ask it
//                // in an explicit way with a further CAN message of type ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS
//                // at this point, in the tx phase the two CAN commands (set and then get) are sent and the
//                // reply of teh get is received at 1 ms from now in the next rx phase (hopefully).
//                // at this point the rx handler of the openloop param fills the value of joint.status.ofpid.positionreference
//                // which is broadcasted up to robotInterface about 1.8 ms after now.
//                // ... i can smell danger of a race condition if a high level user calls setRefOutputRaw() and then getRefOutputRaw()
//                // without any wait time in between.
//                // ... moreover: the getRefOutputRaw() returns the value buffered in EMS and asked to mc4 the last time a setRefOutputRaw()
//                // was called.
//                // anyway: for now it works fine and having asked around to people in the lab the thing is not worrying them
//                // a solution: getRefOutputRaw() does not read the joint.status but directly
//                // send a ask<eoprot_tag_mc_joint_status_ofpid_positionreference> WHICH IS A NEW TAG TO BE ADDED.
//                // then we use the proxy so that this request is sent to the mc4 which gives the value and then the value
//                // is sent up with a say<eoprot_tag_mc_joint_status_ofpid_positionreference, value> 
//                // in such a way the value is always correct   
//               
//                icubCanProto_setpoint_current_t setpoint_current;
//                setpoint_current.value = setPoint->to.current.value;
//                
//                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS;           
//                val_ptr =  &setpoint_current;    
//                
//            } break;

//            case eomc_setpoint_positionraw:
//            {    
//                pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, setPoint->to.position.value);
//                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION; 
//                val_ptr =  &pos; 
//            } break;
//            
//            default:
//            {
//                return;
//            } break;
//        }

//        // 3) send msg
//        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, val_ptr);
//        
//        
//        if(eomc_setpoint_current == setPoint->type)
//        {
//            //since mc4 don't send perodic message with openloop refernce
//            //i need to ask it direct to mc4
//            msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS; 
//            eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, NULL);
//        }
//        return;
//    }

//    // marco.accame on 03mar15: one should not send a ask<setpoint> to the ems. and robotInterface does not do it.
//    // but also the code in here is incorrect because: the proxy stores and id32 of type setpoint, the UPDT() function 
//    // send to the MC4 a can message of type ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE, the can parser when parses the reply
//    // in eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque() fill the setpoint-torque with rx value and then passed it
//    // to proxy with id32 of type eoprot_tag_mc_joint_config_impedance ... it cannot work.
//    // THUS: i remove it from here but also from eo_icubCanProto_parser_pol_mb_cmd__getDesiredTorque() .....
////#ifdef USE_PROTO_PROXY
////    if(eo_ropcode_ask == rd->ropcode)
////    {
////        //only for setpoint of type current and torque exist the get command
////        switch(setPoint->type)
////        {
////            case eomc_setpoint_torque:
////            {
////                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE;
////            } break;
////
////
//////            case eomc_setpoint_current:
//////            {
//////                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS;
//////                
//////            }break;
////
////            default:
////            {
////                return;
////            } break;
////        }
////#if 0        
//////        eOappTheDB_hid_ethProtoRequest_t req = 
//////        {
//////            .id32 = rd->id32,
//////            .signature = rd->signature,
//////            .nvRam_ptr = rd->data,
//////            .numOfExpectedResp = 1,
//////            .numOfREceivedResp = 0
//////        };  
//////        
//////        eOresult_t res = eo_appTheDB_appendEthProtoRequest(db, eoprot_entity_mc_joint, jxx, &req);
//////        if(eores_OK != res)
//////        {
//////            eOerrmanDescriptor_t errdes = {0};
//////            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
//////            errdes.sourceaddress    = 0;
//////            errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_forward_callback_fails);
//////            errdes.par16            = 0; 
//////            errdes.par64            = ((uint64_t)rd->signature << 32) | (rd->id32); 
//////            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
//////            return;
//////        }
////#else        
////        EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
////        eOproxy_params_t *param = eo_proxy_Params_Get(proxy, rd->id32);
////        if(NULL == param)
////        {
////            eOerrmanDescriptor_t errdes = {0};
////            errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
////            errdes.sourceaddress    = 0;
////            errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_forward_callback_fails);
////            errdes.par16            = 0; 
////            errdes.par64            = ((uint64_t)rd->signature << 32) | (rd->id32); 
////            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
////            return;
////        }
////        param->p08_1 = 1;       // we expect one can frame
////        param->p08_2 = 0;       // and we havent received any yet
////#endif         
////        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, NULL);
////        
////        return;
////    }
////#endif

//#endif //0
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eObool_t *cmd = (eObool_t*)rd->data;

    if(eobool_true == *cmd)
    {
        eOcanprot_command_t command = {0};
        command.class = eocanprot_msgclass_pollingMotorControl;    
        command.type  = ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY;
        command.value = NULL;
        eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
    }
    
    
//#if 0    
//    eObool_t                                *cmd = (eObool_t*)rd->data;
//    eOmc_jointId_t                         jxx = eoprot_ID2index(rd->id32);
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY
//    };
//
//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//
//    if(1 == *cmd)
//    {
//        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, NULL);
//    }
//#endif //0    
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_calibrator_t *calibrator = (eOmc_calibrator_t*)rd->data;
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


//#if 0    
//    eOresult_t                              res;
//    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
//    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)rd->data;
//    icubCanProto_calibrator_t               iCubCanProtCalibrator;
//    eOmc_joint_status_t                     *jstatus = NULL;
//    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
//    eOicubCanProto_msgDestination_t         msgdest;
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) 0
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
//    EOappMeasConv* appMeasConv = eo_measconv_GetHandle();

//    // 1) set control mode in status nv (a mirror nv)
//    #warning --> marco.accame: removed function as it is useless
//    jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
//    if(NULL == jstatus)
//    {
//        return; //error
//    }
//    //jstatus_ptr->basic.controlmodestatus = eomc_controlmode_calib;

//    
//    // 2) send control mode value to mc4
//    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
//    if(eores_OK != res)
//    {
//        return;
//    }

//    //set destination of all messages 
//    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
//    
//    
// 
//    //prepare calibration data to send
//    /*Note: currently calibration params are raw, so they doesn't need to be converted in icub can proto measures;
//    but i prepare code for future: when calibration param used new measure unit.*/
//    iCubCanProtCalibrator.type = (icubCanProto_calibration_type_t)calibrator->type;
//    switch(calibrator->type)
//    {
//        case eomc_calibration_type0_hard_stops:
//        {
//            iCubCanProtCalibrator.params.type0.pwmlimit = calibrator->params.type0.pwmlimit;
////            iCubCanProtCalibrator.params.type0.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type0.velocity);           
//            iCubCanProtCalibrator.params.type0.velocity = calibrator->params.type0.velocity;
//        }break;
//            
//        case eomc_calibration_type1_abs_sens_analog:
//        {
////             icubCanProto_position_t pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, calibrator->params.type1.position);
////             /*sice pos param is a word of 16 bits i must check min and max*/
////             if((pos < INT16_MIN)||(pos>INT16_MAX))
////             {
////                 return;
////             }
////             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
////             iCubCanProtCalibrator.params.type1.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type1.velocity);
//             iCubCanProtCalibrator.params.type1.position = calibrator->params.type1.position; 
//             iCubCanProtCalibrator.params.type1.velocity =  calibrator->params.type1.velocity;
//        }break;

//        case eomc_calibration_type2_hard_stops_diff:
//        {
//            iCubCanProtCalibrator.params.type2.pwmlimit = calibrator->params.type2.pwmlimit;
////            iCubCanProtCalibrator.params.type2.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type2.velocity);           
//            iCubCanProtCalibrator.params.type2.velocity = calibrator->params.type2.velocity;           
//        }break;

//        case eomc_calibration_type3_abs_sens_digital:
//        {
////              icubCanProto_position_t pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, calibrator->params.type3.position);
////             /*sice pos param is a word of 16 bits i must check min and max*/
////             if((pos < INT16_MIN)||(pos>INT16_MAX))
////             {
////                 return;
////             }
////             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
////             iCubCanProtCalibrator.params.type3.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type3.velocity);           
//            iCubCanProtCalibrator.params.type1.position = calibrator->params.type3.position; 
//            iCubCanProtCalibrator.params.type3.velocity = calibrator->params.type3.velocity;            
//            iCubCanProtCalibrator.params.type3.offset = calibrator->params.type3.offset;
//        }break;

//        case eomc_calibration_type4_abs_and_incremental:
//        {
////             icubCanProto_position_t pos = eo_measconv_jntPosition_I2E(appMeasConv, jxx, calibrator->params.type4.position);
////             //here position is in int16_t ==> so i must verify if pos is out of int16_t range
////             if((pos > INT16_MAX) || (pos < INT16_MIN))
////             {
////                 return;
////                 #warning VALE --> how to manage this error???
////             }
////             iCubCanProtCalibrator.params.type4.position = (icubCanProto_position4calib_t)pos; 
////            iCubCanProtCalibrator.params.type4.velocity = eo_measconv_jntVelocity_I2E(appMeasConv, jxx, calibrator->params.type4.velocity);            
//            iCubCanProtCalibrator.params.type4.position = calibrator->params.type4.position;
//            iCubCanProtCalibrator.params.type4.velocity = calibrator->params.type4.velocity;
//            iCubCanProtCalibrator.params.type4.maxencoder = calibrator->params.type4.maxencoder;
//        }break;
//        
//    }

//    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER;
//    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &iCubCanProtCalibrator);
//#endif //0 
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_controlmode_command_t *controlmode = (eOmc_controlmode_command_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    icubCanProto_controlmode_t icubcanProto_controlmode = icubCanProto_controlmode_idle;
    
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;    
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &icubcanProto_controlmode;   
          
    if(eores_OK != s_translate_eOmcControlMode2icubCanProtoControlMode(*controlmode, jxx, &icubcanProto_controlmode))
    {
        return;
    }
    
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, rd->id32);
       
    
//#if 0    
//    eOresult_t                              res;
//    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
//    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
//    eOmc_joint_status_t                     *jstatus = NULL;
//    eOmc_controlmode_command_t              *controlmode_cmd_ptr = (eOmc_controlmode_command_t*)rd->data;
//    icubCanProto_controlmode_t              icubcanProto_controlmode;
//    eOicubCanProto_msgDestination_t         msgdest;
//    EOappTheDB                              *db = eo_appTheDB_GetHandle();

//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) 0
//    };

//    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

//    #warning --> marco.accame: the following function is useless, thus i remove it
//    jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
//    if(NULL == jstatus)
//    {
//        return; //error
//    }
//    

//    // 2) get joint can location
//    res = eo_appTheDB_GetJointCanLocation(db, jxx,  &canLoc, NULL);
//    if(eores_OK != res)
//    {
//        return;
//    }
//    
//    // 4) set destination of all messages 
//    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);

//    
////     #include "hal.h"
////     char str[60];
////     
////     snprintf(str, sizeof(str), "received control mode =%d joint %d", *controlmode_cmd_ptr, jxx);
////     hal_trace_puts(str);
//    
////i commented this code because with new control modes, i don't need to send ena or disa pwm.    
////     if( (eomc_controlmode_cmd_switch_everything_off == *controlmode_cmd_ptr) ||
////         (eomc_controlmode_cmd_force_idle == *controlmode_cmd_ptr)            ||
////         (eomc_controlmode_cmd_idle == *controlmode_cmd_ptr) )
////         {
////             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
////             eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);

////             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
////             eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
////             
////             snprintf(str, sizeof(str), "in controlmode: snd disa pwm e idle joint %d", jxx);
////             hal_trace_puts(str);
////             return;
////         }
////     
////     if((eomc_controlmode_idle == jstatus_ptr->basic.controlmodestatus) &&
////         (eomc_controlmode_cmd_force_idle != *controlmode_cmd_ptr)     &&
////         (eomc_controlmode_cmd_idle != *controlmode_cmd_ptr)           &&
////         (eomc_controlmode_cmd_switch_everything_off != *controlmode_cmd_ptr))
////     {
////         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
////         eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
////     
////         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
////         eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
////          snprintf(str, sizeof(str), "in controlmode: snd ena pwm e run joint %d", jxx);
////             hal_trace_puts(str);
////     }
//    
//    
//        
//        
//    res = s_translate_eOmcControlMode2icubCanProtoControlMode(*controlmode_cmd_ptr, jxx, &icubcanProto_controlmode);
//    if(eores_OK != res)
//    {
//        return;
//    }
//    
////     snprintf(str, sizeof(str), "in controlmode: snd icubcontrolmode =%d joint %d", icubcanProto_controlmode, jxx);
////     hal_trace_puts(str);
//    
//    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
//    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &icubcanProto_controlmode);
    
  
    
////     if(eomc_controlmode_idle == jstatus_ptr->basic.controlmodestatus)
////     {
////         if(eomc_controlmode_cmd_position != *controlmode_cmd_ptr)
////         {
////             //i can exit from idle only if i received set control mode position command
////             return;
////         }
////         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
////         eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
////     
////         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
////         eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
////     }
////     else
////     {
////         if(eomc_controlmode_cmd_switch_everything_off == *controlmode_cmd_ptr)
////         {
////             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
////             eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);

////             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
////             eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, NULL);
////         }
////         else
////         {
////             res = s_translate_eOmcControlMode2icubCanProtoControlMode(*controlmode_cmd_ptr, &icubcanProto_controlmode);
////             if(eores_OK != res)
////             {
////                 return;
////             }
////             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
////             eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &icubcanProto_controlmode);
////         }
////     }
//#endif //0
}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{    
    eOmc_interactionmode_t *interaction = (eOmc_interactionmode_t*)rd->data;    
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
       
    
//#if 0    
//    eOmc_interactionmode_t                  *interaction = (eOmc_interactionmode_t*)rd->data;
//    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
//    eOmc_joint_status_t                     *jstatus = NULL;
//    EOappCanSP                              *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
//    icubCanProto_interactionmode_t          icub_interctmode;
//    EOappTheDB                              *db = eo_appTheDB_GetHandle();
//    
//    #warning --> marco.accame: the following function is useless, thus i remove it
//    jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), (eOmc_jointId_t)jxx);
//    if(NULL == jstatus)
//    {
//        return; //error
//    }
//        
//    
//    eOicubCanProto_msgCommand_t             msgCmd = 
//    {
//        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
//        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE
//    };

//    

//    if(eOmc_interactionmode_stiff == *interaction)
//    {
//        icub_interctmode = icubCanProto_interactionmode_stiff;
//    }
//    else if(eOmc_interactionmode_compliant == *interaction)
//    {
//        icub_interctmode = icubCanProto_interactionmode_compliant;
//    }
//    else
//    {
//        //icub_interctmode = icubCanProto_interactionmode_unknownError;
//        return;
//    }

//    eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, &icub_interctmode);
//    
//#endif //0
}


extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_torque_t *torque = (eOmeas_torque_t*)rd->data;
    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
    icubCanProto_torque_t icub_torque = 0;
    
//     EOappMeasConv* appMeasConv = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
//     icubCanProto_torque_t icub_torque =  eo_measconv_torque_I2S(appMeasConv, jxx, *torque);

    // marco.accame on 27 may 2015: dont know why there is this offset. thus, ... i left as i found it
    icub_torque = *torque + 0x8000;
    eo_virtualstrain_SetTorque(eo_virtualstrain_GetHandle(), jxx, icub_torque);
    
  
//#if 0    
//    eOmeas_torque_t *torquevalue_ptr = (eOmeas_torque_t*)rd->data;
//    eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);
//    icubCanProto_torque_t icub_torque;
//    
////     EOappMeasConv* appMeasConv = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
////     icubCanProto_torque_t icub_torque =  eo_measconv_torque_I2S(appMeasConv, jxx, *torquevalue_ptr);

//    icub_torque = *torquevalue_ptr +0x8000;
//    eo_appTheDB_SetVirtualStrainValue(eo_appTheDB_GetHandle(), jxx, icub_torque);
//#endif //0    
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_translate_eOmcControlMode2icubCanProtoControlMode(eOmc_controlmode_command_t eomc_controlmode, eOmc_jointId_t jId,
                                                                      icubCanProto_controlmode_t *icubcanProto_controlmode)
{
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
            return(eores_NOK_generic);
        }
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
            return(eores_NOK_generic);
        }
            
    }
    return(eores_OK);
}

//I implement following function in order to translate new control modes to older modes.
// static eOresult_t s_translate_eOmcControlMode2icubCanProtoControlMode(eOmc_controlmode_command_t eomc_controlmode, eOmc_jointId_t jId,
//                                                                       icubCanProto_controlmode_t *icubcanProto_controlmode)
// {
//     eOmc_joint_commands_t       *jcmds_ptr;
//     eOresult_t                  res;
//     
//     // get joint can location
//     res = eo_appTheDB_GetJointCommandsPtr(eo_appTheDB_GetHandle(), jId,  &jcmds_ptr);
//     if(eores_OK != res)
//     {
//         return(res);
//     }
//     
//     #warning VALE: ho lasciato anche i vecchi control mode per compatibilita'!
//     
//     switch(eomc_controlmode)
//     {
//         case eomc_controlmode_cmd_position:
//         case eomc_controlmode_cmd_mixed:
//         case eomc_controlmode_cmd_direct:
//         {
//             if(jcmds_ptr->interactionmode == eOmc_interactionmode_stiff)
//             {
//                 *icubcanProto_controlmode = icubCanProto_controlmode_position;
//                 return(eores_OK);
//             }
//             
//             if(jcmds_ptr->interactionmode == eOmc_interactionmode_compliant)
//             {
//                 *icubcanProto_controlmode = icubCanProto_controlmode_impedance_pos;
//                 return(eores_OK);
//             }
//             
//             return(eores_NOK_generic);
//             
//         }//break;
//         case eomc_controlmode_cmd_velocity:
//         {
//             if(jcmds_ptr->interactionmode == eOmc_interactionmode_stiff)
//             {
//                 *icubcanProto_controlmode = icubCanProto_controlmode_velocity;
//                 return(eores_OK);
//             }
//             
//             if(jcmds_ptr->interactionmode == eOmc_interactionmode_compliant)
//             {
//                 *icubcanProto_controlmode = icubCanProto_controlmode_impedance_vel;
//                 return(eores_OK);
//             }
//             
//             return(eores_NOK_generic);
//             
//         }//break;
//         case eomc_controlmode_cmd_torque:
//         {
//             *icubcanProto_controlmode = icubCanProto_controlmode_torque;
//         }break;
//         case eomc_controlmode_cmd_impedance_pos: //old to remove!!!
//         {
//             *icubcanProto_controlmode = icubCanProto_controlmode_impedance_pos;
//         }break;
//         case eomc_controlmode_cmd_impedance_vel: //old to remove!!!
//         {
//              *icubcanProto_controlmode = icubCanProto_controlmode_impedance_vel;
//         }break;
//         case eomc_controlmode_cmd_current:
//         {
//             *icubcanProto_controlmode = icubCanProto_controlmode_current;
//         }
//         case eomc_controlmode_cmd_openloop:
//         {
//             *icubcanProto_controlmode = icubCanProto_controlmode_openloop;
//         }break;
//         case eomc_controlmode_cmd_switch_everything_off: //old to remove
//         {
//             return(eores_NOK_generic);
//         }
//         default:
//         {
//             return(eores_NOK_generic);
//         }
//             
//     }
//     return(eores_OK);
// }

// type is: 0 pos, 1 torque
static void s_onpid(const EOnv* nv, const eOropdescriptor_t* rd, pid_type_t type)
{
    static const uint8_t cmd_set_pid[2] = {ICUBCANPROTO_POL_MC_CMD__SET_POS_PID, ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID};
    static const uint8_t cmd_set_pidlimits[2] = {ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS, ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS};
    static const uint8_t cmd_get_pid[2] = {ICUBCANPROTO_POL_MC_CMD__GET_POS_PID, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID};
    static const uint8_t cmd_get_pidlimits[2] = {ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS};
        
    eOmc_PID_t *pid = (eOmc_PID_t*)rd->data;
    eOmc_jointId_t jxx = eoprot_ID2index(rd->id32);
    
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



