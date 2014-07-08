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
//#include "EOtheErrorManager.h"

#include "EOMotionControl.h"


//application
#include "EOtheEMSapplBody.h"
#include "EOappTheDataBase.h"
#include "EOicubCanProto_specifications.h"
#include "EOappMeasuresConverter.h"
#include "EOappMeasuresConverter_hid.h"


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
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
    eOmc_joint_config_t             *cfg = (eOmc_joint_config_t*)nv->ram;
    memcpy(cfg, &joint_default_value.config, sizeof(eOmc_joint_config_t));
}

extern void eoprot_fun_INIT_mc_joint_status(const EOnv* nv)
{
    eOmc_joint_status_t             *status = (eOmc_joint_status_t*)nv->ram;
    memcpy(status, &joint_default_value.status, sizeof(eOmc_joint_status_t));
}


//joint-update
extern void eoprot_fun_UPDT_mc_joint_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;    
    eOmc_joint_config_t                     *cfg = (eOmc_joint_config_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_joint_status_t                     *jstatus_ptr = NULL;
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
    
    /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()); 

    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }
    
    //first of all set conversion factor
    res = eo_appMeasConv_SetJntEncoderConversionFactor(appMeasConv_ptr, jxx, (eOappMeasConv_encConversionFactor_t)eo_common_Q17_14_to_float(cfg->encoderconversionfactor));
    if(eores_OK != res)
    {
        return;
    }
    res = eo_appMeasConv_SetJntEncoderConversionOffset(appMeasConv_ptr, jxx, (eOappMeasConv_encConversionOffset_t)eo_common_Q17_14_to_float(cfg->encoderconversionoffset));
    if(eores_OK != res)
    {
        return;
    }



    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    // 1) send pid position 
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_POS_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidposition);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidposition);


    // 2) send torque pid
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidtorque);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->pidtorque);

    // 3) send velocity pid: currently is not send: neither MC4 nor 2foc use pid velocity.

    // 4) set limits
    minpos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, cfg->limitsofjoint.min);
    maxpos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, cfg->limitsofjoint.max);
    
    if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
    {
        //swap min and max
        pos_icubCanProtValue = minpos_icubCanProtValue;
        minpos_icubCanProtValue = maxpos_icubCanProtValue;
        maxpos_icubCanProtValue = pos_icubCanProtValue;
    }
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&minpos_icubCanProtValue);
 
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&maxpos_icubCanProtValue);

    // 5) set vel timeout
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&cfg->velocitysetpointtimeout);

    // 6) set impedance
    impedence_icubCanProtValues.stiffness = eo_appMeasConv_impedenceStiffness_I2S(appMeasConv_ptr, jxx, cfg->impedance.stiffness);
    impedence_icubCanProtValues.damping = eo_appMeasConv_impedenceDamping_I2S(appMeasConv_ptr, jxx, cfg->impedance.damping);
    impedence_icubCanProtValues.offset = eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, cfg->impedance.offset);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues.offset);
    
    
    // 7) set monitormode status
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
    eOresult_t                              res;
    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    EOappTheDB *db = eo_appTheDB_GetHandle();
    res = eo_appTheDB_GetJointCanLocation(db, jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }
    
    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    if(eo_ropcode_set == rd->ropcode)
    {
        // send pid position 
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_POS_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);
        
        return;
    }
    
#ifdef USE_PROTO_PROXY
    if(eo_ropcode_ask == rd->ropcode)
    {
        eOappTheDB_hid_ethProtoRequest_t req = 
        {
            .id32 = rd->id32,
            .nvRam_ptr = nv->ram,
            .numOfExpectedResp = 2,
            .numOfREceivedResp = 0
        };
        
        eOresult_t res = eo_appTheDB_appendEthProtoRequest(db, eoprot_entity_mc_joint, jxx, &req);
        if(eores_OK != res)
        {
            #warning aggiungi controllo in diagnostica!!!
        }
        // send pid position 
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_POS_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
    }
#endif
}

extern void eoprot_fun_UPDT_mc_joint_config_pidtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    eOmc_PID_t                              *pid_ptr = (eOmc_PID_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    EOappTheDB *db = eo_appTheDB_GetHandle();
    res = eo_appTheDB_GetJointCanLocation(db, jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);

    
    if(eo_ropcode_set == rd->ropcode)
    {
        // send pid torque
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)pid_ptr);
        return;
    }
    
    
#ifdef USE_PROTO_PROXY
    if(eo_ropcode_ask == rd->ropcode)
    {
        eOappTheDB_hid_ethProtoRequest_t req = 
        {
            .id32 = rd->id32,
            .nvRam_ptr = nv->ram,
            .numOfExpectedResp = 2,
            .numOfREceivedResp = 0
        };
        
        eOresult_t res = eo_appTheDB_appendEthProtoRequest(db, eoprot_entity_mc_joint, jxx, &req);
        if(eores_OK != res)
        {
            #warning aggiungi controllo in diagnostica!!!
        }
        // send pid position 
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
    }
#endif
}




extern void eoprot_fun_UPDT_mc_joint_config_impedance(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eObrd_types_t                           boardType;
    icubCanProto_impedance_t                impedence_icubCanProtValues;
    eOmc_impedance_t                        *impedance_ptr = (eOmc_impedance_t*)nv->ram;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    EOappTheDB *db = eo_appTheDB_GetHandle();
    /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());


    res = eo_appTheDB_GetJointCanLocation(db, jxx,  &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    
    if(eo_ropcode_set == rd->ropcode)
    {
        impedence_icubCanProtValues.stiffness = eo_appMeasConv_impedenceStiffness_I2S(appMeasConv_ptr, jxx, impedance_ptr->stiffness);
        impedence_icubCanProtValues.damping = eo_appMeasConv_impedenceDamping_I2S(appMeasConv_ptr, jxx, impedance_ptr->damping);
        impedence_icubCanProtValues.offset = eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, impedance_ptr->offset);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&impedence_icubCanProtValues.offset);
        return;
    }
    
#ifdef USE_PROTO_PROXY
    if(eo_ropcode_ask == rd->ropcode)
    {
        eOappTheDB_hid_ethProtoRequest_t req = 
        {
            .id32 = rd->id32,
            .nvRam_ptr = nv->ram,
            .numOfExpectedResp = 2,
            .numOfREceivedResp = 0
        };
        
        eOresult_t res = eo_appTheDB_appendEthProtoRequest(db, eoprot_entity_mc_joint, jxx, &req);
        if(eores_OK != res)
        {
            #warning aggiungi controllo in diagnostica!!!
        }
        // send pid position 
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
        return;
    }
#endif
}

extern void eoprot_fun_UPDT_mc_joint_config_limitsofjoint(const EOnv* nv, const eOropdescriptor_t* rd)
{

    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOresult_t                              res;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOicubCanProto_msgDestination_t         msgdest;
    icubCanProto_position_t                 pos_icubCanProtValue, minpos_icubCanProtValue, maxpos_icubCanProtValue;
    EOappMeasConv                           *appMeasConv_ptr;
    eOmeas_position_limits_t                *limits_ptr = nv->ram;
    
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    EOappTheDB *db = eo_appTheDB_GetHandle();
    
    

    res = eo_appTheDB_GetJointCanLocation(db, jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }
    
    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
   
    
    
    
    if(eo_ropcode_set == rd->ropcode)
    {
        /*Since icub can proto uses encoder tack like position unit, i need of the converter: from icub to encoder*/
        appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()); 
        
        minpos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, limits_ptr->min);
        maxpos_icubCanProtValue = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, limits_ptr->max);
    
        if(maxpos_icubCanProtValue < minpos_icubCanProtValue)
        {
            //swap min and max
            pos_icubCanProtValue = minpos_icubCanProtValue;
            minpos_icubCanProtValue = maxpos_icubCanProtValue;
            maxpos_icubCanProtValue = pos_icubCanProtValue;
        }
        
        
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&minpos_icubCanProtValue);
     
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&maxpos_icubCanProtValue);
        return;
    }

#ifdef USE_PROTO_PROXY
    if(eo_ropcode_ask == rd->ropcode)
    {
        eOappTheDB_hid_ethProtoRequest_t req = 
        {
            .id32 = rd->id32,
            .nvRam_ptr = nv->ram,
            .numOfExpectedResp = 2,
            .numOfREceivedResp = 0
        };
        
        eOresult_t res = eo_appTheDB_appendEthProtoRequest(db, eoprot_entity_mc_joint, jxx, &req);
        if(eores_OK != res)
        {
            #warning aggiungi controllo in diagnostica!!!
        }

        msgCmd.cmdId =     ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
        return;
    }
#endif
}


extern void eoprot_fun_UPDT_mc_joint_config_velocitysetpointtimeout(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_time_t                          *time_ptr = (eOmeas_time_t*)nv->ram;
    eOmc_jointId_t                         jxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, (void*)time_ptr);  
}


extern void eoprot_fun_UPDT_mc_joint_config_motionmonitormode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t              res;
    eOmc_jointId_t          jxx = eoprot_ID2index(rd->id32);
    eOmc_joint_status_t     *jstatus_ptr;
    
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

extern void eoprot_fun_UPDT_mc_joint_config_encoderconversionfactor(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;    
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOutil_emulfloat32_t                    *encfactor = (eOutil_emulfloat32_t*)nv->ram;


    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()); 


    res = eo_appMeasConv_SetJntEncoderConversionFactor(appMeasConv_ptr, jxx, (eOappMeasConv_encConversionFactor_t)eo_common_Q17_14_to_float(*encfactor));
    if(eores_OK != res)
    {
        return; //error 
    }
}

extern void eoprot_fun_UPDT_mc_joint_config_encoderconversionoffset(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;    
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOutil_emulfloat32_t                    *encoffset = (eOutil_emulfloat32_t*)nv->ram;


    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()); 

    res = eo_appMeasConv_SetJntEncoderConversionOffset(appMeasConv_ptr, jxx, (eOappMeasConv_encConversionOffset_t)eo_common_Q17_14_to_float(*encoffset));
    if(eores_OK != res)
    {
        return; //error 
    }

}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_setpoint(const EOnv* nv, const eOropdescriptor_t* rd)
{
 //   char str[96];
    
    eOresult_t                              res;
    eOmc_setpoint_t                         *setPoint = (eOmc_setpoint_t*)nv->ram;
    void                                    *val_ptr = NULL;
    eOmc_joint_status_t                     *jstatus_ptr;
    eOmc_joint_config_t                     *jconfig_ptr;
    icubCanProto_position_t                 pos;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    EOappTheDB                              *db = eo_appTheDB_GetHandle();

    eOicubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    /*Since icub can proto uses encoder tacks like position unit, i need of the converter: from icub to encoder*/
    EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());

    // 1) set monitor status = notreachedyet if monitormode is forever
    res = eo_appTheDB_GetJointConfigPtr(db, (eOmc_jointId_t)jxx,  &jconfig_ptr);
    if(eores_OK != res)
    {
        return; //i should never be here
    }
    
    if(eo_ropcode_set == rd->ropcode)
    {
        
        if(eomc_motionmonitormode_forever == jconfig_ptr->motionmonitormode)
        {
            res = eo_appTheDB_GetJointStatusPtr(db, (eOmc_jointId_t)jxx,  &jstatus_ptr);
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
        
        // 2) prepare setpoint to send
        switch(setPoint->type)
        {
            case eomc_setpoint_position:
            {
                icubCanProto_setpoint_position_t  setpoint_pos;
                
                setpoint_pos.value = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, setPoint->to.position.value);
                //reference velocity of position set point must be always >0, so here absolute func is used.
                setpoint_pos.withvelocity = eo_appMeasConv_jntVelocity_I2E_abs(appMeasConv_ptr,jxx, setPoint->to.position.withvelocity);
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE; 
                val_ptr =  &setpoint_pos; 
            }break;

            case eomc_setpoint_velocity:
            {
                icubCanProto_setpoint_velocity_t  setpoint_vel;
                
                setpoint_vel.withacceleration = eo_appMeasConv_jntAcceleration_I2E_abs(appMeasConv_ptr, jxx, setPoint->to.velocity.withacceleration);           
                setpoint_vel.value = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, setPoint->to.velocity.value);
                
                if (setpoint_vel.withacceleration < 1)
                {
                    setpoint_vel.withacceleration = 1;
                }            
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE;                 
                val_ptr =  &setpoint_vel;   
                
            }break;

            case eomc_setpoint_torque:
            {
                icubCanProto_setpoint_torque_t setpoint_torque;
                
                setpoint_torque.value = eo_appMeasConv_torque_I2S(appMeasConv_ptr,jxx, setPoint->to.torque.value);
                
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE;           
                val_ptr =  &setpoint_torque;    
            }break;

            case eomc_setpoint_current:
            {
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT;                             
                val_ptr =  &(setPoint->to.current.value);    
            }break;

            case eomc_setpoint_positionraw:
            {    
                pos = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, setPoint->to.position.value);
                msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION; 
                val_ptr =  &pos; 
            }break;
            
            default:
            {
                return;
            }
        }

        // 3) send msg
        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, val_ptr);
        return;
    }

#ifdef USE_PROTO_PROXY
    if(eo_ropcode_ask == rd->ropcode)
    {
        if(setPoint->type != eomc_setpoint_torque)
        {   
            return;
        }
        
        
        eOappTheDB_hid_ethProtoRequest_t req = 
        {
            .id32 = rd->id32,
            .nvRam_ptr = nv->ram,
            .numOfExpectedResp = 1,
            .numOfREceivedResp = 0
        };
        
        eOresult_t res = eo_appTheDB_appendEthProtoRequest(db, eoprot_entity_mc_joint, jxx, &req);
        if(eores_OK != res)
        {
            #warning aggiungi controllo in diagnostica!!!
        }
        msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE;
        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, NULL);
        return;
    }
#endif

}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_stoptrajectory(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eObool_t                                *cmd = (eObool_t*)nv->ram;
    eOmc_jointId_t                         jxx = eoprot_ID2index(rd->id32);
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

    if(1 == *cmd)
    {
        eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, NULL);
    }
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_calibration(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)nv->ram;
    icubCanProto_calibrator_t               iCubCanProtCalibrator;
    eOmc_joint_status_t                     *jstatus_ptr;
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

    // 1) set control mode in status nv (a mirror nv)
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx, &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }
    //jstatus_ptr->basic.controlmodestatus = eomc_controlmode_calib;

    
    // 2) send control mode value to mc4
    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    
 
    //prepare calibration data to send
    /*Note: currently calibration params are raw, so they doesn't need to be converted in icub can proto measures;
    but i prepare code for future: when calibration param used new measure unit.*/
    iCubCanProtCalibrator.type = (icubCanProto_calibration_type_t)calibrator->type;
    switch(calibrator->type)
    {
        case eomc_calibration_type0_hard_stops:
        {
            iCubCanProtCalibrator.params.type0.pwmlimit = calibrator->params.type0.pwmlimit;
//            iCubCanProtCalibrator.params.type0.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type0.velocity);           
            iCubCanProtCalibrator.params.type0.velocity = calibrator->params.type0.velocity;
        }break;
            
        case eomc_calibration_type1_abs_sens_analog:
        {
//             icubCanProto_position_t pos = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, calibrator->params.type1.position);
//             /*sice pos param is a word of 16 bits i must check min and max*/
//             if((pos < INT16_MIN)||(pos>INT16_MAX))
//             {
//                 return;
//             }
//             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
//             iCubCanProtCalibrator.params.type1.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type1.velocity);
             iCubCanProtCalibrator.params.type1.position = calibrator->params.type1.position; 
             iCubCanProtCalibrator.params.type1.velocity =  calibrator->params.type1.velocity;
        }break;

        case eomc_calibration_type2_hard_stops_diff:
        {
            iCubCanProtCalibrator.params.type2.pwmlimit = calibrator->params.type2.pwmlimit;
//            iCubCanProtCalibrator.params.type2.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type2.velocity);           
            iCubCanProtCalibrator.params.type2.velocity = calibrator->params.type2.velocity;           
        }break;

        case eomc_calibration_type3_abs_sens_digital:
        {
//              icubCanProto_position_t pos = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, calibrator->params.type3.position);
//             /*sice pos param is a word of 16 bits i must check min and max*/
//             if((pos < INT16_MIN)||(pos>INT16_MAX))
//             {
//                 return;
//             }
//             iCubCanProtCalibrator.params.type1.position = (icubCanProto_position4calib_t)pos; 
//             iCubCanProtCalibrator.params.type3.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type3.velocity);           
            iCubCanProtCalibrator.params.type1.position = calibrator->params.type3.position; 
            iCubCanProtCalibrator.params.type3.velocity = calibrator->params.type3.velocity;            
            iCubCanProtCalibrator.params.type3.offset = calibrator->params.type3.offset;
        }break;

        case eomc_calibration_type4_abs_and_incremental:
        {
//             icubCanProto_position_t pos = eo_appMeasConv_jntPosition_I2E(appMeasConv_ptr, jxx, calibrator->params.type4.position);
//             //here position is in int16_t ==> so i must verify if pos is out of int16_t range
//             if((pos > INT16_MAX) || (pos < INT16_MIN))
//             {
//                 return;
//                 #warning VALE --> how to manage this error???
//             }
//             iCubCanProtCalibrator.params.type4.position = (icubCanProto_position4calib_t)pos; 
//            iCubCanProtCalibrator.params.type4.velocity = eo_appMeasConv_jntVelocity_I2E(appMeasConv_ptr, jxx, calibrator->params.type4.velocity);            
            iCubCanProtCalibrator.params.type4.position = calibrator->params.type4.position;
            iCubCanProtCalibrator.params.type4.velocity = calibrator->params.type4.velocity;
            iCubCanProtCalibrator.params.type4.maxencoder = calibrator->params.type4.maxencoder;
        }break;
        
    }

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &iCubCanProtCalibrator);
 
}


extern void eoprot_fun_UPDT_mc_joint_cmmnds_controlmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOresult_t                              res;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_joint_status_t                     *jstatus_ptr;
    eOmc_controlmode_command_t              *controlmode_cmd_ptr = (eOmc_controlmode_command_t*)nv->ram;
    icubCanProto_controlmode_t              icubcanProto_controlmode;
    eOicubCanProto_msgDestination_t         msgdest;
    EOappTheDB                              *db = eo_appTheDB_GetHandle();

    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());


    // 1) ge control mode in status nv
    res = eo_appTheDB_GetJointStatusPtr(db, (eOmc_jointId_t)jxx, &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }

    // 2) get joint can location
    res = eo_appTheDB_GetJointCanLocation(db, jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }
    
    // 4) set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);

    
//     #include "hal.h"
//     char str[60];
//     
//     snprintf(str, sizeof(str), "received control mode =%d joint %d", *controlmode_cmd_ptr, jxx);
//     hal_trace_puts(str);
    
//i commented this code because with new control modes, i don't need to send ena or disa pwm.    
//     if( (eomc_controlmode_cmd_switch_everything_off == *controlmode_cmd_ptr) ||
//         (eomc_controlmode_cmd_force_idle == *controlmode_cmd_ptr)            ||
//         (eomc_controlmode_cmd_idle == *controlmode_cmd_ptr) )
//         {
//             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
//             eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

//             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
//             eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
//             
//             snprintf(str, sizeof(str), "in controlmode: snd disa pwm e idle joint %d", jxx);
//             hal_trace_puts(str);
//             return;
//         }
//     
//     if((eomc_controlmode_idle == jstatus_ptr->basic.controlmodestatus) &&
//         (eomc_controlmode_cmd_force_idle != *controlmode_cmd_ptr)     &&
//         (eomc_controlmode_cmd_idle != *controlmode_cmd_ptr)           &&
//         (eomc_controlmode_cmd_switch_everything_off != *controlmode_cmd_ptr))
//     {
//         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
//         eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
//     
//         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
//         eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
//          snprintf(str, sizeof(str), "in controlmode: snd ena pwm e run joint %d", jxx);
//             hal_trace_puts(str);
//     }
    
    
        
        
    res = s_translate_eOmcControlMode2icubCanProtoControlMode(*controlmode_cmd_ptr, jxx, &icubcanProto_controlmode);
    if(eores_OK != res)
    {
        return;
    }
    
//     snprintf(str, sizeof(str), "in controlmode: snd icubcontrolmode =%d joint %d", icubcanProto_controlmode, jxx);
//     hal_trace_puts(str);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &icubcanProto_controlmode);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
//     if(eomc_controlmode_idle == jstatus_ptr->basic.controlmodestatus)
//     {
//         if(eomc_controlmode_cmd_position != *controlmode_cmd_ptr)
//         {
//             //i can exit from idle only if i received set control mode position command
//             return;
//         }
//         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
//         eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
//     
//         msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
//         eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
//     }
//     else
//     {
//         if(eomc_controlmode_cmd_switch_everything_off == *controlmode_cmd_ptr)
//         {
//             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
//             eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

//             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
//             eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
//         }
//         else
//         {
//             res = s_translate_eOmcControlMode2icubCanProtoControlMode(*controlmode_cmd_ptr, &icubcanProto_controlmode);
//             if(eores_OK != res)
//             {
//                 return;
//             }
//             msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
//             eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &icubcanProto_controlmode);
//         }
//     }

}

extern void eoprot_fun_UPDT_mc_joint_cmmnds_interactionmode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmc_interactionmode_t                  *interaction = (eOmc_interactionmode_t*)rd->data;
    eOmc_jointId_t                          jxx = eoprot_ID2index(rd->id32);
    eOresult_t                              res;
    eOmc_joint_status_t                     *jstatus_ptr;
    EOappCanSP                              *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    icubCanProto_interactionmode_t          icub_interctmode;
    EOappTheDB                              *db = eo_appTheDB_GetHandle();
    
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx, &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }
    
    
    eOicubCanProto_msgCommand_t             msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE
    };

    

    if(eOmc_interactionmode_stiff == *interaction)
    {
        icub_interctmode = icubCanProto_interactionmode_stiff;
    }
    else if(eOmc_interactionmode_compliant == *interaction)
    {
        icub_interctmode = icubCanProto_interactionmode_compliant;
    }
    else
    {
        //icub_interctmode = icubCanProto_interactionmode_unknownError;
        return;
    }

    eo_appCanSP_SendCmd2Joint(appCanSP_ptr, (eOmc_jointId_t)jxx, msgCmd, &icub_interctmode);

}


extern void eoprot_fun_UPDT_mc_joint_inputs_externallymeasuredtorque(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmeas_torque_t *torquevalue_ptr = (eOmeas_torque_t*)nv->ram;
    eOmc_jointId_t  jxx = eoprot_ID2index(rd->id32);
    icubCanProto_torque_t icub_torque;
    
//     EOappMeasConv* appMeasConv_ptr = eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle());
//     icubCanProto_torque_t icub_torque =  eo_appMeasConv_torque_I2S(appMeasConv_ptr, jxx, *torquevalue_ptr);

    icub_torque = *torquevalue_ptr +0x8000;
    eo_appTheDB_SetVirtualStrainValue(eo_appTheDB_GetHandle(), jxx, icub_torque);
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
        }break;
        case eomc_controlmode_cmd_velocity:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_velocity;
        }break;
        case eomc_controlmode_cmd_torque:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_torque;
        }break;
        case eomc_controlmode_cmd_impedance_pos:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_impedance_pos;
        }break;
        case eomc_controlmode_cmd_impedance_vel:
        {
             *icubcanProto_controlmode = icubCanProto_controlmode_impedance_vel;
        }break;
        case eomc_controlmode_cmd_current:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_current;
        }break;
        case eomc_controlmode_cmd_openloop:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_openloop;
        }break;
        case eomc_controlmode_cmd_switch_everything_off:
        {
            return(eores_NOK_generic);
        }
        case eomc_controlmode_cmd_force_idle:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_forceIdle;
        }break;
        case eomc_controlmode_cmd_mixed:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_mixed;
        }break;
        case eomc_controlmode_cmd_direct:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_direct;
        }break;
        case eomc_controlmode_cmd_idle:
        {
            *icubcanProto_controlmode = icubCanProto_controlmode_idle;
        }break;
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



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



