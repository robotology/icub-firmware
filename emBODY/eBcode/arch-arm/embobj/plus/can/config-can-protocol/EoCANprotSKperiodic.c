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
#include "EoProtocolSK.h"

// but also to retrieve information of other things ...

#include "EOMtheEMSappl.h"

#include "EOtheErrorManager.h"
#include "EoError.h"

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

static void* s_eocanprotSKperiodic_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern eOresult_t eocanprotSKperiodic_parser_ANY_PERIODIC_SKIN_MSG(eOcanframe_t *frame, eOcanport_t port)
{
    // this can frame is from skin only ... i dont do the check that the board must be a skin
    // i retrieve the skin entity related to the frame    
    eOsk_skin_t *skin = NULL;
    eOprotIndex_t index = EOK_uint08dummy;
    
    if(NULL == (skin = s_eocanprotSKperiodic_get_entity(eoprot_endpoint_skin, eoprot_entity_sk_skin, frame, port, &index)))
    {
        #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(eores_OK);  
    }        
       
    
    // marco.accame:        
    // if skin->config.sigmode is eosk_sigmode_dontsignal then we dont need using the payload of the can frame. 
    // however, also if skin->config.sigmode is eosk_sigmode_signal but we are not in RUN mode we should not put 
    // frames inside the arrayofcandata. this latter for example is tha case if we are still in the cfg->run transition 
    // and thus not  yet inside the control-loop which empties the arrayofcandata, or also if  the udp packet with go2run 
    // rop<> gets lost.
    
    // we may decode some canframes of this kind if we pass from run to config mode and we process frame buffered in the rx-fifo    
    if(eosk_sigmode_dontsignal == skin->config.sigmode)
    {
        return(eores_OK);
    } 

    eOsmStatesEMSappl_t applstate = eo_sm_emsappl_STcfg;
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &applstate);   
    if(eo_sm_emsappl_STrun != applstate)
    {
        return(eores_OK);
    }
    
    // otherwise we put the canframe content inside teh arrayofcandata
    eOsk_candata_t candata = {0};
    uint16_t info = EOSK_CANDATA_INFO(frame->size, frame->id);
    candata.info = info;    
    memcpy(candata.data, frame->data, sizeof(candata.data));   
    
    if(eores_OK != eo_array_PushBack((EOarray*)(&skin->status.arrayofcandata), &candata))
    {   
        eOerrmanDescriptor_t des = {0};
        des.code            = eoerror_code_get(eoerror_category_Skin, eoerror_value_SK_arrayofcandataoverflow);
        des.par16           = (frame->id & 0x0fff) | ((frame->size & 0x000f) << 12);
        des.par64           = eo_common_canframe_data2u64((eOcanframe_t*)frame);
        des.sourceaddress   = EOCANPROT_FRAME_GET_SOURCE(frame);
        des.sourcedevice    = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &des); 
    }

    return(eores_OK);       
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void* s_eocanprotSKperiodic_get_entity(eOprotEndpoint_t endpoint, eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eOcanmap_location_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = eocanmap_insideindex_none;
    
    ii = eo_canmap_GetEntityIndexExtraCheck(eo_canmap_GetHandle(), loc, endpoint, entity);
    
    if(EOK_uint08dummy == ii)
    {     
        #warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(NULL);
    }
    
    ret = eoprot_entity_ramof_get(eoprot_board_localboard, endpoint, entity, ii);
    
    if(NULL != index)
    {
        *index = ii;        
    }  

    return(ret);   
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





