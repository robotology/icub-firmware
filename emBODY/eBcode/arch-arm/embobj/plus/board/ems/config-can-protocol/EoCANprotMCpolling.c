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

#include "EOMtheEMSappl.h"

#include "EOtheEMSapplBody.h"

#include "EOtheCANmapping.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANprotocolCfg.h" 


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

// parser helper funtions

//static eOresult_t s_parser_POL_MC_CMD__MOTION_DONE(eOcanframe_t *frame, eOcanport_t port);

// former helper funtions

static void s_former_POL_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);

static void s_former_POL_MC_CMD_setpid(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type);
static void s_former_POL_MC_CMD_setpid_limits(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



//extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__exceptions(eOcanframe_t *frame, eOcanport_t port)
//{   // manage in here msgtype lower than ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION
//    
//    eOresult_t res = eores_OK;
//    
//    uint8_t type = EOCANPROT_FRAME_POLLING_GET_TYPE(frame);
//    
//    switch(type)
//    {
//        case ICUBCANPROTO_POL_MC_CMD__NO_MESSAGE:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;
//        
//        case ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;

//        case ICUBCANPROTO_POL_MC_CMD__MOTION_DONE:
//        {
//            res = s_parser_POL_MC_CMD__MOTION_DONE(frame, port);          
//        } break;

//        case ICUBCANPROTO_POL_MC_CMD__GET_ADDITIONAL_INFO:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;
//        
//        case ICUBCANPROTO_POL_MC_CMD__GET_DEBUG_PARAM:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;        

//        case ICUBCANPROTO_POL_MC_CMD__GET_ENCODER_POSITION:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break; 
//        
//        case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break; 

//        case ICUBCANPROTO_POL_MC_CMD__GET_BOARD_ID:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;  

//        default:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;                 
//    }
//    
//     
//    return(res);    
//}

//extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__exceptions(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
//{   // manage in here msgtype lower than ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION
//    eOresult_t res = eores_OK;
//    
//    uint8_t type = EOCANPROT_FRAME_POLLING_GET_TYPE(frame);
//    
//    switch(type)
//    {
//        case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_RUN(descriptor, frame);
//        } break;
//        
//        case ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__CONTROLLER_IDLE(descriptor, frame);
//        } break;
//        
//        case ICUBCANPROTO_POL_MC_CMD__CALIBRATE_ENCODER:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__CALIBRATE_ENCODER(descriptor, frame);
//        } break;        
//        
//        case ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__ENABLE_PWM_PAD(descriptor, frame);
//        } break; 

//        case ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__ENABLE_PWM_PAD(descriptor, frame);
//        } break;        

//        case ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__GET_CONTROL_MODE(descriptor, frame);
//        } break;  

//        case ICUBCANPROTO_POL_MC_CMD__MOTION_DONE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__MOTION_DONE(descriptor, frame);
//        } break;  

//        case ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE(descriptor, frame);
//        } break; 

//        case ICUBCANPROTO_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT(descriptor, frame);
//        } break; 
//               
//        case ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__POSITION_MOVE(descriptor, frame);
//        } break;   

//        case ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__VELOCITY_MOVE(descriptor, frame);
//        } break; 

//        case ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_TORQUE(descriptor, frame);
//        } break; 

////        case ICUBCANPROTO_POL_MC_CMD__GET_DESIRED_TORQUE:
////        {   
////            res = eocanprotMCpolling_former_POL_MC_CMD__GET_DESIRED_TORQUE(descriptor, frame);
////        } break; 

//        case ICUBCANPROTO_POL_MC_CMD__STOP_TRAJECTORY:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__STOP_TRAJECTORY(descriptor, frame);
//        } break; 

//        case ICUBCANPROTO_POL_MC_CMD__SET_COMMAND_POSITION:
//        {   
//            res = eocanprotMCpolling_former_POL_MC_CMD__SET_COMMAND_POSITION(descriptor, frame);
//        } break;         
//        
//        default:
//        {   // not managed
//            res = eores_NOK_generic;
//        } break;                 
//    }   
//     
//    return(res);  
//}



extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port)
{
    // not expected. print a diagnostics error
    return(eores_OK);
}



    
extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port)
{    
#ifdef USE_PROTO_PROXY
    
    eOprotIndex_t index = EOK_uint08dummy;  

    eOcanmap_entitylocation_t loc = {0};
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
    #warning marco.accame: must simplify teh conversion from one measure to another. why to call so many functions?
    limits->min = eo_appMeasConv_jntPosition_E2I(eo_emsapplBody_GetMeasuresConverterHandle(eo_emsapplBody_GetHandle()), index, icub_pos);

    param->p08_2 ++;
    
    if(param->p08_1 == param->p08_2)
    {
        // send back response
        eOmeas_position_limits_t limits_aux = {0};
               
        if(limits->max < limits->min)
        {
            limits_aux.max = limits->min;
            limits_aux.min = limits->max;
        }
        else
        {
            memcpy(&limits_aux, limits, sizeof(eOmeas_position_limits_t));
        }
        
        eOresult_t res = eo_proxy_ReplyROP_Load(proxy, id32, &limits_aux);  
        eom_emsappl_SendTXRequest(eom_emsappl_GetHandle());        
    }
    
    
#endif //USE_PROTO_PROXY
    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_OK);
}



// former public functions


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
    icubCanProto_calibrator_t* calib = (icubCanProto_calibrator_t*)descriptor->value;
   
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
        
        default:
        {
            #warning --> TODO error about unknown param ...
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
    #warning --> marco.accame: is this command used or not?
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_CONTROL_MODE);
    return(eores_OK);         
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__MOTION_DONE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__MOTION_DONE);
    return(eores_OK);     
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CONTROL_MODE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE);
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_SPEED_ESTIM_SHIFT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    eOicubCanProto_estimShift_t *shift = (eOicubCanProto_estimShift_t*)descriptor->value;
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
    icubCanProto_setpoint_position_t *setpointp = (icubCanProto_setpoint_position_t *)descriptor->value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 7, ICUBCANPROTO_POL_MC_CMD__POSITION_MOVE);
    // now i prepare data[1] -> data[6]
    *((icubCanProto_position_t*)(&frame->data[1])) = setpointp->value;
    *((icubCanProto_velocity_t*)(&frame->data[5])) = setpointp->withvelocity;  
    
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__VELOCITY_MOVE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_setpoint_velocity_t *setpointv = (icubCanProto_setpoint_velocity_t *)descriptor->value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__VELOCITY_MOVE);
    // now i prepare data[1] -> data[4]
    *((icubCanProto_velocity_t*)(&frame->data[1])) = setpointv->value;
    *((icubCanProto_acceleration_t*)(&frame->data[3])) = setpointv->withacceleration;  
    
    return(eores_OK);       
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_DESIRED_TORQUE(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_setpoint_torque_t *setpointt = (icubCanProto_setpoint_torque_t *)descriptor->value;
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_DESIRED_TORQUE);
    // now i prepare data[1] -> data[4]    
    // marco.accame: value is a int16_t but i have found it copied byte by byte, so i keep it in this way
    frame->data[1] = ((uint8_t*)&(setpointt->value))[0];
    frame->data[2] = ((uint8_t*)&(setpointt->value))[1];
    frame->data[4] = frame->data[3] = (frame->data[2] & 0x80) ? 0xFF : 0;
    
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
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->value);
    
    return(eores_OK);    
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION);    
    // now i prepare data[1] -> data[4]   
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->value);    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_MIN_POSITION);
    return(eores_OK);  
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 5, ICUBCANPROTO_POL_MC_CMD__SET_MAX_POSITION);    
    // now i prepare data[1] -> data[4]   
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->value);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 1, ICUBCANPROTO_POL_MC_CMD__GET_MAX_POSITION);
    return(eores_OK);  
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_VELOCITY(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_MAX_VELOCITY);    
    // now i prepare data[1] -> data[2]   
    *((icubCanProto_velocity_t*)(&frame->data[1])) = *((icubCanProto_velocity_t*)descriptor->value);    
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_CURRENT_LIMIT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    // NOTE: eo-protocol uses 16 bits for current (sign+value), while icubcanprotocol uses 32bits.
    // in here we should not use negative currents, nevertheless we convert the value correctly
    eOmeas_current_t *eomeasCurrent = (eOmeas_current_t*)descriptor->value;
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
    memcpy(&frame->data[1], descriptor->value, 4);
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_SHIFT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_velocityShift_t shift = *((icubCanProto_velocityShift_t*)descriptor->value);
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_VEL_SHIFT);    
    // now i prepare data[1] -> data[2]  
    *((uint16_t*)(&frame->data[1])) = shift;    
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


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_VEL_TIMEOUT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_POL_MC_prepare_frame(descriptor, frame, 3, ICUBCANPROTO_POL_MC_CMD__SET_VEL_TIMEOUT);  
    // now i prepare data[1] -> data[2]  
    *((uint16_t*)(&frame->data[1])) = *((uint16_t*)descriptor->value);    
    return(eores_OK);
}


extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_PARAMS(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_impedance_t *imp = (icubCanProto_impedance_t *)descriptor->value;
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

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_IMPEDANCE_OFFSET(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    icubCanProto_torque_t *offset = (icubCanProto_torque_t *)descriptor->value;
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

// parser functions

//extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__GET_CONTROL_MODE(eOcanframe_t *frame, eOcanport_t port)
//{
//       
//}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__MOTION_DONE(eOcanframe_t *frame, eOcanport_t port)
{
    eOresult_t res = eores_OK; 
    
    // retrieve the joint related to the frame
    eOcanmap_entitylocation_t loc = {0};
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);
    loc.insideindex = EOCANPROT_FRAME_POLLING_MC_GET_INTERNALINDEX(frame);
    
    eOprotIndex_t jointindex = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint);
    
    if(EOK_uint08dummy == jointindex)
    {
        #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(eores_OK);
    }
    
    eOmc_joint_t *joint = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, jointindex);
    
    if(NULL == joint)
    {
        #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(eores_OK);        
    }
    
    eOmc_motionmonitorstatus_t motionmonitorstatus = (eOmc_motionmonitorstatus_t) joint->status.basic.motionmonitorstatus;
    
    if(eomc_motionmonitorstatus_notmonitored == motionmonitorstatus)
    {
        // pc104 isn't interested in motion monitoring
        return(eores_OK);
    }
    
    joint->status.basic.motionmonitorstatus = (eOmc_motionmonitorstatus_t)frame->data[1];    
    
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

// - parser helper funtions




// former helper funtions

static void s_former_POL_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type)
{
    frame->id           = EOCANPROT_CREATE_CANID(eocanprot_msgclass_pollingMotorControl, 0, descriptor->destinationaddress);
    frame->id_type      = eocanframeID_std11bits;
    frame->frame_type   = eocanframetype_data; 
    frame->size         = len;
    frame->data[0]      = EOCANPROT_CREATE_POLLING_MC_DATA0(descriptor->internalindex, type);    
}


static void s_former_POL_MC_CMD_setpid(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t type)
{
    eOmc_PID_t *pid = (eOmc_PID_t*) descriptor->value;    
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
    eOmc_PID_t *pid = (eOmc_PID_t*) descriptor->value;    
    s_former_POL_MC_prepare_frame(descriptor, frame, 8, type);   
    // now i prepare data[1] -> data[7]  
    *((int16_t*)(&frame->data[1])) = (int16_t) pid->offset;
    *((int16_t*)(&frame->data[3])) = (int16_t) pid->limitonoutput;
    *((int16_t*)(&frame->data[5])) = (int16_t) pid->limitonintegral;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





