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
#include "EoCommon.h"

// in here is whatever is required to offer parsing of can frames and copying into protocol data structures.

#include "EOtheCANprotocol.h" 

#include "EoProtocol.h"
#include "EoProtocolMC.h"

// but also to retrieve information of other things ...

#include "iCubCanProtocol.h"

#include "EOproxy.h"
#include "EOtheBOARDtransceiver.h"
#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANmapping.h"

#include "EOtheCANdiscovery2.h"

#include "EOtheMC4boards.h"

#include "EOMtheEMSappl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANprotocol_functions.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void* s_eocanprotMCpolling_get_entity(eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index);


// parser helper funtions

//static eOresult_t s_parser_POL_MC_CMD__MOTION_DONE(eOcanframe_t *frame, eOcanport_t port);

// former helper funtions

static void s_former_POL_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);

static void s_former_POL_MC_CMD_setpid(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type);
static void s_former_POL_MC_CMD_setpid_limits(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type);

static void s_former_POL_MC_CMD_setpid_7(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type);
static void s_former_POL_MC_CMD_setpid_limits_7(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type);

static eOresult_t s_parser_POL_MC_CMD_getposition(eOcanframe_t *frame, eOcanport_t port, uint8_t type);
static eOresult_t s_parser_POL_MC_CMD_getpid_etc(eOcanframe_t *frame, eOcanport_t port, uint8_t type);
static eOresult_t s_parser_POL_MC_CMD_getimpedance(eOcanframe_t *frame, eOcanport_t port, uint8_t type);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_RUN(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN);
    return(eores_OK);      
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_IDLE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE);
    return(eores_OK); 
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__CALIBRATE_ENCODER(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_calibrator_t* calib = (icubCanProto_calibrator_t*)descriptor->cmd.value;
   
    s_former_POL_MC_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER);

    // now i prepare data[1] to data[7]
    
    frame->data[1] = calib->type;
    
    switch(calib->type)
    {
        case icubCanProto_calibration_type0_hard_stops:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type0.pwmlimit;
            *((uint16_t*)(&frame->data[4])) = calib->params.type0.velocity;
            memset(&frame->data[6], 0, 2); // pad with 0          
        } break;

        case icubCanProto_calibration_type1_abs_sens_analog:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type1.position;
            *((uint16_t*)(&frame->data[4])) = calib->params.type1.velocity;
            memset(&frame->data[6], 0, 2); // pad with 0          
        } break;

        case icubCanProto_calibration_type2_hard_stops_diff:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type2.pwmlimit;
            *((uint16_t*)(&frame->data[4])) = calib->params.type2.velocity;
            memset(&frame->data[6], 0, 2); // pad with 0          
        } break;


        case icubCanProto_calibration_type3_abs_sens_digital:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type3.position;
            *((uint16_t*)(&frame->data[4])) = calib->params.type3.velocity;
            *((uint16_t*)(&frame->data[6])) = calib->params.type3.offset;        
        } break;

        case icubCanProto_calibration_type4_abs_and_incremental:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type4.position;
            *((uint16_t*)(&frame->data[4])) = calib->params.type4.velocity;
            *((uint16_t*)(&frame->data[6])) = calib->params.type4.maxencoder;        
        } break;
        
        case icubCanProto_calibration_type6_mais:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type6.position;
            *((uint16_t*)(&frame->data[4])) = calib->params.type6.velocity;
            memset(&frame->data[6], 0, 2); // pad with 0        
        } break;
        
        case icubCanProto_calibration_type7_hall_sensor:
        {
            *((uint16_t*)(&frame->data[2])) = calib->params.type7.position;
            *((uint16_t*)(&frame->data[4])) = calib->params.type7.velocity;
            memset(&frame->data[6], 0, 2); // pad with 0       
        } break;
        
        default:
        {
            //#warning --> TODO error about unknown param ...
        } break;
    }
       
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__ENABLE_PWM_PAD(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD);
    return(eores_OK);   
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__DISABLE_PWM_PAD(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD);
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_CONTROL_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE);
    return(eores_OK);         
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_CONTROL_MODE(eOcanframe_t *frame, eOcanport_t port)
{
    //#warning -> i am not sure it is still in use, thus i just put an error return
    return(eores_NOK_generic);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__MOTION_DONE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__MOTION_DONE);
    return(eores_OK);     
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__MOTION_DONE(eOcanframe_t *frame, eOcanport_t port)
{
    eOmc_joint_t *joint = NULL;
    eOprotIndex_t index = EOK_uint08dummy; 
    
    // retrieve the joint related to the frame    
    if(NULL == (joint = s_eocanprotMCpolling_get_entity(eoprot_entity_mc_joint, frame, port, &index)))
    {
        return(eores_OK);        
    }   
    
    // in byte data[1] there is: 0/1 
    joint->status.core.modes.ismotiondone = (eObool_t)frame->data[1];    
   
    // and now let's manage the proxy
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, index, eoprot_tag_mc_joint_status_core_modes_ismotiondone);
    
    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    }  
    
    param->p08_2 ++;
    
    if(param->p08_1 == param->p08_2)
    {
        eOresult_t res = eo_proxy_ReplyROP_Load(proxy, id32, NULL);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());       
    }        
    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_controlmode_t *controlmode = (icubCanProto_controlmode_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 2, ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE);
    frame->data[1] = *controlmode;
    
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_estimShift_t *shift = (icubCanProto_estimShift_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT);
    // now i prepare data[1] -> data[4]
    frame->data[1]  = shift->estimShiftJointVel;
    frame->data[2]  = shift->estimShiftJointAcc;
    frame->data[3]  = shift->estimShiftMotorVel;
    frame->data[4]  = shift->estimShiftMotorAcc;    
    
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__POSITION_MOVE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_setpoint_position_t *setpointp = (icubCanProto_setpoint_position_t *)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 7, ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE);
    // now i prepare data[1] -> data[6]
    *((icubCanProto_position_t*)(&frame->data[1])) = setpointp->value;
    *((icubCanProto_velocity_t*)(&frame->data[5])) = setpointp->withvelocity;  
    
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__VELOCITY_MOVE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_setpoint_velocity_t *setpointv = (icubCanProto_setpoint_velocity_t *)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE);
    // now i prepare data[1] -> data[4]
    *((icubCanProto_velocity_t*)(&frame->data[1])) = setpointv->value;
    *((icubCanProto_acceleration_t*)(&frame->data[3])) = setpointv->withacceleration;  
    
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_TORQUE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_setpoint_torque_t *setpointt = (icubCanProto_setpoint_torque_t *)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE);
    // now i prepare data[1] -> data[4]    
    // marco.accame: value is a int16_t but i have found it copied byte by byte, so i keep it in this way
    frame->data[1] = ((uint8_t*)&(setpointt->value))[0];
    frame->data[2] = ((uint8_t*)&(setpointt->value))[1];
    frame->data[4] = frame->data[3] = (frame->data[2] & 0x80) ? 0xFF : 0;
    
    return(eores_OK);         
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_DESIRED_TORQUE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE);    
    return(eores_OK);             
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__STOP_TRAJECTORY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY);
    return(eores_OK);        
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_COMMAND_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION);
    // now i prepare data[1] -> data[4]    
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->cmd.value);
    
    return(eores_OK);    
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION);    
    // now i prepare data[1] -> data[4]   
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->cmd.value);    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION);
    return(eores_OK);  
}


extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port)
{       
    return(s_parser_POL_MC_CMD_getposition(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION));
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION);    
    // now i prepare data[1] -> data[4]   
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->cmd.value);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION);
    return(eores_OK);  
}






extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_PWM_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_PWM_LIMIT);      
    *((icubCanProto_pwm_t*)(&frame->data[1])) = *((icubCanProto_pwm_t*)descriptor->cmd.value);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_PWM_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_PWM_LIMIT);
    return(eores_OK);  
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_PWM_LIMIT(eOcanframe_t *frame, eOcanport_t port)
{  
    eOprotIndex_t index = EOK_uint08dummy;  

    eOcanmap_location_t loc = {0};
    loc.port            = port;
    loc.addr            = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex     = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    index = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
    if(EOK_uint08dummy == index)
    {
        //s_eo_icubCanProto_mb_send_runtime_error_diagnostics(6);
        return(eores_OK);
    }
    
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, index, eoprot_tag_mc_motor_config_pwmlimit);
   

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmeas_pwm_t *pwmLimit = (eOmeas_pwm_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);    
    
    *pwmLimit = *((eOmeas_pwm_t*)(&frame->data[1]));

    param->p08_2 ++;
    
    if(param->p08_1 == param->p08_2)
    {
        eOresult_t res = eo_proxy_ReplyROP_Load(proxy, id32, NULL);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
          
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_MOTOR_POS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_MAX_MOTOR_POS);    
    // now i prepare data[1] -> data[4]   
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->cmd.value);
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_MOTOR_POS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_MIN_MOTOR_POS);    
    // now i prepare data[1] -> data[4]   
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->cmd.value);
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_MAX_POSITION(eOcanframe_t *frame, eOcanport_t port)
{  
    return(s_parser_POL_MC_CMD_getposition(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION));    
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_VELOCITY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY);    
    // now i prepare data[1] -> data[2]   
    *((icubCanProto_velocity_t*)(&frame->data[1])) = *((icubCanProto_velocity_t*)descriptor->cmd.value);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    // NOTE: eo-protocol uses 16 bits for current (sign+value), while icubcanprotocol uses 32bits.
    // in here we should not use negative currents, nevertheless we convert the value correctly
    eOmeas_current_t *eomeasCurrent = (eOmeas_current_t*)descriptor->cmd.value;
    int32_t maxcurrent = *eomeasCurrent;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT);    
    // now i prepare data[1] -> data[4]   
    *((int32_t*)(&frame->data[1])) = maxcurrent;    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_BCAST_POLICY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_BCAST_POLICY);    
    // now i prepare data[1] -> data[4]   
    memcpy(&frame->data[1], descriptor->cmd.value, 4);
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_SHIFT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_velocityShift_t shift = *((icubCanProto_velocityShift_t*)descriptor->cmd.value);
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT);    
    // now i prepare data[1] -> data[2]  
    *((icubCanProto_velocityShift_t*)(&frame->data[1])) = shift;    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_CMD_setpid(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PID);
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PID(eOcanframe_t *frame, eOcanport_t port)
{
    return(s_parser_POL_MC_CMD_getpid_etc(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID));
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_TORQUE_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_CMD_setpid_limits(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_TORQUE_PIDLIMITS);
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_TORQUE_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_TORQUE_PIDLIMITS(eOcanframe_t *frame, eOcanport_t port)
{
    return(s_parser_POL_MC_CMD_getpid_etc(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS));
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_CMD_setpid(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_POS_PID);
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_POS_PID);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PID(eOcanframe_t *frame, eOcanport_t port)
{
    return(s_parser_POL_MC_CMD_getpid_etc(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_POS_PID));
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_POS_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_CMD_setpid_limits(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_POS_PIDLIMITS);
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_POS_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_POS_PIDLIMITS(eOcanframe_t *frame, eOcanport_t port)
{
    return(s_parser_POL_MC_CMD_getpid_etc(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS));
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_TIMEOUT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT);  
    // now i prepare data[1] -> data[2]  
    *((uint16_t*)(&frame->data[1])) = *((uint16_t*)descriptor->cmd.value);    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_impedance_t *imp = (icubCanProto_impedance_t *)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_PARAMS);  
    // now i prepare data[1] -> data[7]  
    // stiffnes and damping occupy two bytes
    *((icubCanProto_stiffness_t*)(&frame->data[1])) = (icubCanProto_stiffness_t)imp->stiffness;
    *((icubCanProto_damping_t*)(&frame->data[3])) = (icubCanProto_damping_t)imp->damping; 
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS);    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_PARAMS(eOcanframe_t *frame, eOcanport_t port)
{
    return(s_parser_POL_MC_CMD_getimpedance(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS));
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_OFFSET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_torque_t *offset = (icubCanProto_torque_t *)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_IMPEDANCE_OFFSET);  
    // now i prepare data[1] -> data[2]  
    *((icubCanProto_torque_t*)(&frame->data[1])) = *((icubCanProto_torque_t*)(offset)); 
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_IMPEDANCE_OFFSET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET);    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_IMPEDANCE_OFFSET(eOcanframe_t *frame, eOcanport_t port)
{
    return(s_parser_POL_MC_CMD_getimpedance(frame, port, ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET));
}




extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_FIRMWARE_VERSION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eObrd_protocolversion_t *reqprot = (eObrd_protocolversion_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION);
    
    frame->data[1] = reqprot->major;
    frame->data[2] = reqprot->minor;
    
    return(eores_OK);      
}




extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_FIRMWARE_VERSION(eOcanframe_t *frame, eOcanport_t port)
{
    // must: retrieve the board given the location, assign the rx values, verify if the fw version is ok 
    // then: (but it could be put somewhere else) ... if all boards are ready: stop, send config, start mais
    // i would like to do it inside a method of ...
    
    eOcanmap_location_t loc = {0};
    loc.port                = port;
    loc.addr                = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex         = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    eObool_t match = (1 == frame->data[7]) ? eobool_true : eobool_false;    
    
    eObrd_info_t detected = {0};
    detected.type               = frame->data[1];
    detected.firmware.major     = frame->data[2];
    detected.firmware.minor     = frame->data[3];
    detected.firmware.build     = frame->data[4];
    detected.protocol.major     = frame->data[5];
    detected.protocol.minor     = frame->data[6];   
    
    
    eo_candiscovery2_OneBoardIsFound(eo_candiscovery2_GetHandle(), loc, match, &detected);

          
    return(eores_OK);
}



extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID);   
    memcpy(frame->data+1,descriptor->cmd.value,7);
    return(eores_OK);      
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    //#warning marco.accame CHECK: see if ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS has len 7 or 8 as some other SET_xxx_PIDLIMITS 
    s_former_POL_MC_CMD_setpid_limits_7(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PIDLIMITS);
    return(eores_OK);      
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PID(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    // ... apparently s_former_POL_MC_CMD_setpid uses len 8 and not len 7 and has pid->scale
    //#warning marco.accame CHECK: see if ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID has len 7 or 8 as some other SET_xxx_PID 
    s_former_POL_MC_CMD_setpid_7(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID);
    return(eores_OK);      
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VELOCITY_PIDLIMITS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    // ... apparently s_former_POL_MC_CMD_setpid_limits uses len 8 and not len 7 
    //#warning marco.accame CHECK: see if ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS has len 7 or 8 as some other SET_xxx_PIDLIMITS 
    s_former_POL_MC_CMD_setpid_limits_7(descriptor, frame, ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PIDLIMITS);
    return(eores_OK);      
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_CURRENT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eOmeas_current_t *current = (eOmeas_current_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_CURRENT);
   
    *((eOmeas_current_t*)(&frame->data[1])) = *current;
    frame->data[3] = 0; // LSB Id
    frame->data[4] = 0; // MSB Id
    return(eores_OK);       
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_I2T_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eOmc_i2tParams_t *i2tparams = (eOmc_i2tParams_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS);   
    *((uint16_t*)(&frame->data[1])) = i2tparams->time;
    *((uint16_t*)(&frame->data[3])) = i2tparams->tresh;
    return(eores_OK);     
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_OPENLOOP_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_setpoint_current_t *setpoint = (icubCanProto_setpoint_current_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_OPENLOOP_PARAMS);   
    *((int16_t*)(&frame->data[1])) = setpoint->value;
    return(eores_OK);    
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_OPENLOOP_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_OPENLOOP_PARAMS);   
    return(eores_OK);        
}


extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_OPENLOOP_PARAMS(eOcanframe_t *frame, eOcanport_t port)
{    
    eOmc_joint_t *joint = NULL;
    
    if(NULL == (joint = s_eocanprotMCpolling_get_entity(eoprot_entity_mc_joint, frame, port, NULL)))
    {
        return(eores_OK);        
    }
  
    //it is: joint->status.ofpid.openloop.refolo = *((int16_t*)&frame->data[1]);  
    joint->status.target.trgt_openloop = *((int16_t*)&frame->data[1]);
  
    return(eores_OK);    
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_INTERACTION_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_interactionmode_t *imode = (icubCanProto_interactionmode_t*)descriptor->cmd.value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_INTERACTION_MODE);   
    *((icubCanProto_interactionmode_t*)(&frame->data[1])) = *imode;
    return(eores_OK);  
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MOTOR_CONFIG(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 7, ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG);   
    memcpy(frame->data+1,descriptor->cmd.value,6);
    return(eores_OK);  
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void* s_eocanprotMCpolling_get_entity(eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eOcanmap_location_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
       
    ii = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, entity);
    
    if(EOK_uint08dummy == ii)
    {     
        //#warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(NULL);
    }
    
    ret = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, entity, ii);
    
    if(NULL != index)
    {
        *index = ii;        
    }  

    return(ret);   
}



// - parser helper funtions




// former helper funtions

static void s_former_POL_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type)
{   // every message coming from the ems has actually id 0
    frame->id           = EOCANPROT_CREATE_CANID(eocanprot_msgclass_pollingMotorControl, 0, descriptor->loc.addr);
    frame->id_type      = eocanframeID_std11bits;
    frame->frame_type   = eocanframetype_data; 
    frame->size         = len;
    frame->data[0]      = EOCANPROT_CREATE_POLLING_MC_DATA0(descriptor->loc.insideindex, type);    
}


static void s_former_POL_MC_CMD_setpid(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type)
{
    eOmc_PID_t *pid = (eOmc_PID_t*) descriptor->cmd.value;    
    s_former_POL_MC_prepare_frame(descriptor, frame, 8, type);    
    // now i prepare data[1] -> data[7]  
    *((int16_t*)(&frame->data[1])) = (int16_t) pid->kp;
    *((int16_t*)(&frame->data[3])) = (int16_t) pid->ki;
    *((int16_t*)(&frame->data[5])) = (int16_t) pid->kd;
    frame->data[7] = (int8_t) pid->scale;   
}

static void s_former_POL_MC_CMD_setpid_limits(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type)
{
    //  Currently in messages.h there is a check on frame_len equal to 8, else frame is discarded 
    //  so i left size = 8 even if correct size is 7 
    eOmc_PID_t *pid = (eOmc_PID_t*) descriptor->cmd.value;    
    s_former_POL_MC_prepare_frame(descriptor, frame, 8, type);   
    // now i prepare data[1] -> data[7]  
    *((int16_t*)(&frame->data[1])) = (int16_t) pid->offset;
    *((int16_t*)(&frame->data[3])) = (int16_t) pid->limitonoutput;
    *((int16_t*)(&frame->data[5])) = (int16_t) pid->limitonintegral;
}


static void s_former_POL_MC_CMD_setpid_7(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type)
{
    eOmc_PID_t *pid = (eOmc_PID_t*) descriptor->cmd.value;    
    s_former_POL_MC_prepare_frame(descriptor, frame, 7, type);    
    // now i prepare data[1] -> data[6]  
    *((int16_t*)(&frame->data[1])) = (int16_t) pid->kp;
    *((int16_t*)(&frame->data[3])) = (int16_t) pid->ki;
    *((int16_t*)(&frame->data[5])) = (int16_t) pid->kd;
}

static void s_former_POL_MC_CMD_setpid_limits_7(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type)
{
    eOmc_PID_t *pid = (eOmc_PID_t*) descriptor->cmd.value;    
    s_former_POL_MC_prepare_frame(descriptor, frame, 7, type);   
    // now i prepare data[1] -> data[6]  
    *((int16_t*)(&frame->data[1])) = (int16_t) pid->offset;
    *((int16_t*)(&frame->data[3])) = (int16_t) pid->limitonoutput;
    *((int16_t*)(&frame->data[5])) = (int16_t) pid->limitonintegral;
}


// cmd can be: ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION, ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION
static eOresult_t s_parser_POL_MC_CMD_getposition(eOcanframe_t *frame, eOcanport_t port, uint8_t type)
{   
    eOprotIndex_t index = EOK_uint08dummy;  

    eOcanmap_location_t loc = {0};
    loc.port            = port;
    loc.addr            = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex     = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    index = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    if(EOK_uint08dummy == index)
    {
        //s_eo_icubCanProto_mb_send_runtime_error_diagnostics(6);
        return(eores_OK);
    }
    
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, index, eoprot_tag_mc_joint_config_limitsofjoint);
   

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmeas_position_limits_t *limits = (eOmeas_position_limits_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);    
    
    icubCanProto_position_t icub_pos =  *((icubCanProto_position_t*)(&frame->data[1]));
    // ok, now i must convert from one to another ...
    eOmeas_position_t pos = eo_mc4boards_Convert_Position_fromCAN(eo_mc4boards_GetHandle(), index, icub_pos);
    
    if(ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION == type)
    {
        limits->min = pos;
    }
    else if(ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION == type)
    {
        limits->max = pos;
    }
    else
    {   // i must have called it badly
        return(eores_NOK_generic);
    }

    param->p08_2 ++;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response               
        if(limits->max < limits->min)
        {   // exchange them
            eOmeas_position_t tmp = limits->min;
            limits->min = limits->max;
            limits->max = tmp;
        }
       
        eOresult_t res = eo_proxy_ReplyROP_Load(proxy, id32, NULL);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
          
    return(eores_OK);
}


// cmd can be: ICUBCANPROTO_POL_MC_CMD__GET_POS_PID, ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS, 
//             ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID, ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS
static eOresult_t s_parser_POL_MC_CMD_getpid_etc(eOcanframe_t *frame, eOcanport_t port, uint8_t type)
{    
    eOprotIndex_t index = EOK_uint08dummy;  

    eOcanmap_location_t loc = {0};
    loc.port            = port;
    loc.addr            = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex     = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    index = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    if(EOK_uint08dummy == index)
    {
        //s_eo_icubCanProto_mb_send_runtime_error_diagnostics(6);
        return(eores_OK);
    }
    
    eOprotTag_t tag = eoprot_tag_none;
    
    switch(type)
    {
        case ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID:
        case ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PIDLIMITS:     tag = eoprot_tag_mc_joint_config_pidtorque;  
        break;

        case ICUBCANPROTO_POL_MC_CMD__GET_POS_PID:
        case ICUBCANPROTO_POL_MC_CMD__GET_POS_PIDLIMITS:        tag = eoprot_tag_mc_joint_config_pidposition;  
        break; 

        default:                                                tag = eoprot_tag_none;
        break;
    }
    
    if(eoprot_tag_none == tag)
    {   // returns NOK because it is something i cannot parse. i must have called this function badly ...
        return(eores_NOK_generic);
    }
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, index, tag);
   

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 
    
    eOmc_PID_t* pid = (eOmc_PID_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);
    
    if((ICUBCANPROTO_POL_MC_CMD__GET_TORQUE_PID == type) || (ICUBCANPROTO_POL_MC_CMD__GET_POS_PID == type))
    {
        pid->kp = *((int16_t*)&frame->data[1]);
        pid->ki = *((int16_t*)&frame->data[3]);
        pid->kd = *((int16_t*)&frame->data[5]); 
    }
    else// no need to verify that type value is a LIMITS i already did it in the switch-case part.
    {
        pid->offset = *((int16_t*)(&frame->data[1]));
        pid->limitonoutput = *((int16_t*)(&frame->data[3]));
        pid->limitonintegral = *((int16_t*)(&frame->data[5]));  
    }    
    

    
    param->p08_2 ++;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        //marco.accame -> can eo_proxy_ReplyROP_Load() use the same memory as the nv? much be better using NULL. think of using memmove.
        eOresult_t res = eo_proxy_ReplyROP_Load(proxy, id32, NULL);  // if NULL it does not copy dat into the nv and uses ram inside the netvar
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
       
    return(eores_OK);
}


// cmd can be: ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS, ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET
static eOresult_t s_parser_POL_MC_CMD_getimpedance(eOcanframe_t *frame, eOcanport_t port, uint8_t type)
{
    
    eOprotIndex_t index = EOK_uint08dummy;  

    eOcanmap_location_t loc = {0};
    loc.port            = port;
    loc.addr            = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex     = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    index = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    if(EOK_uint08dummy == index)
    {
        //s_eo_icubCanProto_mb_send_runtime_error_diagnostics(6);
        return(eores_OK);
    }
    
    
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, index, eoprot_tag_mc_joint_config_impedance);
   

    EOproxy * proxy = eo_transceiver_GetProxy(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
    eOproxy_params_t *param = eo_proxy_Params_Get(proxy, id32);
    if(NULL == param)
    {
        eOerrmanDescriptor_t errdes = {0};
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_proxy_ropdes_notfound);
        errdes.par16            = 0; 
        errdes.par64            = id32; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
        return(eores_OK);
    } 

    eOmc_impedance_t *impedance = (eOmc_impedance_t*)eoprot_variable_ramof_get(eoprot_board_localboard, id32);    
    
    if(ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_PARAMS == type)
    {
        icubCanProto_stiffness_t icub_stiff = *((icubCanProto_stiffness_t*)(&frame->data[1]));
        icubCanProto_damping_t icub_dump = *((icubCanProto_damping_t*)(&frame->data[3]));
        impedance->stiffness = eo_mc4boards_Convert_impedanceStiffness_S2I(eo_mc4boards_GetHandle(), index, icub_stiff);
        impedance->damping = eo_mc4boards_Convert_impedanceDamping_S2I(eo_mc4boards_GetHandle(), index, icub_dump);        
        
    }
    else if(ICUBCANPROTO_POL_MC_CMD__GET_IMPEDANCE_OFFSET == type)
    {
        icubCanProto_torque_t icub_trq = *((icubCanProto_torque_t*)(&frame->data[1]));
        impedance->offset = eo_mc4boards_Convert_torque_S2I(eo_mc4boards_GetHandle(), index, icub_trq);        
    }
    else
    {   // i must have called it badly
        return(eores_NOK_generic);
    }

    param->p08_2 ++;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response               
        eOresult_t res = eo_proxy_ReplyROP_Load(proxy, id32, NULL);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
       

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





