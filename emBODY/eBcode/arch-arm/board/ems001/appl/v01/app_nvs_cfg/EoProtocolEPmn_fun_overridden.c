/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EoProtocolEPmn_fun_overridden.c
    @brief      This file keeps c...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoManagement.h"
#include "EOnv_hid.h"

#include "EOtheBOARDtransceiver.h"
#include "EOMtheEMSappl.h"
#include "EOMtheEMSapplCfg.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolMN.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//static void s_eoprot_ep_mn_fun_generic_configcommand(eOmn_ropsigcfg_command_t* ropsigcfgcmd);

static void s_eoprot_ep_mn_fun_configcommand(eOmn_command_t* command);

static void s_eoprot_ep_mn_fun_querynumofcommand(eOmn_command_t* command);
static void s_eoprot_ep_mn_fun_queryarraycommand(eOmn_command_t* command);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void eoprot_fun_INIT_mn_comm_status(const EOnv* nv)
{
    eOmn_comm_status_t* status = (eOmn_comm_status_t*)nv->ram;
    eOmn_transceiver_properties_t* transp = &status->transceiver;
    
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();
    
    transp->listeningPort                   = emscfg->socketcfg.localport;
    transp->destinationPort                 = emscfg->transcfg.hostipv4port;
    transp->maxsizeRXpacket                 = emscfg->socketcfg.inpdatagramsizeof;
    transp->maxsizeTXpacket                 = emscfg->socketcfg.outdatagramsizeof;
    transp->maxsizeROPframeRegulars         = emscfg->transcfg.sizes.capacityofropframeregulars;
    transp->maxsizeROPframeReplies          = emscfg->transcfg.sizes.capacityofropframereplies;
    transp->maxsizeROPframeOccasionals      = emscfg->transcfg.sizes.capacityofropframeoccasionals;
    transp->maxsizeROP                      = emscfg->transcfg.sizes.capacityofrop;
    transp->maxnumberRegularROPs            = emscfg->transcfg.sizes.maxnumberofregularrops;
    memset(transp->filler06, 0, sizeof(transp->filler06)); 
}



extern void eoprot_fun_UPDT_mn_comm_cmmnds_command_querynumof(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    //eOprotIndex_t index = eoprot_ID2index(nv->ep, nv->id);
    
    eOmn_command_t* command = (eOmn_command_t*)nv->ram;
    
    eOmn_opc_t opc = (eOmn_opc_t)command->cmd.opc;
    
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
           
        switch(opc)
        {

            case eomn_opc_query_numof_EPs:
            case eomn_opc_query_numof_ENs:
            case eomn_opc_query_numof_REGROPs:          
            {
                s_eoprot_ep_mn_fun_querynumofcommand(command); 
            } break;
            
            default:
            {
            } break;
            
        }        
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig

    }    

}

extern void eoprot_fun_UPDT_mn_comm_cmmnds_command_queryarray(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    //eOprotIndex_t index = eoprot_ID2index(nv->ep, nv->id);
    
    eOmn_command_t* command = (eOmn_command_t*)nv->ram;
    
    eOmn_opc_t opc = (eOmn_opc_t)command->cmd.opc;
    
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
           
        switch(opc)
        {

            case eomn_opc_query_array_EPs:              
            case eomn_opc_query_array_EPdes:  
            case eomn_opc_query_array_ENdes:
            case eomn_opc_query_array_REGROPs:                
            {
                s_eoprot_ep_mn_fun_queryarraycommand(command); 
            } break;
            
            default:
            {
            } break;
            
        }        
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig

    }    

}

extern void eoprot_fun_UPDT_mn_comm_cmmnds_command_config(const EOnv* nv, const eOropdescriptor_t* rd) 
{
    //eOprotIndex_t index = eoprot_ID2index(nv->ep, nv->id);
    
    eOmn_command_t* command = (eOmn_command_t*)nv->ram;
    
    eOmn_opc_t opc = (eOmn_opc_t)command->cmd.opc;
    
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
           
        switch(opc)
        {

            case eomn_opc_config_REGROPs_clear:
            case eomn_opc_config_REGROPs_assign:
            case eomn_opc_config_REGROPs_append:
            case eomn_opc_config_REGROPs_remove:
            {
                s_eoprot_ep_mn_fun_configcommand(command);                  
            } break;
            
            default:
            {
            } break;
            
        }        
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig

    }    

}


extern void eoprot_fun_UPDT_mn_appl_cmmnds_go2state(const EOnv* nv, const eOropdescriptor_t* rd) 
{

    eOmn_appl_state_t *newstate_ptr = (eOmn_appl_state_t *)nv->ram;

    switch(*newstate_ptr)
    {
        case applstate_config:
        {
            eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);
        } break;

        case applstate_running:
        {
            uint32_t canBoardsReady;
            char str[60];
            if(!eo_appTheDB_areConnectedCanBoardsReady(eo_emsapplBody_GetDataBaseHandle(eo_emsapplBody_GetHandle()), &canBoardsReady))
            {
                snprintf(str, sizeof(str), "not all boards are ready mask=%u", canBoardsReady);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "on rec go2run cmd", str);
            }
            eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);
        } break;
        
        case applstate_error:
        {
            //I don't expect to receive go to error cmd
            eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STerr);
        } break;

    }

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eoprot_ep_mn_fun_querynumofcommand(eOmn_command_t* command)
{
    EOtransceiver* theems00transceiver; 
    eOmn_cmd_querynumof_t* cmdquerynumof = (eOmn_cmd_querynumof_t*)&command->cmd;
    eOmn_cmd_replynumof_t* cmdreplynumof = (eOmn_cmd_replynumof_t*)&command->cmd;

    eOropdescriptor_t ropdesc;
    
    eOmn_opc_t opc = (eOmn_opc_t) cmdquerynumof->opcpar.opc;

    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
    {
        return;
    }
    
    // retrieve all useful parametes
    uint8_t endpoint    = cmdquerynumof->opcpar.endpoint;

    // then clean data to be sent back:
    memset(command, 0, sizeof(eOmn_command_t));

       
    switch(opc)
    {
    
        case eomn_opc_query_numof_EPs:
        {   // must give back the number of endpoints                   
            cmdreplynumof->opcpar.opc       = eomn_opc_reply_numof_EPs;  
            cmdreplynumof->opcpar.endpoint  = eoprot_endpoint_all;
            cmdreplynumof->opcpar.numberof  = eoprot_endpoints_numberof_get(eoprot_board_localboard);

            
            // ok, now i send the occasional rop
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replynumof);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc);            
        } break;
              

        case eomn_opc_query_numof_ENs:
        {   // must give back the number of entities            
            cmdreplynumof->opcpar.opc       = eomn_opc_reply_numof_ENs;
            cmdreplynumof->opcpar.endpoint  = endpoint;
            if(eoprot_endpoint_all == endpoint)
            {
                cmdreplynumof->opcpar.numberof  = eoprot_entities_numberof_get(eoprot_board_localboard);
            }
            else
            {
                cmdreplynumof->opcpar.numberof = eoprot_entities_in_endpoint_numberof_get(eoprot_board_localboard, endpoint);
            }
           
            // ok, now i send the occasional rop
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replynumof);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc);            
        } break;
       
        
        case eomn_opc_query_numof_REGROPs:
        {   
            cmdreplynumof->opcpar.opc       = eomn_opc_reply_numof_REGROPs;
            cmdreplynumof->opcpar.endpoint  = endpoint;
            
            if(eoprot_endpoint_all == endpoint)
            {
                cmdreplynumof->opcpar.numberof  = eo_transceiver_RegularROP_ArrayID32Size(theems00transceiver);
            }
            else
            {
                cmdreplynumof->opcpar.numberof  = eo_transceiver_RegularROP_ArrayID32SizeWithEP(theems00transceiver, endpoint);    
            }
            
            // ok, now i send the occasional rop
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replynumof);
            ropdesc.data    = NULL; // so that data from the EOnv is retrieved.
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc);
           
        } break;
 
        
        default:
        {
            
        } break;
    }
    
    
}


static void s_eoprot_ep_mn_fun_queryarraycommand(eOmn_command_t* command)
{
    EOtransceiver* theems00transceiver; 
    eOmn_cmd_queryarray_t* cmdqueryarray = (eOmn_cmd_queryarray_t*)&command->cmd;
    eOmn_cmd_replyarray_t* cmdreplyarray = (eOmn_cmd_replyarray_t*)&command->cmd;

    eOropdescriptor_t ropdesc;
    
    eOmn_opc_t opc = (eOmn_opc_t) cmdqueryarray->opcpar.opc;

    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
    {
        return;
    }
    
    // retrieve all useful parametes
    uint8_t endpoint    = cmdqueryarray->opcpar.endpoint;
    uint8_t setnumber   = cmdqueryarray->opcpar.setnumber;
    uint8_t setsize     = cmdqueryarray->opcpar.setsize; 
    // then clean data to be sent back:
    memset(command, 0, sizeof(eOmn_command_t));

       
    switch(opc)
    {
           
        case eomn_opc_query_array_EPs:
        {
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eOnvEP8_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }                
            
            // ok... now i form a rop to send back. at first i write into the nv.
            EOarray* ep08array = eo_array_New(setsize, sizeof(eOnvEP8_t), cmdreplyarray->array);
            //uint8_t total = eoprot_endpoints_numberof_get(eoprot_board_localboard);
                                  
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(ep08array);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_EPs;
            cmdreplyarray->opcpar.endpoint  = eoprot_endpoint_all;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            eoprot_endpoints_array_get(eoprot_board_localboard, ep08array, setnumber*setsize);                      
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc); 
           
        } break;      

        
        case eomn_opc_query_array_EPdes:
        {   
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eoprot_endpoint_descriptor_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }  
            
            EOarray* epdesarray = eo_array_New(setsize, sizeof(eoprot_endpoint_descriptor_t), cmdreplyarray->array);
            //uint8_t total = eoprot_endpoints_numberof_get(eoprot_board_localboard);                 
            
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(epdesarray);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_EPdes;
            cmdreplyarray->opcpar.endpoint  = eoprot_endpoint_all;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            eoprot_endpoints_arrayofdescriptors_get(eoprot_board_localboard, epdesarray, setnumber*setsize);                      
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc); 
           
        } break;           

        
        case eomn_opc_query_array_ENdes:
        {            
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eoprot_entity_descriptor_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }              
            
            EOarray* endesarray = eo_array_New(setsize, sizeof(eoprot_entity_descriptor_t), cmdreplyarray->array);
            //uint8_t total = eoprot_entities_numberof_get(eoprot_board_localboard);
            
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(endesarray);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_ENdes;
            cmdreplyarray->opcpar.endpoint  = endpoint;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            
            if(eoprot_endpoint_all == endpoint) 
            {                            
                eoprot_entities_arrayofdescriptors_get(eoprot_board_localboard, endesarray, setnumber*setsize); 
            }
            else
            {
                eoprot_entities_in_endpoint_arrayofdescriptors_get(eoprot_board_localboard, endpoint, endesarray, setnumber*setsize);
            }
            
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc); 

           
        } break;  
        
        
        case eomn_opc_query_array_REGROPs:
        {
            uint8_t capacity = (sizeof(cmdreplyarray->array) - sizeof(eOarray_head_t)) / sizeof(eOnvID32_t);
            if(0 == setsize)
            {
                setsize = capacity;   
            }  

            // ok... now i form a rop to send back. at first i write into the nv.
            EOarray* id32array = eo_array_New(setsize, sizeof(eOnvID32_t), cmdreplyarray->array);
            // we have total rops ...
            uint8_t total = eo_transceiver_RegularROP_ArrayID32Size(theems00transceiver);            

            
            // ok, prepare the occasional rops
            memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
            ropdesc.ropcode = eo_ropcode_sig;
            ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_comm, 0, eoprot_tag_mn_comm_cmmnds_command_replyarray);
            ropdesc.data    = NULL; // so that dat from teh EOnv is retrieved.            

            eo_array_Reset(id32array);
            cmdreplyarray->opcpar.opc       = eomn_opc_reply_array_REGROPs;
            cmdreplyarray->opcpar.endpoint  = endpoint;
            cmdreplyarray->opcpar.setnumber = setnumber;
            cmdreplyarray->opcpar.setsize   = setsize;
            
            if(eoprot_endpoint_all == endpoint)
            {
                eo_transceiver_RegularROP_ArrayID32Get(theems00transceiver, setnumber*setsize, id32array);    
            }
            else
            {    
                eo_transceiver_RegularROP_ArrayID32GetWithEP(theems00transceiver, endpoint, setnumber*setsize, id32array);
            }
            
            eo_transceiver_OccasionalROP_Load(theems00transceiver, &ropdesc); 
           
        } break;         
        
        default:
        {
            
        } break;
    }
    
    
}

static void s_eoprot_ep_mn_fun_configcommand(eOmn_command_t* command)
{
    uint8_t size, i;
    eOropSIGcfg_t *sigcfg;
    eOropdescriptor_t ropdesc;
    EOtransceiver* theems00transceiver; 
    
    eOmn_cmd_config_t* cmdconfig = (eOmn_cmd_config_t*)&command->cmd;
    EOarray *array = (EOarray*)cmdconfig->array;
    
    uint16_t targetcapacity = (sizeof(cmdconfig->array) - sizeof(eOarray_head_t))  / sizeof(eOropSIGcfg_t);
    
    
    eOmn_opc_t opc = (eOmn_opc_t) cmdconfig->opcpar.opc;

    eOresult_t res;
    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
    {
        return;
    }
    
    
    switch(opc)
    {
    
        case eomn_opc_config_REGROPs_clear:
        {   // just clear
            eo_transceiver_RegularROPs_Clear(theems00transceiver);
        } break;
        
        case eomn_opc_config_REGROPs_assign:
        {   // clear and load all the sigcfg in the array
      
            if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (targetcapacity != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > targetcapacity))
            {
                return;
            }  
            
            eo_transceiver_RegularROPs_Clear(theems00transceiver);

            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == cmdconfig->opcpar.plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == cmdconfig->opcpar.plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;    
                ropdesc.signature               = cmdconfig->opcpar.signature;   
                res = eo_transceiver_RegularROP_Load(theems00transceiver, &ropdesc);
                res = res;
                if(eores_OK != res)
                {
                    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
                    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , 1000);
                }
            }        
        } break;
        
        case eomn_opc_config_REGROPs_append:
        {   // dont clear and load all the sigcfg in the array
            if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (targetcapacity != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > targetcapacity))
            {
                return;
            }            
            
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == cmdconfig->opcpar.plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == cmdconfig->opcpar.plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;
                ropdesc.signature               = cmdconfig->opcpar.signature;
                res = eo_transceiver_RegularROP_Load(theems00transceiver, &ropdesc);
                res = res;
                if(eores_OK != res)
                {
                    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
                    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , 1000);
                }
            }         
        } break;        

        case eomn_opc_config_REGROPs_remove:
        {   // remove all the sigcfg in the array
            if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (targetcapacity != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > targetcapacity))
            {
                return;
            }            
            
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == cmdconfig->opcpar.plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == cmdconfig->opcpar.plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;
                ropdesc.signature               = cmdconfig->opcpar.signature;
                res = eo_transceiver_RegularROP_Unload(theems00transceiver, &ropdesc);
                res = res;
            }         
        } break;          

        
        default:
        {
            
        } break;
    }


}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

