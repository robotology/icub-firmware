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

#include "EoProtocol.h"
#include "EoProtocolAS.h"

// but also to retrieve information of other things ...
#include "EOtheErrorManager.h"
#include "EoError.h"

#include "EOtheCANmapping.h"
#include "EOtheMAIS.h"
#include "EOtheSTRAIN.h"
#include "EOtheTemperatures.h"
#include "EOthePSC.h"
#include "EOthePOS.h"

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

static void s_former_PER_AS_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type);


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
    // either one or the other is active.
    eo_psc_AcceptCANframe(eo_psc_GetHandle(), frame, port);
    eo_pos_AcceptCANframe(eo_pos_GetHandle(), frame, port);    
    return(eores_OK);  
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from strain only ... i dont do the check that the board must be a strain
    eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processDebugForce);
    return(eores_OK);    
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from strain only ... i dont do the check that the board must be a strain
    eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processDebugTorque);
    return(eores_OK);    
}


extern eOresult_t eocanprotASperiodic_former_PER_AS_MSG__FORCE_VECTOR(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_PER_AS_prepare_frame(descriptor, frame, 6, ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR); 
    memcpy(&frame->data[0], descriptor->cmd.value, 6);      
    return(eores_OK);                
}

extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__FORCE_VECTOR(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from strain only ... i dont do the check that the board must be a strain
    // process force
    eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processForce);
    return(eores_OK);
}


extern eOresult_t eocanprotASperiodic_former_PER_AS_MSG__TORQUE_VECTOR(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    s_former_PER_AS_prepare_frame(descriptor, frame, 6, ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR); 
    memcpy(&frame->data[0], descriptor->cmd.value, 6);      
    return(eores_OK);                
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__TORQUE_VECTOR(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from strain only ... i dont do the check that the board must be a strain
    // process torque
    eo_strain_AcceptCANframe(eo_strain_GetHandle(), frame, port, processTorque);
    return(eores_OK);
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__HES0TO6(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from mais only ... i dont do the check that the board must be a mais
    // process values
    eo_mais_AcceptCANframe(eo_mais_GetHandle(), frame, port, processHES0TO6);
    return(eores_OK);    
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__HES7TO14(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from mais only ... i dont do the check that the board must be a mais
    // process values
    eo_mais_AcceptCANframe(eo_mais_GetHandle(), frame, port, processHES7TO14);
    return(eores_OK);    
}


extern eOresult_t eocanprotASperiodic_parser_PER_AS_MSG__THERMOMETER_MEASURE(eOcanframe_t *frame, eOcanport_t port)
{
    if(eobool_true == eocanprotASperiodic_redefinable_SkipParsingOf_ANY_PERIODIC_THERMOMETER_MSG(frame, port))
    {
        return(eores_OK);
    }    

    eo_temperatures_AcceptCANframe(eo_temperatures_GetHandle(), eoas_temperature_t1, frame, port);    
    return(eores_OK);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_former_PER_AS_prepare_frame(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame, uint8_t len, uint8_t type)
{   // for periodic the descriptor->address contains ... the origin
    uint8_t origin = descriptor->loc.addr;
    frame->id           = EOCANPROT_CREATE_CANID_PERIODIC(eocanprot_msgclass_periodicAnalogSensor, origin, type);
    frame->id_type      = eocanframeID_std11bits;
    frame->frame_type   = eocanframetype_data; 
    frame->size         = len;
}

// marco.accame on 23 nov 2020. 
// this function retrieves the entity from the can frame and it works fine. but we dont use it in here because we use
// the _AcceptCANframe() methods of the related objects. 
// nevertheless i would like to keep it even if under a #if 0 
//     //if(NULL == (strain = (eOas_strain_t*) s_eocanprotASperiodic_get_entity(eoprot_endpoint_analogsensors, eoprot_entity_as_strain, frame, port, &index)))
#if 0
static void* s_eocanprotASperiodic_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eObrd_canlocation_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eobrd_caninsideindex_none;
    
    ii = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, endpoint, entity);
    
    if(EOK_uint08dummy == ii)
    {     
        //#warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(NULL);
    }
    
    ret = eoprot_entity_ramof_get(eoprot_board_localboard, endpoint, entity, ii);
    
    if(NULL != index)
    {
        *index = ii;        
    }  

    return(ret);   
}

#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





