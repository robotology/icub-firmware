
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"

// in here is whatever is required to offer parsing of can frames and copying into protocol data structures.

#include "EoProtocol.h"
#include "EoProtocolMC.h"

#include "EOtheCANmapping.h"

// but also to retrieve information of other things ...

#include "Controller.h"


//#include "EOtheMC4boards.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

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
// empty-section


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

static eObool_t s_eocanprotMCperiodic_is_from_unused2foc_in_eb5(eOcanframe_t *frame, eOcanport_t port);

static void* s_eocanprotMCperiodic_get_entity(eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, eObrd_caninsideindex_t insideindex, uint8_t *index);

static eObrd_cantype_t s_eocanprotMCperiodic_get_boardtype(eOcanframe_t *frame, eOcanport_t port);

static eOresult_t s_eocanprotMCperiodic_convert_icubCanProtoControlMode2eOmcControlMode(icubCanProto_controlmode_t icubcanProto_controlmode,
                                                                                        eOmc_controlmode_t *eomc_controlmode);
static eOresult_t s_eocanprotMCperiodic_convert_icubCanProtoInteractionMode2eOmcInteractionMode(icubCanProto_interactionmode_t icubcanProto_intermode,
                                                                                                eOmc_interactionmode_t *eomc_intermode);
static void s_former_PER_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------




extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__DEBUG(eOcanframe_t *frame, eOcanport_t port)
{
    // i decode this fram only if it comes from 2foc and i discard it if it comes from a mc4.
    // in date 19 sept 2017 we decided that the 2foc boards would send this diagnostics frame when they find an error of type tbd
    // as a first case we want to sue it for debugging the case of motor encoder dirty which happens on icub-v3.   
    
    eOerrmanDescriptor_t des = {0};
    
    eObrd_cantype_t boardtype = s_eocanprotMCperiodic_get_boardtype(frame, port);
        
    if(eobrd_cantype_foc == boardtype)
    {
        // i just forward the full canframe into a debug message. later on i will add a proper message type.        
        des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag02);
        des.par16 = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64 = eo_common_canframe_data2u64(frame);
        des.sourcedevice = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        des.sourceaddress = EOCANPROT_FRAME_GET_SOURCE(frame);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);               
    
    }
    else if(eobrd_cantype_mc4 == boardtype)
    {   
        // we have a mc4, ... we just drop it
    }
    
    return(eores_OK); 
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__2FOC(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from 2foc only ... as the name of the message says. i dont do the check that the board must be a 2foc
    // i retrieve the motor related to the frame    
    eOmc_motor_t *motor = NULL;
    eOprotIndex_t motorindex = EOK_uint08dummy;
    
    if(NULL == (motor = (eOmc_motor_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_motor, frame, port, eobrd_caninsideindex_first, &motorindex)))
    {
        if(eobool_true == s_eocanprotMCperiodic_is_from_unused2foc_in_eb5(frame, port))
        {   // the board eb5 has an additional 1foc board which sends a can frame of this kind but it does not serve any motor
            // or joint. hence, its can address is not recognised by the EOtheCANmapping object.
            // for this board we must not issue the error and we must do nothing
            return(eores_OK);            
        }        
        return(eores_OK);  
    }    
    
    // note of marco.accame: the following code is ok as long as the 2foc has been configured to send up in its periodic message 
    // current, velocity, and position. if so, frame->data contains: [current:2bytes, velocity:2bytes, position:4bytes]. 
    // the following code extract these values. 
    motor->status.basic.mot_current  = ((int16_t*)frame->data)[0];
    motor->status.basic.mot_velocity = ((int16_t*)frame->data)[1];
    motor->status.basic.mot_position = ((int32_t*)frame->data)[1];
     
    //eo_emsController_AcquireMotorEncoder(motorindex, motor->status.basic.mot_current, motor->status.basic.mot_velocity, motor->status.basic.mot_position);

    MController_update_motor_odometry_fbk_can(motorindex, frame->data);
    
    return(eores_OK);
}



extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__POSITION(eOcanframe_t *frame, eOcanport_t port)
//{    
//    // this frame is from mc4 only, thus ... it manages two joints.
//    // i retrieve the two joints related to the can frame. 
//    eOprotIndex_t jointindex = 0;
//    eOmc_joint_t *joint = NULL;
//    icubCanProto_position_t pos = 0;
//        
//    uint8_t j=0;
//    // the two joints have ...
//    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
//    const uint8_t offset[2] = {0, 4};
//    for(j=0; j<2; j++)
//    {
//        if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, insideindex[j], &jointindex)))
//        {
//            return(eores_OK);        
//        }
//        
//        pos = *((icubCanProto_position_t*)&(frame->data[offset[j]])); 
//        joint->status.core.measures.meas_position = eo_mc4boards_Convert_Position_fromCAN(eo_mc4boards_GetHandle(), jointindex, pos);                      
//    }

//    return(eores_OK);        
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__MOTOR_POSITION(eOcanframe_t *frame, eOcanport_t port)
{
    // this frame is from mc4 only, thus ... it manages two joints.
    // i retrieve the two joints related to the can frame. 
    eOprotIndex_t motorindex = 0;
    eOmc_motor_t *motor = NULL;
    icubCanProto_position_t pos = 0;
        
    uint8_t j=0;
    // the two motors have ...
    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
    const uint8_t offset[2] = {0, 4};
    for(j=0; j<2; j++)
    {
        if(NULL == (motor = (eOmc_motor_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_motor, frame, port, insideindex[j], &motorindex)))
        {
            return(eores_OK);        
        }
        
        pos = *((icubCanProto_position_t*)&(frame->data[offset[j]])); 
        motor->status.basic.mot_position = pos;
    }

    return(eores_OK);       
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PID_VAL(eOcanframe_t *frame, eOcanport_t port)
{
    // this frame is from mc4 only, thus ...
    // i retrieve the two joints related to the can frame. such a frame manages two joints per can address
    eOprotIndex_t jointindex = 0;
    eOmc_joint_t *joint = NULL;
    
    
    uint8_t j=0;
    // the two joints have ...
    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
    const uint8_t offset[2] = {0, 2};    
    for(j=0; j<2; j++)
    {
        if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, insideindex[j], &jointindex)))
        {
            return(eores_OK);        
        }
        #if 1
            //#warning CAVEAT: jstatus->ofpid.output is now an int16_t and the can frame should have a int16_t ... however it works like that
            // marco.accame on 02apr15: i ahve seen together with marco.randazzo that the mc4 send uint16, thus in here there is a double error which make things work 
            // see s_eo_icubCanProto_parser_per_mb_cmd__pidVal() in ems4rd-v01.uvproj
            joint->status.core.ofpid.legacy.output = *((uint16_t*)&(frame->data[offset[j]]));
        #else
        {
            // marco.accame on 02apr15: this is the ways it should be after we have changed the type of jstatus->ofpid.output
            // ... and if we consider the content of the can frame as a signed int int16_t .... however the can frame contains a uint16 (as checked in mc4 code)
            int16_t value = 0;
            value = *((int16_t*)&(frame->data[offset[j]]));
            joint->status.ofpid.legacy.output = value;
        }
        #endif                
    }
   
    return(eores_OK);
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__STATUS(eOcanframe_t *frame, eOcanport_t port)
{
    // both 2foc and mc4 can send this frame. the parsing is however different because:
    // - the 2foc contains info related to one joint only and it is used the EMScontroller to parse the frame,
    // - the mc4 contains info related to two joints and we need to parse the frame directly
    
    eOprotIndex_t jointindex = 0;
    eOmc_joint_t *joint = NULL;
    eOmc_controlmode_t eomc_controlmode = eomc_controlmode_idle;
    eOerrmanDescriptor_t des = {0};
    
    eObrd_cantype_t boardtype = s_eocanprotMCperiodic_get_boardtype(frame, port);
    
    
    if(eobrd_cantype_foc == boardtype)
    {   
        // in case we have a 2foc ... i treat the first joint only   
        // first joint: use eobrd_caninsideindex_first and gets the first 2 bytes of the frame         
        if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, eobrd_caninsideindex_first, &jointindex)))
        {
            if(eobool_true == s_eocanprotMCperiodic_is_from_unused2foc_in_eb5(frame, port))
            {
                return(eores_OK);
            }
            return(eores_OK);        
        }        
        
       
        // marco.accame: the variable eomc_controlmode is not used because eo_emsController_ReadMotorstatus() gets directly what it needs,
        //               thus no need to perform the conversion 
        //res = s_eocanprotMCperiodic_convert_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[1], &eomc_controlmode);
        //if(eores_OK != res)
        //{
        //    #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        //    return(eores_OK);    
        //}
        //eo_emsController_ReadMotorstatus(jointindex, frame->data);
        MController_update_motor_state_fbk(jointindex, frame->data);
        // l'aggiornamento delle nv del giunto sara' fatto nel DO.
        // se l'appl e' in config sicuramente i giunti sono in idle e quindi non c'e' ninete da aggiornare
        // marco.accame: the previous code contained the following function which does nothing but putting into motor.status.filler04[]
        // some flags ... probably for debug. well, we dont do it in here, but if needed we can add it.        
        // s_eo_appTheDB_UpdateMototStatusPtr(mId, frame, runmode);
              
    }
//    else if(eobrd_cantype_mc4 == boardtype)
//    {   // we have a mc4, thus we must manage two joints
//        uint8_t j=0;
//        // the two joints have ...
//        const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
//        const uint8_t offset[2] = {1, 3};
//        
//        for(j=0; j<2; j++) // 2 joints ....
//        {                       
//            // joint i-th
//            if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, insideindex[j], &jointindex)))
//            {
//                return(eores_OK);        
//            }  
//            // manage controlmode
//            if(eores_OK != s_eocanprotMCperiodic_convert_icubCanProtoControlMode2eOmcControlMode((icubCanProto_controlmode_t) frame->data[offset[j]], &eomc_controlmode))
//            {
//                //#warning -> TODO: add diagnostics about not found control mode as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
//                return(eores_OK);    
//            }
//            
//            joint->status.core.modes.controlmodestatus = eomc_controlmode;
//            
//            if(eomc_controlmode_hwFault == eomc_controlmode)
//            {        
//                des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_hwfault2);
//                des.par16 = (frame->data[1] << 8) | eomc_controlmode;
//                des.sourceaddress = jointindex;
//                des.sourcedevice = eo_errman_sourcedevice_localboard;
//                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);               
//            }                        
//        }  
//    }
    
    return(eores_OK);    
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__CURRENT(eOcanframe_t *frame, eOcanport_t port)
{
    // this frame is from mc4 only, thus ...
    // i retrieve the two motors related to the can frame. such a frame manages two motors per can address
    eOprotIndex_t motorindex = 0;
    eOmc_motor_t *motor = NULL;
        
    uint8_t m=0;
    // the two motors have ...
    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
    const uint8_t offset[2] = {0, 2};    
    for(m=0; m<2; m++)
    {
        if(NULL == (motor = (eOmc_motor_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_motor, frame, port, insideindex[m], &motorindex)))
        {
            return(eores_OK);        
        }        
        motor->status.basic.mot_current = *((uint16_t*)&(frame->data[offset[m]]));          
    }
   
    return(eores_OK);
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PRINT(eOcanframe_t *frame, eOcanport_t port)
{
    // this frame contains a can print. i forward it to robotinterface using the error manager    
    eOerrmanDescriptor_t errdes = {0};
    errdes.code                 = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_canprint);
    errdes.par16                = frame->size;
    errdes.par64                = eo_common_canframe_data2u64((eOcanframe_t*)frame);
    errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
    errdes.sourceaddress        = EOCANPROT_FRAME_GET_SOURCE(frame);                
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes);             
       
    return(eores_OK);
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__VELOCITY(eOcanframe_t *frame, eOcanport_t port)
//{
//    // this frame is from mc4 only, thus ...
//    // i retrieve the two joints related to the can frame. such a frame manages two joints per can address
//    eOprotIndex_t jointindex = 0;
//    eOmc_joint_t *joint = NULL;
//    icubCanProto_velocity_t vel_icubCanProtValue = 0;
//    icubCanProto_acceleration_t acc_icubCanProtValue = 0;
//    
//    
//    uint8_t j=0;
//    // the two joints have ...
//    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
//    const uint8_t offsetvelocity[2] = {0, 4};    
//    const uint8_t offsetacceleration[2] = {2, 6}; 
//    for(j=0; j<2; j++)
//    {
//        if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, insideindex[j], &jointindex)))
//        {
//            return(eores_OK);        
//        }
//        vel_icubCanProtValue = *((icubCanProto_velocity_t*)&(frame->data[offsetvelocity[j]]));
//        acc_icubCanProtValue = *((icubCanProto_acceleration_t*)&(frame->data[offsetacceleration[j]]));
//        
//        joint->status.core.measures.meas_velocity = eo_mc4boards_Convert_Velocity_fromCAN(eo_mc4boards_GetHandle(), jointindex, vel_icubCanProtValue);
//    
//        joint->status.core.measures.meas_acceleration = eo_mc4boards_Convert_Acceleration_fromCAN(eo_mc4boards_GetHandle(), jointindex, acc_icubCanProtValue);               
//    }
//   
//    return(eores_OK);
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PID_ERROR(eOcanframe_t *frame, eOcanport_t port)
//{
//    // this frame is from mc4 only, thus ...
//    // i retrieve the two joints related to the can frame. such a frame manages two joints per can address
//    eOprotIndex_t jointindex = 0;
//    eOmc_joint_t *joint = NULL;
//    uint16_t pidpos_error = 0;
//    uint16_t pidtrq_error = 0;
//    
//    
//    uint8_t j=0;
//    // the two joints have ...
//    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
//    const uint8_t offsetpos[2] = {0, 2};    
//    const uint8_t offsettrq[2] = {4, 6}; 
//    for(j=0; j<2; j++)
//    {
//        if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, insideindex[j], &jointindex)))
//        {
//            return(eores_OK);        
//        }
//        
//        pidpos_error = *((uint16_t*)&(frame->data[offsetpos[j]]));
//        pidtrq_error = *((uint16_t*)&(frame->data[offsettrq[j]]));
//        
//        if(eomc_controlmode_torque == joint->status.core.modes.controlmodestatus)
//        {
//            joint->status.core.ofpid.legacy.error = pidtrq_error;
//        }
//        else
//        {
//            joint->status.core.ofpid.legacy.error = pidpos_error;
//        }            
//    }
//   
//    return(eores_OK);
//}
{
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__ADDITIONAL_STATUS(eOcanframe_t *frame, eOcanport_t port)
//{
//    // this frame is from mc4 only, thus ...
//    // i retrieve the two joints related to the can frame. such a frame manages two joints per can address
//    eOprotIndex_t jointindex = 0;
//    eOmc_joint_t *joint = NULL;
//    icubCanProto_interactionmode_t caninteractionmodes[2] = {icubCanProto_interactionmode_stiff, icubCanProto_interactionmode_stiff};
//       
//    uint8_t j=0;
//    // the two joints have ...
//    const eObrd_caninsideindex_t insideindex[2] = {eobrd_caninsideindex_first, eobrd_caninsideindex_second};
//    caninteractionmodes[0] = (icubCanProto_interactionmode_t)(frame->data[0] & 0x0f);          // for first joint
//    caninteractionmodes[1] = (icubCanProto_interactionmode_t)((frame->data[0] & 0xf0) >> 4);     // for second joint
//    eOmc_interactionmode_t tmp = eOmc_interactionmode_stiff;
//    for(j=0; j<2; j++)
//    {
//        if(NULL == (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, frame, port, insideindex[j], &jointindex)))
//        {
//            return(eores_OK);        
//        }
//        
//        if(eores_OK != s_eocanprotMCperiodic_convert_icubCanProtoInteractionMode2eOmcInteractionMode(caninteractionmodes[j], &tmp))
//        {
//            return(eores_OK);        
//        }
//        joint->status.core.modes.interactionmodestatus = tmp;        
//    }
//   
//    return(eores_OK);
//}
{
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotMCperiodic_former_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_PER_MC_prepare_frame(descriptor, frame, 8, ICUBCANPROTO_PER_MC_MSG__EMSTO2FOC_DESIRED_CURRENT);     
    // all the data is in payload
    int16_t *pwmList = (int16_t*)descriptor->cmd.value;    
    *((uint16_t*)(&frame->data[0])) = pwmList[0];
    *((uint16_t*)(&frame->data[2])) = pwmList[1];
    *((uint16_t*)(&frame->data[4])) = pwmList[2];
    *((uint16_t*)(&frame->data[6])) = pwmList[3];          

    return(eores_OK);        
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_eocanprotMCperiodic_is_from_unused2foc_in_eb5(eOcanframe_t *frame, eOcanport_t port)
{
    const eOprotBRD_t eb5board = 4;
    eOprotBRD_t localboard = eoprot_board_local_get();
    
    if(localboard != eb5board)
    {
        return(eobool_false);
    }
    
    if( (eOcanport1 == port) &&  (2 == EOCANPROT_FRAME_GET_SOURCE(frame)) )
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }        
}


static void* s_eocanprotMCperiodic_get_entity(eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, eObrd_caninsideindex_t insideindex, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eObrd_canlocation_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = insideindex;
    
    ii = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, entity);
    
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


static eObrd_cantype_t s_eocanprotMCperiodic_get_boardtype(eOcanframe_t *frame, eOcanport_t port)
{
    eObrd_canlocation_t loc = {0};
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none; // dont care of it if we look for a board
    return(eo_canmap_GetBoardType(eo_canmap_GetHandle(), loc));
}  


static eOresult_t s_eocanprotMCperiodic_convert_icubCanProtoControlMode2eOmcControlMode(icubCanProto_controlmode_t icubcanProto_controlmode,
                                                                                        eOmc_controlmode_t *eomc_controlmode)
{
    eOresult_t res = eores_OK;

    switch(icubcanProto_controlmode)
    {
        case icubCanProto_controlmode_idle:
        {
            *eomc_controlmode = eomc_controlmode_idle;
        } break;
        
        case icubCanProto_controlmode_position:
        {
            *eomc_controlmode = eomc_controlmode_position;
        } break;
        
        case icubCanProto_controlmode_velocity:
        {
            *eomc_controlmode = eomc_controlmode_velocity; //
        } break;
        
        case icubCanProto_controlmode_torque:
        {
            *eomc_controlmode = eomc_controlmode_torque;
        } break;
        
        case icubCanProto_controlmode_impedance_pos:
        {
            *eomc_controlmode = eomc_controlmode_impedance_pos;
        } break;

        case icubCanProto_controlmode_impedance_vel:
        {
            *eomc_controlmode = eomc_controlmode_impedance_vel;
        } break;

        case icubCanProto_controlmode_current:
        {
            *eomc_controlmode = eomc_controlmode_current;
        } break;
        case icubCanProto_controlmode_mixed:
        {
            *eomc_controlmode = eomc_controlmode_mixed;
        } break;
        
        case icubCanProto_controlmode_direct:
        {
            *eomc_controlmode = eomc_controlmode_direct;
        } break;

        case icubCanProto_controlmode_openloop:
        {
            *eomc_controlmode = eomc_controlmode_openloop;
        } break;
        
        case  icubCanProto_controlmode_calibration:
        {
            *eomc_controlmode = eomc_controlmode_calib;
        } break;
        
        case icubCanProto_controlmode_hwFault:
        {
            *eomc_controlmode = eomc_controlmode_hwFault;
        } break;
        
        case icubCanProto_controlmode_notConfigured:
        {
            *eomc_controlmode = eomc_controlmode_notConfigured;
        } break;
        
        case icubCanProto_controlmode_configured:
        {
            *eomc_controlmode = eomc_controlmode_configured;
        } break;
        
        case icubCanProto_controlmode_unknownError:
        {
            *eomc_controlmode = eomc_controlmode_unknownError;
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    return(res);
}


static eOresult_t s_eocanprotMCperiodic_convert_icubCanProtoInteractionMode2eOmcInteractionMode(icubCanProto_interactionmode_t icubcanProto_intermode,
                                                                                                eOmc_interactionmode_t *eomc_intermode)
{
    eOresult_t res = eores_OK;
    
    switch(icubcanProto_intermode)
    {
        case icubCanProto_interactionmode_stiff:
        {
            *eomc_intermode = eOmc_interactionmode_stiff;
        } break;
        
        case icubCanProto_interactionmode_compliant:
        {
            *eomc_intermode = eOmc_interactionmode_compliant;
        } break;
    
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    return(res);
}

// former helper funtions

static void s_former_PER_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type)
{   // the source address in actually inside descriptor->address
    frame->id           = EOCANPROT_CREATE_CANID_PERIODIC(eocanprot_msgclass_periodicMotorControl, descriptor->loc.addr, type);
    frame->id_type      = eocanframeID_std11bits;
    frame->frame_type   = eocanframetype_data; 
    frame->size         = len;
}

 

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





