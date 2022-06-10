
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
#include "EoProtocolAS.h"

// but also to retrieve information of other things ...
#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANmapping.h"

//#include "embot_app_eth_theFTservice.h"

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


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//static void s_former_PER_AS_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__USERDEF(eOcanframe_t *frame, eOcanport_t port)
{
    //  marco.accame on 08apr2019: here is support for reception of a userdef message  
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__POS(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE(eOcanframe_t *frame, eOcanport_t port)
//{
//    //// this can frame is from strain only ... i dont do the check that the board must be a strain
//    //eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processDebugForce);

//    embot::app::eth::theFTservice::canFrameDescriptor cfd 
//    { 
//        port, 
//        frame, 
//        embot::app::eth::theFTservice::canFrameDescriptor::Type::unspecified
//    };
//    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
//    
//    return(eores_OK);    
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE(eOcanframe_t *frame, eOcanport_t port)
//{
//    // this can frame is from strain only ... i dont do the check that the board must be a strain
//    //eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processDebugTorque);

//    embot::app::eth::theFTservice::canFrameDescriptor cfd 
//    { 
//        port, 
//        frame, 
//        embot::app::eth::theFTservice::canFrameDescriptor::Type::unspecified
//    };
//    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
//    
//    return(eores_OK);    
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_former_PER_AS_MSG__FORCE_VECTOR(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
//{
//    s_former_PER_AS_prepare_frame(descriptor, frame, 6, ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR); 
//    memcpy(&frame->data[0], descriptor->cmd.value, 6);      
//    return(eores_OK);                
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__FORCE_VECTOR(eOcanframe_t *frame, eOcanport_t port)
//{
//    // this can frame is from strain only ... i dont do the check that the board must be a strain
//    // process force
//    //eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processForce);

//    embot::app::eth::theFTservice::canFrameDescriptor cfd 
//    { 
//        port, 
//        frame, 
//        embot::app::eth::theFTservice::canFrameDescriptor::Type::force
//    };
//    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
//    
//    return(eores_OK);
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_former_PER_AS_MSG__TORQUE_VECTOR(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
//{
//    s_former_PER_AS_prepare_frame(descriptor, frame, 6, ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR); 
//    memcpy(&frame->data[0], descriptor->cmd.value, 6);      
//    return(eores_OK);                
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__TORQUE_VECTOR(eOcanframe_t *frame, eOcanport_t port)
//{
//    // this can frame is from strain only ... i dont do the check that the board must be a strain
//    // process torque
//    //eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processTorque);

//    embot::app::eth::theFTservice::canFrameDescriptor cfd 
//    { 
//        port, 
//        frame, 
//        embot::app::eth::theFTservice::canFrameDescriptor::Type::torque
//    };
//    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
//    
//    return(eores_OK);
//}
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__HES0TO6(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__HES7TO14(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_NOK_generic); 
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__THERMOMETER_MEASURE(eOcanframe_t *frame, eOcanport_t port)
//{
//    embot::app::eth::theFTservice::canFrameDescriptor cfd 
//    { 
//        port, 
//        frame, 
//        embot::app::eth::theFTservice::canFrameDescriptor::Type::temperature
//    };
//    embot::app::eth::theFTservice::getInstance().AcceptCANframe(cfd);
//    
//    
//    if(eobool_true == eocanprotASperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_THERMOMETER_MSG(frame, port))
//    {
//        return(eores_OK);
//    }    

//    eo_temperatures_AcceptCANframe(eo_temperatures_GetHandle(), eoas_temperature_t1, frame, port); 

//    
//    return(eores_OK);    
//}
{
    return(eores_NOK_generic); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


//static void s_former_PER_AS_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type)
//{   // for periodic the descriptor->address contains ... the origin
//    uint8_t origin = descriptor->loc.addr;
//    frame->id           = EOCANPROT_CREATE_CANID_PERIODIC(eocanprot_msgclass_periodicAnalogSensor, origin, type);
//    frame->id_type      = eocanframeID_std11bits;
//    frame->frame_type   = eocanframetype_data; 
//    frame->size         = len;
//}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





