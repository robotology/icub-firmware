
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_eth_theServiceMC.h"

#include "stdlib.h"
#include "EoCommon.h"

// in here is whatever is required to offer parsing of can frames and copying into protocol data structures.

#include "EoProtocol.h"
#include "EoProtocolMC.h"
//#include "Controller.h"

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

//static eObrd_cantype_t s_eocanprotMCperiodic_get_boardtype(eOcanframe_t *frame, eOcanport_t port);

static void s_former_PER_MC_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__DEBUG(eOcanframe_t *frame, eOcanport_t port)
{
    // marco.accame: we could add the management of this message maybe to theServiceMC ...  
    //  embot::app::eth::theServiceMC::getInstance().process({port, frame, 0});
    
    eOerrmanDescriptor_t des = {0};
    
    // i just forward the full canframe into a debug message. later on i will add a proper message type.        
    des.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag02);
    des.par16 = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
    des.par64 = eo_common_canframe_data2u64(frame);
    des.sourcedevice = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
    des.sourceaddress = EOCANPROT_FRAME_GET_SOURCE(frame);
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, NULL, NULL, &des);               

   
    return(eores_OK); 
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__2FOC(eOcanframe_t *frame, eOcanport_t port)
{
    embot::app::eth::theServiceMC::getInstance().process({port, frame, 0});
            
    return(eores_OK);
}



extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__POSITION(eOcanframe_t *frame, eOcanport_t port)
{   
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__MOTOR_POSITION(eOcanframe_t *frame, eOcanport_t port)
{
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)
    return(eores_OK);       
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PID_VAL(eOcanframe_t *frame, eOcanport_t port)
{
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)
    return(eores_OK);
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__STATUS(eOcanframe_t *frame, eOcanport_t port)
{
    embot::app::eth::theServiceMC::getInstance().process({port, frame, 0});
        
    return(eores_OK);    
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__CURRENT(eOcanframe_t *frame, eOcanport_t port)
{
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)    
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
{
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__PID_ERROR(eOcanframe_t *frame, eOcanport_t port)
{
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotMCperiodic_parser_PER_MC_MSG__ADDITIONAL_STATUS(eOcanframe_t *frame, eOcanport_t port)
{
    // not from modern can-based mc boards managed by the amc (amcbldc, 2foc)
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


//static eObrd_cantype_t s_eocanprotMCperiodic_get_boardtype(eOcanframe_t *frame, eOcanport_t port)
//{
//    eObrd_canlocation_t loc = {0};
//    loc.port = port;
//    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
//    loc.insideindex = eobrd_caninsideindex_none; // dont care of it if we look for a board
//    return(eo_canmap_GetBoardType(eo_canmap_GetHandle(), loc));
//}  


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





