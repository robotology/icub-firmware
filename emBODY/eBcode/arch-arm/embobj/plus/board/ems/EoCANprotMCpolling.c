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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__exceptions(eOcanframe_t *frame, eOcanport_t port)
{
  
    return(eores_OK);    
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__exceptions(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    
     return(eores_OK);
}



extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__SET_MIN_POSITION(eOcanframe_t *frame, eOcanport_t port)
{
    // not expected. print a diagnostics error
    return(eores_OK);
}

#warning --> marco.accame; attenzioen che internalindex sia solo 0/1 e non altro valore ... 
extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    frame->id           = EOCANPROT_CREATE_CANID(eocanprot_msgclass_pollingMotorControl, 0, descriptor->destinationaddress);
    frame->id_type      = 0; 
    frame->frame_type   = 0; 
    frame->size         = 5;
    //frame->data[0]      = ((descriptor->internalindex & 0x1)  << 7) | ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION;
    frame->data[0]      = EOCANPROT_CREATE_POLLING_MC_DATA0(descriptor->internalindex, ICUBCANPROTO_POL_MC_CMD__SET_MIN_POSITION);
    
    *((icubCanProto_position_t*)(&frame->data[1])) = *((icubCanProto_position_t*)descriptor->value);
    
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

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__GET_MIN_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_parser_POL_MC_CMD__SET_MAX_POSITION(eOcanframe_t *frame, eOcanport_t port)
{
    return(eores_OK);
}

extern eOresult_t eocanprotMCpolling_former_POL_MC_CMD__SET_MAX_POSITION(eOcanprot_descriptor_t *descriptor, eOcanframe_t *frame)
{
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





